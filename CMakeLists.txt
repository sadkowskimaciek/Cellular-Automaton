cmake_minimum_required(VERSION 3.16)
project(model_dysk_4)

set(CMAKE_CXX_STANDARD 17)

# PRZENIEŚ TO NA GÓRĘ - przed FetchContent!
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif()

include(FetchContent)
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.1
)
FetchContent_MakeAvailable(SFML)

add_executable(model_dysk_4 main.cpp)
target_link_libraries(model_dysk_4 sfml-graphics sfml-window sfml-system)

# DODAJ to na końcu
if(WIN32)
    target_link_libraries(model_dysk_4 -static-libgcc -static-libstdc++ -static)
endif()