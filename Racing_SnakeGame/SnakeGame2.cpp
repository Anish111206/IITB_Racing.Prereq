#include <bits/stdc++.h>
#include <ncurses.h>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

using namespace std;

/* ================= Position ================= */
struct Position {
    int y;
    int x;

    bool operator==(const Position& other) const {
        return y == other.y && x == other.x;
    }
};

/* ================= Snake ================= */
class Snake {
private:
    deque<Position> body;
    int dirY, dirX;

public:
    Snake(int startY, int startX) {
        body.push_back({startY, startX});
        body.push_back({startY, startX - 1});
        body.push_back({startY, startX - 2});
        dirY = 0;
        dirX = 1;
    }

    void changeDirection(int y, int x) {
        if (body.size() > 1 && dirY == -y && dirX == -x)
            return;
        dirY = y;
        dirX = x;
    }

    Position nextHead() const {
        Position head = body.front();
        return {head.y + dirY, head.x + dirX};
    }

    void move(bool grow) {
        body.push_front(nextHead());
        if (!grow)
            body.pop_back();
    }

    bool collision(const Position& pos) const {
        for (const auto& segment : body)
            if (segment == pos)
                return true;
        return false;
    }

    const deque<Position>& getBody() const {
        return body;
    }
};

/* ================= Food ================= */
class Food {
private:
    Position pos;

public:
    Food() { pos = {0, 0}; }

    void spawn(int maxY, int maxX, const Snake& snake) {
        do {
            pos.y = rand() % (maxY - 2) + 1;
            pos.x = rand() % (maxX - 2) + 1;
        } while (snake.collision(pos));
    }

    Position getPosition() const {
        return pos;
    }
};

/* ================= Game ================= */
class Game {
private:
    WINDOW* win;
    int height, width;
    bool running;
    int score;
    Snake snake;
    Food food;

public:
    Game(int h, int w)
        : height(h), width(w),
          snake(h / 2, w / 2),
          running(true), score(0) {}

    void init() {
        initscr();
        noecho();
        curs_set(0);
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);
        refresh();

        win = newwin(height, width, 0, 0);
        box(win, 0, 0);
        wrefresh(win);

        srand(time(nullptr));
        food.spawn(height, width, snake);
    }

    void input() {
    int ch = getch();
    switch (ch) {
        case KEY_UP:
        case 'w':
        case 'W':
            snake.changeDirection(-1, 0);
            break;

        case KEY_DOWN:
        case 's':
        case 'S':
            snake.changeDirection(1, 0);
            break;

        case KEY_LEFT:
        case 'a':
        case 'A':
            snake.changeDirection(0, -1);
            break;

        case KEY_RIGHT:
        case 'd':
        case 'D':
            snake.changeDirection(0, 1);
            break;

        case 'q':
        case 'Q':
            running = false;
            break;
    }
}


    void logic() {
        Position next = snake.nextHead();

        if (next.y <= 0 || next.y >= height - 1 ||
            next.x <= 0 || next.x >= width - 1 ||
            snake.collision(next)) {
            running = false;
            return;
        }

        bool grow = (next == food.getPosition());
        snake.move(grow);

        if (grow) {
            score += 10;
            food.spawn(height, width, snake);
        }
    }

    void render() {
        werase(win);
        box(win, 0, 0);

        mvwaddch(win, food.getPosition().y, food.getPosition().x, 'x');

        for (const auto& segment : snake.getBody())
            mvwaddch(win, segment.y, segment.x, 'c');

        mvprintw(height, 0, "Score: %d | Press Q to quit", score);
        refresh();
        wrefresh(win);
    }

    void run() {
        while (running) {
            input();
            logic();
            render();
            usleep(100000); // speed control
        }
        gameOver();
    }

    void gameOver() {
        clear();
        mvprintw(height / 2, (width - 9) / 2, "GAME OVER");
        mvprintw(height / 2 + 1, (width - 15) / 2, "Final Score: %d", score);
        mvprintw(height / 2 + 3, (width - 20) / 2, "Press any key to exit");
        nodelay(stdscr, FALSE);
        getch();
        endwin();
    }
};

/* ================= Main ================= */
int main() {
    Game game(20, 50);
    game.init();
    game.run();
    return 0;
}

