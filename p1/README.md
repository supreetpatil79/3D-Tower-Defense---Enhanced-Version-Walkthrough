# 3D Tower Defense Game
**Developed by** [Supreet Patil](https://github.com/supreetpatil79)

A graphics-heavy 3D tower defense game built with OpenGL/GLUT featuring advanced particle systems, modern UI/UX, and stunning visual effects.

![Game Screenshot](https://img.shields.io/badge/OpenGL-3D%20Graphics-blue)
![C++](https://img.shields.io/badge/C++-11-orange)
![Platform](https://img.shields.io/badge/Platform-macOS-lightgrey)

## Features

### 🎆 Advanced Particle System
- **2000+ simultaneous particles** with physics simulation
- Spectacular explosions (50+ particles per enemy death)
- Muzzle flashes when towers shoot
- Glowing bullet trails
- Realistic gravity and velocity-based movement

### 💡 Graphics & Visual Effects
- **Dynamic lighting** with enhanced ambient, diffuse, and specular
- **Glowing tower cores** with dual-layer pulsing effects
- **Rotating energy rings** on towers
- **Screen shake** on explosions for impact
- **Animated terrain grid** with pulsing cyberpunk aesthetic
- **Enhanced fog system** for atmospheric depth

### 🎨 Modern UI/UX
- **Glassmorphism panels** with semi-transparent backgrounds
- **Floating damage numbers** in 3D world space
- **Combo counter system** tracking consecutive kills
- **Animated welcome screen** with pulsing elements
- **Professional HUD** with color-coded information
- **Real-time particle counter** for performance monitoring

### 🎮 Gameplay
- **3 difficulty levels** (Easy, Medium, Hard)
- **3 tower types** with unique abilities:
  - **SLOW Tower** (Blue) - Energy spire with rotating rings
  - **BASIC Tower** (Red) - Industrial cannon with glowing core
  - **AREA Tower** (Green) - Heavy defense hub with satellites
- **3 enemy types** with distinct behaviors:
  - **NORMAL** - Yellow drones with orbital rings
  - **FAST** - Green needles with speed trails
  - **TANK** - Purple mechs with heavy armor
- **Wave-based progression** with increasing difficulty
- **Resource management** system

## Screenshots

### Enhanced Version Features
- ✨ Particle explosions
- 🔫 Muzzle flashes
- 💫 Glowing trails
- 📊 Floating damage numbers
- 📈 Combo system
- 💥 Screen shake
- 🌟 Pulsing effects

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

### Architecture
- Custom particle system with physics
- 3D-to-2D projection for floating text
- Multi-layer rendering with transparency
- Dynamic lighting system
- Smooth interpolation for animations
- Efficient vector-based data structures

## Versions

### Original (`main.cpp`)
- Basic 3D tower defense gameplay
- Simple graphics and UI
- Core game mechanics

### Enhanced (`main_enhanced.cpp`)
- **Particle system** - Explosions, trails, muzzle flashes
- **Advanced graphics** - Glowing effects, dynamic lighting
- **Modern UI** - Glassmorphism, floating text, combos
- **Visual polish** - Screen shake, animations, feedback
- **Performance optimized** - 60 FPS with 2000 particles

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

## Future Enhancements

Potential additions:
- Sound effects and music
- More tower types and upgrades
- Additional enemy varieties
- Power-ups and special abilities
- Multiplayer support
- Level editor

---

**Enjoy defending your base with spectacular visual effects!** 🎮✨
