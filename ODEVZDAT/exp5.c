#include <iostream>
//#include <SDL2/SDL.h>
#include <cmath>
#include <ctime>
#include <vector>
#include <utility>
#include <time.h>

// maxima na jeden ctverecek moorova okoli, 1.4 aru
#define T_MAX 31.5
#define PS_MAX 32.76

// jeden ar, nase minimalni jednotka
struct ar {
	float s; // prumerny pocet hrabosu na 1 aru
	// x, y souradnice
	int x;
	int y;
	// dalsi veci jako JEDY, livable, dravci atd.
	int plodina; // 0 = tolice, 1 = psenice
	int livable; // pole je obyvatelne
	int zasoba_stutoxu; // pocet granuli stutoxu za obdobi. max 128 granuli na ctverec pri 2hg/ha, pri 10kg/ha 5*128
	int stutox; // aktualni stutox na poli
};

// ukazalo se ze mezi polem 2,3,4 je aktivni potok, takze prestoze k nim mame statistiky, pole 3 a 4 byla zrusena
// to je duvod proc je rozloha automatu 100x43, puvodni rozloha pole 1 a 2 byla uchovana, aby se zajistila kompatibilita, hodnoty jsou vypocitany pro 1.4 aru

struct ar pole[43][100]; // jeden hektar = jedno pole

// uprav trochu aby se to menilo
float distr(float a) {
	return ((float)rand()/(float)(RAND_MAX)) * a;
}

// vrat nahodne cislo z rozsahu
int range(int min,int max) { 
	return (rand() % (max - min + 1)) + min; 
} 

int main(int argc, char const *argv[]) {

  	srand(time(0));
  
    //SDL_Window * window;
    //SDL_Renderer * renderer;
	
    //int w = 6; // graficka v/s metru

    float buffer[43][100]; // buffer na meziukladani
	
    // inicializace SDL
    //if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    //    std::cout << "Chyba SDL: " << SDL_GetError() << '\n';
    //   return -1;
    //}
    // vytvoreni SDL okna
    //if (SDL_CreateWindowAndRenderer(600, 600,SDL_WINDOW_OPENGL, &window, &renderer) < 0) {
    //    std::cout << "Chyba SDL: " << SDL_GetError() << '\n';
    //    SDL_DestroyRenderer(renderer);
    //    SDL_DestroyWindow(window);
    //    SDL_Quit();
    //    return -2;
    //}
	
	// resetuj pole a napln typem potravin, taky pocatecnim mnozstvim
	for(int i = 0; i < 43; i++) {
		for(int j = 0; j < 100; j++) {
		
			// reset bufferu
			buffer[i][j] = 0;
		
			if(i <= 17) {
				pole[i][j].plodina = 0; // pole 1, tolice
			}
			else if(i >= 18 && i < 43) {
				pole[i][j].plodina = 1; // pole 2, ozima psenice
			}
			//else { // POLE 3 a 4 ZRUSENO
			//	pole[i][j].plodina = 0; // pole 3 a 4, tolice
			//}
			
			// napln promennymi
			pole[i][j].x = i;
			pole[i][j].y = j;
			
			if(i < 43) // deaktivuj pole 3 a 4
				pole[i][j].livable = 1;

			// NAPLN ZASOBY JEDU
			pole[i][j].zasoba_stutoxu = 128; // 2kg/ha, *5 pro 10kg
			pole[i][j].stutox = 0; // aktualni stutox
			
			// napln hrabosi pocatecnimi
			if(pole[i][j].plodina) {
				pole[i][j].s = range(1,7); // prepocteno z hrubych hodnot na jare z ha na 1.4 aru, coz je ctverec jeden
			}
			else
				pole[i][j].s = range(11,14); // prepocteno taktez
		}
	}

	//SDL_Rect rect;

	// jeden cyklus ma 2t, rok ma 52t, tzn za rok 26 cyklu (1. ledna az 31. prosince)
	int c = 4; // zaciname na zacatku brezna prvniho roku
	float nor_ps; // DEBUG: celkovy pocet nor v psenici
	float nor_tol; // DEBUG: celkovy pocet nor v tolici
	int y = 0; // prvni rok, z druehho roku jen leden a unor, za se do jara
	
	while (c != 4 || !y) {

		nor_ps = 0;
		nor_tol = 0; // DEBUG: vynuluj nory celkem
		
		// AUTOMATA START

		// We want to use black background
		//SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
		// Clear the screen with current background color
		//SDL_RenderClear(renderer);
		
		for(int i = 0; i < 43; i++) {
			for(int j = 0; j < 100; j++) {

				// nachystej si ctverec
				//rect.x=i*w;
				//rect.y=j*w;
				//rect.w=w;
				//rect.h=w;
				
				// HLUBOKA ORBA V ZARI
				if(c == 16) {
					if(pole[i][j].plodina) { // psenice
						pole[i][j].livable = 0;
						if(rand() % 100 < 95) // prezije takovych 5% populace max
							buffer[i][j] = 0;
					}
				}

				// MIGRACE START = k migraci jsem nakonec nemel zadna poradna data
				if(!pole[i][j].livable) {

				    	//	if(sum > 220 & sum <= 250) {
					    	//	buffer[i][j] = sum/9 + 1.31; // s1
				    	//	}					
				    	//	else if(sum > 120 && sum <= 170) {
					    //		buffer[i][j] = sum/9 + 1.1; // s2
				    	//	}				
				    	//	else if(sum > 170 && sum <= 180) {
					    //		buffer[i][j] = sum/9 + 1.2; // s3
				    	//	}					
				    	//	else if(sum > 180 && sum <= 220) {
					    //		buffer[i][j] = sum/9 + 1.2; // s4
				    	//	}	
					
				}
				else { // OBYVATELNE START
					
					int out = 0; // pocet tech co jsou mimo mapu
					float sum = 0; // MOOROVO OKOLI = 9 prvku
		
					for(int a = -1; a <= 1; a++) {
						for(int b = -1; b <= 1; b++) {
							if(i+a < 0 || i+a > 99 || j+b < 0 || j+b > 99) // mimo, takze do tech co jsou mimo uloz prumer zbytku
								out++;
							else if((i == 42 && a == 1) || (i == 43 && a == -1)) // potok puli obe casti
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
					
					// uloz celkovy pocet nor
					if(pole[i][j].plodina) // psenice
						nor_ps += pole[i][j].s;			
					else
						nor_tol += pole[i][j].s; // tolice

					
					// JARO
					if(c >= 4 && c <= 9) {

						// psenice
						if(pole[i][j].plodina) {

				    		if(sum <= 0.7*PS_MAX) {
					    		buffer[i][j] = 0; // s0
				    		}				
				    		else if(sum > 0.7*PS_MAX & sum <= 1.3*PS_MAX) {
					    		buffer[i][j] = 6.5; // s1
				    		}					
				    		else if(sum > 1.3*PS_MAX && sum <= 3*PS_MAX) {
					    		buffer[i][j] = 13.1; // s2
				    		}				
				    		else if(sum > 3*PS_MAX && sum <= 4*PS_MAX) {
					    		buffer[i][j] = 19.6; // s3
				    		}					
				    		else if(sum > 4*PS_MAX && sum <= 5*PS_MAX) {
					    		buffer[i][j] = 26.2 - distr(9-c); // s4
				    		}	
				    		else if(sum > 5*PS_MAX) {
					    		buffer[i][j] = 26.7 - distr(9-c); // s5
				    		}	
						}	
						else { // tolice
		
				    		if(sum > 60 & sum <= 120) {
					    		buffer[i][j] = sum/9 + 1.31; // s1
				    		}					
				    		else if(sum > 120 && sum <= 150) {
					    		buffer[i][j] = sum/9 + 0.9; // s2
				    		}				
				    		else if(sum > 150 && sum <= 180) {
					    		buffer[i][j] = sum/9 + 0.7; // s3
				    		}					
				    		else if(sum > 180 && sum <= 200) {
					    		buffer[i][j] = sum/9 + 1.1; // s4
				    		}	
						}		    	
				    			
					}
					// LETO
					// populacni boom v plne parade
					else if(c >= 10 && c <= 15) {
						// psenice
						if(pole[i][j].plodina) {

							if(c == 15)
								buffer[i][j] = 32.76;	
				    		else if(sum <= 0.7*PS_MAX) {
					    		buffer[i][j] = 0; // s0
				    		}				
				    		else if(sum > 0.7*PS_MAX & sum <= 1.3*PS_MAX) {
					    		buffer[i][j] = 6.5; // s1
				    		}					
				    		else if(sum > 1.3*PS_MAX && sum <= 3.2*PS_MAX) {
					    		buffer[i][j] = 13.1; // s2
				    		}				
				    		else if(sum > 3.2*PS_MAX && sum <= 4.1*PS_MAX) {
					    		buffer[i][j] = 19.6; // s3
				    		}					
				    		else if(sum > 4.1*PS_MAX && sum <= 4.6*PS_MAX) {
					    		buffer[i][j] = 26.2 - distr(15-c); // s4
				    		}	
				    		else if(sum > 4.6*PS_MAX) {
					    		buffer[i][j] = 32.76 - distr(15-c); // s5
				    		}	
						}
						else {
				    		if(sum > 220 & sum <= 250) {
					    		buffer[i][j] = sum/9 + 1.31; // s1
				    		}					
				    		else if(sum > 120 && sum <= 170) {
					    		buffer[i][j] = sum/9 + 1.1; // s2
				    		}				
				    		else if(sum > 170 && sum <= 180) {
					    		buffer[i][j] = sum/9 + 1.2; // s3
				    		}					
				    		else if(sum > 180 && sum <= 220) {
					    		buffer[i][j] = sum/9 + 1.2; // s4
				    		}	
						}
	  
					}
					
					// PODZIM PRO TOLICI POUZE, rozdeleno kvuli orbe
					else if(c >= 16 && c <= 21 && !pole[i][j].plodina) {
						if(c < 19)
							buffer[i][j] = pole[i][j].s * 1.058; // konstanta
						else
							buffer[i][j] = pole[i][j].s * 1.143; // konstanta
					}
					
					// PODZIM - drzi se a jeste roste, ale ne uz tak moc
					else if(c >= 18 && c <= 21) {
					
						// psenice
						if(pole[i][j].plodina) { // 80->100->560
						
							if(c == 18)
								buffer[i][j] = pole[i][j].s * 1.25; // udaj
							else if(c < 21)
								buffer[i][j] = pole[i][j].s * 2.43; // konstanta vypocitana
						}
								  
					}

					// ZIMA
					else {
						buffer[i][j] = pole[i][j].s * 0.6;
					}				
					
				}

				// VLIV STUTOXU 2 - od zacatku srpna do zacatku prosince, max. 120 dni
				if(c == 21 && pole[i][j].plodina) {				
					// nasyp jed na pole
					if(pole[i][j].zasoba_stutoxu > 0) { // jeste muzeme nasypat jed
						int mn = range(2,4) * buffer[i][j]; // 2-4 granuli na noru při 2kg/ha
						if((int)mn > pole[i][j].zasoba_stutoxu) {// nasyp zbytek stutoxu do nor
							pole[i][j].stutox = pole[i][j].zasoba_stutoxu;
							pole[i][j].zasoba_stutoxu = 0; // stutox dosel
						}
						else { // dosyp stutox
							pole[i][j].stutox = (int)mn;
							pole[i][j].zasoba_stutoxu = pole[i][j].zasoba_stutoxu - (int)mn;
						}
						
						int stav = (int)buffer[i][j]; // aktualni stav na poli
						int ubytek = 0; // ubytek hrabosu, pak zmen buffer touto hodnotou
											
						// pockej na vysledek
						for(int a = 0; a < stav; a++) {
						
							if(((rand() % 100) < 75) && (ubytek < stav) && (pole[i][j].stutox > 0)) { // ucinnost jedu 95% o neco snizena velkoplosnou aplikaci, navic snizeno opakovanou navnadou
								ubytek++;
								pole[i][j].stutox--;
							}
						}
						
						// spocitej ubytky
						if((buffer[i][j] - ubytek) < 0)
							buffer[i][j] = 0;
						else
							buffer[i][j] -= ubytek;
						
						if(pole[i][j].plodina) { // psenice
							if((nor_ps - ubytek) < 0)
								nor_ps = 0;
							else
								nor_ps -= ubytek;
						}
						else { // tolice
							if((nor_tol - ubytek) < 0)
								nor_tol = 0;
							else
								nor_tol -= ubytek;
						}
					
					}
					pole[i][j].stutox = 0; // stutox se rozpadne po 2-3 dnech					
				}

				
				// TEPRVE TED VYKRESLUJ, vnejsi vlivy muzou zmenit barvu
				
				//if(pole[i][j].plodina) { // psenice

				//	if(buffer[i][j] == 0)
				//		SDL_SetRenderDrawColor(renderer, 0xDA, 0xA5, 0x20, 0xFF);
	
				//	else if(buffer[i][j] <= 7)
				//		SDL_SetRenderDrawColor(renderer, 0xFF, 0xaa, 0x98, 0xFF);
					
				//	else if(buffer[i][j] <= 14)
				//		SDL_SetRenderDrawColor(renderer, 0xFF, 0x56, 0x32, 0xFF);
					
				//	else if(buffer[i][j] <= 21)
				//		SDL_SetRenderDrawColor(renderer, 0xFe, 0x41, 0x18, 0xFF);

				//	else if(buffer[i][j] <= 28)
				//		SDL_SetRenderDrawColor(renderer, 0xfe, 0x2d, 0, 0xFF);

				//	else
				//		SDL_SetRenderDrawColor(renderer, 0xe5, 0x28, 0, 0xFF);	
			
				//}
				//else {
				
					// tohle je pro tolici
					//if(buffer[i][j] == 0)
					//	SDL_SetRenderDrawColor(renderer, 0x22, 0x8B, 0x22, 0xFF);
	
					//else if(buffer[i][j] <= 20)
					//	SDL_SetRenderDrawColor(renderer, 0xFF, 0xaa, 0x98, 0xFF);
					
					//else if(buffer[i][j] <= 40)
					//	SDL_SetRenderDrawColor(renderer, 0xFF, 0x56, 0x32, 0xFF);
					
					//else if(buffer[i][j] <= 60)
					//	SDL_SetRenderDrawColor(renderer, 0xFe, 0x41, 0x18, 0xFF);

					//else if(buffer[i][j] <= 80)
					//	SDL_SetRenderDrawColor(renderer, 0xfe, 0x2d, 0, 0xFF);

					//else
					//	SDL_SetRenderDrawColor(renderer, 0xe5, 0x28, 0, 0xFF);	
				
				//}						
				
				//SDL_RenderFillRect(renderer, &rect);
				//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
				//SDL_RenderDrawRect(renderer, &rect);						

			}
		}
		
		// AUTOMATA END
		
		// vykresli snimek
		//SDL_RenderPresent(renderer);
		//SDL_Delay(100);
		
		// prehraj data na dalsi kolo
		for(int i = 0; i < 43; i++) {
			for(int j = 0; j < 100; j++) {
				pole[i][j].s = buffer[i][j];
			}
		}

		char const *month; // mesic
		if(c < 2)
			month = "leden";
		else if(c < 4)
			month = "unor";
		else if(c < 6)
			month = "brezen";
		else if(c < 8)
			month = "duben";
		else if(c < 10)
			month = "kveten";
		else if(c < 12)
			month = "cerven";
		else if(c < 14)
			month = "cervenec";
		else if(c < 16)
			month = "srpen";
		else if(c < 18)
			month = "zari";
		else if(c < 20)
			month = "rijen";
		else if(c < 22)
			month = "listopad";
		else
			month = "prosinec";

		std::cout << "mesic: " << month << "\tnor v psenici: "<< (int)nor_ps << "\tnor psen./1ha: " << (int)((nor_ps/2500)*71.428) << "\tnor v tolici: " << (int)nor_tol << "\tnor tol./1ha: " << (int)((nor_tol/1800)*71.428) << "\n";
		
		c++; // pricti cyklus

		// pole se stava obyvatelne zacatkem rijna, uz to tam roste, naszp tam hrabose
		if(c == 18) {
			for(int i = 0; i < 43; i++) {
				for(int j = 0; j < 100; j++) {
					if(pole[i][j].plodina) {
						pole[i][j].s = 1.24 - distr(0.24); // prumerne 1.12
						pole[i][j].livable = 1;
					}
				}
			}		
		}

		if(c == 24 && !y) { // pokracuj druhym rokem
			y++; // pricti rok
			c = 0; // jed druhy rok
		}

	}

	//SDL_DestroyRenderer(renderer);
	//SDL_DestroyWindow(window);
	//SDL_Quit();
	return 0;

}
