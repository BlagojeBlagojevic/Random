#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 10
#define HEIGHT 10
#define STATE_COUNT 3 // For example: 0=Grass, 1=Dirt, 2=Water

typedef struct {
    int possible[STATE_COUNT];
    int collapsed;
} Cell;

Cell grid[WIDTH][HEIGHT];

// adjacencyRules[from_state][to_state]
int adjacencyRules[STATE_COUNT][STATE_COUNT] = {
    {1, 1, 0}, // Grass can be next to Grass or Dirt
    {1, 1, 1}, // Dirt can be next to anything
    {0, 1, 1}  // Water can be next to Dirt or Water
};

void initializeGrid() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            grid[x][y].collapsed = -1;
            for (int s = 0; s < STATE_COUNT; s++) {
                grid[x][y].possible[s] = 1;
            }
        }
    }
}

int countPossible(Cell *cell) {
    int count = 0;
    for (int i = 0; i < STATE_COUNT; i++)
        if (cell->possible[i]) count++;
    return count;
}

void collapseCell(int x, int y) {
    int options[STATE_COUNT];
    int count = 0;

    for (int i = 0; i < STATE_COUNT; i++) {
        if (grid[x][y].possible[i]) {
            options[count++] = i;
        }
    }

    if (count == 0) return;

    int choice = options[rand() % count];
    for (int i = 0; i < STATE_COUNT; i++) {
        grid[x][y].possible[i] = 0;
    }
    grid[x][y].possible[choice] = 1;
    grid[x][y].collapsed = choice;
}

void propagate(int x, int y) {
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    for (int dir = 0; dir < 4; dir++) {
        int nx = x + dx[dir];
        int ny = y + dy[dir];

        if (nx < 0 || ny < 0 || nx >= WIDTH || ny >= HEIGHT) continue;

        if (grid[nx][ny].collapsed != -1) continue;

        int valid[STATE_COUNT] = {0};

        for (int s = 0; s < STATE_COUNT; s++) {
            if (!grid[x][y].possible[s]) continue;

            for (int ns = 0; ns < STATE_COUNT; ns++) {
                if (adjacencyRules[s][ns]) {
                    valid[ns] = 1;
                }
            }
        }

        for (int i = 0; i < STATE_COUNT; i++) {
            if (!valid[i]) grid[nx][ny].possible[i] = 0;
        }
    }
}

void printGrid() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (grid[x][y].collapsed == -1)
                printf(".");
            else {
                char c = '?';
                if (grid[x][y].collapsed == 0) c = 'G'; // Grass
                else if (grid[x][y].collapsed == 1) c = 'D'; // Dirt
                else if (grid[x][y].collapsed == 2) c = 'W'; // Water
                printf("%c", c);
            }
        }
        printf("\n");
    }
}

int findLowestEntropyCell(int *outX, int *outY) {
    int minEntropy = STATE_COUNT + 1;
    int found = 0;

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (grid[x][y].collapsed == -1) {
                int entropy = countPossible(&grid[x][y]);
                if (entropy < minEntropy && entropy > 0) {
                    *outX = x;
                    *outY = y;
                    minEntropy = entropy;
                    found = 1;
                }
            }
        }
    }

    return found;
}

int main() {
    srand(time(NULL));
    initializeGrid();

    while (1) {
        int x, y;
        if (!findLowestEntropyCell(&x, &y)) break;

        collapseCell(x, y);
        propagate(x, y);
        printGrid();
        printf("\n");
    }

    printf("Final map:\n");
    printGrid();

    return 0;
}
