# 3D Tower Defense Game



A 3D tower defense game built with C++ and OpenGL/GLUT. It features a custom particle system, dynamic lighting, and a modern UI.

![C++](https://img.shields.io/badge/C++-11-orange)
![Platform](https://img.shields.io/badge/Platform-macOS-lightgrey)
![OpenGL](https://img.shields.io/badge/OpenGL-3D-blue)

## Features

### Particle System
* Can handle 2000+ simultaneous particles with basic physics
* Explosion effects upon enemy defeat
* Muzzle flashes
* Bullet trails
* Gravity and velocity calculations

### Graphics & Effects
* Dynamic lighting (ambient, diffuse, specular)
* Glowing elements on tower cores
* Screen shake for impact feedback
* Animated terrain grid
* Fog system

### UI/UX
* Semi-transparent panels
* Floating damage numbers
* Combo counter
* Animated welcome screen
* On-screen particle counter for performance monitoring

### Gameplay
* 3 difficulty levels (Easy, Medium, Hard)
* 3 tower types:
  * **SLOW Tower** - Energy spire
  * **BASIC Tower** - Standard cannon
  * **AREA Tower** - Area of effect defense
* 3 enemy types:
  * **NORMAL** - Standard drones
  * **FAST** - Speedy units
  * **TANK** - High health units
* Wave-based progression
* Resource management

## Version Differences

### Enhanced Version (`main_enhanced.cpp`)
This version includes the visual upgrades:
- Particle explosions and trails
- Muzzle flashes
- Floating damage numbers
- Screen shake
- Combo system

### Original Version (`main.cpp`)
The base game without the additional particle effects or advanced UI features.

## Installation

### Prerequisites
- macOS with Xcode Command Line Tools
- OpenGL and GLUT frameworks (included with macOS)
- C++11 compatible compiler

### Build

```bash
# Clone the repository
git clone https://github.com/supreetpatil79/3D-Tower-Defense---Enhanced-Version-Walkthrough.git
cd p1

# Compile original version
g++ -std=c++11 -O2 main.cpp -framework OpenGL -framework GLUT -o main

# Compile enhanced version
g++ -std=c++11 -O2 main_enhanced.cpp -framework OpenGL -framework GLUT -o main_enhanced
```

## Usage

### Run the Game

```bash
# Run original version
./main

# Run enhanced version (recommended)
./main_enhanced
```

### Controls

#### Menu Navigation
- **I** - Instructions
- **S** - Start Game
- **N** - Next page / Continue level
- **B** - Back page
- **M** - Main menu
- **R** - Restart (when game over)

#### Gameplay
- **Arrow Keys** - Move grid selection
- **P** - Place Slow Tower (10 resources) - Blue
- **S** - Place Basic Tower (20 resources) - Red
- **A** - Place Area Tower (30 resources) - Green
- **W/A/S/D** - Rotate camera
- **Q/E** - Zoom in/out

## Technical Details

### Performance
- **Target FPS**: 60
- **Frame Time**: 16ms
- **Max Particles**: 2000
- **Typical Particle Count**: 200-500 during combat
- **Window Size**: 1200x800 (enhanced) / 800x600 (original)

### Code Statistics
- **Enhanced Version**: 1,318 lines
- **Original Version**: 928 lines
- **Language**: C++11
- **Graphics API**: OpenGL with GLUT

## Architecture Details
- Custom particle system
- 3D-to-2D projection mapping for UI
- Multi-layer rendering
- Dynamic lighting model
- Vector-based data structures

## Color Palette

**Cyberpunk Theme:**
- Background: Deep blue-black `(0.02, 0.02, 0.1)`
- Accents: Cyan `(0.3, 0.7, 1.0)`
- Highlights: Bright cyan `(0.5, 0.9, 1.0)`
- Warnings: Red `(1.0, 0.3, 0.3)`
- Success: Green `(0.2, 1.0, 0.5)`
- Special: Magenta `(1.0, 0.5, 1.0)`

## Development

### Project Structure
```
p1/
├── main.cpp              # Original version
├── main_enhanced.cpp     # Enhanced version with particles & effects
├── .gitignore           # Git ignore rules
└── README.md            # This file
```

### Building from Source
The game uses standard OpenGL/GLUT libraries available on macOS. No external dependencies required.

## License

This project is open source and available for educational purposes.

## Credits

- **Developer**: Supreet Patil
- **Graphics**: OpenGL & GLUT
- **Particle System**: Custom implementation
- **UI Design**: Glassmorphism aesthetic
- **Game Design**: Classic tower defense mechanics

## Planned Enhancements
- Sound effects and music
- New tower variations
- Additional enemy types
- Level editor
  