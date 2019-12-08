#include <iostream>
#include <SDL2/SDL.h>
#include <cmath>
#include <ctime>
#include <vector>
#include <utility>

// jeden ar, nase minimalni jednotka
struct ar {
	int s; // stav, 0 - 3
	int x;
	int y;
	// dalsi veci jako overlay, JEDY, KVALITA ZIVOTA, PREDATORI ATD.
};

int main(int argc, char const *argv[]) {

    SDL_Window * window;
    SDL_Renderer * renderer;
	
    int w = 30; // graficka v/s metru

	int buffer[10][10]; // buffer na meziukladani
	struct ar pole[10][10]; // jeden hektar
	
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
	
	// resetuj pole
	for(int i = 0; i < 10; i++) {
		for(int j = 0; j < 10; j++) {
			pole[i][j].x = i;
			pole[i][j].y = j;
			pole[i][j].s = 0;
		}
	}
	
	pole[4][5].s = 3;
	pole[5][5].s = 2;

	SDL_Rect rect;

	int c = 0; // pocet cyklu

	while (c < 10) {

		// AUTOMATA START
		
		// We want to use black background
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
		// Clear the screen with current background color
		SDL_RenderClear(renderer);
		
		for(int i = 0; i < 10; i++) {
			for(int j = 0; j < 10; j++) {

				// nachystej si ctverec
				rect.x=i*w;
				rect.y=j*w;
				rect.w=w;
				rect.h=w;
				
				int sum = pole[i][j].s; // MOOROVO OKOLI = 9 prvku
				for(int a = -1; a <= 1; a++) {
					for(int b = -1; b <= 1; b++) {
						if(i+a < 0 || i+a > 9 || j+b < 0 || j+b > 9) // mimo
							;
						else { // scitej nory v akru
							sum += pole[i+a][j+b].s;
						}
					}
				}
				
				// analyza jednoho aru
				if(sum == 0) {
					buffer[i][j] = 0;
					SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
				}					
				else if(sum >= 1 && sum <= 4) {
					buffer[i][j] = 1;
					SDL_SetRenderDrawColor(renderer, 0xFF, 0xC3, 0xB7, 0xFF);
				}					
				else if(sum >= 5 && sum <= 20) {
					buffer[j][i] = 2;
					SDL_SetRenderDrawColor(renderer, 0xFF, 0x6B, 0x4B, 0xFF);
				}					
				else {
					buffer[j][i] = 3;
					SDL_SetRenderDrawColor(renderer, 0xDE, 0x27, 0, 0xFF);
				}
				
				// APLIKUJ NA ARU RUZNE VLIVY, VNITRNI I VNEJSI
				
				// extremni prehrabosovani, vnitrni vliv
				if(sum > 20) {
					buffer[i][j] = 2;
				}
				// tady seda klidne aplikvoat neco zpetne na to okoli
				// da se sledovat hodnota, jak tu byla dlouho napr. 3 a teprve pak roznaset nemoci
				// vyrobit nove stavy, jako nemoci atd.
				
				// vymysli jak delat krokovani po 20 dnech, s tim jak si postupne stavi nory
				
				// vykresli ten ctverecek
				SDL_RenderFillRect(renderer, &rect);
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
				SDL_RenderDrawRect(renderer, &rect);						

			}
		}
		
		// AUTOMATA END
		
		// vykresli frejm
		SDL_RenderPresent(renderer);
		SDL_Delay(1000);
		
		// prehraj data na dalsi kolo
		for(int i = 0; i < 10; i++) {
			for(int j = 0; j < 10; j++) {
				pole[i][j].s = buffer[i][j];
			}
		}
		
		c++; // pricti cyklus

	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;

}
