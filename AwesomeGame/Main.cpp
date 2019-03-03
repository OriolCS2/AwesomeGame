#include "SDL/include/SDL.h"

#pragma comment (lib, "SDL/libx86/SDL2.lib")
#pragma comment (lib, "SDL/libx86/SDL2main.lib")

int main(int argc, char* argv[]) {

	SDL_Init(SDL_INIT_EVERYTHING);

	bool loop = true;

	SDL_Window* window = SDL_CreateWindow("Awesome Game", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_Event event;

	SDL_Rect square;
	square.x = 100;
	square.y = 100;
	square.w = 100;
	square.h = 100;

	while (loop) {

		if (SDL_PollEvent(&event) != 0) {

			if (event.type == SDL_QUIT)
				loop = false;
	
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
					loop = false;
				if (event.key.keysym.scancode == SDL_SCANCODE_W) {
					if (square.y > 0) {
						square.y -= 10;
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_A) {
					if (square.x > 0) {
						square.x -= 10;
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_S) {
					if (square.y < 600 - square.h) {
						square.y += 10;
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_D) {
					if (square.x < 800 - square.w) {
						square.x += 10;
					}
				}
			}
		}

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(renderer, &square);

		SDL_RenderPresent(renderer);
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		SDL_RenderClear(renderer);
	}
	SDL_Quit();
	return 0;
}