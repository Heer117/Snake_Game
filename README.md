# Two-Player Snake (Terminal Game)

A console-based, two-player Snake game written in C++ for Unix-like terminals. Both snakes race to eat the same food, growing and racking up score, until one crashes into a wall, itself, or the other snake.

## Features

- **Two independent snakes** on a shared board
  - Player 1: Arrow keys (Up / Down / Left / Right)
  - Player 2: `W` `A` `S` `D`
- **Shared food** — whichever snake reaches it first grows and scores a point
- **Independent scores** for each player, plus a running high score, shown live below the board
- **Collision detection** against walls, self, and the opposing snake — the game ends the moment either snake collides, and announces which player lost
- **Increasing difficulty** — the game speeds up slightly each time a snake eats
- **Restart/quit** — press `R` to restart or `Q` to quit, both mid-game and on the game-over screen
- Emoji-based rendering (👀 head, 🩷 body, 🪱 food, 🟫 border, ⬜ empty tile)

## Requirements

- A Unix-like OS (Linux/macOS) or WSL on Windows — the game uses `termios.h`, `unistd.h`, and `fcntl.h` for raw, non-blocking terminal input, which are POSIX-only
- `g++` (or another C++ compiler with C++11 support or later)
- A terminal that renders emoji correctly and is reasonably large (the board is 20×12 tiles plus borders and a status line)

## Build

```bash
g++ -std=c++11 -O2 snake.cpp -o snake
```

## Run

```bash
./snake
```

## Controls

| Key(s)              | Action                         |
| -------------------- | ------------------------------ |
| `↑` `↓` `←` `→`       | Move Player 1's snake          |
| `W` `A` `S` `D`       | Move Player 2's snake          |
| `R`                   | Restart the game               |
| `Q`                   | Quit                            |

Snakes cannot reverse directly into themselves (e.g. moving Right then instantly pressing Left is ignored).

## How it Plays

1. Both snakes start at the center of the board, three tiles long, facing right (Player 2 spawns a few rows below Player 1).
2. A single piece of food spawns on a random empty tile.
3. Each game tick, both snakes advance one tile in their current direction.
4. Whichever snake's head reaches the food first grows by one segment, earns a point, and a new food tile spawns. The game speeds up slightly.
5. The round ends immediately when either snake:
   - Goes out of bounds,
   - Runs into its own body, or
   - Runs into the other snake (including its head).
6. The losing player is announced along with final scores. Press `R` to play again or `Q` to exit.

## Project Structure

```
snake.cpp   — single-file implementation (game loop, input handling, rendering, collision logic)
```

Key components inside `snake.cpp`:

- **`Position`** — a simple `(x, y)` grid coordinate
- **`Snake`** — tracks a snake's body segments and direction; handles movement, growth, and self-collision
- **`Food`** — spawns on a random tile not occupied by either snake
- **`Game`** — owns both snakes, the food, scores, and the terminal I/O; drives the main loop (`run()`), input parsing (`processInput()`), per-tick logic (`update()`), and rendering (`drawBoard()`)

## Known Limitations

- Terminal-only, POSIX-specific input handling — will not build as-is on native Windows (no WSL).
- Board size, tile emojis, and speed curve are hard-coded constants at the top of the file.
- No persistent high-score storage between runs — high score resets each time the program is launched.

## License

Add a license of your choice here (e.g. MIT).
