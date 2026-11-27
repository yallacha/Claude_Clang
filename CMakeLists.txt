cmake_minimum_required(VERSION 3.20)
project(Claude_Clang LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra)

# 프로덕션 코드: src/*.c (include/ 는 공개 헤더)
file(GLOB_RECURSE PROD_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.c)
if(PROD_SOURCES)
    add_library(claude_clang STATIC ${PROD_SOURCES})
    target_include_directories(claude_clang PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)
endif()

# 테스트는 ci/quality.sh (LLVM 커버리지 포함)로 실행한다. 단위/통합/TDD 테스트는 분리된 디렉터리를 사용한다:
#   tests/tdd, tests/unit, tests/integration, tests/system
enable_testing()
