// Minimal test harness (no external framework).
// Includes the production code DIRECTLY from the repository root.
// No production file is modified, moved, copied or refactored.

#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <sstream>

#define main main_orig   // production main() is never called
#include "../../snake.cpp"
#undef main

static int failures = 0;
static int checks = 0;

#define CHECK(...)                                                          \
    do {                             \
        ++checks;                                                      \
        if (!(__VA_ARGS__)) {                                          \
            ++failures;                                                \
            std::printf("FAIL %s:%d  %s\n", __FILE__, __LINE__,        \
                        #__VA_ARGS__);                                 \
        }                                                              \
    } while (0)

// ------- Rule 1: moves one cell per tick in the current direction -------
static void test_rule1_moves_one_cell_per_tick() {
    Snake s;                       // starts at (10,6) heading RIGHT
    Position before = s.head();
    s.move();
    CHECK(s.head().x == before.x + 1);
    CHECK(s.head().y == before.y);
    CHECK((int)s.getBody().size() == 3);   // length unchanged when not growing

    s.setDir(Snake::DOWN);
    s.move();
    CHECK(s.head().x == before.x + 1);
    CHECK(s.head().y == before.y + 1);
}

// ------- Rule 2 (reversal half only): cannot reverse directly -------
// NOTE: the arrow-key half of Rule 2 is NOT covered here (input path is
// unreachable without changing production code; see Part B report).
static void test_rule2_no_direct_reversal() {
    Snake s;                       // heading RIGHT
    s.setDirFromKey('a');          // direct reverse requested -> ignored
    CHECK(s.head() == Position{10, 6});
    s.move();
    CHECK(s.head() == Position{11, 6});   // still moved RIGHT
    s.setDir(Snake::UP);
    s.move();
    CHECK(s.head() == Position{11, 5});
    s.setDirFromKey('s');          // direct reverse of UP requested -> ignored
    s.move();
    CHECK(s.head() == Position{11, 4});   // still moved UP
}

// ------- Rule 4 (Part D): wall hit ends the game, through the real run() loop -------
static size_t countOccurrences(const std::string &text, const std::string &needle) {
    size_t count = 0, pos = 0;
    while ((pos = text.find(needle, pos)) != std::string::npos) { ++count; pos += needle.size(); }
    return count;
}

static void test_rule4_wall_hit_ends_game() {
    // Head starts at (10,6) heading RIGHT on a 20x12 board, so the 10th move
    // reaches x=20 (the wall). 'x' is a no-op key; 'q' ends run() after the
    // game-over prompt. The script must stay exactly 10 fillers + 'q'.
    std::istringstream in("xxxxxxxxxxq");
    std::ostringstream out;
    Game g(in, out);
    g.run();                                                   // real production loop

    CHECK(out.str().find("GAME OVER") != std::string::npos);
    CHECK(countOccurrences(out.str(), "\nScore: ") == 10);     // died on the wall tick
    CHECK(out.str().find("Thanks for playing!") != std::string::npos);
}

int main() {
    test_rule1_moves_one_cell_per_tick();
    test_rule2_no_direct_reversal();
    test_rule4_wall_hit_ends_game();

    std::printf("%d checks, %d failures\n", checks, failures);
    return failures == 0 ? 0 : 1;
}
