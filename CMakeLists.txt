cmake_minimum_required(VERSION 3.21)
project(CS163_DataVis)

set(CMAKE_CXX_STANDARD 17)

# --- 1. Fetch Raylib Automatically ---
include(FetchContent)
FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.0  # We specify version 5.0 for stability
)
FetchContent_MakeAvailable(raylib)

# --- 2. Add Your Source Files ---
# As you create new .cpp files, you MUST add them to this list!
add_executable(CS163_DataVis
        main.cpp
        # Later you will add:
        # src/Visualizer.cpp
        # src/DoublyLinkedList.cpp
)

# --- 3. Link Raylib to your app ---
target_link_libraries(CS163_DataVis raylib)