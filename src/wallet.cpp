// src/wallet.cpp
#include "wallet.h"
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/core_names.h>
#include <sstream>

Wallet::Wallet() {
    // Use EVP_PKEY for key generation
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
    if (!ctx) throw std::runtime_error("Failed to create EVP_PKEY_CTX");

    if (EVP_PKEY_keygen_init(ctx) <= 0 ||
        EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, NID_secp256k1) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize keygen");
    }

    keyPair = nullptr;
    if (EVP_PKEY_keygen(ctx, &keyPair) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Failed to generate key");
    }
    EVP_PKEY_CTX_free(ctx);

    // Serialize public key
    BIO* bio = BIO_new(BIO_s_mem());
    PEM_write_bio_PUBKEY(bio, keyPair);
    char* buffer;
    long len = BIO_get_mem_data(bio, &buffer);
    publicKey = std::string(buffer, len);
    BIO_free(bio);

    // Serialize private key (store securely in production)
    bio = BIO_new(BIO_s_mem());
    PEM_write_bio_PrivateKey(bio, keyPair, nullptr, nullptr, 0, nullptr, nullptr);
    len = BIO_get_mem_data(bio, &buffer);
    privateKey = std::string(buffer, len);
    BIO_free(bio);
}

Wallet::~Wallet() {
    EVP_PKEY_free(keyPair); // Updated to EVP_PKEY_free
}

std::string Wallet::getPublicKey() const { return publicKey; }

std::string Wallet::signTransaction(const std::string& data) const {
    // Use EVP for signing
    EVP_MD_CTX* mdCtx = EVP_MD_CTX_new();
    if (!mdCtx) throw std::runtime_error("Failed to create EVP_MD_CTX");

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.length(), hash);

    if (EVP_DigestSignInit(mdCtx, nullptr, EVP_sha256(), nullptr, keyPair) <= 0) {
        EVP_MD_CTX_free(mdCtx);
        throw std::runtime_error("Failed to init signing");
    }

    size_t sigLen = 0;
    EVP_DigestSignUpdate(mdCtx, hash, SHA256_DIGEST_LENGTH);
    EVP_DigestSignFinal(mdCtx, nullptr, &sigLen); // Get signature length

    unsigned char* sig = static_cast<unsigned char*>(OPENSSL_malloc(sigLen));
    if (EVP_DigestSignFinal(mdCtx, sig, &sigLen) <= 0) {
        OPENSSL_free(sig);
        EVP_MD_CTX_free(mdCtx);
        throw std::runtime_error("Failed to sign");
    }

    std::string signature(reinterpret_cast<char*>(sig), sigLen);
    OPENSSL_free(sig);
    EVP_MD_CTX_free(mdCtx);
    return signature;
}