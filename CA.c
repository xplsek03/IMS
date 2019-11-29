#include <iostream>
#include "SDL2/SDL.h"
#include <cmath>
#include <ctime>
#include <vector>
#include <utility>

// konstanty pocasi
float snih[] = {22,18.5,0.5,0,0,0,0,0,0,0,5,2};
int srazky[] = {46,25,23,27,35,47,43,39,98,23,14,37}; // normalni jizni moravy 35mm, tzn problem zacne od 50mm
float teplota[] = {2.27,-2,2.28,14.12,17.86,19.79,21.42,22.48,15.81,11.96,6.09,1.72};

float mortalita; // koeficient vnejsich vlivu

// funkce na vypocitani zivotni urovne z hlediska pocasi a dalsich vnejsich vlivu. Nastavuje se nad jednim konkretnim polem!
void weather(int cyklus) {
    int mortality = 0;
    if(srazky[cyklus] > 50)
      mortality += 50;
    else if(teplota < 0 && snih[cyklus] < 5)
      mortality += 20;
    for(int i = 0; i < 10; i++)
      for(int j = 0; j < 10; j++)
	mortalita = mortality;
}

// pak taky z hlediska dravcu

// funkce na orbu neni, pokud je nastavena orba zabiji proste hrabose

// jeden ar, nase minimalni jednotka
struct ar {
	int s; // stav, 0 - 3
	int x;
	int y;
	// dalsi veci jako overlay, JEDY, KVALITA ZIVOTA, PREDATORI ATD.
};

struct ar pole[10][10]; // jeden hektar = jedno pole

int main(int argc, char const *argv[]) {

  std::srand(std::time(0));
  
    SDL_Window * window;
    SDL_Renderer * renderer;
	
    int w = 30; // graficka v/s metru

    int buffer[10][10]; // buffer na meziukladani
	
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
	
	// nastav pocatecni nory
	pole[4][5].s = 1;
	pole[5][5].s = 1;

	SDL_Rect rect;

	// jeden cyklus ma 2t, rok ma 52t, tzn za rok 26 cyklu (1. ledna az 31. prosince)
	int c = 0; // pocet cyklu

	while (c < 26) {

		// AUTOMATA START
		
		// vnejsi prirodni vlivy
		
		weather(((c % 2) == 1) ? c-1 : c);
	  
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
				
				// JARO
				if(c >= 4 && c <= 9) {
				    if(sum == 0) {
					    buffer[i][j] = 0;
					    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
				    }				
				    else if(sum >= 1 && sum <= 4) {
					    buffer[i][j] = 1;
					    SDL_SetRenderDrawColor(renderer, 0xFF, 0xaa, 0x98, 0xFF);
				    }					
				    else if(sum >= 5 && sum <= 10) {
					    buffer[j][i] = 2;
					    SDL_SetRenderDrawColor(renderer, 0xFF, 0x56, 0x32, 0xFF);
				    }				
				    else if(sum >= 10 && sum <= 20) {
					    buffer[j][i] = 3;
					    SDL_SetRenderDrawColor(renderer, 0xFe, 0x41, 0x18, 0xFF);
				    }					
				    else if(sum > 20) {
					    buffer[j][i] = 4;
					    SDL_SetRenderDrawColor(renderer, 0xfe, 0x2d, 0, 0xFF);
				    }			  
				}
				// LETO
				else if(c >= 10 && c <= 15) {
				    if(sum == 0) {
					    buffer[i][j] = 0;
					    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
				    }				
				    else if(sum >= 1 && sum <= 20) {
					    buffer[i][j] = 1;
					    SDL_SetRenderDrawColor(renderer, 0xFF, 0xaa, 0x98, 0xFF);
				    }					
				    else if(sum >= 21 && sum <= 40) {
					    buffer[j][i] = 2;
					    SDL_SetRenderDrawColor(renderer, 0xFF, 0x56, 0x32, 0xFF);
				    }				
				    else if(sum >= 41 && sum <= 60) {
					    buffer[j][i] = 3;
					    SDL_SetRenderDrawColor(renderer, 0xFe, 0x41, 0x18, 0xFF);
				    }					
				    else if(sum > 61) {
					    buffer[j][i] = 4;
					    SDL_SetRenderDrawColor(renderer, 0xfe, 0x2d, 0, 0xFF);
				    }					  
				}
				
				// PODZIM
				else if(c >= 16 && c <= 21) {
				  				  
				}
				// ZIMA
				else {
				  
				}
				
				// APLIKUJ NA ARU RUZNE VLIVY, NEJDRIV VNEJSI
				if(mortalita) {
				    if(buffer[j][i] = 1)
				      buffer[j][i] = 0;
				    else if(buffer[j][i] = 2)
				      buffer[j][i] = 1;
				    else if(buffer[j][i] = 3)
				      buffer[j][i] = 1;
				    else if(buffer[j][i] = 4)
				      buffer[j][i] = 2;
				}
				
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
