#include "SDL2/SDL_events.h"
#include "SDL2/SDL_rect.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "snake.h"

#define WINDOW_WIDTH 1366
#define WINDOW_HEIGHT 768
#define GRID_SIZE 15
#define CELL_SIZE 32
#define TICK_RATE 200

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture *head, *body, *cell, *food, *dead_head, *dead_body;

Direction direction;
Snake *position_head, *position_tail;
int grid_data[GRID_SIZE][GRID_SIZE];

int grid_width;
int grid_height;
int offset_x;
int offset_y;

int setup(void);
int handleInput(SDL_Event *);
void cleanup(void);
void renderGame(void);
void updateGameLogic(void);
void generateFood(void);

int main(int argc, char* argv[]) {
    int setup_failure = setup();
    if (setup_failure) {
        printf("Error in setup\n");
    }

    Uint32 last_tick_time = SDL_GetTicks();

    // Main loop
    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            handleInput(&event);
        }
        Uint32 current_time = SDL_GetTicks();

        if (current_time - last_tick_time >= TICK_RATE) {
            printf("Tick\n");
            updateGameLogic();
            renderGame();
            last_tick_time = current_time;  // Update the last tick time
        }

        SDL_Delay(10);
    }

    cleanup();
    return 0;
}

int setup(void) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);  // Initialize PNG support

    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load our assets
    head = IMG_LoadTexture(renderer, "assets/Head.png");
    body = IMG_LoadTexture(renderer, "assets/Body.png");
    cell = IMG_LoadTexture(renderer, "assets/Empty.png");
    food = IMG_LoadTexture(renderer, "assets/Food.png");
    dead_head = IMG_LoadTexture(renderer, "assets/DeadHead.png");
    dead_body = IMG_LoadTexture(renderer, "assets/DeadBody.png");

    if (!head || !body || !cell || !food || !dead_head || !dead_body) {
        printf("Failed to load textures: %s\n", IMG_GetError());
        return -1;
    }

    direction = RIGHT;
    position_head = malloc(sizeof(Snake));
    position_head->position.x = 7;
    position_head->position.y = 0;
    position_head->next = NULL;
    position_tail = position_head;

    for (int *p = grid_data[0]; p < &grid_data[GRID_SIZE][0]; p++) {
        *p = 0;
    }
    grid_data[position_head->position.x][position_head->position.y] = 1;

    grid_width = CELL_SIZE * GRID_SIZE;
    grid_height = CELL_SIZE * GRID_SIZE;

    // Calculate the offset to center the grid
    offset_x = (WINDOW_WIDTH - grid_width) / 2;
    offset_y = (WINDOW_HEIGHT - grid_height) / 2;

    srand((unsigned) time(NULL));
    generateFood();

    return 0;
}

void generateFood() {
    int x = rand() % 15;
    int y = rand() % 15;
    while (grid_data[x][y]) {
        x = rand() % 15;
        y = rand() % 15;
    }
    grid_data[x][y] = 2;
}

int handleInput(SDL_Event *event) {
    if (event->type == SDL_QUIT) {
        cleanup();
        exit(0);
    } else if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            case SDLK_DOWN:
                direction = DOWN;
                break;
            case SDLK_UP:
                direction = UP;
                break;
            case SDLK_LEFT:
                direction = LEFT;
                break;
            case SDLK_RIGHT:
                direction = RIGHT;
                break;
        }
    }
    return 0;
}

void updateGameLogic(void) {
    Snake *new_position = malloc(sizeof(Snake));
    new_position->position.x = position_head->position.x;
    new_position->position.y = position_head->position.y;
    new_position->next = NULL;
    switch(direction) {
        case LEFT:
            new_position->position.y -= 1;
            break;
        case RIGHT:
            new_position->position.y += 1;
            break;
        case UP:
            new_position->position.x -= 1;
            break;
        case DOWN:
            new_position->position.x += 1;
            break;
    }
    if (new_position->position.x < 0 || new_position->position.x >= GRID_SIZE || new_position->position.y < 0 || new_position->position.y >= GRID_SIZE) {
        cleanup();
        exit(0);
    }

    position_head->next = new_position;
    position_head = new_position;

    switch(grid_data[new_position->position.x][new_position->position.y]) {
        case 0:
            grid_data[position_head->position.x][position_head->position.y] = 1;
            grid_data[position_tail->position.x][position_tail->position.y] = 0;
            Snake *tmp = position_tail->next;
            free(position_tail);
            position_tail = tmp;
            break;
        case 1:
            cleanup();
            exit(0);
            break;
        case 2:
            grid_data[position_head->position.x][position_head->position.y] = 1;
            generateFood();
            break;
    }
}

void renderGame() {
    // Clear screen
    SDL_RenderClear(renderer);

    // Render images in a grid
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            // Calculate the position for each image
            int x = offset_x + col * CELL_SIZE;
            int y = offset_y + row * CELL_SIZE;

            // Define the destination rectangle for the image
            SDL_Rect dstRect = {x, y, CELL_SIZE, CELL_SIZE};

            // Render the texture
            switch (grid_data[row][col]) {
                case 0:
                    SDL_RenderCopy(renderer, cell, NULL, &dstRect);
                    break;
                case 1:
                    if (row == position_head->position.x && col == position_head->position.y) {
                        SDL_Point center = { dstRect.w / 2, dstRect.h / 2 };
                        int angle = 0;
                        switch (direction) {
                            case RIGHT:
                                angle = 90;
                                break;
                            case LEFT:
                                angle = -90;
                                break;
                            case UP:
                                angle = 0;
                                break;
                            case DOWN:
                                angle = 180;
                                break;
                        }
                        SDL_RenderCopyEx(renderer, head, NULL, &dstRect, angle, &center, SDL_FLIP_NONE);
                    } else {
                        SDL_RenderCopy(renderer, body, NULL, &dstRect);
                    }
                    break;
                case 2:
                    SDL_RenderCopy(renderer, food, NULL, &dstRect);
                    break;
            }
        }
    }

    // Update the screen
    SDL_RenderPresent(renderer);
}

void cleanup() {
    while (position_tail) {
        Snake *tmp = position_tail->next;
        free(position_tail);
        position_tail = tmp;
    }
    SDL_DestroyTexture(head);
    SDL_DestroyTexture(body);
    SDL_DestroyTexture(cell);
    SDL_DestroyTexture(food);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
