#include <chrono>
#include <deque>
#include <iostream>
#include <random>
#include <thread>
#include <vector>
#include <algorithm>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#endif

struct Point {
    int x;
    int y;
    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
};

enum class Direction { Up, Down, Left, Right };

#ifndef _WIN32
class TerminalGuard {
public:
    TerminalGuard() { enableRawMode(); }
    ~TerminalGuard() { disableRawMode(); }

    int getKey() {
        int ch = -1;
        unsigned char c;
        if (read(STDIN_FILENO, &c, 1) == 1) {
            ch = c;
        }
        return ch;
    }

    bool kbhit() {
        int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
        unsigned char c;
        int nread = read(STDIN_FILENO, &c, 1);
        fcntl(STDIN_FILENO, F_SETFL, oldf);
        if (nread == 1) {
            buffer.push_back(c);
            return true;
        }
        return false;
    }

    int readBuffered() {
        if (buffer.empty()) return -1;
        int c = buffer.front();
        buffer.pop_front();
        return c;
    }

private:
    void enableRawMode() {
        tcgetattr(STDIN_FILENO, &origTerm);
        termios raw = origTerm;
        raw.c_lflag &= ~(ECHO | ICANON);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    }

    void disableRawMode() { tcsetattr(STDIN_FILENO, TCSANOW, &origTerm); }

    termios origTerm{};
    std::deque<unsigned char> buffer;
};
#endif

class SnakeGame {
public:
    SnakeGame(int width, int height)
        : width(width), height(height), rng(std::random_device{}()) {
        reset();
    }

    void run() {
#ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
#endif
        while (!gameOver) {
            handleInput();
            update();
            render();
            std::this_thread::sleep_for(std::chrono::milliseconds(effectiveDelay));
        }
        renderGameOver();
    }

private:
    int width;
    int height;
    std::deque<Point> snake;
    Direction dir = Direction::Right;
    Direction nextDir = Direction::Right;
    std::vector<Point> foods;
    bool gameOver = false;
    int score = 0;
    int frameDelay = 200;
    int effectiveDelay = 200;
    int boostDelay = 80;
    std::mt19937 rng;
    std::chrono::steady_clock::time_point lastFoodSpawn;
    std::chrono::steady_clock::time_point boostUntil;

#ifndef _WIN32
    TerminalGuard terminal;
#endif

    void reset() {
        snake.clear();
        snake.push_back({width / 2, height / 2});
        snake.push_back({width / 2 - 1, height / 2});
        snake.push_back({width / 2 - 2, height / 2});
        dir = Direction::Right;
        nextDir = Direction::Right;
        score = 0;
        frameDelay = 200;
        effectiveDelay = frameDelay;
        gameOver = false;
        foods.clear();
        spawnFood(20);
        lastFoodSpawn = std::chrono::steady_clock::now();
        boostUntil = lastFoodSpawn;
    }

    bool isOccupied(const Point& p) {
        for (const auto& s : snake) {
            if (s == p) return true;
        }
        for (const auto& f : foods) {
            if (f == p) return true;
        }
        return false;
    }

    void spawnFood(int count) {
        if (width <= 2 || height <= 2) return;
        std::uniform_int_distribution<int> distX(1, width - 2);
        std::uniform_int_distribution<int> distY(1, height - 2);
        int maxTry = (width - 2) * (height - 2);
        for (int i = 0; i < count; ++i) {
            for (int t = 0; t < maxTry; ++t) {
                Point p{distX(rng), distY(rng)};
                if (!isOccupied(p)) {
                    foods.push_back(p);
                    break;
                }
            }
        }
    }

    void handleInput() {
#ifdef _WIN32
        if (_kbhit()) {
            int ch = _getch();
            processKey(ch);
        }
#else
        if (terminal.kbhit()) {
            int ch = terminal.readBuffered();
            processKey(ch);
        }
#endif
    }

    void processKey(int ch) {
        if (ch == 'w' || ch == 'W') nextDir = Direction::Up;
        else if (ch == 's' || ch == 'S') nextDir = Direction::Down;
        else if (ch == 'a' || ch == 'A') nextDir = Direction::Left;
        else if (ch == 'd' || ch == 'D') nextDir = Direction::Right;
        else if (ch == ' ') boostUntil = std::chrono::steady_clock::now() + std::chrono::seconds(2);
        else if (ch == 'q' || ch == 'Q') gameOver = true;
    }

    bool isOpposite(Direction a, Direction b) {
        return (a == Direction::Up && b == Direction::Down) ||
               (a == Direction::Down && b == Direction::Up) ||
               (a == Direction::Left && b == Direction::Right) ||
               (a == Direction::Right && b == Direction::Left);
    }

    void update() {
        if (isOpposite(dir, nextDir)) {
            nextDir = dir;
        }
        dir = nextDir;

        Point head = snake.front();
        switch (dir) {
            case Direction::Up: head.y -= 1; break;
            case Direction::Down: head.y += 1; break;
            case Direction::Left: head.x -= 1; break;
            case Direction::Right: head.x += 1; break;
        }

        if (head.x <= 0 || head.x >= width - 1 || head.y <= 0 || head.y >= height - 1) {
            gameOver = true;
            return;
        }

        for (const auto& s : snake) {
            if (head == s) {
                gameOver = true;
                return;
            }
        }

        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastFoodSpawn).count() >= 1) {
            spawnFood(20);
            lastFoodSpawn = now;
        }

        effectiveDelay = frameDelay;
        if (now <= boostUntil) {
            effectiveDelay = std::min(frameDelay, boostDelay);
        }

        snake.push_front(head);
        bool ate = false;
        for (size_t i = 0; i < foods.size(); ++i) {
            if (head == foods[i]) {
                foods.erase(foods.begin() + static_cast<long>(i));
                score += 10;
                if (frameDelay > 80) frameDelay -= 3;
                ate = true;
                break;
            }
        }
        if (!ate) {
            snake.pop_back();
        }
    }

    void render() {
        std::vector<std::string> buffer(height, std::string(width, ' '));
        for (int x = 0; x < width; ++x) {
            buffer[0][x] = '-';
            buffer[height - 1][x] = '-';
        }
        for (int y = 0; y < height; ++y) {
            buffer[y][0] = '|';
            buffer[y][width - 1] = '|';
        }

        if (width > 1 && height > 1) {
            buffer[0][0] = '+';
            buffer[0][width - 1] = '+';
            buffer[height - 1][0] = '+';
            buffer[height - 1][width - 1] = '+';
        }

        for (const auto& f : foods) {
            buffer[f.y][f.x] = '*';
        }
        bool isHead = true;
        for (const auto& s : snake) {
            buffer[s.y][s.x] = isHead ? '@' : 'o';
            isHead = false;
        }

#ifdef _WIN32
        system("cls");
#else
        std::cout << "\x1B[2J\x1B[H";
#endif
        std::cout << "================== S N A K E ==================\n";
        std::cout << "Score: " << score << "   Foods: " << foods.size()
                  << "   Speed: " << (1000 / effectiveDelay) << "x\n";
        for (const auto& line : buffer) {
            std::cout << line << '\n';
        }
        std::cout << "WASD move  |  Space boost  |  Q quit  |  New foods every 1s" << std::endl;
    }

    void renderGameOver() {
        std::cout << "\nGame Over! Final Score: " << score << std::endl;
    }
};

static std::pair<int, int> getConsoleSize() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        int cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        return {cols, rows};
    }
#else
    winsize w{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        return {static_cast<int>(w.ws_col), static_cast<int>(w.ws_row)};
    }
#endif
    return {40, 20};
}

int main() {
    auto [cols, rows] = getConsoleSize();
    if (cols < 20) cols = 20;
    if (rows < 10) rows = 10;
    SnakeGame game(cols, rows);
    game.run();
    return 0;
}
