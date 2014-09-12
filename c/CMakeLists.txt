# set(CMAKE_VERBOSE_MAKEFILE 1)

PROJECT(PORModule)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

set(MODULE_NAME "por_module")
set(PETRINET_MODULE_NAME "petrinet")
set(TEST_MODULE_NAME "por_module_test")

set(CMAKE_CXX_COMPILER g++)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wextra -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3")

include_directories($ENV{GTEST_DIR}/include)
# link_directories(lib) 

# set_target_properties(hellodemo LINK_FLAGS -lc)
# set_target_properties(hellodemo COMPILE_FLAGS -Wall)

#add_definitions(-DDEBUG)

add_executable(${MODULE_NAME} ${PROJECT_SOURCE_DIR}/src/por_module/main.cc)
add_executable(${PETRINET_MODULE_NAME} ${PROJECT_SOURCE_DIR}/src/petrinet/main.cc)

file(GLOB CC_SOURCES ${PROJECT_SOURCE_DIR}/test/*.cc)
add_executable(${TEST_MODULE_NAME} ${CC_SOURCES})

target_link_libraries(${TEST_MODULE_NAME} $ENV{GTEST_DIR}/build/libgtest.a $ENV{GTEST_DIR}/build/libgtest_main.a) 
