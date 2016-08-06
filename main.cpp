#include <math.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include "render.h"
#include "map.h"
#include <SDL2/SDL.h>

using namespace std;

#define GAME_QUIT 0
#define GAME_LOADERROR 1
#define GAME_OVER 2
#define GAME_END 3

int gameLoop(Map *map, Render *render){
	SDL_Event event;
	bool running = true;
	long frames = 0;
	while(running){
		render->drawFrame();
		map->doPhysics();
		if(map->isLose()){
			if(map->getLives() < 0){
				running = false;
				return GAME_OVER;
			} else{SDL_GetTicks();
				if(map->blockAmount <= 0){
					if(!map->nextLevel()){
						running = false;
						return GAME_END;
					}
					map->addPoints(POINTS_NEXTLEVEL);
				} else{
					map->restartLevel();
				}
			}
		}
		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		SDL_PumpEvents();
		SDL_PollEvent(&event);
		if(keys[SDL_SCANCODE_ESCAPE] || event.type == SDL_QUIT){
			running = false;
			return GAME_QUIT;
		}
		if(keys[SDL_SCANCODE_RIGHT])
			map->movePalette(PALETTE_SPEED);
		if(keys[SDL_SCANCODE_LEFT])
			map->movePalette(-PALETTE_SPEED);
		if(keys[SDL_SCANCODE_S])
			map->undockBalls();
		if(keys[SDL_SCANCODE_C] && DEBUG_MODE)
			map->createBall(map->getBallAmount() % 6, true);
		frames++;
	}
	return GAME_OVER;
}

int main(int argc, char **argv) {
	srand(time(NULL));
	Map *map = new Map();
	Render *render = new Render(map);
	bool newGame = true;
	while(newGame){
		map->setPalette(PALETTE_DEFAULT, PALETTE_SMALL, true);
		if(!map->loadMap(LEVEL_FILE)){
			printf("[ERROR]\tCannot open level file\n");
			return -1;
		}
		switch(gameLoop(map, render)){
			default:
				return 0;
			case GAME_OVER:
				if(!render->dialogYesNo("Przegrales. Grasz jeszcze raz?")) {
					newGame = false;
				} else{
					newGame = true;
				}
				break;
			case GAME_END:
				if(!render->dialogYesNo("To byl ostatni poziom. Grasz od poczatku?")) {
					newGame = false;
				} else{
					newGame = true;
				}
				break;
		}
	}
	return 0;
};
