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
        src/States/MenuState.cpp
        src/States/DLLState.cpp
        src/GUI/Toolbar.cpp
        src/App.cpp
        src/States/SelectState.cpp
        include/GUI/Toolbar.h
        include/States/MenuState.h
        include/States/DLLState.h
        include/States/SelectState.h
        include/States/State.h
        include/App.h
        include/Renderers/DLLRenderer.h
        src/Renderers/DLLRenderer.cpp
        src/tinyfiledialogs.c
        include/tinyfiledialogs.h
        src/DataStructures/MinHeap.cpp
        src/Renderers/MinHeapRenderer.cpp
        src/States/MinHeapState.cpp
        include/DataStructures/MinHeap.h
        include/Renderers/MinHeapRenderer.h
        include/States/MinHeapState.h
        include/DataStructures/AVLTree.h
        include/Renderers/AVLRenderer.h
        include/States/AVLState.h
        src/DataStructures/AVLTree.cpp
        src/Renderers/AVLRenderer.cpp
        src/States/AVLState.cpp
        include/DataStructures/Trie.h
        include/Renderers/TrieRenderer.h
        src/DataStructures/Trie.cpp
        include/States/TrieState.h
        src/Renderers/TrieRenderer.cpp
        src/States/TrieState.cpp
        include/DataStructures/Graph.h
        src/DataStructures/Graph.cpp
        include/Renderers/GraphRenderer.h
        src/Renderers/GraphRenderer.cpp
        include/States/GraphState.h
        src/States/GraphState.cpp
        include/UIHelper.h
)

# --- 3. Link Raylib to your app ---
target_link_libraries(CS163_DataVis raylib)