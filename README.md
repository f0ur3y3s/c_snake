# Cnake - A Snake clone in C

<div align="center">
<img src=https://github.com/incub4t0r/c_snake/blob/master/img/c_snake.png width=300>
</div>

- [Cnake - A Snake clone in C](#cnake---a-snake-clone-in-c)
  - [Introduction](#introduction)
  - [Technical Details](#technical-details)
  - [Known issues](#known-issues)
  - [Building](#building)
  - [Running](#running)

## Introduction

Cnake is a simple Snake clone written in C. Cnake does NOT use any external libraries for rendering, and instead uses the CLI to draw the game. 

Control is done using the WASD keys.

## Technical Details

- The snake is represented as a linked list of segments. 
- The entire screen is only rendered once at the start of the game.
  - The rest of the entities, such as food and the snake are drawn individually to the screen.
- Uses console codes to move the cursor and clear the screen.

## Known issues

- Self-collision is not working as intended.
- Border collision is not working as intended.
- Timestep is not implemented to run at a stable FPS, instead the game runs as fast as possible.

## Building

To build the project, run `make`.

## Running

To run the project, run `./bin/main`.
