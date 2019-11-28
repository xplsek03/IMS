#include <iostream>
#include <SDL2/SDL.h>
#include <cmath>
#include <ctime>
#include <vector>
#include <utility>

// struktura jednoho metru hektaru

	// stavy:	0 - nic
	//			1 - hrabos

struct metr {
	int x;
	int y;
	int s; // stav pole
	int jidlo; // pocet jidla na tomhle metru, 440g
	int jed; // pocet granuli na tomhle metru	
	
	// populace
	int samice;
	int tehotne_samice;
	int samci;
	int deti;
};

// globalni pole
// prochazej od zacatku celej hektar
struct metr pole[10][10];

// rndm cisla z range
int range(int min, int max) {
	return ((rand() % (max + 1 - min)) + min);
}
	
// hrabosi z jednoho pole zerou
void eat_and_stuff(int x, int y, int count) {
	
	for(int r = 1; r < 35; r++) { // radius bude 34 na kazdou stranu

		// vypocet nahodnyho smeru a trochu korekce vysledku bliz k nore 1:3
		int i = range(-r,r);
		if(range(-3,6) < 0)
			i /= 2;
		int j = range(-r,r);
		if(range(-3,6) < 0)
			j /= 2;
		
		// vygeneruj nahodne smery, kudy bude zrat. Porad se ale snazi jist co nejbliz hnizda

		
		if(x+i >= 0 && x+i < 10 && y+j >= 0 && y+j < 10) {
		
			while(pole[x+i][y+j].jidlo) {
				count--;
				pole[x+i][y+j].jidlo -= 5;
				
				if(!count)
					return;
				
			}
		}
	}
	
}	

// pareni
void screw(int x, int y, int samci) {
	for(int c = 0; c < samci; c++) {
		for(int r = 1; r < 35; r++) { // radius bude 34 na kazdou stranu

			for(int i = -r, j = -r; i <= r; i++,j++) {
				if(x+i >= 0 && x+i < 10 && y+j >= 0 && y+j < 10) {
					if((pole[x+i][y+j].samice - pole[x+i][y+j].tehotne_samice) > 0) { // pouze dostupne samice
						pole[x+i][y+j].deti += 5; // priblizne 5
						pole[x+i][y+j].samice--; // odstran samici
						pole[x+i][y+j].tehotne_samice++; // tehotna samice
						continue; // tady implementuj vytrvalost, max pocet pareni za 14 dnu
					}
				}
			}
		}		
	}
}

int main(int argc, char const *argv[]) {
	
	int celkem = 0; // celkovej pocet tech zatracenejch mysi
	
	time_t t;
	
    SDL_Window * window;
    SDL_Renderer * renderer;
    int w = 6; // graficka v/s metru
    bool isRunning = true;
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

	// HRABOS DEF
	
	// zamichej cas
	srand((unsigned) time(&t));
	
	// promenne vnejsich vlivu
	//float vlhkost;
	//float teplota; // vzduchu
	//float snih; // vrstva snehu
	//float srazky; // mm srazek
	
	// resetuj pole
	for(int i = 0; i < 10; i++) {
		for(int j = 0; j < 10; j++) {
			pole[i][j].x = i;
			pole[i][j].y = j;
			pole[i][j].jidlo = 440;
		}
	}
	
	// pridej hrabose
	pole[3][3].s = 1; // je tu hrabos
	pole[3][3].samci = 1; // je tu 1 hrabos	

	// pridej hrabose
	pole[5][5].s = 1; // je tu hrabos
	pole[5][5].samice = 1; // je tu 1 hrabos

	celkem += 2; // DEBUG

	SDL_Rect rect;

int c = 0;

	while (isRunning && c < 10) {
		
		// samec se muze pohnout o 34 poli na kazdou stranu
		// samice se muze pohnout o 9 poli na akzdou stranu, pokud neni brezi
		// nedospely samec je ulzoenej ve strukture pole
		
		// prechody:	rand, ze muze mit deti v dalsim cyklu
		// pokud je v okoli predator, rand ze umre
		// pokud je v okoli jed, rand ze sni jed
		// pokud je tam dost urody, pomer toho ze sezere jed a ne urodu
		
		// koeficient pocasi s epocita na zacaktu kazdeho cyklu a je sance, ze ten hrbaos chcipne



		// AUTOMATA START
		
		// We want to use black background
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
		// Clear the screen with current background color
		SDL_RenderClear(renderer);
		
		// DO DRAW
		//SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {

				rect.x=i*w;
				rect.y=j*w;
				rect.w=w;
				rect.h=w;

				// pokud je tam hrabos
				if(pole[i][j].s == 1) {
					
					// vyres tehotne samice
					pole[i][j].samice += pole[i][j].tehotne_samice; // tehotne zpet do pristupneho stavu
					pole[i][j].tehotne_samice = 0; // reset
					
					// vyres cerstve deti
					for(int d = 0; d < pole[i][j].deti; d++) {
						if(rand() & 1) { // je to kluk
							pole[i][j].samci++;					
							celkem++;
						}
						else { // je to holka
							pole[i][j].samice++;
							celkem++;
						}
					}
					pole[i][j].deti = 0; // smaz deti
					
					eat_and_stuff(i,j, pole[i][j].samci + pole[i][j].samice); // hrabosi se nazerou
					screw(i,j,pole[i][j].samci); // pareni

					SDL_SetRenderDrawColor(renderer, 0xFF, 0, 0, 0xFF);
					SDL_RenderFillRect(renderer, &rect);			
					SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
					SDL_RenderDrawRect(renderer, &rect);
				}
				else {
					if(!pole[i][j].jidlo) {
						SDL_SetRenderDrawColor(renderer, 0x66, 0x33, 0, 0xFF);
						SDL_RenderFillRect(renderer, &rect);		
						SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
						SDL_RenderDrawRect(renderer, &rect);						
					}
					else if(pole[i][j].jidlo != 440) {
						SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0, 0xFF);
						SDL_RenderFillRect(renderer, &rect);		
						SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
						SDL_RenderDrawRect(renderer, &rect);						
					}
					else {
						SDL_SetRenderDrawColor(renderer, 0, 0xFF, 0, 0xFF);
						SDL_RenderFillRect(renderer, &rect);		
						SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
						SDL_RenderDrawRect(renderer, &rect);						
					}
					

				}
				
			}
		}
		
		// AUTOMATA END


		// Show currrent frame
		SDL_RenderPresent(renderer);
		
		// Sleep a bit TODO: fps gapping?
		SDL_Delay(1000); // 50
		
		std::cout << "cyklus " + c << '\n';
		std::cout << "populace: " << celkem << "\n\n";
		c++;

  }

  // Be kind and clean afterwards
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
