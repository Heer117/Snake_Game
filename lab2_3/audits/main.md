# Code Smells Audit: main

| Category | Smell | Severity | Confidence | Location | Description |
|----------|-------|----------|------------|----------|-------------|
| Bloaters | Large Class | HIGH | C3 | `snake.cpp:88` | `Game` class violates SRP by mixing game state, game loop, ANSI rendering, and POSIX terminal I/O. |
| Dispensables | Dead Code | LOW | C4 | `snake.cpp:48` | `Snake::setDirFromKey` is never called. Input mapping is handled in `Game::processInput`. |
| Change Preventers | Divergent Change | HIGH | C3 | `snake.cpp:160` | `Game::processInput` hardcodes snake control keys directly. Adding a player requires changing the core game class. |
| Change Preventers | Shotgun Surgery | HIGH | C3 | `snake.cpp:89` | `snake` is a single hardcoded member. Adding another snake requires duplicating fields and duplicating logic in `update()`, `drawBoard()`, and `processInput()`. |
| Couplers | Inappropriate Intimacy | MEDIUM | C3 | `snake.cpp:105` | `Game` directly manipulates POSIX `termios` and `fcntl` (lines 105-114). The OS terminal dependency should be extracted. |
