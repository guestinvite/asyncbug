cmake_minimum_required(VERSION 2.18)

project(mcve)

file( GLOB_RECURSE SRC src/*)
file( GLOB_RECURSE INC include/*)

set(BOOST_MAIN_REPO "C:/WORK/Libs/Boost-1.61.0-msvc12-x86_64/Debug")

link_directories(${BOOST_MAIN_REPO}/lib)
add_executable(${PROJECT_NAME} ${INC} ${SRC})

set (BOOST_INC_DIRS ${BOOST_MAIN_REPO}/include)
target_include_directories(${PROJECT_NAME} PUBLIC include ${BOOST_INC_DIRS})

target_link_libraries(${PROJECT_NAME} PUBLIC
 libboost_system-vc120-mt-gd-1_61
 libboost_regex-vc120-mt-gd-1_61
 )


