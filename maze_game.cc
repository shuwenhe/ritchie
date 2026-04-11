#include <chrono>
#include <cstring>
#include <deque>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

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

enum class Direction { Up, Down, Left, Right, None };

struct Player {
    int id = 0;
    Point pos = {0, 0};
    int health = 3;
    Direction nextDir = Direction::None;
    int weaponCooldown = 0;
    bool alive = true;
};

struct Weapon {
    Point pos;
    int damage;
    bool active;
};

#ifndef _WIN32
class TerminalGuard {
public:
    TerminalGuard() { enableRawMode(); }
    ~TerminalGuard() { disableRawMode(); }

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

class MazeGame {
public:
    MazeGame(int width, int height)
        : width(width), height(height), rng(std::random_device{}()) {
        init();
    }

    void run() {
#ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
#endif
        lastWeaponTime = std::chrono::system_clock::now();
        while (getAliveCount() > 1) {
            handleInput();
            update();
            render();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        renderGameEnd();
    }

private:
    int width;
    int height;
    std::vector<std::vector<int>> maze;
    std::vector<Player> players;
    std::vector<Weapon> weapons;
    std::mt19937 rng;
    int frameCount = 0;
    std::chrono::system_clock::time_point lastWeaponTime;

#ifndef _WIN32
    TerminalGuard terminal;
#endif

    void init() {
        maze.assign(height, std::vector<int>(width, 0));
        generateMaze();

        players.resize(4);
        players[0] = {0, {2, 2}, 3, Direction::None, 0, true};
        players[1] = {1, {width - 3, 2}, 3, Direction::None, 0, true};
        players[2] = {2, {2, height - 3}, 3, Direction::None, 0, true};
        players[3] = {3, {width - 3, height - 3}, 3, Direction::None, 0, true};
    }

    void generateMaze() {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                    maze[y][x] = 1;
                } else if (x % 3 == 0 && y % 3 == 0) {
                    maze[y][x] = (rng() % 2 == 0) ? 1 : 0;
                }
            }
        }
    }

    int getAliveCount() const {
        int count = 0;
        for (const auto& p : players) {
            if (p.alive) count++;
        }
        return count;
    }

    Player* getAlivePlayer() {
        for (auto& p : players) {
            if (p.alive) return &p;
        }
        return nullptr;
    }

    void handleInput() {
#ifdef _WIN32
        if (_kbhit()) {
            int ch = _getch();
            processKey(ch);
        }
#else
        while (terminal.kbhit()) {
            int ch = terminal.readBuffered();
            processKey(ch);
        }
#endif
    }

    void processKey(int ch) {
        if (ch == 'w' || ch == 'W') players[0].nextDir = Direction::Up;
        else if (ch == 's' || ch == 'S') players[0].nextDir = Direction::Down;
        else if (ch == 'a' || ch == 'A') players[0].nextDir = Direction::Left;
        else if (ch == 'd' || ch == 'D') players[0].nextDir = Direction::Right;
        else if (ch == 'i' || ch == 'I') players[1].nextDir = Direction::Up;
        else if (ch == 'k' || ch == 'K') players[1].nextDir = Direction::Down;
        else if (ch == 'j' || ch == 'J') players[1].nextDir = Direction::Left;
        else if (ch == 'l' || ch == 'L') players[1].nextDir = Direction::Right;
        else if (ch == 't' || ch == 'T') players[2].nextDir = Direction::Up;
        else if (ch == 'g' || ch == 'G') players[2].nextDir = Direction::Down;
        else if (ch == 'f' || ch == 'F') players[2].nextDir = Direction::Left;
        else if (ch == 'h' || ch == 'H') players[2].nextDir = Direction::Right;
        else if (ch == 'o' || ch == 'O') players[3].nextDir = Direction::Up;
        else if (ch == ';' || ch == ':') players[3].nextDir = Direction::Down;
        else if (ch == 'p' || ch == 'P') players[3].nextDir = Direction::Left;
        else if (ch == '\'' || ch == '"') players[3].nextDir = Direction::Right;
        else if (ch == 'q' || ch == 'Q') exit(0);
    }

    bool isWalkable(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height) return false;
        return maze[y][x] == 0;
    }

    void spawnWeapon() {
        std::uniform_int_distribution<int> distX(2, width - 3);
        std::uniform_int_distribution<int> distY(2, height - 3);
        while (true) {
            Point p{distX(rng), distY(rng)};
            if (isWalkable(p.x, p.y)) {
                bool occupied = false;
                for (const auto& w : weapons) {
                    if (w.active && w.pos == p) {
                        occupied = true;
                        break;
                    }
                }
                if (!occupied) {
                    weapons.push_back({p, 1, true});
                    return;
                }
            }
        }
    }

    void update() {
        auto now = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastWeaponTime).count();
        if (elapsed >= 10) {
            spawnWeapon();
            lastWeaponTime = now;
        }

        for (auto& p : players) {
            if (!p.alive) continue;
            if (p.weaponCooldown > 0) p.weaponCooldown--;

            Point newPos = p.pos;
            switch (p.nextDir) {
                case Direction::Up: newPos.y--; break;
                case Direction::Down: newPos.y++; break;
                case Direction::Left: newPos.x--; break;
                case Direction::Right: newPos.x++; break;
                case Direction::None: break;
            }

            if (isWalkable(newPos.x, newPos.y)) {
                p.pos = newPos;
            }

            for (auto& w : weapons) {
                if (w.active && w.pos == p.pos) {
                    p.weaponCooldown = 50;
                    w.active = false;
                }
            }
        }

        for (int i = 0; i < 4; ++i) {
            if (!players[i].alive || players[i].weaponCooldown == 0) continue;
            for (int j = 0; j < 4; ++j) {
                if (i == j || !players[j].alive) continue;
                if (players[i].pos == players[j].pos) {
                    players[j].health--;
                    if (players[j].health <= 0) {
                        players[j].alive = false;
                    }
                    players[i].weaponCooldown = 0;
                }
            }
        }
    }

    void render() {
        std::vector<std::string> buffer(height, std::string(width, ' '));

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (maze[y][x] == 1) {
                    buffer[y][x] = '#';
                }
            }
        }

        for (const auto& w : weapons) {
            if (w.active && w.pos.y >= 0 && w.pos.y < height && w.pos.x >= 0 && w.pos.x < width) {
                buffer[w.pos.y][w.pos.x] = '*';
            }
        }

        for (const auto& p : players) {
            if (p.alive && p.pos.y >= 0 && p.pos.y < height && p.pos.x >= 0 && p.pos.x < width) {
                char symbol = '1' + p.id;
                if (p.weaponCooldown > 0) symbol = 'A' + p.id;
                buffer[p.pos.y][p.pos.x] = symbol;
            }
        }

#ifdef _WIN32
        system("cls");
#else
        std::cout << "\x1B[2J\x1B[H";
#endif

        for (const auto& line : buffer) {
            std::cout << line << '\n';
        }

        std::cout << "P1(WASD) HP:" << (players[0].alive ? std::to_string(players[0].health) : "X") << " | ";
        std::cout << "P2(IJKL) HP:" << (players[1].alive ? std::to_string(players[1].health) : "X") << " | ";
        std::cout << "P3(TFGH) HP:" << (players[2].alive ? std::to_string(players[2].health) : "X") << " | ";
        std::cout << "P4(O;P') HP:" << (players[3].alive ? std::to_string(players[3].health) : "X") << std::endl;
        std::cout << "Weapons spawn every 10s. Touch weapon to attack for 5 frames. Q to quit." << std::endl;
    }

    void renderGameEnd() {
        std::cout << "\n============ GAME OVER ============\n";
        Player* winner = getAlivePlayer();
        if (winner) {
            std::cout << "Player " << (winner->id + 1) << " WINS! Congratulations!\n";
        }
        std::cout << "==================================\n";
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
        return {static_cast<int>(w.ws_col), static_cast<int>(w.ws_row) - 3};
    }
#endif
    return {80, 24};
}

int main() {
    auto [cols, rows] = getConsoleSize();
    if (cols < 40) cols = 40;
    if (rows < 20) rows = 20;
    MazeGame game(cols, rows);
    game.run();
    return 0;
}
