 # 🎳 2D Bowling Game

A fun and interactive 2D bowling  game built in **C** using the [Raylib](https://www.raylib.com/) library. This game features multiple pin arrangements, a power bar,sound effect and a final score screen.

## 🚀 Features

- 🎯 Three unique pin patterns:
  - Triangle
  - random
  - Circle
- 🎳 Physics-based ball movement with curve or straight throws
- 🔊 Sound effects on pin collisions
- 💥 Realistic falling pin animation
- ⚡ Power meter for charge-based throws
- 🧮 Score tracking across 3 rounds
- 🖥️ Simple and clean graphics with texture support

## 🕹️ Controls

| Key               | Action                           |
|----------------   |----------------------------------|
| `←` / `→`         | Adjust throw angle               |
| `SPACE` (hold)    | Charge power                     |
| `SPACE` (release) | Throw the ball                   |
| `L`               | Toggle straight/curve throw      |

## 📸 Screenshot
 It in shown in the present folder as Screenshot 2025-08-05 175434.png.
     

## 🧱 Built With

- [Raylib](https://www.raylib.com/) – A simple and easy-to-use library to enjoy game programming

## 🛠️ Compilation

Make sure you have Raylib installed.

in the vscode:
gcc main.c -o bowling.exe -lraylib -lopengl32 -lgdi32 -lwinmm
bowling.exe


### On Linux/Mac:

```bash
gcc main.c -o bowling -lraylib -lm -ldl -lpthread
./bowling
