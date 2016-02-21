#include <SDL.h>


#include <iostream>
#include <vector>

//MovementDirections
enum class Direction
{
	LEFT,
	RIGHT
};

struct Player
{
	Player()
	{
		m_movementSpeed = 7.5;
	}
	SDL_Rect m_pos;
	int m_movementSpeed = 0;
};

struct Enemy
{
	Enemy(SDL_Rect pos, int movementSpeed, Direction direction)
	{
		m_pos = pos;
		m_movementSpeed = movementSpeed;
		m_direction = direction;
 	}
	SDL_Rect m_pos;
	int m_movementSpeed = 0;
	Direction m_direction;

};

bool init(SDL_Window** window, SDL_Surface** windowSurface, SDL_Renderer** renderer);
void render(SDL_Renderer** renderer, SDL_Rect& player, SDL_Rect& winBox);
void close(SDL_Window** window, SDL_Renderer** renderer);
void addEntity(SDL_Rect& rect, int x, int y, int h, int w);
void inputMananger(SDL_Event& e, SDL_Rect& player, int movementSpeed);
void gameBounds(SDL_Rect& player);
void addEnemy();
void enemyMovement();
bool playerCollisions(std::vector<Enemy>& enemies, Player& player);
void resetPlayerPos(Player& player);
bool checkCollisions(Enemy enemy, Player& player);
bool checkWinCollision(SDL_Rect& winBox, Player& player);

std::vector<Enemy> enemies;

int main(int argc, char* args[])
{
	SDL_Window* window = nullptr;
	SDL_Surface* windowSurface = nullptr;
	SDL_Renderer* renderer = nullptr;

	if (!init(&window, &windowSurface, &renderer))
	{
		printf("Failed to initialize SDL.\n", SDL_GetError());
	}
	else
	{
		bool quit = false;
		SDL_Event e;
		Player player;
		SDL_Rect winBox;

		addEnemy();
		addEntity(player.m_pos, 375, 25, 25, 25);
		addEntity(winBox, 0, 750, 50, 800);

		//Game loop
		while (!quit)
		{
			while (SDL_PollEvent(&e) != 0)
			{
				if (e.type == SDL_QUIT) {
					quit = true;
				}
				if (e.type == SDL_KEYDOWN)
				{
					inputMananger(e, player.m_pos, player.m_movementSpeed);
				}
			}

			if (playerCollisions(enemies, player)) {
				resetPlayerPos(player);
			}

			enemyMovement();
			gameBounds(player.m_pos);
			render(&renderer, player.m_pos, winBox);

			//Check to see whether the game is won
			if (checkWinCollision(winBox, player)) {
				quit = true;
			}

			SDL_Delay(16); //Simulate 16 fps
		}
	}

	close(&window, &renderer);
	return 0;
}

//Initialize SDL
bool init(SDL_Window** window, SDL_Surface** windowSurface, SDL_Renderer** renderer)
{
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 800;
	bool success = true;
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Couldn't initialize video.\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Create the window
		(*window) = SDL_CreateWindow("My Application", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == nullptr)
		{
			printf("Couldn't create the window.\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Assign window surface
			(*windowSurface) = SDL_GetWindowSurface((*window));
			if (windowSurface == nullptr)
			{
				printf("Couldn't assign window's surface.", SDL_GetError());
				success = false;
			}
			else
			{
				//Create renderer
				(*renderer) = SDL_CreateRenderer((*window), -1, 0);
				if (renderer == nullptr)
				{
					printf("Couldn't create the renderer.", SDL_GetError());
					success = false;
				}
			}

		}
	}
	return success;
}

//Renders all objects
void render(SDL_Renderer** renderer, SDL_Rect& player, SDL_Rect& winBox)
{
	SDL_RenderClear((*renderer)); //Clear the renderer
	
	//Render the player
	SDL_SetRenderDrawColor((*renderer), 255, 0, 0, 0);
	SDL_RenderFillRect((*renderer), &player);

	//Render all enemies
	SDL_SetRenderDrawColor((*renderer), 0, 255, 0, 255);
	for (auto i : enemies)
	{
		SDL_RenderFillRect((*renderer), &i.m_pos);
	}

	//Render the win box
	SDL_SetRenderDrawColor((*renderer), 0, 0, 255, 0);
	SDL_RenderFillRect((*renderer), &winBox);


	SDL_SetRenderDrawColor((*renderer), 255, 255, 255, 255); //Background

	SDL_RenderPresent((*renderer)); //Apply all renders
}

//Stop all SDL proccesses and quit game
void close(SDL_Window** window, SDL_Renderer** renderer)
{
	SDL_DestroyWindow((*window));
	window = nullptr;

	SDL_DestroyRenderer((*renderer));
	renderer = nullptr;

	SDL_Quit();
}

//Add entity into the game
void addEntity(SDL_Rect& rect, int x, int y, int h, int w)
{
	rect.x = x;
	rect.y = y;
	rect.h = h;
	rect.w = w;
}

//Handles the players input
void inputMananger(SDL_Event& e, SDL_Rect& player, int movementSpeed)
{
	switch (e.key.keysym.sym)
	{
	case SDLK_d:
		player.x += movementSpeed;
		break;

	case SDLK_a:
		player.x -= movementSpeed;
		break;

	case SDLK_w:
		player.y -= movementSpeed;
		break;

	case SDLK_s:
		player.y += movementSpeed;
		break;

	default:
		printf("Unrecognized input.\n");
		break;
	}
}

void gameBounds(SDL_Rect& player)
{
	if (player.x < 0) {
		player.x = 0;
	}
	if (player.x > 775) {
		player.x = 775;
	}
	if (player.y < 0) {
		player.y = 0;
	}
}

void addEnemy()
{
	int randYPos = 80;
	const int enemyHeight = 25;
	const int enemyWidth = 25;
	const int enemyMovementSpeed = 5;
	const int numbOfEnemy = 8;
	
	for (int i = 0; i < numbOfEnemy; i++)
	{
		if (rand() % 2 == 0)
		{
			enemies.push_back(Enemy({ rand() % 750 + 1, randYPos, enemyHeight, enemyWidth }, enemyMovementSpeed, Direction::LEFT));
		}
		else
		{
			enemies.push_back(Enemy({ rand() % 750 + 1, randYPos, enemyHeight, enemyWidth }, enemyMovementSpeed, Direction::RIGHT));
		}
		randYPos += 80;
	}
}

void enemyMovement()
{
	for (auto &enemy : enemies)
	{
		//Move Left
		if (enemy.m_direction == Direction::LEFT)
		{
			enemy.m_pos.x -= enemy.m_movementSpeed;
			if (enemy.m_pos.x < 0)
			{
				enemy.m_pos.x = 0;
				enemy.m_direction = Direction::RIGHT;
			}
		}
		//Move Right
		else
		{
			enemy.m_pos.x += enemy.m_movementSpeed;
			if (enemy.m_pos.x > 775)
			{
				enemy.m_pos.x = 775;
				enemy.m_direction = Direction::LEFT;
			}
		}
	}
}

bool playerCollisions(std::vector<Enemy>& enemies, Player& player)
{
	//Enemy Collisions
	for (auto enemy : enemies)
	{
		if (checkCollisions(enemy, player)) {
			return true;
		}
	}

	return false;
}

bool checkCollisions(Enemy enemy, Player& player)
{	//Player collisions
	int playerLeft = player.m_pos.x;
	int playerRight = player.m_pos.x + player.m_pos.w;
	int playerTop = player.m_pos.y;
	int playerBot = player.m_pos.y + player.m_pos.h;


	int enemyLeft = enemy.m_pos.x;
	int enemyRight = enemy.m_pos.x + enemy.m_pos.w;
	int enemyTop = enemy.m_pos.y;
	int enemyBot = enemy.m_pos.y + enemy.m_pos.h;

	//Check for collisions
	if (playerLeft > enemyRight) {
		return false;
	}
	if (playerRight < enemyLeft) {
		return false;
	}
	if (playerTop > enemyBot) {
		return false;
	}
	if (playerBot < enemyTop) {
		return false;
	}

	return true;
}
void resetPlayerPos(Player& player)
{
	player.m_pos.x = 375;
	player.m_pos.y = 25;
}

bool checkWinCollision(SDL_Rect& winBox, Player& player)
{
	//Player collisions
	int playerBot = player.m_pos.y + player.m_pos.h;

	//winBox collisions
	int winBoxTop = winBox.y;


	if (playerBot < winBoxTop) {
		return false;
	}

	return true;
}
