# Battle Box Shooter

A lightweight 2D arena shooter written in **C++** using **SFML 2.6.2** for graphics/audio and **Box2D 3.1.1** for physics.  
Designed to run on **Windows 10/11 (x64)** with Visual Studio 2022.

---

## 📁 Folder Setup

Recommended structure (place at root of **C:\\**):

C:\SFML-2.6.2\
C:\box2d-3.1.1\
C:\GameProject\      ← this repository

If placed elsewhere, update the **Include** and **Library** directories in your project settings.

---

## ⚙️ Requirements

- Visual Studio 2022 (Desktop C++ workload)
- SFML **2.6.2**
- Box2D **3.1.1**
- Windows 10/11 x64

---

## 🧩 Configuration

### Include directories
C:\SFML-2.6.2\include  
C:\box2d-3.1.1\include

### Library directories
C:\SFML-2.6.2\lib  
C:\box2d-3.1.1\lib

### Additional dependencies
sfml-graphics.lib  
sfml-window.lib  
sfml-system.lib  
sfml-audio.lib  
box2d.lib

Copy all **SFML DLLs** from `C:\SFML-2.6.2\bin` into the output folder with your built `.exe`.

---

## 🧱 Building the Project

### Option 1 – Visual Studio
1. Open the solution in `GameProject/`.
2. Set configuration to **x64** and choose **Release** or **Debug**.
3. Build and run.

### Option 2 – CMake
cmake -S . -B build -A x64 -DSFML_ROOT=C:/SFML-2.6.2 -DBOX2D_ROOT=C:/box2d-3.1.1  
cmake --build build --config Release

---

## 🕹️ Controls

| Key | Action |
|-----|--------|
| **Enter** | Start game from title screen |
| **Left / Right** | Move player |
| **Space** | Jump (double jump) |
| **S** | Shoot |
| **Ctrl + S** | Save progress (score + alive enemies) and exit |
| **Ctrl + D** | Toggle high score view |
| **O** | Resume saved game |
| **R** | Restart after win/loss |
| **Esc** | Quit |

---

## 💾 Save and High Score

The game stores progress in `savegame.txt` (created automatically).

Example file format:  
resumeScore resumeEnemies highScore

Older two-value saves are still compatible.  
Saving (Ctrl+S) updates the file and exits safely.  
High score automatically updates after every round.

---

## 🧠 Box2D 3.1.1 Notes

This project uses Box2D **3.x** with the handle-based API (`b2WorldId`, `b2BodyId`, etc.).  
Older 2.x tutorials using pointers will **not** match this version.

---

## 🧩 Troubleshooting

- **Missing DLLs:** Copy all `.dll` files from `C:\SFML-2.6.2\bin` next to the `.exe`.  
- **Link errors:** Ensure x64 build and correct include/lib paths.  
- **No sound or font:** Verify file names and paths (e.g., `arial.ttf`, sound files).  
- **Game doesn’t save:** Check write permissions in the build folder.

---

## 🎨 Assets

- Font: `C:\Windows\Fonts\arial.ttf` (default Windows font)  
- Audio: Royalty-free effects from **Pixabay**  
- Save file: `savegame.txt` generated automatically

---

## 📄 Project Structure

GameProject/  
│  
├── GameProject.cpp      → Main game loop and logic  
├── GameProject.hpp      → Structs, constants, helper functions  
├── Assets/              → (optional) sound and image files  
└── savegame.txt         → Auto-created save file

---

## 🪶 Credits

**Developers:** Group AM  
Abdullah Arshad  
Mirza Sikandar Tariq  
Rao Muhammad Safee  
Muhammad Zohaib Azam  

**Libraries:** [SFML 2.6.2](https://www.sfml-dev.org) | [Box2D 3.1.1](https://box2d.org)  
**Audio:** [Pixabay Sounds](https://pixabay.com/sound-effects/)  
**Font:** Arial (Windows system font)

---

## 📜 License

Released under the MIT License (or your chosen license).
