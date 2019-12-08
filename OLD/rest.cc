	// Prozkoumej, jestli uzivatel na neco klika
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
    
    
    // konstanty pocasi
//float snih[] = {22,18.5,0.5,0,0,0,0,0,0,0,5,2};
//int srazky[] = {46,25,23,27,35,47,43,39,98,23,14,37}; // normalni jizni moravy 35mm, tzn problem zacne od 50mm
//float teplota[] = {2.27,-2,2.28,14.12,17.86,19.79,21.42,22.48,15.81,11.96,6.09,1.72};
//float mortalita; // koeficient vnejsich vlivu
// funkce na vypocitani zivotni urovne z hlediska pocasi a dalsich vnejsich vlivu. Nastavuje se nad jednim konkretnim polem!
//void weather(int cyklus) {
//
	//mortalita = 0;
	   //if(srazky[cyklus] > 50)
     // mortalita += 0.4;
    //else if(teplota < 0 && snih[cyklus] < 5)
     // mortalita += 0.4;
//}

					
					// JARO
					// snazime se o zacatek prudkeho rustu populace, ale ne tak prudkeho jako pres leto
					// prvni mesic jara vyrobime zachytna parici stanoviste, aby nahodou nedoslo k nejakemu "pravdepodobnostnimu problemu" :)
					//if(c == 4) {
						//if(safety < 2) {
				    		//if(rand() % 100 > 80) {
				    			//if(sum < 2) {
				    				//printf("rand start: %i %i \n",i,j);
					    			//buffer[i][j] = 2;
				    			//}			
				    			//safety++;
				    		//}
				    	//}	
					//}



					int rnd;
					if(!y)
						rnd = 2;
					else
						rnd = 3;
				    if(sum < 2) {
					    buffer[i][j] = 0;
				    }				
				    else if(sum >= 2 && sum <= 23) {
					    if(rand() % rnd == 0)
					    	buffer[i][j] = 1;
					    else
					    	buffer[i][j] = 0;
				    }								
				    else if(sum >= 23) {
					    if(rand() % rnd == 0)
					    	buffer[i][j] = 1;
					    else
					    	buffer[i][j] = 0;
				    }	
				    
								// vnejsi problemy START	
					//weather(((c % 2) == 1) ? c-1 : c);	  
					//if(mortalita)
					//	buffer[i][j] *= mortalita;
