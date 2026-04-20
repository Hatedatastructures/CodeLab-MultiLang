#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <windows.h>

// 定义游戏区域的大小
#define WIDTH 20
#define HEIGHT 20

// 定义方向
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

// 蛇的结构体
typedef struct {
    int x, y;
} SnakePart;

typedef struct {
    SnakePart parts[100];
    int length;
    int direction;
} Snake;

// 食物的结构体
typedef struct {
    int x, y;
} Food;

Snake snake;
Food food;
int gameOver = 0;

// 初始化游戏
void initializeGame() {
    snake.length = 1;
    snake.parts[0].x = WIDTH / 2;
    snake.parts[0].y = HEIGHT / 2;
    snake.direction = RIGHT;

    food.x = rand() % WIDTH;
    food.y = rand() % HEIGHT;
}

// 打印游戏界面
void printGame() {
    system("cls");
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1) {
                printf("#"); // 边界
            } else if (i == snake.parts[0].y && j == snake.parts[0].x) {
                printf("O"); // 蛇头
            } else if (i == food.y && j == food.x) {
                printf("F"); // 食物
            } else {
                int isBodyPart = 0;
                for (int k = 1; k < snake.length; k++) {
                    if (i == snake.parts[k].y && j == snake.parts[k].x) {
                        printf("o"); // 蛇身
                        isBodyPart = 1;
                        break;
                    }
                }
                if (!isBodyPart) {
                    printf(" ");
                }
            }
        }
        printf("\n");
    }
}

// 更新蛇的位置
void updateSnake() {
    // 先移动蛇的身体部分
    for (int i = snake.length - 1; i > 0; i--) {
        snake.parts[i] = snake.parts[i - 1];
    }

    // 更新蛇头的位置
    switch (snake.direction) {
        case UP:
            snake.parts[0].y--;
            break;
        case DOWN:
            snake.parts[0].y++;
            break;
        case LEFT:
            snake.parts[0].x--;
            break;
        case RIGHT:
            snake.parts[0].x++;
            break;
    }

    // 检查是否撞到边界或自己
    if (snake.parts[0].x == 0 || snake.parts[0].x == WIDTH - 1 ||
        snake.parts[0].y == 0 || snake.parts[0].y == HEIGHT - 1) {
        gameOver = 1;
    }
    for (int i = 1; i < snake.length; i++) {
        if (snake.parts[0].x == snake.parts[i].x && snake.parts[0].y == snake.parts[i].y) {
            gameOver = 1;
        }
    }

    // 检查是否吃到食物
    if (snake.parts[0].x == food.x && snake.parts[0].y == food.y) {
        snake.length++;
        food.x = rand() % (WIDTH - 2) + 1;
        food.y = rand() % (HEIGHT - 2) + 1;
    }
}

// 检查键盘输入并改变方向
void checkInput() {
    if (_kbhit()) {
        switch (_getch()) {
            case 'w':
                if (snake.direction != DOWN) snake.direction = UP;
                break;
            case 's':
                if (snake.direction != UP) snake.direction = DOWN;
                break;
            case 'a':
                if (snake.direction != RIGHT) snake.direction = LEFT;
                break;
            case 'd':
                if (snake.direction != LEFT) snake.direction = RIGHT;
                break;
        }
    }
}

int main() {
    srand(time(NULL));
    initializeGame();

    while (!gameOver) {
        printGame();
        checkInput();
        updateSnake();
        Sleep(100); // 控制蛇的速度
    }

    printf("Game Over! Your score is: %d\n", snake.length - 1);
    return 0;
}
