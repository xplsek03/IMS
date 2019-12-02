#include <iostream>
#include <SDL2/SDL.h>
#include <cmath>
#include <ctime>
#include <vector>
#include <utility>
#include <time.h>

// jeden ar, nase minimalni jednotka
struct ar {
	float s; // prumerny pocet hrabosu na 1 aru (max. 5.5 nebo tak)
	int x;
	int y;
	// dalsi veci jako overlay, JEDY, KVALITA ZIVOTA (POLE), predatori atd.
	int plodina; // typ plodiny ktera tam je
	int livable; // jestli se tam ma sirit, protoze tam je co jist
	
};

struct ar pole[100][100]; // jeden hektar = jedno pole

int main(int argc, char const *argv[]) {

  	srand(time(0));
  
    SDL_Window * window;
    SDL_Renderer * renderer;
	
    int w = 6; // graficka v/s metru

    float buffer[100][100]; // buffer na meziukladani
	
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
	
	// resetuj pole a napln typem potravin
	for(int i = 0; i < 100; i++) {
		for(int j = 0; j < 100; j++) {
		
			// reset bufferu
			buffer[i][j] = 0;
		
			if(i < 50) {
				pole[i][j].plodina = 0; // remiz
			}
			else {
				pole[i][j].plodina = 1; // ozima psenice
			}
		
			pole[i][j].livable = 1; // pole je ze zacatku obyvatelne
			pole[i][j].x = i;
			pole[i][j].y = j;
			pole[i][j].s = 0;
		}
	}
	
	// nastav pocatecni nory
	for(int i = 0; i < 5; i++) {
		int x = 10 + rand() % 91;
		int y = 10 + rand() % 91;
		
		for(int a = -1; a <= 1; a++) {
			for(int b = -1; b <= 1; b++) {
				if(rand() & 1) { // 1:2
					pole[x+a][y-b].s = 1 + rand() % 4;	
				}
			}
		}
			
		
	}

	SDL_Rect rect;

	// jeden cyklus ma 2t, rok ma 52t, tzn za rok 26 cyklu (1. ledna az 31. prosince)
	int y = 0; // rok 0/1
	int c = 4; // zaciname na zacatku brezna prvniho roku
	float nor; // DEBUG: celkovy pocet nor v cyklu
	int safety; // kolik zachytnejch nor se objevi na jare na 1ha
	int x; // pouzity pocet aru
	
	while (c < 24) {

		nor = 0; // DEBUG: vynuluj nory celkem
		safety = 0;
		x = 0;
		
		// AUTOMATA START

		// We want to use black background
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
		// Clear the screen with current background color
		SDL_RenderClear(renderer);
		
		for(int i = 0; i < 100; i++) {
			for(int j = 0; j < 100; j++) {

				// nachystej si ctverec
				rect.x=i*w;
				rect.y=j*w;
				rect.w=w;
				rect.h=w;

				// neni to obyvatelne
				if(!pole[i][j].livable) {
				
					// nejaci hrabosi tam jsou, presidlit tam kde je jidlo
					if(pole[i][j].s) {
						int found = 0; // nalezeno nove stanoviste
						int x = 0;
						int y = 0;
						while(!found) {
							x = rand() % 50;
							y = rand() % 50;
							if(!pole[x][y].s) {
								found = 1;
								buffer[x][y] = pole[i][j].s;
							}								
						}
					}
					buffer[i][j] = 0;
					
				}
				else {
					
					int out = 0; // pocet tech co jsou mimo mapu
					float sum = 0; // MOOROVO OKOLI = 9 prvku
		
					for(int a = -1; a <= 1; a++) {
						for(int b = -1; b <= 1; b++) {
							if(i+a < 0 || i+a > 99 || j+b < 0 || j+b > 99) // mimo, takze do tech co jsou mimo uloz prumer zbytku
								out++;
							else { // scitej nory v akru
								sum += pole[i+a][j+b].s;
							}
						}
					}
					// uloz do sum prumer prvku mimo mapu
					if(out) {
						sum += ((sum/(9-out))*out);
					}
					nor += sum; // uloz z jednoho aru pocet nor				
					
					if(sum)
						x++;
					
					// soucasti nastavenych automatu jsou automaticti dravci a prumerne teploty. extremy se zapocitaji zvlast.
					// v prvnich dvou letech je delsi rozmnozovaci obdobi, v ramci gradace
	
					// JARO
					if(c >= 4 && c <= 9) { // c > 4 pokud zapnuta nahoda na jare
				    	if(sum < 2) {
					    	buffer[i][j] = 0;
				    	}				
				    	else if(sum >= 2 && sum <= 8) {
					    	buffer[i][j] = 1;
				    	}					
				    	else if(sum > 8 && sum <= 19) {
					    	buffer[i][j] = 2;
				    	}				
				    	else if(sum > 19 && sum <= 29) {
					    	buffer[i][j] = 3;
				    	}					
				    	else if(sum > 29 && sum <= 40) {
					    	buffer[i][j] = 4;
				    	}	
				    	else if(sum > 40) {
					    	buffer[i][j] = 5;
				    	}		  
					}
					// LETO
					// populacni boom v plne parade
					else if(c >= 10 && c <= 15) {
				    	if(sum < 2) {
					    	buffer[i][j] = 0;
				    	}				
				    	else if(sum >= 2 && sum <= 8) {
					    	buffer[i][j] = 1;
				    	}					
				    	else if(sum > 8 && sum <= 10) {
					    	buffer[i][j] = 2;
				    	}				
				    	else if(sum > 10 && sum <= 17) {
					    	buffer[i][j] = 3;
				    	}					
				    	else if(sum > 17 && sum <= 29) {
					    	buffer[i][j] = 4;
				    	}	
				    	else if(sum > 29 && sum <= 42) {
					    	buffer[i][j] = 5;
				    	}	
				    	else if(sum > 42 && sum <= 50) {
					    	buffer[i][j] = 6;
				    	} 
				    	else if(sum > 50 && y == 0)
				    		buffer[i][j] = 5; 		  
					}
					
					// PODZIM 1. polovina - drzi se
					// postupne zacne stagnovat
					else if(c >= 16 && c <= 18) {
				    	if(sum < 2) {
					    	buffer[i][j] = 0;
				    	}				
				    	else if(sum >= 2 && sum <= 11) {
					    	buffer[i][j] = 1;
				    	}								
				    	else if(sum > 11 && sum <= 40) {
					    	buffer[i][j] = 2;
				    	}
				    	else if(sum > 40) {
					    	if(rand() & 1)
					    		buffer[i][j] = 3;
					    	else
					    		buffer[i][j] = 2;
				    	}		 							  
					}
					// PODZIM 2. polovina - upada
					// postupne zacne vymirat
					else if(c >= 19 && c <= 21) {
				    	if(sum < 4) {
					    	buffer[i][j] = 0;
				    	}				
				    	else if(sum >= 4 && sum <= 23) {
					    	if(rand() & 1)
					    		buffer[i][j] = 1;
					    	else
					    		buffer[i][j] = 0;
				    	}								
				    	else if(sum >= 23) {
					    	if(rand() & 1)
					    		buffer[i][j] = 1;
					    	else
					    		buffer[i][j] = 0;
				    	}		  				  
					}
					// ZIMA
					else {
						if(buffer[i][j] > 0) {
							if(rand() % 3 == 0)
								buffer[i][j]--;
						}
					}				
					
				}
				
				if(c == 16) { // hluboka orba v zari
					if(i >= 50)
						pole[i][j].livable = 0;
					if(rand() % 100 < 80) // prezije takovych 10% populace
						buffer[i][j] = 0;
				}
				
				if(c == 22) { // hrabos uz se muze sirit do ozimu
					if(i >= 50)
						pole[i][j].livable = 1;
				}
				
				// TEPRVE TED VYKRESLUJ, vnejsi vlivy muzou zmenit barvu
				int f_to_i = buffer[i][j];
				switch(f_to_i) {
					case 0:
						if(!pole[i][j].livable)
							SDL_SetRenderDrawColor(renderer, 0x8B, 0x45, 0x13, 0xFF);
						else if(i < 50)
							SDL_SetRenderDrawColor(renderer, 0x22, 0x8B, 0x22, 0xFF);
						else
							SDL_SetRenderDrawColor(renderer, 0xDA, 0xA5, 0x20, 0xFF);
						break;
					case 1:
						SDL_SetRenderDrawColor(renderer, 0xFF, 0xaa, 0x98, 0xFF);
						break;
					case 2:
						SDL_SetRenderDrawColor(renderer, 0xFF, 0x56, 0x32, 0xFF);
						break;
					case 3:
						SDL_SetRenderDrawColor(renderer, 0xFe, 0x41, 0x18, 0xFF);
						break;
					case 4:
						SDL_SetRenderDrawColor(renderer, 0xfe, 0x2d, 0, 0xFF);
						break;
					case 5:
						SDL_SetRenderDrawColor(renderer, 0xe5, 0x28, 0, 0xFF);	
						break;
					case 6:
						SDL_SetRenderDrawColor(renderer, 0xb2, 0x1f, 0, 0xFF);
				}				
				
				SDL_RenderFillRect(renderer, &rect);
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
				SDL_RenderDrawRect(renderer, &rect);						

			}
		}
		
		// AUTOMATA END
		
		// vykresli frejm
		SDL_RenderPresent(renderer);
		SDL_Delay(100);
		
		// prehraj data na dalsi kolo
		for(int i = 0; i < 100; i++) {
			for(int j = 0; j < 100; j++) {
				pole[i][j].s = buffer[i][j];
			}
		}
		
		c++; // pricti cyklus
				
		std::cout << "rok: " << y+1 << "\tmesic: " << ((c%2 == 0) ? c/2 : ((c+1)/2)) << "\tnor: "<< nor << "\thrabosu: " << nor/2.5 << " - " <<  nor/2 << "\thrabosu na pouzitych hektarech: " << (nor/2.5)/(x/100) << "\n";

		if(c == 24 && !y) { // pokracuj druhym rokem
			y++; // pricti rok
			c = 0; // jed druhy rok
		}

	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;

}
