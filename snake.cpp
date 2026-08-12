#include <iostream>
#include <vector>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <cstdlib>
#include <ctime>
using namespace std;

const string SNAKE_HEAD = "ðŸ‘€";
const string SNAKE_BODY = "ðŸ©·";
const string FOOD_ICON  = "ðŸª±";
const string TILE       = "â¬œ";
const string BORDER     = "ðŸŸ«";

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
        reset({WIDTH / 2, HEIGHT / 2}, RIGHT);
    }
    void reset(Position start, Dir d) {
        body.clear();
        body.push_back(start);
        body.push_back({start.x - 1, start.y});
        body.push_back({start.x - 2, start.y});
        dir = d;
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
    void spawn(const Snake &s) {
        do {
            pos.x = rand() % WIDTH;
            pos.y = rand() % HEIGHT;
        } while (s.occupies(pos));
    }
    void spawn(const Snake &s1, const Snake &s2) {
        do {
            pos.x = rand() % WIDTH;
            pos.y = rand() % HEIGHT;
        } while (s1.occupies(pos) || s2.occupies(pos));
    }
};

class Game {
    Snake snake;
    Snake snake2;
    Food food;
    int score = 0;
    int score2 = 0;
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
                if (p == snake.head()) cout << SNAKE_HEAD;
                else if (snake.occupies(p)) cout << SNAKE_BODY;
                else if (p == snake2.head()) cout << SNAKE_HEAD;
                else if (snake2.occupies(p)) cout << SNAKE_BODY;
                else if (p == food.get()) cout << FOOD_ICON;
                else cout << TILE;
            }
            cout << BORDER << '\n';
        }
        for (int i = 0; i < WIDTH + 2; i++) cout << BORDER;
        cout << "\nPlayer 1 Score: " << score << "   Player 2 Score: " << score2
             << "   High Score: " << highScore << '\n';
    }

    void processInput() {
        char control = readControl();
        if (!control) return;
        if (control == 'U') snake.setDir(Snake::UP);
        else if (control == 'D') snake.setDir(Snake::DOWN);
        else if (control == 'L') snake.setDir(Snake::LEFT);
        else if (control == 'R') snake.setDir(Snake::RIGHT);
        else if (control == 'w' || control == 'W') snake2.setDir(Snake::UP);
        else if (control == 's' || control == 'S') snake2.setDir(Snake::DOWN);
        else if (control == 'a' || control == 'A') snake2.setDir(Snake::LEFT);
        else if (control == 'd' || control == 'D') snake2.setDir(Snake::RIGHT);
        else if (control == 'q' || control == 'Q') running = false;
        else if (control == 'r' || control == 'R') restart();
    }

    void update() {
        snake.move();
        Position h = snake.head();

        if (h.x < 0 || h.x >= WIDTH || h.y < 0 || h.y >= HEIGHT ||
            snake.collidesWithSelf() || snake2.occupies(h)) {
            gameOver = true;
            losingPlayer = 1;
            return;
        }

        if (h == food.get()) {
            score++;
            snake.move(true);
            food.spawn(snake, snake2);
            if (score > highScore) highScore = score;
            if (speedFactor > 0.3) speedFactor *= 0.95;
        }

        snake2.move();
        h = snake2.head();

        if (h.x < 0 || h.x >= WIDTH || h.y < 0 || h.y >= HEIGHT ||
            snake2.collidesWithSelf() || snake.occupies(h)) {
            gameOver = true;
            losingPlayer = 2;
            return;
        }

        if (h == food.get()) {
            score2++;
            snake2.move(true);
            food.spawn(snake, snake2);
            if (score2 > highScore) highScore = score2;
            if (speedFactor > 0.3) speedFactor *= 0.95;
        }
    }

    void restart() {
        snake.reset();
        snake2.reset({WIDTH / 2, HEIGHT / 2 + 3}, Snake::RIGHT);
        score = 0;
        score2 = 0;
        losingPlayer = 0;
        gameOver = false;
        speedFactor = 1.0;
        food.spawn(snake, snake2);
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
            cout << "Player " << losingPlayer << " lost.\n";
            cout << "Final Scores - Player 1: " << score << "   Player 2: " << score2
                 << "   High Score: " << highScore << '\n';
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
        cout << "\nThanks for playing! Final Scores - Player 1: " << score
             << "   Player 2: " << score2 << "\n";
    }
};

int main() {
    Game g;
    g.run();
    return 0;
}
