# Ubiquitous Language

## Game Entities

| Term | Definition | Aliases to avoid | In code |
| ----------- | ------------------------------------------------------- | --------------------- | -------------------------- |
| **Snake** | The player-controlled entity comprising a head and body segments that moves continuously | Player character | `Snake` — `snake.cpp:24` |
| **Food** | An item appearing on the board that the snake can eat to increase its score and length | Apple, pellet, fruit | `Food` — `snake.cpp:76` |
| **Position** | The (x, y) grid coordinates of an entity or tile on the board | Coordinate, location | `Position` — `snake.cpp:19` |
| **Direction** | The orientation (UP, DOWN, LEFT, RIGHT) the snake is moving | Heading | `Dir` — `snake.cpp:26` |

## State and Environment

| Term | Definition | Aliases to avoid | In code |
| ----------- | ------------------------------------------------------- | --------------------- | -------------------------- |
| **Game** | The overarching session and state orchestrator | Session, engine | `Game` — `snake.cpp:88` |
| **Board** | The rectangular grid area where the gameplay occurs | Grid, map, arena | Implicit (`WIDTH`/`HEIGHT`/`drawBoard`) — `snake.cpp:141` ⚠ |
| **Score** | The number of food items the player has collected in the current game | Points | `score` — `snake.cpp:91` |
| **High Score** | The maximum score achieved across all played games in the current session | Record, best score | `highScore` — `snake.cpp:92` |
| **Tile** | A single unoccupied cell on the board | Blank, empty space | `TILE` — `snake.cpp:13` |
| **Border** | The impassable boundary enclosing the board | Wall | `BORDER` — `snake.cpp:14` |

## Relationships

- A **Game** contains exactly one **Snake**, one **Food**, and one **Board**.
- A **Snake** occupies multiple **Positions** on the **Board**.
- A **Food** occupies exactly one **Position** on the **Board**.
- When a **Snake** moves onto the **Position** of the **Food**, the **Score** increases and the **Snake** grows.
- When a **Snake** moves onto a **Border** or its own body's **Position**, the **Game** ends.

## Example dialogue

> **Dev:** "What happens if the **Snake** collides with the **Border**?"
> **Domain expert:** "The **Game** enters the game-over state immediately."
> **Dev:** "Does the **High Score** update?"
> **Domain expert:** "Yes, if the current **Score** is strictly greater than the **High Score**."
> **Dev:** "And how do we handle the **Food** when a new **Game** starts?"
> **Domain expert:** "The **Food** must spawn at a random **Position** on the **Board** that is not currently occupied by the **Snake**."

## Flagged ambiguities

- The term "Board" is used conceptually to describe the play area, but there is no explicitly defined `Board` struct or class in the code; it is implicitly managed by `WIDTH`, `HEIGHT`, and the `drawBoard` function.

## Code drift

| Canonical term | Called in code | Location              | Note                                                          |
| -------------- | -------------- | --------------------- | ------------------------------------------------------------- |
| **Board**    | *(absent)*     | `snake.cpp:141`   | Discussed conceptually and drawn via `drawBoard`, but no entity or state represents it structurally |
