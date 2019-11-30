#include <iostream>
#include <SDL2/SDL.h>
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

	mortalita = 0;

	if(cyklus >= 21) { // od pulky listipadu masove vymri
		mortalita += 0.15;
	}	
		
    if(srazky[cyklus] > 50)
      mortalita += 0.4;
    else if(teplota < 0 && snih[cyklus] < 5)
      mortalita += 0.4;
}

// pak taky z hlediska dravcu

// funkce na orbu neni, pokud je nastavena orba zabiji proste hrabose

// jeden ar, nase minimalni jednotka
struct ar {
	float s; // prumerny pocet hrabosu na 1 aru (max. 5.5 nebo tak)
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

    float buffer[10][10]; // buffer na meziukladani
	
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
		
			// reset bufferu
			buffer[i][j] = 0;
		
			pole[i][j].x = i;
			pole[i][j].y = j;
			pole[i][j].s = 0;
		}
	}
	
	// nastav pocatecni nory, 2 hrabosi
	buffer[4][5] = 1;
	buffer[5][5] = 2;

	SDL_Rect rect;

	// jeden cyklus ma 2t, rok ma 52t, tzn za rok 26 cyklu (1. ledna az 31. prosince)
	int c = 0; // pocet cyklu
	float nor; // DEBUG: celkovy pocet nor v cyklu
	float nor_minule = 2; // DENUG: celkovy pocet nor minuly mesic, kvuli nepareni v zime. navic zaciname na 2
	
	while (c < 24) {

		nor = 0; // DEBUG: vynuluj nory celkem

		// AUTOMATA START
		
		// vnejsi prirodni vlivy, po mesici		
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
				
				int out = 0; // pocet tech co jsou mimo mapu
				float sum = 0; // MOOROVO OKOLI = 9 prvku

				
				if(c >= 3 && c <= 21) {	// provadej jen v paricich mesicich		
					for(int a = -1; a <= 1; a++) {
						for(int b = -1; b <= 1; b++) {
							if(i+a < 0 || i+a > 9 || j+b < 0 || j+b > 9) // mimo, takze do tech co jsou mimo uloz prumer zbytku
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
				}
				else { // v neparicich obdobich jenom scitej
					nor += pole[i][j].s;
				}
				
				// if(pocet der na 9 arech) then: pocet der na jednom aru
				
				// JARO
				if(c >= 3 && c <= 9) {
				    if(sum < 2) {
					    buffer[i][j] = 0;
				    }				
				    else if(sum >= 2 && sum <= 8) { // cim bliz nastavis 8 devitce, tim pomalejc to poroste
					    buffer[i][j] = 1;
				    }					
				    else if(sum > 8 && sum <= 17) {
					    buffer[i][j] = 2;
				    }				
				    else if(sum > 17 && sum <= 26) {
					    buffer[i][j] = 3;
				    }					
				    else if(sum > 26 && sum <= 35) {
					    buffer[i][j] = 4;
				    }	
				    else if(sum > 35 && sum <= 44) {
					    buffer[i][j] = 5;
				    }	
				    else if(sum > 44) {
					    buffer[i][j] = 6;
				    }	  
				}
				// LETO
				else if(c >= 10 && c <= 15) {
				    if(sum < 2) {
					    buffer[i][j] = 0;
				    }				
				    else if(sum >= 2 && sum <= 8) {
					    buffer[i][j] = 1;
				    }					
				    else if(sum > 8 && sum <= 12) {
					    buffer[i][j] = 2;
				    }				
				    else if(sum > 12 && sum <= 18) {
					    buffer[i][j] = 3;
				    }					
				    else if(sum > 18 && sum <= 27) {
					    buffer[i][j] = 4;
				    }	
				    else if(sum > 27 && sum <= 40) {
					    buffer[i][j] = 5;
				    }	
				    else if(sum > 40) {
					    buffer[i][j] = 6;
				    }  		  
				}
				
				// PODZIM
				// vymirani s eda zpusobit tak ze nizsi stavy priradis pri vyssim sum
				else if(c >= 16 && c <= 21) {
				    if(sum < 2) {
					    buffer[i][j] = 0;
				    }				
				    else if(sum >= 2 && sum <= 11) {
					    buffer[i][j] = 1;
				    }					
				    else if(sum > 11 && sum <= 20) {
					    buffer[i][j] = 2;
				    }				
				    else if(sum > 20 && sum <= 37) {
					    buffer[i][j] = 3;
				    }					
				    else if(sum > 37 && sum <= 40) {
					    buffer[i][j] = 4;
				    }	
				    else if(sum > 40 && sum <= 50) {
					    buffer[i][j] = 5;
				    }	
				    else if(sum > 50) {
					    buffer[i][j] = 6;
				    }  			  				  
				}
				// ZIMA - POUZE KONEC, NE LEDEN/UNOR
				else if(c >= 21) {

				}
				
				// vnitrni problemy START				
				if(sum > 50)
					buffer[i][j] /= 1.5;
				
				// vnejsi problemy START
				if(mortalita) {
					buffer[i][j] *= mortalita;
				}
			
				// tady seda klidne aplikvoat neco zpetne na to okoli
				// da se sledovat hodnota, jak tu byla dlouho napr. 3 a teprve pak roznaset nemoci
				// vyrobit nove stavy, jako nemoci atd.
				
				// TEPRVE TED VYKRESLUJ, vnejsi vlivy muzou zmenit barvu
				int f_to_i = buffer[i][j];
				switch(f_to_i) {
					case 0:
						SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
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
		for(int i = 0; i < 10; i++) {
			for(int j = 0; j < 10; j++) {
				pole[i][j].s = buffer[i][j];
			}
		}
		
		c++; // pricti cyklus
		
		std::cout << "14 dni x " << c << ": " << (!nor ? (int)nor_minule : (int)nor) << "\n";
		if(nor)
			nor_minule = nor; // DEBUG: uloz pocet nor pro dalsi mesic
		if(c % 2 == 0)
			std::cout << "\n";

	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;

}
