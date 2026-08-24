#include <iostream>
#include <vector>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <cstdlib>
#include <ctime>
using namespace std;

const string SNAKE_HEAD = "👀";
const string SNAKE_BODY = "🩷";
const string FOOD_ICON  = "🪱";
const string TILE       = "⬜";
const string BORDER     = "🟫";

const int WIDTH = 20;
const int HEIGHT = 12;

struct Position {
    int x, y;
    bool operator==(const Position &o) const { return x == o.x && y == o.y; }
};

class Snake {
public:
    enum Dir { UP, DOWN, LEFT, RIGHT };
private:
    vector<Position> body;
    Dir dir;
public:
    Snake() { reset(); }
    void reset() {
        body.clear();
        int midX = WIDTH / 2, midY = HEIGHT / 2;
        body.push_back({midX, midY});
        body.push_back({midX - 1, midY});
        body.push_back({midX - 2, midY});
        dir = RIGHT;
    }
    Position head() const { return body.front(); }
    const vector<Position>& getBody() const { return body; }
    void setDir(Dir d) {
        if ((dir == UP && d == DOWN) || (dir == DOWN && d == UP) ||
            (dir == LEFT && d == RIGHT) || (dir == RIGHT && d == LEFT))
            return;
        dir = d;
    }
    void setDirFromKey(char k) {
        if (k == 'w' || k == 'W') setDir(UP);
        else if (k == 's' || k == 'S') setDir(DOWN);
        else if (k == 'a' || k == 'A') setDir(LEFT);
        else if (k == 'd' || k == 'D') setDir(RIGHT);
    }
    void move(bool grow = false) {
        Position nh = head();
        if (dir == UP) nh.y--;
        else if (dir == DOWN) nh.y++;
        else if (dir == LEFT) nh.x--;
        else if (dir == RIGHT) nh.x++;
        body.insert(body.begin(), nh);
        if (!grow) body.pop_back();
    }
    bool collidesWithSelf() const {
        Position h = head();
        for (size_t i = 1; i < body.size(); ++i)
            if (body[i] == h) return true;
        return false;
    }
    bool occupies(const Position &p) const {
        for (auto &b : body)
            if (b == p) return true;
        return false;
    }
};

class Food {
    Position pos;
public:
    Position get() const { return pos; }
    void spawn(const vector<Snake> &snakes) {
        bool occupied;
        do {
            pos.x = rand() % WIDTH;
            pos.y = rand() % HEIGHT;
            occupied = false;
            for (const auto& s : snakes) {
                if (s.occupies(pos)) {
                    occupied = true;
                    break;
                }
            }
        } while (occupied);
    }
};

class Game {
    vector<Snake> snakes;
    Food food;
    vector<int> scores;
    int highScore = 0;
    int losingPlayer = 0;
    bool running = true;
    bool gameOver = false;
    double speedFactor = 1.0;
    const int baseDelay = 180000; // start slow (microseconds)

    struct termios oldt;

    void hideCursor() { cout << "\033[?25l"; }
    void showCursor() { cout << "\033[?25h"; }
    void clearScreen() { cout << "\033[2J"; }
    void moveCursorTopLeft() { cout << "\033[H"; }

    void enableNonBlocking() {
        tcgetattr(STDIN_FILENO, &oldt);
        struct termios newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    }
    void disableNonBlocking() {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }

    char readKey() {
        char ch = 0;
        ssize_t r = read(STDIN_FILENO, &ch, 1);
        if (r <= 0) return 0;
        return ch;
    }

    char readControl() {
        char ch = readKey();
        if (!ch) return 0;
        if (ch == '\033') {
            char ch1 = readKey();
            if (!ch1) return 0;
            if (ch1 == '[') {
                char ch2 = readKey();
                if (!ch2) return 0;
                if (ch2 == 'A') return 'U';
                if (ch2 == 'B') return 'D';
                if (ch2 == 'C') return 'R';
                if (ch2 == 'D') return 'L';
            }
            return 0;
        } else return ch;
    }

    void drawBoard() {
        moveCursorTopLeft();
        for (int i = 0; i < WIDTH + 2; i++) cout << BORDER;
        cout << '\n';
        for (int y = 0; y < HEIGHT; y++) {
            cout << BORDER;
            for (int x = 0; x < WIDTH; x++) {
                Position p{x, y};
                bool isSnake = false;
                for (const auto& s : snakes) {
                    if (p == s.head()) { cout << SNAKE_HEAD; isSnake = true; break; }
                    else if (s.occupies(p)) { cout << SNAKE_BODY; isSnake = true; break; }
                }
                if (!isSnake) {
                    if (p == food.get()) cout << FOOD_ICON;
                    else cout << TILE;
                }
            }
            cout << BORDER << '\n';
        }
        for (int i = 0; i < WIDTH + 2; i++) cout << BORDER;
        if (scores.size() >= 2) {
            cout << "\nPlayer 1 Score: " << scores[0] << "   Player 2 Score: " << scores[1]
                 << "   High Score: " << highScore << '\n';
        } else {
            cout << "\nScore: " << (!scores.empty() ? scores[0] : 0) << "   High Score: " << highScore << '\n';
        }
    }

    void processInput() {
        char control = readControl();
        if (!control) return;
        if (snakes.empty()) return;
        
        if (control == 'U') snakes[0].setDir(Snake::UP);
        else if (control == 'D') snakes[0].setDir(Snake::DOWN);
        else if (control == 'L') snakes[0].setDir(Snake::LEFT);
        else if (control == 'R') snakes[0].setDir(Snake::RIGHT);
        
        if (snakes.size() > 1) {
            if (control == 'w' || control == 'W') snakes[1].setDir(Snake::UP);
            else if (control == 's' || control == 'S') snakes[1].setDir(Snake::DOWN);
            else if (control == 'a' || control == 'A') snakes[1].setDir(Snake::LEFT);
            else if (control == 'd' || control == 'D') snakes[1].setDir(Snake::RIGHT);
        }
        
        if (control == 'q' || control == 'Q') running = false;
        else if (control == 'r' || control == 'R') restart();
    }

    void update() {
        for (size_t i = 0; i < snakes.size(); ++i) {
            snakes[i].move();
            Position h = snakes[i].head();

            bool collision = false;
            if (h.x < 0 || h.x >= WIDTH || h.y < 0 || h.y >= HEIGHT || snakes[i].collidesWithSelf()) {
                collision = true;
            } else {
                for (size_t j = 0; j < snakes.size(); ++j) {
                    if (i != j && snakes[j].occupies(h)) {
                        collision = true;
                        break;
                    }
                }
            }

            if (collision) {
                gameOver = true;
                losingPlayer = i + 1;
                return;
            }

            if (h == food.get()) {
                scores[i]++;
                snakes[i].move(true);
                food.spawn(snakes);
                if (scores[i] > highScore) highScore = scores[i];
                if (speedFactor > 0.3) speedFactor *= 0.95;
            }
        }
    }

    void restart() {
        snakes.resize(2);
        snakes[0].reset();
        snakes[1].reset({WIDTH / 2, HEIGHT / 2 + 3}, Snake::RIGHT);
        scores.assign(snakes.size(), 0);
        gameOver = false;
        losingPlayer = 0;
        speedFactor = 1.0;
        food.spawn(snakes);
    }

public:
    Game() { srand(time(0)); }
    void run() {
        enableNonBlocking();
        hideCursor();
        clearScreen();
        restart();

        while (running) {
            while (!gameOver && running) {
                processInput(); // instant response
                update();
                drawBoard();
                usleep((int)(baseDelay * speedFactor));
            }

            if (!running) break;

            // Show Game Over only once
            cout << "\nGAME OVER \n";
            if (losingPlayer > 0) cout << "Player " << losingPlayer << " lost.\n";
            if (scores.size() >= 2) {
                cout << "Final Scores - Player 1: " << scores[0] << "   Player 2: " << scores[1]
                     << "   High Score: " << highScore << '\n';
            } else {
                cout << "Final Score: " << (!scores.empty() ? scores[0] : 0) << "   High Score: " << highScore << '\n';
            }
            cout << "Press R to restart or Q to quit.\n";

            // Wait for player choice
            bool choiceMade = false;
            while (!choiceMade && running) {
                char c = readControl();
                if (!c) { usleep(250000); continue; }
                if (c == 'r' || c == 'R') { restart(); choiceMade = true; }
                else if (c == 'q' || c == 'Q') { running = false; choiceMade = true; }
            }
        }

        showCursor();
        disableNonBlocking();
        if (scores.size() >= 2) {
            cout << "\nThanks for playing! Final Scores - Player 1: " << scores[0]
                 << "   Player 2: " << scores[1] << "\n";
        } else {
            cout << "\nThanks for playing! Final Score: " << (!scores.empty() ? scores[0] : 0) << "\n";
        }
    }
};

int main() {
    Game g;
    g.run();
    return 0;
}
