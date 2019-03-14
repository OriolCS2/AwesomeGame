#include <iostream>
#include <time.h>
#include <ctime>
#include <sstream> 

#include "SDL/include/SDL.h"
#include "SDL_Mixer/include/SDL_mixer.h"
#include "SDL_image/include/SDL_image.h"
#include "SDL_ttf/include/SDL_ttf.h"


#pragma comment (lib, "SDL/libx86/SDL2.lib")
#pragma comment (lib, "SDL/libx86/SDL2main.lib")
#pragma comment (lib, "SDL_Mixer/libx86/SDL2_mixer.lib")
#pragma comment (lib, "SDL_image/libx86/SDL2_image.lib")
#pragma comment (lib, "SDL_ttf/libx86/SDL2_ttf.lib")

#define WINDOW_WIDTH 1100
#define WINDOW_HEIGHT 700
#define MAX_ACTIVE_BULLETS 8
#define MAX_ENEMY_BULLETS 5
#define MAX_ACTIVE_ENEMIES 5
#define MAX_EXPLOSIONS 7
#define MAX_SPAWNABLE_MINIONS 5

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

struct Explosion {
	int num_anim;
	SDL_Rect rect;
	Explosion(SDL_Rect rect) {
		this->rect.x = rect.x;
		this->rect.y = rect.y;
		this->rect.w = rect.w;
		this->rect.h = rect.h;
		num_anim = 0;
	}
	bool Update(SDL_Renderer* renderer);
};

Explosion *active_explosions[MAX_EXPLOSIONS];

struct Enemy {
	SDL_Rect enemy_rect;
	int speed;
	EnemyMovementType type;
	Enemy(int x, int y, EnemyMovementType type) {
		enemy_rect.x = x;
		enemy_rect.y = y;
		enemy_rect.w = 110;
		enemy_rect.h = 60;
		speed = 2;
		this->type = type;
	}
	int time = 0;
	bool left_finished = false;
	bool Update();
	void CreateEnemyBullet();
};

struct Boss {
	SDL_Rect boss_rect{3000,WINDOW_HEIGHT/2 - 150,500,300};
	int speed = 1;
	int time = 0;
	bool attack_up = false;
	bool attack_down = false;
	bool return_up = false;
	bool return_down = false;
	bool return_ = false;
	bool attack = false;
	int cont = 2;
};
Boss boss;
struct minion {
	SDL_Rect minions_rect;
	int speed;
	int sign = rand() % 2;
	minion() {
		minions_rect.x = 2000;
		minions_rect.y = 2000;
		minions_rect.w = 90;
		minions_rect.h = 50;
		speed = 2;
	}
};

Enemy* active_enemies[MAX_ACTIVE_ENEMIES];

minion* active_minion[MAX_SPAWNABLE_MINIONS];

bool game_on = false;

void Input();
void CreateBullet();
void MoveBullets(SDL_Renderer* renderer);
void CheckCollisionBulletEnemy();
void MoveEnemies(SDL_Renderer* renderer);
void CheckPlayerCollision();
void Spawn(SDL_Renderer* renderer);
void BlitAnims(SDL_Renderer* renderer);
void SpawnBoss(SDL_Renderer* renderer);
void moveBoss(SDL_Renderer*);
void RenderScore(SDL_Renderer* renderer);
void signDecider(minion* minion);
void DeleteEnemies();

int lives = 5;
int score = 0;
int old_score = -1;
SDL_Surface* score_to_print = nullptr;
SDL_Color color{ 255,0,0 };
TTF_Font* font = nullptr;
int boss_lives = 30;
int enemies_destroyed = 0;
bool minionsNeeded = true;
SDL_Texture* minion_tex = nullptr;
SDL_Event event;

SDL_Rect square{ -200, WINDOW_HEIGHT/2 - 35,120,70 };
int square_speed = 1;

PlayerInput player_input;
bool loop = true;

SDL_Texture* enemy_1 = nullptr;
SDL_Texture* enemy_2 = nullptr;
SDL_Texture* boss_text = nullptr;

SDL_Texture* laser_player1 = nullptr;
SDL_Texture* laser_enemy = nullptr;

SDL_Texture* ex1 = nullptr;
SDL_Texture* ex2 = nullptr;
SDL_Texture* ex3 = nullptr;
SDL_Texture* ex4 = nullptr;

SDL_Texture* red_Square = nullptr;

bool player_alive = true;
bool spawning = true;
bool being_immortal = true;
int time_immortal = 0;
bool boss_spawned = false;

Mix_Chunk *player_laser = nullptr;
Mix_Chunk *enemy_laser = nullptr;
Mix_Chunk *player_explosion = nullptr;
Mix_Chunk *enemy_explosion = nullptr;
std::string IntToString(int number);
SDL_Texture* tex_score = nullptr;
SDL_Texture* heart[5];

void PrintHearts(SDL_Renderer* renderer);

Mix_Music * song = nullptr;

int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_EVERYTHING);
	Mix_Init(SDL_INIT_AUDIO);
	IMG_Init(IMG_INIT_PNG);
	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,MIX_DEFAULT_FORMAT, 2, 1000);
	TTF_Init();
	song = Mix_LoadMUS("music/SONG.ogg");
	font = TTF_OpenFont("UI/Sunday-Afternoon.ttf", 20);
	player_laser = Mix_LoadWAV("fx/player_laser.wav");
	enemy_laser = Mix_LoadWAV("fx/aaaaaaaaaaa.wav");
	player_explosion = Mix_LoadWAV("fx/explosion_player.wav");
	enemy_explosion = Mix_LoadWAV("fx/explosion_enemy.wav");

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
	// boss
	SDL_Surface* bss = IMG_Load("ships/death_star.png");
	boss_text = SDL_CreateTextureFromSurface(renderer, bss);
	SDL_FreeSurface(bss);
	//background 1
	SDL_Surface* b = IMG_Load("background/simpsons_background.png");
	SDL_Texture* background_texture = SDL_CreateTextureFromSurface(renderer, b);
	SDL_FreeSurface(b);
	
	//background 2
	SDL_Surface* b2 = IMG_Load("background/background1.png");
	SDL_Texture* background_texture2 = SDL_CreateTextureFromSurface(renderer, b2);
	SDL_FreeSurface(b2);

	//back 1
	SDL_Surface* B2 = IMG_Load("background/back1.png");
	SDL_Texture* back1 = SDL_CreateTextureFromSurface(renderer, B2);
	SDL_FreeSurface(B2);
	SDL_Surface* B21 = IMG_Load("background/back1.png");
	SDL_Texture* back12 = SDL_CreateTextureFromSurface(renderer, B21);
	SDL_FreeSurface(B21);
	SDL_Surface* B22 = IMG_Load("background/back1.png");
	SDL_Texture* back13 = SDL_CreateTextureFromSurface(renderer, B22);
	SDL_FreeSurface(B22);
	SDL_Rect back1_rect1{ 0, WINDOW_HEIGHT - 314,550,314 };
	SDL_Rect back1_rect2{ WINDOW_WIDTH / 2, WINDOW_HEIGHT - 314,550,314 };
	SDL_Rect back1_rect3{ WINDOW_WIDTH, WINDOW_HEIGHT - 314,550,314 };

	//back 2
	SDL_Surface* B1 = IMG_Load("background/back2.png");
	SDL_Texture* back2 = SDL_CreateTextureFromSurface(renderer, B1);
	SDL_FreeSurface(B1);
	SDL_Surface* B15 = IMG_Load("background/back2.png");
	SDL_Texture* back2_1 = SDL_CreateTextureFromSurface(renderer, B15);
	SDL_FreeSurface(B15);

	SDL_Rect back2_rect1{ 0, 0,WINDOW_WIDTH,WINDOW_HEIGHT };
	SDL_Rect back2_rect2{ WINDOW_WIDTH, 0 ,WINDOW_WIDTH,WINDOW_HEIGHT };

	SDL_Surface* h = IMG_Load("UI/heart.png");
	heart[0] = SDL_CreateTextureFromSurface(renderer, h);
	heart[1] = SDL_CreateTextureFromSurface(renderer, h);
	heart[2] = SDL_CreateTextureFromSurface(renderer, h);
	heart[3] = SDL_CreateTextureFromSurface(renderer, h);
	heart[4] = SDL_CreateTextureFromSurface(renderer, h);
	SDL_FreeSurface(h);

	//laser player 1
	SDL_Surface* l1 = IMG_Load("lasers/laser_player1.png");
	laser_player1 = SDL_CreateTextureFromSurface(renderer, l1);
	SDL_FreeSurface(l1);

	//laser enemy 1
	SDL_Surface* le1 = IMG_Load("lasers/laser_enemy.png");
	laser_enemy = SDL_CreateTextureFromSurface(renderer, le1);
	SDL_FreeSurface(le1);

	//minion
	SDL_Surface* m = IMG_Load("ships/minion.png");
	minion_tex = SDL_CreateTextureFromSurface(renderer, m);
	SDL_FreeSurface(m);


	//explosions
	SDL_Surface* exp1 = IMG_Load("explosion/ex1.png");
	ex1 = SDL_CreateTextureFromSurface(renderer, exp1);
	SDL_FreeSurface(exp1);

	SDL_Surface* exp2 = IMG_Load("explosion/ex2.png");
	ex2 = SDL_CreateTextureFromSurface(renderer, exp2);
	SDL_FreeSurface(exp2);

	SDL_Surface* exp3 = IMG_Load("explosion/ex3.png");
	ex3 = SDL_CreateTextureFromSurface(renderer, exp3);
	SDL_FreeSurface(exp3);

	SDL_Surface* exp4 = IMG_Load("explosion/ex4.png");
	ex4 = SDL_CreateTextureFromSurface(renderer, exp4);
	SDL_FreeSurface(exp4);

	int cont = 2;
	
	Mix_PlayMusic(song, -1);

	while (loop) {
		Mix_Volume(-1, 50);
		Mix_VolumeChunk(enemy_explosion, 100);
		Mix_VolumeChunk(enemy_laser, 50);
		Mix_VolumeMusic(100);
		if (game_on) {
			SDL_RenderCopy(renderer, back2, NULL, &back2_rect1);
			SDL_RenderCopy(renderer, back2_1, NULL, &back2_rect2);


			SDL_RenderCopy(renderer, back1,NULL, &back1_rect1);
			SDL_RenderCopy(renderer, back12, NULL, &back1_rect2);
			SDL_RenderCopy(renderer, back13, NULL, &back1_rect3);
			if (cont % 2 == 0) {
				back2_rect1.x -= 1;
				back2_rect2.x -= 1;
				back1_rect1.x -= 2;
				back1_rect2.x -= 2;
				back1_rect3.x -= 2;
			}

			if (back1_rect1.x + back1_rect1.w <= 0) {
				back1_rect1.x = WINDOW_WIDTH;
			}
			if (back1_rect2.x + back1_rect2.w <= 0) {
				back1_rect2.x = WINDOW_WIDTH;
			}
			if (back1_rect3.x + back1_rect3.w <= 0) {
				back1_rect3.x = WINDOW_WIDTH;
			}
			if (back2_rect1.x + back2_rect1.w <= 0) {
				back2_rect1.x = WINDOW_WIDTH;
			}
			if (back2_rect2.x + back2_rect2.w <= 0) {
				back2_rect2.x = WINDOW_WIDTH;
			}
			++cont;

			BlitAnims(renderer);
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
			if (boss_spawned)
			{
				moveBoss(renderer);

			}
			else {
				MoveEnemies(renderer);
			}
			CheckCollisionBulletEnemy();
			MoveBullets(renderer);
			


			if (player_alive)
				SDL_RenderCopy(renderer, red_Square, NULL, &square);
			
			
			//if ((enemies_destroyed >= 50) && (!boss_spawned)) 
			if((score>10)&&(!boss_spawned))
			{
				SpawnBoss(renderer);
				boss_spawned = true;
				DeleteEnemies();
			}

			

			RenderScore(renderer);			
			PrintHearts(renderer);
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


	//Mix_FreeChunk(player_laser);

	IMG_Quit();
	//Mix_CloseAudio();
	//Mix_Quit();
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
				if (player_alive) {
					Mix_PlayChannel(-1, player_laser,0);
					CreateBullet();
				}
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
	for (int i = 0; i < MAX_ACTIVE_ENEMIES; ++i) {
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
	}
}

void CheckPlayerCollision()
{
	for (int i = 0; i < MAX_ACTIVE_ENEMIES; ++i) {
		if (active_enemies[i] != nullptr) {
			if (SDL_HasIntersection(&square,&active_enemies[i]->enemy_rect)) {
			player_alive = false;
				--lives;
				for (int z = 0; z < MAX_EXPLOSIONS; ++z) {
					if (active_explosions[z] == nullptr) {
						active_explosions[z] = new Explosion(square);
						break;
					}
				}
				Mix_PlayChannel(-1, player_explosion, 0);
				square.x = -300;
				square.y = WINDOW_HEIGHT / 2 - square.h / 2;
				spawning = true;
				Mix_PlayChannel(-1, enemy_explosion, 0);
				delete active_enemies[i];
				active_enemies[i] = nullptr;
				enemies_destroyed++;
			}
		}
	}
	for (int i = 0; i < MAX_ENEMY_BULLETS; ++i) {
		if (active_enemy_bullets[i] != nullptr) {
			if (SDL_HasIntersection(&square,&active_enemy_bullets[i]->bullet)) {
				player_alive = false;
				--lives;
				for (int z = 0; z < MAX_EXPLOSIONS; ++z) {
					if (active_explosions[z] == nullptr) {
						active_explosions[z] = new Explosion(square);
						break;
					}
				}
				Mix_PlayChannel(-1, player_explosion, 0);
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
		player_input.pressing_A = false;
		player_input.pressing_D = false;
		player_input.pressing_S = false;
		player_input.pressing_W = false;
		spawning = false;

		being_immortal = true;
		time_immortal = SDL_GetTicks();
	}
	else if (lives == 0) {
		game_on = false;
		enemies_destroyed = 0;
		score = 0;
		boss_spawned = false;
		boss_lives = 30;
		minionsNeeded = true;
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
		for (int i = 0; i < MAX_EXPLOSIONS; ++i) {
			if (active_explosions[i] != nullptr) {
				delete active_explosions[i];
				active_explosions[i] = nullptr;
			}
		}
		lives = 5;
	}
	else {
		//SDL_RenderCopy(renderer, red_Square, NULL, &square);
		square.x += square_speed;
		player_alive = true;
	}
}

void BlitAnims(SDL_Renderer * renderer)
{
	for (int i = 0; i < MAX_EXPLOSIONS; ++i) {
		if (active_explosions[i] != nullptr) {
			if (!active_explosions[i]->Update(renderer)) {
				delete active_explosions[i];
				active_explosions[i] = nullptr;
			}
		}
	}
}

void CheckCollisionBulletEnemy()
{
	for (int i = 0; i < MAX_ACTIVE_BULLETS; ++i) {
		if (active_bullets[i] != nullptr) {
			for (int j = 0; j < MAX_ACTIVE_ENEMIES; ++j) {
				if (active_enemies[j] != nullptr) {
					if (SDL_HasIntersection(&active_bullets[i]->bullet, &active_enemies[j]->enemy_rect)) {
						delete active_bullets[i];
						active_bullets[i] = nullptr;
						Mix_PlayChannel(-1, enemy_explosion, 0);
						for (int z = 0; z < MAX_EXPLOSIONS; ++z) {
							if (active_explosions[z] == nullptr) {
								active_explosions[z] = new Explosion(active_enemies[j]->enemy_rect);
								break;
							}
						}
						delete active_enemies[j];
						active_enemies[j] = nullptr;
						enemies_destroyed++;
						score += 10;
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
				Mix_PlayChannel(-1, enemy_laser, 0);
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

bool Explosion::Update(SDL_Renderer * renderer)
{
	bool ret = true;

	if (num_anim <= 30)
		SDL_RenderCopy(renderer, ex1, NULL, &rect);
	if (num_anim <= 60 && num_anim > 30)
		SDL_RenderCopy(renderer, ex2, NULL, &rect);
	if (num_anim <= 90 && num_anim > 60)
		SDL_RenderCopy(renderer, ex3, NULL, &rect);
	if (num_anim <= 120 && num_anim > 90)
		SDL_RenderCopy(renderer, ex4, NULL, &rect);
	if (num_anim <= 150 && num_anim > 120)
		ret = false;

	++num_anim;

	return ret;
}

void SpawnBoss(SDL_Renderer* renderer) {
	
	boss.boss_rect.x = 1200;

	for (int i = 0; i < MAX_SPAWNABLE_MINIONS; i++)
	{
		if (active_minion[i] == nullptr) {
			active_minion[i] = new minion(); //creates a minion
			active_minion[i]->minions_rect.x = boss.boss_rect.x;
			active_minion[i]->minions_rect.y = boss.boss_rect.y;
			if (active_minion[i]->sign == 0)
			{
				active_minion[i]->sign = 1; //decides its sign
			}
			else
			{
				active_minion[i]->sign = -1;
			}
		}
	}
	
}

void moveBoss(SDL_Renderer* renderer) {
	
	
	if (boss.boss_rect.x>650)
	{
		boss.boss_rect.x -= boss.speed;
		boss.time = SDL_GetTicks();
	}
	if (boss.time <= SDL_GetTicks() - 1500 && !boss.attack_up && !boss.attack_down && !boss.attack) {
		if (square.y <= 220)
			boss.attack_up = true;
		else if (square.y > 220 && square.y < 440)
			boss.attack = true;
		else 
			boss.attack_down = true;
	}
	if (boss.attack_up && !boss.return_up) {
		if (boss.boss_rect.x >= 0) {
			boss.boss_rect.x -= 2;
			if (boss.cont % 2 == 0)
				boss.boss_rect.y -= boss.speed;
		}
		else {
			boss.return_up = true;
		}
		++boss.cont;
	}
	if (boss.return_up && boss.attack_up) {
		if (boss.boss_rect.x <= 650) {
			boss.boss_rect.x += 2;
			if (boss.cont % 2 == 0)
			 boss.boss_rect.y += boss.speed;
		}
		else {
			boss.attack_up = false;
			boss.return_up = false;
			boss.time = SDL_GetTicks();
		}
		++boss.cont;
	}
	if (boss.attack && !boss.return_) {
		if (boss.boss_rect.x >= 0) {
			boss.boss_rect.x -= 2;
		}
		else {
			boss.return_ = true;
		}
	}
	if (boss.return_ && boss.attack) {
		if (boss.boss_rect.x <= 650) {
			boss.boss_rect.x += 2;
		}
		else {
			boss.attack = false;
			boss.return_ = false;
			boss.time = SDL_GetTicks();
		}
	}
	if (boss.attack_down && !boss.return_down) {
		if (boss.boss_rect.x >= 0) {
			boss.boss_rect.x -= 2;
			if (boss.cont % 2 == 0)
				boss.boss_rect.y += boss.speed;
		}
		else {
			boss.return_down = true;
		}
		++boss.cont;
	}
	if (boss.return_down && boss.attack_down) {
		if (boss.boss_rect.x <= 650) {
			boss.boss_rect.x += 2;
			if (boss.cont % 2 == 0)
				boss.boss_rect.y -= boss.speed;
		}
		else {
			boss.attack_down = false;
			boss.return_down = false;
			boss.time = SDL_GetTicks();
		}
		++boss.cont;
	}
	SDL_RenderCopy(renderer, boss_text, NULL, &boss.boss_rect);
	for (int i = 0; i < MAX_SPAWNABLE_MINIONS; ++i) {
		if (active_minion[i] != nullptr)
			SDL_RenderCopy(renderer, minion_tex, NULL, &active_minion[i]->minions_rect);
	}
}

void moveMinions() {

	int sign=0;

	
	for (int i = 0; i < MAX_SPAWNABLE_MINIONS; i++)
	{
		if (active_minion[i]->minions_rect.x > 650)
		{
			active_minion[i]->minions_rect.x -= square_speed;
		}
		if (active_minion[i]->minions_rect.y>0)
		{
			sign = active_minion[i]->sign;
			active_minion[i]->minions_rect.y = square_speed*sign; //depending on the sign it will go up or down
		}
		else if ((active_minion[i]->minions_rect.y == 0)||(active_minion[i]->minions_rect.y==700))
		{
			active_minion[i]->sign = active_minion[i]->sign*-1;
		}
	}
}

void RenderScore(SDL_Renderer * renderer)
{
	if (score != old_score) {
		SDL_FreeSurface(score_to_print);
		std::string Score = "Score: " + IntToString(score);
		score_to_print = TTF_RenderText_Solid(font, Score.c_str(), color);
		old_score = score;
		SDL_DestroyTexture(tex_score);
		tex_score = SDL_CreateTextureFromSurface(renderer, score_to_print);
	}
	SDL_Rect rect_tex{ 460, 20,400,70 };;
	std::string left = "POINTS LEFT TILL BOSS: " + IntToString(1000 - score);
	SDL_Surface* surf = TTF_RenderText_Solid(font, left.c_str(), { 255,0,0 });
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_RenderCopy(renderer, tex, NULL, &rect_tex);
	SDL_FreeSurface(surf);
	SDL_DestroyTexture(tex);
	
	SDL_Rect rect{ 900, 20,170,70 };
	SDL_RenderCopy(renderer, tex_score, NULL, &rect);
	
}

void DeleteEnemies()
{
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
}

std::string IntToString(int number)
{
	std::stringstream sstream;
	sstream << number;
	return sstream.str();
}

void PrintHearts(SDL_Renderer * renderer)
{
	SDL_Rect rect{ 20,20,70,70 };
	if (lives == 5) {
		SDL_RenderCopy(renderer, heart[0], NULL, &rect);
		rect.x += 80;
		SDL_RenderCopy(renderer, heart[1], NULL, &rect);
		rect.x += 80;
		SDL_RenderCopy(renderer, heart[2], NULL, &rect);
		rect.x += 80;
		SDL_RenderCopy(renderer, heart[3], NULL, &rect);
		rect.x += 80;
		SDL_RenderCopy(renderer, heart[4], NULL, &rect);
	}
	if (lives == 4) {
		SDL_RenderCopy(renderer, heart[0], NULL, &rect);
		rect.x += 80;
		SDL_RenderCopy(renderer, heart[1], NULL, &rect);
		rect.x += 80;
		SDL_RenderCopy(renderer, heart[2], NULL, &rect);
		rect.x += 80;
		SDL_RenderCopy(renderer, heart[3], NULL, &rect);
	}
	if (lives == 3) {
		SDL_RenderCopy(renderer, heart[0], NULL, &rect);
		rect.x += 80;
		SDL_RenderCopy(renderer, heart[1], NULL, &rect);
		rect.x += 80;
		SDL_RenderCopy(renderer, heart[2], NULL, &rect);
	}
	if (lives == 2) {
		SDL_RenderCopy(renderer, heart[0], NULL, &rect);
		rect.x += 80;
		SDL_RenderCopy(renderer, heart[1], NULL, &rect);
	}
	if (lives == 1) {
		SDL_RenderCopy(renderer, heart[0], NULL, &rect);
	}
}

