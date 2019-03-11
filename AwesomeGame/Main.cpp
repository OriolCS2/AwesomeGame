#include "SDL/include/SDL.h"
#include "SDL_Mixer/include/SDL_mixer.h"


#pragma comment (lib, "SDL/libx86/SDL2.lib")
#pragma comment (lib, "SDL/libx86/SDL2main.lib")
#pragma comment (lib, "SDL_Mixer/libx86/SDL2_mixer.lib")

int main(int argc, char* argv[]) {

	SDL_Init(SDL_INIT_EVERYTHING);
	Mix_Init(SDL_INIT_AUDIO);

	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,MIX_DEFAULT_FORMAT, 2, AUDIO_S16SYS);

	Mix_Chunk *fx_sound = nullptr;
	fx_sound = Mix_LoadWAV("fx/fx.wav");

	bool loop = true;
	bool bullet_on = false;

	SDL_Window* window = SDL_CreateWindow("Awesome Game", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_Event event;

	SDL_Rect square;
	square.x = 100;
	square.y = 100;
	square.w = 100;
	square.h = 100;

	struct Bullet {

		int speed = 2;
		SDL_Rect bullet;

	};
	
	Bullet bullet;

	bullet.bullet.w = 50;
	bullet.bullet.h = 35;

	while (loop) {

		if (SDL_PollEvent(&event) != 0) {

			if (event.type == SDL_QUIT)
				loop = false;
	
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
					loop = false;
				if (event.key.keysym.scancode == SDL_SCANCODE_UP){
					if (square.y > 0) {
						square.y -= 10;
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
					if (square.x > 0) {
						square.x -= 10;
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
					if (square.y < 600 - square.h) {
						square.y += 10;
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
					if (square.x < 800 - square.w) {
						square.x += 10;
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
					Mix_PlayChannel(1,fx_sound,1);
					bullet_on = true;
					bullet.bullet.x = square.x + square.w - 10;
					bullet.bullet.y = square.y + (square.h / 2) - 15;

				}
			}
		}

		if (bullet_on) {
			if (bullet.bullet.x >= 800) {
				bullet_on = false;
			}
			SDL_SetRenderDrawColor(renderer, 127, 255, 0, 255);
			SDL_RenderFillRect(renderer, &bullet.bullet);

			bullet.bullet.x += 1;

		}

		
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(renderer, &square);


		
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		SDL_RenderPresent(renderer);
	

		SDL_RenderClear(renderer);
	}


	SDL_Quit();
	return 0;
}