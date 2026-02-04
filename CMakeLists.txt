cmake_minimum_required(VERSION 3.25) # Bumped version for safety
project(CS163_DataVis)

set(CMAKE_CXX_STANDARD 17)

# --- 1. Fetch Raylib Automatically ---
include(FetchContent)
FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG master  # Changed from 5.0 to master to fix the version error
)
FetchContent_MakeAvailable(raylib)
# Tell CMake to look in the 'include' folder for headers
include_directories(include)

# --- 2. Add Your Source Files ---
add_executable(CS163_DataVis
        src/main.cpp
        include/Global.h
        include/Visualizer.h
        src/Visualizer.cpp
        include/VisualNode.h
        include/DataStructures/DoublyLinkedList.h
        src/DataStructures/DoublyLinkedList.cpp
)

# --- 3. Link Raylib to your app ---
target_link_libraries(CS163_DataVis raylib)