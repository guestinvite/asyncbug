cmake_minimum_required(VERSION 2.18)

option(HUNTER_ENABLED "Enable Hunter package manager" OFF)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(HunterGate)

HunterGate(
        URL "https://github.com/ruslo/hunter/archive/v0.23.11.tar.gz"
        SHA1 "b732407d782459f89950ab52bc0ff9612f3e4be2"
)

project(mcve)
set(CMAKE_CXX_STANDARD 14)


file(GLOB_RECURSE SRC src/*)
file(GLOB_RECURSE INC include/*)
add_executable(${PROJECT_NAME} ${INC} ${SRC})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (HUNTER_ENABLED)
    hunter_add_package(Boost COMPONENTS system regex thread)
    find_package(Boost CONFIG REQUIRED COMPONENTS system regex thread)
    target_link_libraries(${PROJECT_NAME} PUBLIC Boost::system Boost::regex Boost::thread)
else ()

    set(BOOST_MAIN_REPO "C:/WORK/Libs/Boost-1.61.0-msvc12-x86_64/Debug")

    link_directories(${BOOST_MAIN_REPO}/lib)

    set(BOOST_INC_DIRS ${BOOST_MAIN_REPO}/include)

    target_include_directories(${PROJECT_NAME} PUBLIC include ${BOOST_INC_DIRS})


    target_link_libraries(${PROJECT_NAME} PUBLIC
            libboost_system-vc120-mt-gd-1_61
            libboost_regex-vc120-mt-gd-1_61
            )


endif ()
