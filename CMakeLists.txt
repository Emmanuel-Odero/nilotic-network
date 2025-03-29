# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(NiloticNetwork)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
# Crow headers should be in /opt/homebrew/include (Homebrew default)

# Source files
add_executable(nilotic
    src/main.cpp
    src/blockchain.cpp
    src/block.cpp
    src/transaction.cpp
    src/wallet.cpp
    src/dao.cpp
    src/smart_contract.cpp
)

# Link libraries (Crow is header-only, no linking needed)
target_link_libraries(nilotic OpenSSL::SSL OpenSSL::Crypto Boost::system)