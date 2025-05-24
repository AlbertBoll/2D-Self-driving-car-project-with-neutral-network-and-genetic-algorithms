cmake_minimum_required(VERSION 3.10.0)
project(SelfDrivingGeneticAlgorithm VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

set(TARGET ${PROJECT_NAME}-${CMAKE_SYSTEM_NAME})

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_ttf REQUIRED)
find_package(SDL3_mixer REQUIRED)
find_package(glm REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/entt/single_include)
add_subdirectory(${CMAKE_SOURCE_DIR}/entt/single_include/entt) 
include_directories(${CMAKE_SOURCE_DIR}/json-3.12.0/single_include/nlohmann)
add_subdirectory(${CMAKE_SOURCE_DIR}/json-3.12.0/single_include/nlohmann)           

add_executable(${TARGET} main.cpp
                         src/SDLWindow.cpp
                         src/SDLRenderer2D.cpp
                         src/AssetManager.cpp
                         src/UUID.cpp
                         src/Scene.cpp
                         src/Entity.cpp
                         src/Component.cpp
                         src/EventManager.cpp
                         src/Simulator.cpp
                         GeneticAlgorithmSimulator.cpp)

target_link_libraries(${TARGET}
                      ${SDL3_LIBRARIES}
                      SDL3_image::SDL3_image
                      SDL3_ttf::SDL3_ttf
                      SDL3_mixer::SDL3_mixer
                      glm::glm
                      entt
                      json
                     )