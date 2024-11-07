#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>  // For loading images

#define WINDOW_WIDTH 1366
#define WINDOW_HEIGHT 768
#define GRID_SIZE 10
#define CELL_SIZE 48

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);  // Initialize PNG support

    SDL_Window* window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load the image you want to display
    SDL_Texture* head = IMG_LoadTexture(renderer, "assets/Head.png");
    SDL_Texture* body = IMG_LoadTexture(renderer, "assets/Body.png");
    SDL_Texture* cell = IMG_LoadTexture(renderer, "assets/Empty.png");
    if (!head) {
        printf("Failed to load texture: %s\n", IMG_GetError());
        return -1;
    }

    // Calculate the image size (assuming all images are the same size)
    int img_width, img_height;
    SDL_QueryTexture(head, NULL, NULL, &img_width, &img_height);

    int rendered_width = CELL_SIZE, rendered_height = CELL_SIZE;

    int grid_width = CELL_SIZE * GRID_SIZE;
int grid_height = CELL_SIZE * GRID_SIZE;

// Calculate the offset to center the grid
int offset_x = (WINDOW_WIDTH - grid_width) / 2;  // Assuming you have a window_width variable
int offset_y = (WINDOW_HEIGHT - grid_height) / 2;

    // Main loop
    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Clear screen
        SDL_RenderClear(renderer);

        // Render images in a grid
        for (int row = 0; row < GRID_SIZE; row++) {
            for (int col = 0; col < GRID_SIZE; col++) {
                // Calculate the position for each image
                int x = offset_x + col * rendered_height;
                int y = offset_y + row * rendered_height;

                // Define the destination rectangle for the image
                SDL_Rect dstRect = {x, y, rendered_width, rendered_height};

                // Render the texture
                SDL_RenderCopy(renderer, head, NULL, &dstRect);
            }
        }

        // Update the screen
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    SDL_DestroyTexture(head);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
