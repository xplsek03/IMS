#include <iostream>
#include <SDL2/SDL.h>
#include <cmath>
#include <ctime>
#include <vector>
#include <utility>

int main(int argc, char const *argv[]) {

    SDL_Window * window;
    SDL_Renderer * renderer;
    int w = 6;
    int minesCount = 10;
    bool isRunning=true;
    srand(time(NULL));

    // inicializace SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Chyba SDL: " << SDL_GetError() << '\n';
        return -1;
    }

    // vytvoreni SDL okna
    if (SDL_CreateWindowAndRenderer(600, 600,SDL_WINDOW_OPENGL, &window, &renderer) < 0) {
        std::cout << "Chyba SDL: " << SDL_GetError() << '\n';
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -2;
    }

    // SEM SI DOPLN CO POTREBUJES
    // struktura jednoho metru hektaru
    struct metr {
        bool isMine;
        int n;  // Neighbors
        bool isRevealed=false;
    };

    // inicializuj hektar
    metr pole[100][100];
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            pole[i][j].isMine = false;
            pole[i][j].n = 0;
        }
    }

    // Fill pole with minesCount of mines.
    int m = 0;
    while(m < minesCount) {
        int x = rand()%10;
        int y = rand()%10;
        if (pole[x][y].isMine)
        continue;
        pole[x][y].isMine=true;
        m++;
    }

  // Calculate neighbors
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      // If this is mine, skip it
      if (pole[i][j].isMine) {
        pole[i][j].n=9; // Choose correct texture from sheet tho
        continue;
      }
      int n=0;
      // Calculate all neighbors on 3x3 grid around tile
      for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
          int xoff=i+x;
          int yoff=j+y;
          // Do not count outside array
          if (xoff<0 || xoff>9 || yoff<0 || yoff>9)
            continue;
          if (pole[xoff][yoff].isMine)
            n++;
        }
        // Update neighbor count to the struct
        pole[i][j].n=n;
      }
    }
  }

  // Main loop
  while (isRunning) {
    // We want to use black background
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    // Clear the screen with current background color
    SDL_RenderClear(renderer);

    // DO DRAW
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    for (int i = 0; i < 100; i++) {
      for (int j= 0; j < 100; j++) {
        // for showing grid at start
        SDL_Rect rect;
        rect.x=i*w;
        rect.y=j*w;
        rect.w=w;
        rect.h=w;
        SDL_SetRenderDrawColor(renderer, 0, 0xFF, 0, 0xFF);
        SDL_RenderFillRect(renderer, &rect);
        // outline
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
        SDL_RenderDrawRect(renderer, &rect);

        // Player has clicked on tile and we want to show tilesheet graphics
        // Continue loop since current tile has not yet been revealed

        //if (!pole[i][j].isRevealed)
        //  continue;

        // 1 HRABOS
        SDL_Rect h;
        h.x=10*w;
        h.y=20*w;
        h.w=w;
        h.h=w;
        SDL_SetRenderDrawColor(renderer, 0xFF, 0, 0, 0xFF);
        SDL_RenderFillRect(renderer, &h);
        // outline
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
        SDL_RenderDrawRect(renderer, &h);
      }
    }

    // Handle inputs
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        isRunning=false;
      }

      if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX, mouseY;
        Uint32 buttons; // NOTE: button_state is 32 bit integer
        buttons = SDL_GetMouseState(&mouseX, &mouseY); // getting state
        // Make sure mouse pointer is in field (and window)
        if (mouseX<0 || mouseX>600 || mouseY<0 || mouseY>600)
          continue;
        if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT) ){ // Left mouse button
          // calculate which tile we are in and reveal it
          int i,j;
          i=mouseX/w;
          j=mouseY/w;
          pole[i][j].isRevealed=true;



          // Crap.. We actually hit cell without neighboring mines
          // TODO: Reveal only horizontal/vertical neigbors; not diagonal?
          if (pole[i][j].n==0) {
            // Flood fill time!!
            std::vector <std::pair<int,int>> pairs;
            auto pair = std::make_pair(i,j);
            pairs.push_back(pair);

            // Loop until all 0's are filled.
            while (!pairs.empty()) {
              // Get last pair out of que
              auto current = pairs.back();
              pairs.pop_back();

              for (int x = -1; x <= 1; x++) {
                for (int y = -1; y <= 1; y++) {
                  int xoff=current.first+x;
                  int yoff=current.second+y;
                  // Do not count outside array
                  if (xoff<0 || xoff>9 || yoff<0 || yoff>9)
                    continue;
                  // skip if revealed
                  if (pole[xoff][yoff].isRevealed)
                    continue;

                  // finally reveal neigbor
                  pole[xoff][yoff].isRevealed=true;
                  // if revealed tile is 0 also. push it back to be checked
                  if (pole[xoff][yoff].n==0) {
                    auto newpair = std::make_pair(xoff, yoff);
                    pairs.push_back(newpair);
                  }
                }
              }
            }
          }
        }
      }
    }

    // Show currrent frame
    SDL_RenderPresent(renderer);
    // Sleep a bit TODO: fps gapping?
    SDL_Delay(20);

  }
  SDL_Delay(200);

  // Be kind and clean afterwards
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
