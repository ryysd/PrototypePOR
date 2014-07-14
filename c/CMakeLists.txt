PROJECT(PORModule)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -Wextra -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3")

include_directories($ENV{GTEST_DIR}/include)
# link_directories(lib) 

# set_target_properties(hellodemo LINK_FLAGS -lc)
# set_target_properties(hellodemo COMPILE_FLAGS -Wall)

#add_definitions(-DDEBUG)

ADD_EXECUTABLE(por_module ${PROJECT_SOURCE_DIR}/src/main.cc)

target_link_libraries(por_module $ENV{GTEST_DIR}/build/libgtest.a $ENV{GTEST_DIR}/build/libgtest_main.a) 
