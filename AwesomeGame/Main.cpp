#include <iostream>
#include <time.h>
#include <ctime>

#include "SDL/include/SDL.h"
#include "SDL_Mixer/include/SDL_mixer.h"
#include "SDL_image/include/SDL_image.h"


#pragma comment (lib, "SDL/libx86/SDL2.lib")
#pragma comment (lib, "SDL/libx86/SDL2main.lib")
#pragma comment (lib, "SDL_Mixer/libx86/SDL2_mixer.lib")
#pragma comment (lib, "SDL_image/libx86/SDL2_image.lib")

#define WINDOW_WIDTH 1100
#define WINDOW_HEIGHT 700
#define MAX_ACTIVE_BULLETS 10
#define MAX_ENEMY_BULLETS 5
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
		bullet.w = 60;
		bullet.h = 30;
		speed = 3;
	}
};

Bullet* active_bullets[MAX_ACTIVE_BULLETS];

struct EnemyBullet {
	int speed;
	SDL_Rect bullet;
	EnemyBullet() {
		bullet.x = 0;
		bullet.y = 0;
		bullet.w = 30;
		bullet.h = 20;
		speed = 2;
	}

};
EnemyBullet* active_enemy_bullets[MAX_ENEMY_BULLETS];

enum class EnemyMovementType {
	STRAIGHT_ON,
	STAY_SHOT,

	NONE
};

struct Enemy {
	SDL_Rect enemy_rect;
	int speed;
	EnemyMovementType type;
	Enemy(int x, int y, EnemyMovementType type) {
		enemy_rect.x = x;
		enemy_rect.y = y;
		enemy_rect.w = 110;
		enemy_rect.h = 60;
		speed = 1;
		this->type = type;
	}
	int time = 0;
	bool left_finished = false;
	bool Update();
	void CreateEnemyBullet();
};

Enemy* active_enemies[MAX_ACTIVE_ENEMIES];

bool game_on = false;

void Input();
void CreateBullet();
void MoveBullets(SDL_Renderer* renderer);
void CheckCollisionBulletEnemy();
void MoveEnemies(SDL_Renderer* renderer);
void CheckPlayerCollision();
void Spawn(SDL_Renderer* renderer);

int lives = 3;

SDL_Event event;

SDL_Rect square{ -200, WINDOW_HEIGHT/2 - 35,120,70 };
int square_speed = 1;

PlayerInput player_input;
bool loop = true;

SDL_Texture* enemy_1 = nullptr;
SDL_Texture* enemy_2 = nullptr;

SDL_Texture* laser_player1 = nullptr;
SDL_Texture* laser_enemy = nullptr;

SDL_Texture* red_Square = nullptr;

bool player_alive = true;
bool spawning = true;
bool being_immortal = true;
int time_immortal = 0;


int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_EVERYTHING);
	Mix_Init(SDL_INIT_AUDIO);
	IMG_Init(IMG_INIT_PNG);
	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,MIX_DEFAULT_FORMAT, 2, AUDIO_S16SYS);
	Mix_Chunk *fx_sound = nullptr;
	fx_sound = Mix_LoadWAV("fx/fx.wav");

	SDL_Window* window = SDL_CreateWindow("Awesome Game", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	// player
	SDL_Surface* p = IMG_Load("ships/player_ship.png"); 
	red_Square = SDL_CreateTextureFromSurface(renderer, p);
	SDL_FreeSurface(p);
	// enemy 1
	SDL_Surface* e1 = IMG_Load("ships/enemy_1.png");
	enemy_1 = SDL_CreateTextureFromSurface(renderer, e1);
	SDL_FreeSurface(e1);
	// enemy 2
	SDL_Surface* e2 = IMG_Load("ships/enemy_2.png");
	enemy_2 = SDL_CreateTextureFromSurface(renderer, e2);
	SDL_FreeSurface(e2);

	//background 1
	SDL_Surface* b = IMG_Load("background/simpsons_background.png");
	SDL_Texture* background_texture = SDL_CreateTextureFromSurface(renderer, b);
	SDL_FreeSurface(b);
	
	//background 2
	SDL_Surface* b2 = IMG_Load("background/background1.png");
	SDL_Texture* background_texture2 = SDL_CreateTextureFromSurface(renderer, b2);
	SDL_FreeSurface(b2);

	//laser player 1
	SDL_Surface* l1 = IMG_Load("lasers/laser_player1.png");
	laser_player1 = SDL_CreateTextureFromSurface(renderer, l1);
	SDL_FreeSurface(l1);

	//laser enemy 1
	SDL_Surface* le1 = IMG_Load("lasers/laser_enemy.png");
	laser_enemy = SDL_CreateTextureFromSurface(renderer, le1);
	SDL_FreeSurface(le1);


	while (loop) {

		if (game_on) {
			SDL_RenderCopy(renderer, background_texture2, NULL, NULL);
			if (!spawning) {
				if (!being_immortal)
					CheckPlayerCollision();
			}
			else {
				Spawn(renderer);
			}
			Input();
			if (being_immortal && SDL_GetTicks() - 1000 >= time_immortal) {
				being_immortal = false;
			}
			CheckCollisionBulletEnemy();
			MoveBullets(renderer);
			MoveEnemies(renderer);


			if (player_alive)
				SDL_RenderCopy(renderer, red_Square, NULL, &square);
			SDL_RenderPresent(renderer);
			SDL_Delay(1);

		}
		else {

			if (SDL_PollEvent(&event) != 0) {
				if (event.key.keysym.scancode == SDL_SCANCODE_RETURN)
					game_on = true;
				if (event.type == SDL_QUIT) {
					loop = false;
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
					loop = false;
				}
			}

			SDL_RenderCopy(renderer, background_texture, NULL, NULL);
			SDL_RenderPresent(renderer);

		}

		SDL_RenderClear(renderer);
	}

	IMG_Quit();
	//SDL_AudioQuit();
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
				if (player_alive)
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

	if (player_alive) {
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
				SDL_RenderCopy(renderer, laser_player1, NULL, &active_bullets[i]->bullet);
				//SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
				//SDL_RenderFillRect(renderer, &active_bullets[i]->bullet);
			}
		}
	}
	for (int i = 0; i < MAX_ENEMY_BULLETS; ++i) {
		if (active_enemy_bullets[i] != nullptr) {
			if (active_enemy_bullets[i]->bullet.x + active_enemy_bullets[i]->bullet.w <= 0) {
				delete active_enemy_bullets[i];
				active_enemy_bullets[i] = nullptr;
			}
			else {
				active_enemy_bullets[i]->bullet.x -= active_enemy_bullets[i]->speed;
				SDL_RenderCopy(renderer, laser_enemy, NULL, &active_enemy_bullets[i]->bullet);
				//SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
				//SDL_RenderFillRect(renderer, &active_enemy_bullets[i]->bullet);
			}
		}
	}
}

void MoveEnemies(SDL_Renderer * renderer)
{
	//int kind = 0,time=0;
	for (int i = 0; i < MAX_ACTIVE_ENEMIES; ++i) {
		//kind= rand() % 2;
		//if (kind==0)
		{
			if (active_enemies[i] != nullptr) { //if the enemy exists
				if (active_enemies[i]->enemy_rect.x + active_enemies[i]->enemy_rect.w <= 0) { //if the enemy is out of the screen 
					delete active_enemies[i];
					active_enemies[i] = nullptr;
				}
				else {
					if (active_enemies[i]->Update()) {
						switch(active_enemies[i]->type) {
						case EnemyMovementType::STRAIGHT_ON:
							SDL_RenderCopy(renderer, enemy_1, NULL, &active_enemies[i]->enemy_rect);
							break;
						case EnemyMovementType::STAY_SHOT:
							SDL_RenderCopy(renderer, enemy_2, NULL, &active_enemies[i]->enemy_rect);
							break;
						}
						
					}
					else {
						delete active_enemies[i];
						active_enemies[i] = nullptr;
					}
				}
			}
			else //if it doesn't exist create one in a random place
			{
				int num_type = rand() % 2;
				switch (num_type) {
				case 0:
					active_enemies[i] = new Enemy(1100 + rand() % 100, rand() % 650, EnemyMovementType::STAY_SHOT);
					break;
				case 1:
					active_enemies[i] = new Enemy(1100 + rand() % 100, rand() % 650, EnemyMovementType::STRAIGHT_ON);
					break;
				default:
					break;
				}
				
			}
		}
		//else if(kind==1)
		/*{
			if (active_enemies[i] != nullptr) { //if the enemy exists
				if (active_enemies[i]->enemy_rect.x + active_enemies[i]->enemy_rect.w <= 0) { //if the enemy is out of the screen 
					active_enemies[i]->enemy_rect.y = rand() % 700;
					active_enemies[i]->enemy_rect.x = 1100;
					delete active_bullets[i];
					active_bullets[i] = nullptr;
				}
				else {
					active_enemies[i]->enemy_rect.x -= active_enemies[i]->speed;
					if (active_enemies[i]->enemy_rect.y<=700) //if the enemy is 
					{
						active_enemies[i]->enemy_rect.y -= 2*(active_enemies[i]->speed);
					}
					else
					{
						active_enemies[i]->enemy_rect.y -= active_enemies[i]->speed;
					}
					SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
					SDL_RenderFillRect(renderer, &active_enemies[i]->enemy_rect);
				}
			}
			else //if it doesn't exist create one in a random place
			{
				active_enemies[i] = new Enemy(rand() % 700, 1100);
			}
		}*/

	}
}

void CheckPlayerCollision()
{
	for (int i = 0; i < MAX_ACTIVE_ENEMIES; ++i) {
		if (active_enemies[i] != nullptr) {
			if (square.x + square.w >= active_enemies[i]->enemy_rect.x && square.y + square.h / 2 >= active_enemies[i]->enemy_rect.y && square.y + square.h / 2 <= active_enemies[i]->enemy_rect.y + active_enemies[i]->enemy_rect.h) {
				player_alive = false;
				--lives;
				square.x = -300;
				square.y = WINDOW_HEIGHT / 2 - square.h / 2;
				spawning = true;
				delete active_enemies[i];
				active_enemies[i] = nullptr;
			}
		}
	}
	for (int i = 0; i < MAX_ENEMY_BULLETS; ++i) {
		if (active_enemy_bullets[i] != nullptr) {
			if (square.x + square.w >= active_enemy_bullets[i]->bullet.x && square.y + square.h / 2 >= active_enemy_bullets[i]->bullet.y && square.y + square.h / 2 <= active_enemy_bullets[i]->bullet.y + active_enemy_bullets[i]->bullet.h) {
				player_alive = false;
				--lives;
				square.x = -300;
				square.y = WINDOW_HEIGHT / 2 - square.h / 2;
				spawning = true;
				delete active_enemy_bullets[i];
				active_enemy_bullets[i] = nullptr;
			}
		}
	}
}

void Spawn(SDL_Renderer* renderer)
{
	
	if (square.x >= 50) {
		spawning = false;
		player_alive = true;
		being_immortal = true;
		time_immortal = SDL_GetTicks();
	}
	else if (lives == 0) {
		game_on = false;
		for (int i = 0; i < MAX_ACTIVE_BULLETS; ++i) {
			if (active_bullets[i] != nullptr) {
				delete active_bullets[i];
				active_bullets[i] = nullptr;
			}
		}
		for (int i = 0; i < MAX_ENEMY_BULLETS; ++i) {
			if (active_enemy_bullets[i] != nullptr) {
				delete active_enemy_bullets[i];
				active_enemy_bullets[i] = nullptr;
			}
		}
		for (int i = 0; i < MAX_ACTIVE_ENEMIES; ++i) {
			if (active_enemies[i] != nullptr) {
				delete active_enemies[i];
				active_enemies[i] = nullptr;
			}
		}
		lives = 3;
	}
	else {
		SDL_RenderCopy(renderer, red_Square, NULL, &square);
		square.x += square_speed;
	}
}




void CheckCollisionBulletEnemy()
{
	for (int i = 0; i < MAX_ACTIVE_BULLETS; ++i) {
		if (active_bullets[i] != nullptr) {
			for (int j = 0; j < MAX_ACTIVE_ENEMIES; ++j) {
				if (active_enemies[j] != nullptr) {
					if (active_enemies[j]->enemy_rect.x <= active_bullets[i]->bullet.x + active_bullets[i]->bullet.w && active_bullets[i]->bullet.y + active_bullets[i]->bullet.h / 2 >= active_enemies[j]->enemy_rect.y && active_bullets[i]->bullet.y + active_bullets[i]->bullet.h / 2 <= active_enemies[j]->enemy_rect.y + active_enemies[j]->enemy_rect.h) {
						delete active_bullets[i];
						active_bullets[i] = nullptr;
						delete active_enemies[j];
						active_enemies[j] = nullptr;
						break;
					}
				}
			}
		}
	}

}

bool Enemy::Update()
{
	bool ret = true;
	switch (type) {
	case EnemyMovementType::STRAIGHT_ON:
		enemy_rect.x -= speed;
		break;
	case EnemyMovementType::STAY_SHOT:
		if (!left_finished) {
			if (enemy_rect.x >= WINDOW_WIDTH - 300) {
				enemy_rect.x -= speed;
				time = SDL_GetTicks();
			}
			else if (time < SDL_GetTicks() - 500) {
				CreateEnemyBullet();
				left_finished = true;
			}
		}
		else {
			enemy_rect.x += speed;
			if (enemy_rect.x > WINDOW_WIDTH)
				ret = false;
		}
		break;
	default:
		break;
	}

	return ret;
}

void Enemy::CreateEnemyBullet()
{
	for (int i = 0; i < MAX_ENEMY_BULLETS; ++i) {
		if (active_enemy_bullets[i] == nullptr) {
			active_enemy_bullets[i] = new EnemyBullet();
			active_enemy_bullets[i]->bullet.x = enemy_rect.x;
			active_enemy_bullets[i]->bullet.y = enemy_rect.y + square.h / 2 - active_enemy_bullets[i]->bullet.h / 2;
			break;
		}
	}
}
