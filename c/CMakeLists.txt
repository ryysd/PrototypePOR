PROJECT(PORModule)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

set(CMAKE_CXX_COMPILER g++)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wextra -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3")

include_directories($ENV{GTEST_DIR}/include)
# link_directories(lib) 

# set_target_properties(hellodemo LINK_FLAGS -lc)
# set_target_properties(hellodemo COMPILE_FLAGS -Wall)

#add_definitions(-DDEBUG)

ADD_EXECUTABLE(por_module ${PROJECT_SOURCE_DIR}/src/main.cc)
ADD_EXECUTABLE(por_module_test ${PROJECT_SOURCE_DIR}/test/test_main.cc ${PROJECT_SOURCE_DIR}/test/test.cc)

target_link_libraries(por_module_test $ENV{GTEST_DIR}/build/libgtest.a $ENV{GTEST_DIR}/build/libgtest_main.a) 
