# Code Smells Audit: lab1-head

| Category | Smell | Severity | Confidence | Location | Description |
|----------|-------|----------|------------|----------|-------------|
| Bloaters | Large Class | HIGH | C3 | `temp_lab1_snake.cpp:96` | `Game` class has grown to include variables for a second player (`snake2`, `score2`), increasing its size and responsibilities further. |
| Dispensables | Duplicate Code | HIGH | C4 | `temp_lab1_snake.cpp:189` | `Game::update` duplicates movement, boundary collision, and food collection logic entirely for `snake` and `snake2`. |
| Dispensables | Duplicate Code | HIGH | C4 | `temp_lab1_snake.cpp:152` | `Game::drawBoard` checks rendering for `snake` and `snake2` separately in the same loop. |
| Change Preventers | Shotgun Surgery | HIGH | C3 | `temp_lab1_snake.cpp:96` | Adding a third player would require copying fields, copying `update()` blocks, adding input mapping in `processInput()`, and updating `drawBoard()` and `restart()`. |
| Bloaters | Long Method | HIGH | C3 | `temp_lab1_snake.cpp:189` | `Game::update` is doing too much: moving two snakes, checking boundary collisions, checking snake-snake collisions, checking self collisions, eating food, and adjusting game speed. |
