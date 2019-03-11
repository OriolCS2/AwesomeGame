#include "SDL/include/SDL.h"
#include "SDL_Mixer/include/SDL_mixer.h"


#pragma comment (lib, "SDL/libx86/SDL2.lib")
#pragma comment (lib, "SDL/libx86/SDL2main.lib")
#pragma comment (lib, "SDL_Mixer/libx86/SDL2_mixer.lib")

#define WINDOW_WIDTH 1100
#define WINDOW_HEIGHT 700
#define MAX_ACTIVE_BULLETS 5
#define MAX_ACTIVE_ENEMIES 5


struct PlayerInput {
	bool pressing_W = false;
	bool pressing_A = false;
	bool pressing_S = false;
	bool pressing_D = false;
}; 

struct Bullet {
	int speed;
	SDL_Rect bullet;
	Bullet() {
		bullet.x = 0;
		bullet.y = 0;
		bullet.w = 30;
		bullet.h = 20;
		speed = 1;
	}
};

Bullet* active_bullets[MAX_ACTIVE_BULLETS];

struct Enemy {
	SDL_Rect enemy_rect;
	int speed;
	Enemy(int x, int y) {
		enemy_rect.x = x;
		enemy_rect.y = y;
		enemy_rect.w = 80;
		enemy_rect.h = 40;
		speed = 1;
	}
};

Enemy* active_enemies[MAX_ACTIVE_ENEMIES];

void Input();
void CreateBullet();
void MoveBullets(SDL_Renderer* renderer);
void CreateEnemies();
void MoveEnemies(SDL_Renderer* renderer);


SDL_Event event;

SDL_Rect square{ 100,100,100,100 };
int square_speed = 1;

PlayerInput player_input;
bool loop = true;
bool enemies_created = false;

int main(int argc, char* argv[]) {

	SDL_Init(SDL_INIT_EVERYTHING);
	Mix_Init(SDL_INIT_AUDIO);

	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,MIX_DEFAULT_FORMAT, 2, AUDIO_S16SYS);

	Mix_Chunk *fx_sound = nullptr;
	fx_sound = Mix_LoadWAV("fx/fx.wav");

	SDL_Window* window = SDL_CreateWindow("Awesome Game", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	while (loop) {

		if (!enemies_created)
			CreateEnemies();

		Input();
		MoveBullets(renderer);
		MoveEnemies(renderer);

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(renderer, &square);
		
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		SDL_RenderPresent(renderer);
	

		SDL_RenderClear(renderer);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

void Input()
{
	if (SDL_PollEvent(&event) != 0) {
		switch (event.type) {
		case SDL_QUIT:
			loop = false;
			break;
		case SDL_KEYDOWN: {
			switch (event.key.keysym.scancode) {
			case SDL_SCANCODE_A:
				player_input.pressing_A = true;
				break;
			case SDL_SCANCODE_S:
				player_input.pressing_S = true;
				break;
			case SDL_SCANCODE_D:
				player_input.pressing_D = true;
				break;
			case SDL_SCANCODE_W:
				player_input.pressing_W = true;
				break;
			case SDL_SCANCODE_SPACE:
				CreateBullet();
				break;
			case SDL_SCANCODE_ESCAPE:
				loop = false;
				break;
			default:
				break;
			}
			break; }
		case SDL_KEYUP: {
			switch (event.key.keysym.scancode) {
			case SDL_SCANCODE_A:
				player_input.pressing_A = false;
				break;
			case SDL_SCANCODE_S:
				player_input.pressing_S = false;
				break;
			case SDL_SCANCODE_D:
				player_input.pressing_D = false;
				break;
			case SDL_SCANCODE_W:
				player_input.pressing_W = false;
				break;
			default:
				break;
			}
			break; }
		default:
			break;
		}
	}

	if (player_input.pressing_A && square.x >= 0) {
		square.x -= square_speed;
	}
	if (player_input.pressing_D && square.x <= WINDOW_WIDTH - square.w) {
		square.x += square_speed;
	}
	if (player_input.pressing_W && square.y >= 0) {
		square.y -= square_speed;
	}
	if (player_input.pressing_S && square.y <= WINDOW_HEIGHT - square.h) {
		square.y += square_speed;
	}
	
}

void CreateBullet()
{
	for (int i = 0; i < MAX_ACTIVE_BULLETS; ++i) {
		if (active_bullets[i] == nullptr) {
			active_bullets[i] = new Bullet();
			active_bullets[i]->bullet.x = square.x + square.w;
			active_bullets[i]->bullet.y = square.y + square.h / 2 - active_bullets[i]->bullet.h / 2;
			break;
		}
	}
}

void MoveBullets(SDL_Renderer* renderer)
{
	for (int i = 0; i < MAX_ACTIVE_BULLETS; ++i) {
		if (active_bullets[i] != nullptr) {
			if (active_bullets[i]->bullet.x >= WINDOW_WIDTH) {
				delete active_bullets[i];
				active_bullets[i] = nullptr;
			}
			else {
				active_bullets[i]->bullet.x += active_bullets[i]->speed;
				SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
				SDL_RenderFillRect(renderer, &active_bullets[i]->bullet);
			}
		}
	}
}

void CreateEnemies()
{
	active_enemies[0] = new Enemy(1500, 200);
	active_enemies[1] = new Enemy(2500, 400);
	active_enemies[2] = new Enemy(2100, 350);
	active_enemies[3] = new Enemy(1900, 500);

	enemies_created = true;
}

void MoveEnemies(SDL_Renderer * renderer)
{
	for (int i = 0; i < MAX_ACTIVE_ENEMIES; ++i) {
		if (active_enemies[i] != nullptr) {
			if (active_enemies[i]->enemy_rect.x + active_enemies[i]->enemy_rect.w <= 0) {
				delete active_bullets[i];
				active_bullets[i] = nullptr;
			}
			else {
				active_enemies[i]->enemy_rect.x -= active_enemies[i]->speed;
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
				SDL_RenderFillRect(renderer, &active_enemies[i]->enemy_rect);
			}
		}
	}



}
