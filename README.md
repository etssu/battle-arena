# Battle Arena Game
The final version of the game was completed during the last week of the Summer Semester 24/25 as part of our coursework. The core logic was implemented in Assignment 4, and the graphics were developed in Assignment 6. 
## How to Build and Run the Project

Make sure you have gcc and the ncurses library installed.

### ✅ Build the project:
In the project directory, run: "make". This will compile all .c files and generate an executable called z6.

### ▶️ Run the game: "./z6"

🧹 Clean compiled files: "make clean". This removes all object files and the executable.

#### PROJECT STRUCTURE
The project is organized into four main C files:

 - data.c
Contains all the data structures and initialization logic. It defines the units, items used throughout the program.

 - graphics.c
Responsible for the visual representation of the game. It uses the ncurses library to draw the battlefield, highlight units, and display both armies in styled boxes.

 - logic.c
Handles the main game mechanics such as unit movement, attacking, turn management, and game rules (e.g., unit selection, surrender logic).

 - main.c
The entry point of the application. It connects all other modules together, initializes the game, and starts the main game loop.

### 🎮 Controls
Once the game is running:

 - Use W A S D to move the selection cursor.

 - Press ENTER to move the selected unit.

 - Press F to attack.

 - Press X to surrender.

 - Units are color-coded:
    🔵 Blue – selected unit
    🔴 Red – enemy in range
    🟡 Yellow – area-of-effect range

Good luck :)

Note: Occasionally, a segmentation fault may occur at the start. If this happens, simply restart the game — it should work fine on the next run.
