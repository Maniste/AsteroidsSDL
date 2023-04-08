#pragma once
#include <iostream>
#include <vector> 

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_timer.h>

struct Vector2 
{
public:
	float x;
	float y;
	Vector2(float xValue = 0.0, float yValue = 0.0);
};


struct Projectile 
{
public:
	bool isActive;
	Vector2 position;
	float rotation;
	float flySpeed;
	Projectile(bool state = false, Vector2 pos = Vector2(), float rot = 0.0, float speed = 0.0);
};


struct Rock 
{
public:
	bool isActive;
	Vector2 position;
	float rotation;
	float flySpeed;
	int formation;
	int scale;
	Rock(bool state = false, Vector2 pos = Vector2(), float rot = 0.0, float speed = 0.0, int form = 0, int scale = 0);
};

enum GameState
{
	State_MainMenu,
	State_Gameplay,
	State_Restart
};

class AstroidsGame 
{
public:  

	bool gameIsRunning;
	int const screenSizeX = 800;
	int const screenSizeY = 800;
	const int frameRate = 60;
	float deltaTime = 0.0f;
	GameState currentState = State_MainMenu;

	void InitGame();
	void GameLoop();
	void UpdateLoop();
	void QuitGame();

	void CalculateDeltaTime();

private:

	bool isPlayerAlive = true;
	bool shootingDown = false;
	bool moveUp = false;
	bool moveDown = false;
	bool moveLeft = false;
	bool moveRight = false;

	int startingAstroids = 4;
	int astroidsToSpawn = 0;

	float playerRotation = 0.0;
	float playerRotationSpeed = 1.1;
	float playerAcceleration = 0.1;
	float playerMass = 2.0;
	float const playerMaxSpeed = 2;
	float playerFriction = 0.995;

	float fireRate = 0.5;
	float currentFireRate = 0.0;
	float projectileSpeed = 15.0;

	//Make sure this is the same size as the vector plz
	int projectileLimit = 20;
	std::vector<Projectile> activeProjectiles = { 
		Projectile(),Projectile(),Projectile(),Projectile(),Projectile(),
		Projectile(),Projectile(),Projectile() ,Projectile() ,Projectile(),
		Projectile(),Projectile(),Projectile() ,Projectile() ,Projectile(),
		Projectile(),Projectile(),Projectile() ,Projectile() ,Projectile()  
	};
	int currentProjectiles = 0;

	//Make sure this is the same size as the vector
	int const maxRocksOnScreen = 50;
	std::vector<Rock> activeRocks = { 
		Rock(),Rock(),Rock(),Rock(),Rock(),
		Rock(),Rock(),Rock() ,Rock() ,Rock(),
		Rock(),Rock(),Rock() ,Rock() ,Rock(),
		Rock(),Rock(),Rock() ,Rock() ,Rock(),
		Rock(),Rock(),Rock() ,Rock() ,Rock(),
		Rock(),Rock(),Rock() ,Rock() ,Rock(),
		Rock(),Rock(),Rock() ,Rock() ,Rock(),
		Rock(),Rock(),Rock() ,Rock() ,Rock(),
		Rock(),Rock(),Rock() ,Rock() ,Rock(),
		Rock(),Rock(),Rock() ,Rock() ,Rock()
	};
	int currentRocks = 0;
	const float rockBaseSpeed = 1.5;
	//how often should a rock spawn
	float rockSpawnTimeBase = 200.0;
	float currentRockSpawnTime = 0.0;

	float lastTime = 0.0;
	float currentTime = 0.0;

	Vector2 playerPosition = Vector2(0.0, 0.0);
	Vector2 playerVelocity = Vector2(0.0, 0.0);

	SDL_Window* _window;
	SDL_Renderer* _renderer;
	SDL_Rect _destination;
	SDL_Texture* _texture;

	void MainMenu();
	void MainMenuInputs();

	void RestartScreen();
	void RestartScreenInputs();
	void ResetValues();

	void GameScreen();
	void GameInputs();
	void HandelPlayerInputs();

	void DrawScreen();
	void DisplayTexture(std::string path, SDL_Renderer* _rend,  Vector2 position, Vector2 scaleMultiplication);
	void DrawLine(SDL_Renderer* _rend, Vector2 pointA, Vector2 pointB);
	void DrawPlayer(SDL_Renderer* _rend, Vector2 pos, float rot);
	void DrawBullet(SDL_Renderer* _rend, Vector2 pos, float rot);
	void DrawAsteroid(SDL_Renderer* _rend,int rockFormation, Vector2 pos, float rot, float scale = 10.0);

	void MovePlayer();
	void TurnPlayer(float dir);
	void PlayerFriction();
	void AddVelocityToPlayer(int Dir);
	void ShootProjectile();
	void KillProjectile(int index);
	void Projectiles();
	Vector2 WarpPosition(Vector2 pos, float scale = 0.0);
	void SpawnRock(int size = 1, Vector2 spawnPoint = Vector2(-9999, -9999), float rot = 0);
	void KillRock(int index);
	bool FireRateCheck();
};
