#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#define SIZE 4
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define ANIMATION_DELAY 100 // Delay between each animation frame in milliseconds


typedef struct {
    int grid[SIZE][SIZE];
    int emptyX;
    int emptyY;
} Board;

typedef struct {
    SDL_Rect rect;
    const char* text;
} Button;

typedef struct {
    SDL_Rect rect;
    const char* text;
} LoadButton;

SDL_Rect highlightRect;
TTF_Font* font;
int score = 0;
Button replayButton;
LoadButton loadButton;

void setRendererBackgroundColor(SDL_Renderer* renderer, int r, int g, int b, int a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderClear(renderer);

    // Draw a blue gradient background
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
        int blue = (y * 255) / WINDOW_HEIGHT;
        SDL_SetRenderDrawColor(renderer, 0, 0, blue, 255);
        SDL_RenderDrawLine(renderer, 0, y, WINDOW_WIDTH, y);
    }
}

void setCellColor(SDL_Renderer* renderer, int r, int g, int b, int a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}


// Déclaration de loadBoardFromFile
void loadBoardFromFile(Board* board, const char* filename);

// Déclaration de drawButton
void drawButton(SDL_Renderer* renderer, Button* button);

void handleLoadButtonEvent(SDL_Event* event, Board* board) {
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        if (SDL_PointInRect(&(SDL_Point){x, y}, &loadButton.rect)) {
            // Specify the full or relative path of your file
            const char* filename = "test/board.txt";
            loadBoardFromFile(board, filename);
        }
    }
}


void initBoard(Board* board) {
    // Initialize the board with ordered numbers and the last cell as empty
    int count = 1;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board->grid[i][j] = count;
            count++;
        }
    }
    board->grid[SIZE - 1][SIZE - 1] = 0;
    board->emptyX = SIZE - 1;
    board->emptyY = SIZE - 1;
}



void drawBoard(SDL_Renderer* renderer, Board* board) {
    // Draw the board using SDL
    SDL_Rect rect;
    rect.w = WINDOW_WIDTH / SIZE;
    rect.h = WINDOW_HEIGHT / SIZE;
    // Dessiner le contour brillant
    SDL_Rect highlightRectWithMargin = {
        .x = rect.x - 2,  // Ajoutez une marge de 2 pixels à gauche
        .y = rect.y - 2,  // Ajoutez une marge de 2 pixels en haut
        .w = rect.w + 4,  // Ajoutez une marge totale de 4 pixels en largeur
        .h = rect.h + 4   // Ajoutez une marge totale de 4 pixels en hauteur
    };
    setCellColor(renderer, 255, 100, 100, 255); // Couleur rouge vif pour le contour brillant
    SDL_RenderDrawRect(renderer, &highlightRectWithMargin);


    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            rect.x = j * rect.w;
            rect.y = i * rect.h;

            if (board->grid[i][j] != 0) {
                setCellColor(renderer, 0, 0, (board->grid[i][j] * 255) / (SIZE * SIZE), 255);
                SDL_RenderFillRect(renderer, &rect);

                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderDrawRect(renderer, &rect);

                SDL_Color textColor = {255, 255, 255, 255};
                char number[2];
                sprintf(number, "%d", board->grid[i][j]);

                SDL_Surface* surface = TTF_RenderText_Solid(font, number, textColor);
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

                SDL_Rect textRect;
                textRect.x = rect.x + rect.w / 2 - surface->w / 2;
                textRect.y = rect.y + rect.h / 2 - surface->h / 2;
                textRect.w = surface->w;
                textRect.h = surface->h;

                SDL_RenderCopy(renderer, texture, NULL, &textRect);

                SDL_FreeSurface(surface);
                SDL_DestroyTexture(texture);
            }
        }
    }

    // Draw the load button outside the loop
    drawButton(renderer, &loadButton);
}


void shuffleBoard(Board* board, int movesCount) {
    // Shuffle the board by making a number of random moves
    srand(time(NULL));

    for (int i = 0; i < movesCount; i++) {
        int direction = rand() % 4; // 0: up, 1: down, 2: left, 3: right

        switch (direction) {
            case 0: // up
                if (board->emptyY > 0) {
                    board->grid[board->emptyY][board->emptyX] = board->grid[board->emptyY - 1][board->emptyX];
                    board->grid[board->emptyY - 1][board->emptyX] = 0;
                    board->emptyY--;
                }
                break;
            case 1: // down
                if (board->emptyY < SIZE - 1) {
                    board->grid[board->emptyY][board->emptyX] = board->grid[board->emptyY + 1][board->emptyX];
                    board->grid[board->emptyY + 1][board->emptyX] = 0;
                    board->emptyY++;
                }
                break;
            case 2: // left
                if (board->emptyX > 0) {
                    board->grid[board->emptyY][board->emptyX] = board->grid[board->emptyY][board->emptyX - 1];
                    board->grid[board->emptyY][board->emptyX - 1] = 0;
                    board->emptyX--;
                }
                break;
            case 3: // right
                if (board->emptyX < SIZE - 1) {
                    board->grid[board->emptyY][board->emptyX] = board->grid[board->emptyY][board->emptyX + 1];
                    board->grid[board->emptyY][board->emptyX + 1] = 0;
                    board->emptyX++;
                }
                break;
        }
    }
}

void drawButton(SDL_Renderer* renderer, Button* button) {
    setCellColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &button->rect);

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, button->text, textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect textRect;
    textRect.x = button->rect.x + button->rect.w / 2 - surface->w / 2;
    textRect.y = button->rect.y + button->rect.h / 2 - surface->h / 2;
    textRect.w = surface->w;
    textRect.h = surface->h;

    SDL_RenderCopy(renderer, texture, NULL, &textRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void drawText(SDL_Renderer* renderer, const char* text, int x, int y) {
    SDL_Color textColor = {0, 0, 0, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect textRect;
    textRect.x = x;
    textRect.y = y;
    textRect.w = surface->w;
    textRect.h = surface->h;

    SDL_RenderCopy(renderer, texture, NULL, &textRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void loadBoardFromFile(Board* board, const char* filename) {
    // Load a board configuration from a file
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open file: %s\n", filename);
        return;
    }

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fscanf(file, "%d", &board->grid[i][j]);
            if (board->grid[i][j] == 0) {
                board->emptyX = j;
                board->emptyY = i;
            }
        }
    }

    fclose(file);
}

void handleEvent(SDL_Event* event, Board* board) {
    static int lastDirection = -1;

    if (event->type == SDL_KEYDOWN) {
        int direction = -1;

        switch (event->key.keysym.sym) {
            case SDLK_UP:
                direction = 0;
                break;
            case SDLK_DOWN:
                direction = 1;
                break;
            case SDLK_LEFT:
                direction = 2;
                break;
            case SDLK_RIGHT:
                direction = 3;
                break;
        }

        

        if (direction != -1 && abs(direction - lastDirection) != 2) {
            // Check if the direction is valid
            int moved = 0;

            switch (direction) {
                case 0: // up
                    if (board->emptyY > 0) {
                        board->grid[board->emptyY][board->emptyX] = board->grid[board->emptyY - 1][board->emptyX];
                        board->grid[board->emptyY - 1][board->emptyX] = 0;
                        board->emptyY--;
                        moved = 1;
                    }
                    break;
                case 1: // down
                    if (board->emptyY < SIZE - 1) {
                        board->grid[board->emptyY][board->emptyX] = board->grid[board->emptyY + 1][board->emptyX];
                        board->grid[board->emptyY + 1][board->emptyX] = 0;
                        board->emptyY++;
                        moved = 1;
                    }
                    break;
                case 2: // left
                    if (board->emptyX > 0) {
                        board->grid[board->emptyY][board->emptyX] = board->grid[board->emptyY][board->emptyX - 1];
                        board->grid[board->emptyY][board->emptyX - 1] = 0;
                        board->emptyX--;
                        moved = 1;
                    }
                    break;
                case 3: // right
                    if (board->emptyX < SIZE - 1) {
                        board->grid[board->emptyY][board->emptyX] = board->grid[board->emptyY][board->emptyX + 1];
                        board->grid[board->emptyY][board->emptyX + 1] = 0;
                        board->emptyX++;
                        moved = 1;
                    }
                    break;
            }

             if (moved) {
            lastDirection = direction;
            SDL_Delay(100); // Ajoutez un délai pour rendre le mouvement plus perceptible

            // Mettez à jour le rectangle de contour après le déplacement
            highlightRect.x = board->emptyX * (WINDOW_WIDTH / SIZE);
            highlightRect.y = board->emptyY * (WINDOW_HEIGHT / SIZE);
            highlightRect.w = WINDOW_WIDTH / SIZE;
            highlightRect.h = WINDOW_HEIGHT / SIZE;
        }
        }
    }
}

int checkVictory(Board* board) {
    // Check if the board is in a winning state
    int count = 1;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board->grid[i][j] != count) {
                return 0;
            }
            count++;
        }
    }
    return 1;
}




int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    TTF_Init();
    SDL_Window* window = SDL_CreateWindow("Jeu de Taquin fait par Nihal Baz et Elijah Lasserre", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    font = TTF_OpenFont("font/Poppins-SemiBoldItalic.ttf", 24); 
    replayButton.rect.x = WINDOW_WIDTH - 100;
    replayButton.rect.y = WINDOW_HEIGHT - 50;
    replayButton.rect.w = 80;
    replayButton.rect.h = 30;
    replayButton.text = "Replay";
    
    loadButton.rect.x = WINDOW_WIDTH - 220;
    loadButton.rect.y = WINDOW_HEIGHT - 50;
    loadButton.rect.w = 100;
    loadButton.rect.h = 30;
    loadButton.text = "Load";


    Board board;
    initBoard(&board);
    shuffleBoard(&board, 1000);

    SDL_Event event;
    int quit = 0;
    int menu = 1;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                if (menu) {
                    if (x >= 300 && x <= 500 && y >= 300 && y <= 400) {
                        menu = 0;
                    }
                } else {
                    if (SDL_PointInRect(&(SDL_Point){x, y}, &replayButton.rect)) {
                        initBoard(&board);
                        shuffleBoard(&board, 1000);
                        score = 0;
                    }
                }
            } else {
                handleEvent(&event, &board);
                handleLoadButtonEvent(&event, &board);
            }
           
       
}

        

        setRendererBackgroundColor(renderer, 255, 255, 255, 255); // Couleur de fond blanche

        if (menu) {
            drawText(renderer, "Menu", 350, 200);
            drawButton(renderer, &(Button){(SDL_Rect){300, 300, 200, 100}, "Start"});
        } else {
            drawBoard(renderer, &board);
            drawButton(renderer, &replayButton);

            char scoreText[50];
            sprintf(scoreText, "Score: %d", score);
            drawText(renderer, scoreText, 10, 10);

            if (checkVictory(&board)) {
                score++;
                printf("Victory! Score: %d\n", score);
                initBoard(&board);
                shuffleBoard(&board, 1000);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(ANIMATION_DELAY);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}