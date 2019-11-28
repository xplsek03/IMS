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