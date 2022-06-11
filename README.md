# 2d-particle-sim
2D particle simulator and player in C using OpenGL and GLFW.
The goal of this project is to create a fast, easy to use and fully customizable point particle simulator in 2D for linux.

## System Requirements
To compile and run you need the following:
- A GPU with **OpenGL 4.5** (or higher) support;
- **G++** and **GLFW** installed on your machine.

## Compile && Run
Do `make` in each the sim/ subdirectory to compile the simulator or in the player/ subdirectory to compile the simulation player.
- sim: `./simulator [filename] [particle_count] [frames]`
- player: `./player [filename] [frames/s]`
