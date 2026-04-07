# 🚀 CS Visualizer: Interactive Data Structures & Algorithms

An interactive, highly polished desktop application for visualizing complex data structures and algorithms. Built from scratch with **C++** and **Raylib**, this tool brings abstract computer science concepts to life with smooth animations, step-by-step execution, and a beautiful, modern UI.

![C++](https://img.shields.io/badge/C++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Raylib](https://img.shields.io/badge/raylib-000000.svg?style=for-the-badge&logo=raylib&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white)

## ✨ Key Features

* **Modern "Synthwave" UI:** A beautifully responsive interface featuring custom geometric icons, dynamic Dark/Light modes, and pixel-perfect typography powered by the **JetBrains Mono** font.
* **Smooth Animation Engine:** Uses Linear Interpolation (LERP) and smoothstep math to physically glide nodes and swing connection lines during state changes.
* **Interactive Graph Physics:** A full sandbox mode where users can click, drag, and pin vertices in real-time, coupled with a live multi-line text parser for adjacency lists.
* **Robust Playback System:** Full control over algorithm execution. Pause, play, step forward/backward, and scrub to the start/end of the animation with adjustable playback speeds.
* **Live Pseudocode Tracking:** Watch the algorithm's logic execute line-by-line in a dedicated tracking panel alongside the visualization.
* **Flexible Inputs:** Support for manual text entry, randomized data generation, and loading data sets directly from `.txt` or `.csv` files.

## 🧮 Supported Data Structures

1. **Doubly Linked List (DLL):** Interactive insertion, deletion, updating, and searching at the Head, Tail, or specific indices.
2. **AVL Balanced Tree:** Visualizes automatic self-balancing with Left and Right rotations, alongside standard BST operations.
3. **Min-Heap:** Step-by-step visualizations of `extractMin`, heapify operations, and priority queue management alongside array representations.
4. **Prefix Trie:** Interactive string insertion, deletion, and prefix-sharing, featuring specialized "End of Word" node rendering.
5. **Graph Algorithms:** 
   * Sandbox creation with directed/undirected options and edge weights.
   * **Dijkstra's Algorithm:** Shortest path finding with live tracking tables.
   * **Kruskal's Algorithm:** Minimum Spanning Tree (MST) generation.

## 🛠️ Installation & Build Instructions

This project uses **CMake** as its build system and requires **Raylib**.

### Prerequisites
* A C++17 compatible compiler (GCC, Clang, or MSVC)
* CMake (3.15 or higher)
* [Raylib](https://www.raylib.com/) installed globally or configured via CMake FetchContent.

### Building the Project
1. Clone the repository:
   ```bash
   git clone https://github.com/Aurroraa/CS163_DataVis.git
   cd CS163_DataVis
   ```
2. Generate build files and compile:
   ```bash
   mkdir build && cd build
   cmake ..
   cmake --build .
   ```
3. **⚠️ Crucial Asset Setup:** Ensure that the `assets` folder (containing the `JetBrainsMono` fonts) is located in the exact same directory as your compiled executable.
   * *If running via CLion or VSCode, you may need to manually copy the `assets` folder into your `cmake-build-debug` or `build` directory.*
4. Run the executable:
   ```bash
   ./CS163_DataVis
   ```
