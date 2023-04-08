#include "AstroidsGame.h"
#include <iostream>
#include <string>
#include <ctime>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_timer.h>
#include <SDL_ttf.h>


using namespace std;

float RandFloat()
{
	return (float)((float)(rand() / (float)(rand())));
}

float DegToRad(float degree)
{
	return degree * M_PI / 180.0;
}

void AstroidsGame::InitGame()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		printf("Init error:  %s\n", SDL_GetError());

	_window = SDL_CreateWindow("Rocket Man", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenSizeX, screenSizeY, 0);

	Uint32 render_flags = SDL_RENDERER_ACCELERATED;
	_renderer = SDL_CreateRenderer(_window, -1, render_flags);
	if (_renderer == NULL)
		printf("ERROR COULD NOT INIT RENDERER");


	SDL_SetRenderDrawColor(_renderer, 2, 2, 2, 255);

	/*
	SDL_Surface* _surface;
	_surface = IMG_Load("assets/face.bmp");

	_texture = SDL_CreateTextureFromSurface(_renderer, _surface);
	SDL_FreeSurface(_surface);

	SDL_QueryTexture(_texture, NULL, NULL, &_destination.w, &_destination.h);
	*/

	_destination.w /= 6;
	_destination.h /= 6;

	_destination.x = (screenSizeX - _destination.w) / 2;
	_destination.y = (screenSizeY - _destination.h) / 2;

	playerPosition = Vector2(screenSizeX / 2, screenSizeY / 2);

	currentFireRate = fireRate;

	gameIsRunning = true;
	printf("Finished initializing game... \n");

	//Set new seed
	double newSeed = std::time(nullptr);
	std::srand(newSeed);

	MainMenu();
}

void AstroidsGame::GameLoop()
{
	Projectiles();
	
	if (currentRocks <= 0)
	{
		astroidsToSpawn++;

		if (astroidsToSpawn > maxRocksOnScreen / 2)
			astroidsToSpawn = maxRocksOnScreen / 2;

		for (int i = 0; i < astroidsToSpawn; i++)
		{
			SpawnRock(8);
		}
	}

	currentTime += deltaTime;
}

void AstroidsGame::UpdateLoop()
{
		switch (currentState)
		{
		case State_MainMenu:
			MainMenuInputs();
			break;
		case State_Gameplay:
			GameInputs();
			GameScreen();
			break;
		case State_Restart:
			RestartScreen();
			break;
		default:
			MainMenu();
			break;
		}
}

void AstroidsGame::QuitGame()
{
	SDL_DestroyTexture(_texture);
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}

void AstroidsGame::MainMenu()
{
	Vector2 titlePos = Vector2(-60, 1);
	Vector2 titleScale = Vector2(2, 2);
	
	Vector2 playButtonPos = Vector2(screenSizeX / 4 + 10, screenSizeY / 2 - 100);
	Vector2 playButtonScale = Vector2(2, 2);

	Vector2 quitButtonPos = Vector2(screenSizeX / 4 + 10, screenSizeY / 2 + 100);
	Vector2 quitButtonScale = Vector2(2, 2);

	DisplayTexture("assets/title.bmp", _renderer, titlePos, titleScale);
	DisplayTexture("assets/PlayButton.bmp", _renderer, playButtonPos, playButtonScale);
	DisplayTexture("assets/ExitButton.bmp", _renderer, quitButtonPos, quitButtonScale);

	SDL_RenderPresent(_renderer);
	SDL_RenderClear(_renderer);
}

void AstroidsGame::MainMenuInputs()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			QuitGame();
			break;
		case SDL_KEYDOWN:

			switch (event.key.keysym.scancode)
			{
			case SDL_SCANCODE_SPACE:
			case SDL_SCANCODE_KP_ENTER:
				currentState = State_Gameplay;
				break;
			case SDL_SCANCODE_ESCAPE:
				gameIsRunning = false;
				break;
			default:
				currentState = State_Gameplay;
				break;
			}
			break;
		default:
			break;
		}
	}


}

void AstroidsGame::RestartScreen()
{
	Vector2 buttonPos = Vector2(screenSizeX / 12, screenSizeY / 4);
	Vector2 buttonScale = Vector2(1, 1);

	DisplayTexture("assets/retryButtons.bmp", _renderer, buttonPos, buttonScale);

	SDL_RenderPresent(_renderer);
	SDL_RenderClear(_renderer);


	RestartScreenInputs();
}

void AstroidsGame::RestartScreenInputs()
{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				QuitGame();
				break;
			case SDL_KEYDOWN:

				switch (event.key.keysym.scancode)
				{
				case SDL_SCANCODE_Y:
				case SDL_SCANCODE_KP_ENTER:
					ResetValues();
					currentState = State_Gameplay;
					break;
				case SDL_SCANCODE_N:
				case SDL_SCANCODE_ESCAPE:
					gameIsRunning = false;
					break;
				default:
					currentState = State_Restart;
					break;
				}
				break;
			default:
				break;
			}
		}
}

void AstroidsGame::ResetValues()
{
	for (int i = 0; i < maxRocksOnScreen; i++)
	{
		activeRocks[i].isActive = false;
	}

	for (int i = 0; i < projectileLimit; i++)
	{
		activeProjectiles[i].isActive = false;
	}

	isPlayerAlive = true;

	playerRotation = 0.0;
	playerPosition = Vector2(screenSizeX / 2, screenSizeY / 2);
	playerVelocity = Vector2(0, 0);

	currentRocks = 0;
	astroidsToSpawn = 0;

	moveUp = false;
	moveDown = false;
	moveLeft = false;
	moveRight = false;
}

void AstroidsGame::GameScreen()
{
	//controls animation loop?
	int speed = 1;

	HandelPlayerInputs();

	MovePlayer();

	GameLoop();
	DrawScreen();
	SDL_Delay(1000 / 60);
}

void AstroidsGame::GameInputs()
{
	SDL_Event Events;
	while (SDL_PollEvent(&Events))
	{
		switch (Events.type)
		{
		case SDL_QUIT:
			QuitGame();
			break;
		case SDL_KEYDOWN:
			SDL_Keysym kSym = Events.key.keysym;

			if (kSym.scancode == SDLK_ESCAPE)
				gameIsRunning = false;

			if (kSym.scancode == SDL_SCANCODE_SPACE || kSym.scancode == SDL_SCANCODE_KP_ENTER)
			{
				if (FireRateCheck() == true)
					if (!shootingDown)
					{
						ShootProjectile();
						shootingDown = true;
					}
			}


			if (kSym.scancode == SDL_SCANCODE_W || kSym.scancode == SDL_SCANCODE_UP)
				moveUp = true;

			if (kSym.scancode == SDL_SCANCODE_S || kSym.scancode == SDL_SCANCODE_DOWN)
				moveDown = true;

			if (kSym.scancode == SDL_SCANCODE_A || kSym.scancode == SDL_SCANCODE_LEFT)
				moveLeft = true;

			if (kSym.scancode == SDL_SCANCODE_D || kSym.scancode == SDL_SCANCODE_RIGHT)
				moveRight = true;
			break;
		case SDL_KEYUP:

			SDL_Keysym upSym = Events.key.keysym;

			if (upSym.scancode == SDL_SCANCODE_W || upSym.scancode == SDL_SCANCODE_UP)
				moveUp = false;

			if (upSym.scancode == SDL_SCANCODE_S || upSym.scancode == SDL_SCANCODE_DOWN)
				moveDown = false;

			if (upSym.scancode == SDL_SCANCODE_A || upSym.scancode == SDL_SCANCODE_LEFT)
				moveLeft = false;

			if (upSym.scancode == SDL_SCANCODE_D || upSym.scancode == SDL_SCANCODE_RIGHT)
				moveRight = false;

			if (upSym.scancode == SDL_SCANCODE_SPACE || upSym.scancode == SDL_SCANCODE_KP_ENTER)
			{
				shootingDown = false;
			}
			break;
		}
	}
}

void AstroidsGame::HandelPlayerInputs()
{
	if (moveUp && !moveDown)
		AddVelocityToPlayer(1);
	else if (!moveUp && moveDown)
		AddVelocityToPlayer(-1);

	if (moveLeft && !moveRight)
		TurnPlayer(-0.1);
	else if (!moveLeft && moveRight)
		TurnPlayer(0.1);

}

void AstroidsGame::DrawScreen()
{
	SDL_RenderClear(_renderer);
	SDL_RenderCopy(_renderer, _texture, NULL, &_destination);

	//SDL_RenderSetScale(_renderer, 2, 2);

	if (isPlayerAlive)
		DrawPlayer(_renderer, playerPosition, playerRotation);

	for (int i = 0; i < projectileLimit; i++)
	{
		if (!activeProjectiles[i].isActive)
			continue;

		DrawBullet(_renderer, 
			activeProjectiles[i].position,
			activeProjectiles[i].rotation);
	}

	for (int i = 0; i < maxRocksOnScreen; i++)
	{
		if (activeRocks[i].isActive == false)
			continue;

		DrawAsteroid(_renderer, activeRocks[i].formation,
			activeRocks[i].position,
			activeRocks[i].rotation, activeRocks[i].scale);
	}

	//MAKE SURE BACKGROUND IS BLACK
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);

	SDL_RenderPresent(_renderer);
}

void AstroidsGame::DisplayTexture(string path, SDL_Renderer* _rend, Vector2 position, Vector2 scaleMultiplication)
{
	const char* charPath = path.c_str();

	SDL_Surface* _playSurface = IMG_Load(charPath);
	SDL_Texture* _texture = SDL_CreateTextureFromSurface(_rend, _playSurface);

	SDL_FreeSurface(_playSurface);

	SDL_Rect _tempRect;

	SDL_QueryTexture(_texture, NULL, NULL, &_tempRect.w, &_tempRect.h);

	//Center the pivot of the rect
	_tempRect.x = (_playSurface->w) + position.x;
	_tempRect.y = (_playSurface->h) + position.y;
	_tempRect.w = _playSurface->w * scaleMultiplication.x;
	_tempRect.h = _playSurface->h * scaleMultiplication.y;

	SDL_RenderCopy(_rend, _texture, NULL, &_tempRect);
}

void AstroidsGame::DrawLine(SDL_Renderer* _rend, Vector2 pointA, Vector2 pointB)
{
	//THIS IS HOW WE WILL DRAW A LINE
	SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
	SDL_RenderDrawLine(_renderer, 0, 0, 1000, 1000);
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
}

void AstroidsGame::DrawPlayer(SDL_Renderer* _rend, Vector2 pos, float rot)
{	
	SDL_Point points[5];
	SDL_Point center;

	center.x = 0;
	center.y = 0;

	points[0].x = center.x;
	points[0].y = center.y - 10;

	points[1].x = center.x - 5;
	points[1].y = center.y + 10;

	points[2].x = center.x;
	points[2].y = center.y + 5;

	points[3].x = center.x + 5;
	points[3].y = center.y + 10;

	points[4].x = center.x;
	points[4].y = center.y - 10;

	//Rotation
	float cosin = cos(rot);
	float sinin = sin(rot);
	
	for (int i = 0; i < 5; i++)
	{
		center.x = pos.x;
		center.y = pos.y;

		Vector2 oldVec = Vector2(points[i].x, points[i].y);
		points[i].x = (oldVec.x * cosin - oldVec.y * sinin) + center.x;
		points[i].y = (oldVec.x * sinin + oldVec.y * cosin) + center.y;
	}
	
	SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
	SDL_RenderDrawLines(_rend, points, 5);

}

void AstroidsGame::DrawBullet(SDL_Renderer* _rend, Vector2 pos, float rot)
{	
	SDL_Point points[5];
	SDL_Point center;

	center.x = 0;
	center.y = 0;

	points[0].x = center.x;
	points[0].y = center.y - 5;

	points[1].x = center.x - 1;
	points[1].y = center.y + 2;

	points[2].x = center.x - 1;
	points[2].y = center.y + 5;

	points[3].x = center.x + 1;
	points[3].y = center.y + 5;

	points[4].x = center.x + 1;
	points[4].y = center.y + 2;

	//Rotation
	float cosin = cos(rot);
	float sinin = sin(rot);

	for (int i = 0; i < 5; i++)
	{
		center.x = pos.x;
		center.y = pos.y;

		Vector2 oldVec = Vector2(points[i].x, points[i].y);
		points[i].x = (oldVec.x * cosin - oldVec.y * sinin) + center.x;
		points[i].y = (oldVec.x * sinin + oldVec.y * cosin) + center.y;
	}

	SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
	SDL_RenderDrawLines(_rend, points, 5);
}

void AstroidsGame::DrawAsteroid(SDL_Renderer* _rend, int rockFormation,Vector2 pos, float rot, float scale)
{
	SDL_Point center;
	SDL_Point points[8];
	center.x = 0;
	center.y = 0;

	switch (rockFormation)
	{
	case 0:
		points[0].x = (center.x - 0) * scale;
		points[0].y = (center.y - 4) * scale;

		points[1].x = (center.x - 2.5) * scale;
		points[1].y = (center.y - 4) * scale;

		points[2].x = (center.x - 4) * scale;
		points[2].y = (center.y - 0) * scale;

		points[3].x = (center.x + 0) * scale;
		points[3].y = (center.y + 4) * scale;

		points[4].x = (center.x + 4) * scale;
		points[4].y = (center.y + 2.5) * scale;

		points[5].x = (center.x + 2.5) * scale;
		points[5].y = (center.y - 4) * scale;

		points[6].x = (center.x + 2.5) * scale;
		points[6].y = (center.y - 4) * scale;

		points[7].x = (center.x - 0) * scale;
		points[7].y = (center.y - 4) * scale;
		break;
	case 1:
		points[0].x = (center.x - 2) * scale;
		points[0].y = (center.y - 4) * scale;

		points[1].x = (center.x - 3) * scale;
		points[1].y = (center.y - 3) * scale;

		points[2].x = (center.x - 4) * scale;
		points[2].y = (center.y - 0) * scale;

		points[3].x = (center.x + 0) * scale;
		points[3].y = (center.y + 4) * scale;

		points[4].x = (center.x + 4) * scale;
		points[4].y = (center.y + 2.5) * scale;

		points[5].x = (center.x + 5) * scale;
		points[5].y = (center.y - 1) * scale;

		points[6].x = (center.x + 4) * scale;
		points[6].y = (center.y - 4) * scale;

		points[7].x = (center.x - 2) * scale;
		points[7].y = (center.y - 4) * scale;

		break;
	case 2:
		points[0].x = (center.x - 1.5) * scale;
		points[0].y = (center.y - 4) * scale;

		points[1].x = (center.x - 3.5) * scale;
		points[1].y = (center.y - 0) * scale;

		points[2].x = (center.x - 4) * scale;
		points[2].y = (center.y - 0) * scale;

		points[3].x = (center.x - 1) * scale;
		points[3].y = (center.y + 4) * scale;

		points[4].x = (center.x + 3.5) * scale;
		points[4].y = (center.y + 3.7) * scale;

		points[5].x = (center.x + 2) * scale;
		points[5].y = (center.y + 2.5) * scale;

		points[6].x = (center.x + 1) * scale;
		points[6].y = (center.y - 4) * scale;

		points[7].x = (center.x - 1.5) * scale;
		points[7].y = (center.y - 4) * scale;
		break;
	default: 
		//Just the first one, incase some mumo jumbo happens
		points[0].x = (center.x - 0) * scale;
		points[0].y = (center.y - 4) * scale;

		points[1].x = (center.x - 2.5) * scale;
		points[1].y = (center.y - 4) * scale;

		points[2].x = (center.x - 4) * scale;
		points[2].y = (center.y - 0) * scale;

		points[3].x = (center.x + 0) * scale;
		points[3].y = (center.y + 4) * scale;

		points[4].x = (center.x + 4) * scale;
		points[4].y = (center.y + 2.5) * scale;

		points[5].x = (center.x + 2.5) * scale;
		points[5].y = (center.y - 4) * scale;

		points[6].x = (center.x + 2.5) * scale;
		points[6].y = (center.y - 4) * scale;

		points[7].x = (center.x - 0) * scale;
		points[7].y = (center.y - 4) * scale;
		break;
	}

	//Rotations
	float cosin = -cos(rot);
	float sinin = sin(rot);

	//					sizeof(points) / sizeof(int)
	for (int i = 0; i < 8; i++)
	{
		center.x = pos.x;
		center.y = pos.y;

		Vector2 oldVec = Vector2(points[i].x, points[i].y);
		points[i].x = (oldVec.x * cosin - oldVec.y * sinin) + center.x;
		points[i].y = (oldVec.x * sinin + oldVec.y * cosin) + center.y;
	}

	SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
	SDL_RenderDrawLines(_rend, points, 8);
}

void AstroidsGame::MovePlayer()
{
	playerPosition.x += playerVelocity.x * deltaTime;
	playerPosition.y += playerVelocity.y * deltaTime;
	
	//Clamping the vector
	if (playerVelocity.x > playerMaxSpeed)
		playerVelocity.x = playerMaxSpeed;
	else if (playerVelocity.x < -playerMaxSpeed)
		playerVelocity.x = -playerMaxSpeed;

	if (playerVelocity.y > playerMaxSpeed)
		playerVelocity.y = playerMaxSpeed;
	else if (playerVelocity.y < -playerMaxSpeed)
		playerVelocity.y = -playerMaxSpeed;

	//If player goes off screen
	if (playerPosition.x + 40 > screenSizeX || playerPosition.x < 0
		|| playerPosition.y + 40 > screenSizeY || playerPosition.y < 0)
		playerPosition = WarpPosition(playerPosition, 40);


	PlayerFriction();
}

void AstroidsGame::PlayerFriction()
{
	
	Vector2 vel = Vector2(playerVelocity.x, playerVelocity.y);
	float magnitude = (playerVelocity.x * playerVelocity.x + playerVelocity.y * playerVelocity.y);

	if (magnitude > 0.01 || magnitude < -0.01)
	{
		playerVelocity.x = playerVelocity.x * playerFriction * deltaTime;
		playerVelocity.y = playerVelocity.y * playerFriction * deltaTime;
	}
	else
	{
		playerVelocity.x = 0.0;
		playerVelocity.y = 0.0;
	}
}

void AstroidsGame::AddVelocityToPlayer(int dir)
{
	playerVelocity.x += sin(playerRotation) * (playerAcceleration * dir) * deltaTime;
	playerVelocity.y += -cos(playerRotation) * (playerAcceleration * dir) * deltaTime;
}

void AstroidsGame::ShootProjectile()
{
	if(currentProjectiles > projectileLimit)
	{
		//reset this to start from the begining
		//so we just modify the oldest entry
		currentProjectiles = 0;
	}
	//Bandaid fix to bug
	//currentProjectiles can somehow become
	//-1 when hitting multiple rocks
	else if (currentProjectiles < 0)
		currentProjectiles = 0;
	

	int newIndex = 0;
		for(int i = 0; i < projectileLimit; i++)
			if (activeProjectiles[i].isActive == false)
			{
				newIndex = i;
				break;
			}


	//Set all data
	activeProjectiles[newIndex].position = playerPosition;
	activeProjectiles[newIndex].rotation = playerRotation;
	activeProjectiles[newIndex].flySpeed = projectileSpeed;
	activeProjectiles[newIndex].isActive = true;
	currentProjectiles++;
	currentFireRate = 0.0;
	//Play sound?
	
}

void AstroidsGame::KillProjectile(int index)
{
	activeProjectiles[index].isActive = false;
	currentProjectiles--;

	cout << to_string(currentProjectiles) << endl;
}

void AstroidsGame::Projectiles()
{
	//if we have any active projectiles
	if (currentProjectiles >= 0)
	{
		for (int i = 0; i < projectileLimit; i++)
		{
			//We are not active, skip to next index
			if (activeProjectiles[i].isActive == false)
				continue;

			//Save projectile position in local vector
			//calculate new position with info saved
			//in the projectile struct
			//set it as the new position

			float speed = activeProjectiles[i].flySpeed;
			Vector2 projectileDir = Vector2(sin(activeProjectiles[i].rotation), -cos(activeProjectiles[i].rotation));

			//For homing rockets maybe?
			//Vector2 projectileDir = Vector2(sin(playerRotation), -cos(playerRotation));

			Vector2 newPos = activeProjectiles[i].position;

			newPos.x += 
				projectileDir.x * speed * deltaTime;
			newPos.y += 
				projectileDir.y * speed * deltaTime;

			activeProjectiles[i].position = newPos;	

			//check if we hit the edge, if so set as inactive
			if (activeProjectiles[i].position.x >= screenSizeX || activeProjectiles[i].position.x <= 0 ||
				activeProjectiles[i].position.y >= screenSizeY || activeProjectiles[i].position.y <= 0 )
			{
				//Kill projectile and go to next element
				KillProjectile(i);
				continue;
			}

			//////////////////////COLLISION | CHECK IF PROJECTILE HIT ROCK/////////////////////////
			for (int y = 0; y < maxRocksOnScreen; y++)
			{
				//go to next rock if not active
				if (activeRocks[y].isActive == false)
					continue;

				Vector2 dist = Vector2
				(
					(activeRocks[y].position.x - activeProjectiles[i].position.x) * (activeRocks[y].position.x - activeProjectiles[i].position.x),
					(activeRocks[y].position.y - activeProjectiles[i].position.y) * (activeRocks[y].position.y - activeProjectiles[i].position.y)
				);

				float distance = sqrtf(dist.x + dist.y);

				//less than 10 pixels times the rock size
				if (distance < (10 * activeRocks[y].scale))
				{
					if (activeRocks[y].scale > 4)
					{
						int scale = activeRocks[y].scale - 4;
						//rTs = Rocks to spawn
						for (int rTs = 0; rTs < 4; rTs++)
						{
							float newRot = -6 + (rand() % 12) * RandFloat();
							SpawnRock(scale, activeRocks[y].position, newRot);
						}
					}

					KillProjectile(i);
					KillRock(y);
				}

			}
		}
	}

	//If we may have any rocks floating around
	if (currentRocks >= 0)
	{
		for (int i = 0; i < maxRocksOnScreen; i++)
		{
			//We are not active, skip to next index
			//We are not active, skip to next index
			if (activeRocks[i].isActive == false)
				continue;

			//So they can move out of the screen a bit before dissapearing
			float astroidSize = activeRocks[i].scale;


			//Save rock position in local vector
			//calculate new position with info saved
			//in the projectile struct
			//set it as the new position
			float speed = activeRocks[i].flySpeed;
			Vector2 projectileDir = Vector2(sin(activeRocks[i].rotation), -cos(activeRocks[i].rotation));
			Vector2 newPos = activeRocks[i].position;

			//Reverse this so we fly into the player
			newPos.x += (projectileDir.x * speed) * deltaTime;
			newPos.y += (projectileDir.y * speed) * deltaTime;

			//check if we hit the edge, warp to other side of screen
			if (newPos.x > screenSizeX + astroidSize|| newPos.x < 0 - astroidSize||
				newPos.y > screenSizeY + astroidSize|| newPos.y < 0 - astroidSize)
			{
				newPos = WarpPosition(newPos, astroidSize);
			}

			activeRocks[i].position = newPos;	

			///////////////////////////////////////COLLISION | CHECK IF PLAYER IS WITHIN RANGE /////////////////////////////////////////////////
			Vector2 dist = Vector2
			(
				(activeRocks[i].position.x - playerPosition.x) * (activeRocks[i].position.x - playerPosition.x),
				(activeRocks[i].position.y - playerPosition.y) * (activeRocks[i].position.y - playerPosition.y)
			);
		
			float distance = sqrtf(dist.x + dist.y);
			if (distance < (5 * activeRocks[i].scale))
			{
				isPlayerAlive = false;
				currentState = State_Restart;
				continue;
			}
		}
	}
}

Vector2 AstroidsGame::WarpPosition(Vector2 pos, float scale)
{
	Vector2 newPos = pos;
	if (pos.x > screenSizeX + scale)
		newPos.x = pos.x - screenSizeX - scale;

	if (pos.x < 0 - scale)
		newPos.x = pos.x + screenSizeX + scale;

	if (pos.y > screenSizeY + scale)
		newPos.y = pos.y - screenSizeY - scale;

	if (pos.y < 0 - scale)
		newPos.y = pos.y + screenSizeY + scale;


	return newPos;
}



void AstroidsGame::SpawnRock(int rockSize, Vector2 spawnXY, float rot)
{
	if (currentRocks >= maxRocksOnScreen)
		return;

	int spawnOnY = std::rand() % 3;
	Vector2 spawnPoint;
	float rotation = 0.0;

	//Give random spawn point
	if (spawnXY.x == -9999 && spawnXY.y == -9999)
	{

		if (spawnOnY == 0)
		{
			spawnPoint.x = std::rand() % screenSizeX;
		}
		else
		{
			spawnPoint.y = std::rand() % screenSizeY;
		}
	}
	else
	{
		spawnPoint.x = spawnXY.x;
		spawnPoint.y = spawnXY.y;
	}

	if (rot == 0)
	{
		if (spawnPoint.x > screenSizeX / 2 && spawnPoint.y < screenSizeY / 2)
		{
			rotation = 3 + (rand() % 2) + (float)RandFloat();
		}
		else if (spawnPoint.x < screenSizeX / 2 && spawnPoint.y < screenSizeY / 2)
		{
			rotation = 3 - (rand() % 2) + (float)RandFloat();
		}


		if (spawnPoint.x < screenSizeX / 2 && spawnPoint.y > screenSizeY / 2)
		{
			rotation = 0.5 + (rand() % 2) + (float)RandFloat();
		}
		else if (spawnPoint.x > screenSizeX / 2 && spawnPoint.y > screenSizeY / 2)
		{
			rotation = 3 + (rand() % 2) + (float)RandFloat();
		}
	}
	else
	{
		rotation = rot;
	}

	if (rot > 6)
		rot = 0;
	else if (rot < 0)
		rot = 6;

	int newIndex = 0;

	for (int i = 0; i < maxRocksOnScreen; i++)
		if (activeRocks[i].isActive == false)
		{
			newIndex = i;
			break;
		}



	activeRocks[newIndex].position = spawnPoint;
	activeRocks[newIndex].rotation = rotation;
	activeRocks[newIndex].flySpeed = rockBaseSpeed;
	activeRocks[newIndex].formation = rand() % 2;
	activeRocks[newIndex].scale = rockSize;
	activeRocks[newIndex].isActive = true;

	currentRocks++;
}

void AstroidsGame::KillRock(int index)
{
	activeRocks[index].isActive = false;
	currentRocks--;
	cout <<"rocks left: " + to_string(currentRocks) << endl;
}

bool AstroidsGame::FireRateCheck()
{
	if (currentFireRate >= fireRate)
	{
		return true;
	}
	else
		currentFireRate += 0.5 * deltaTime;

	return false;
}


void AstroidsGame::TurnPlayer(float dir)
{	
	//3 = 180
	playerRotation += dir * playerRotationSpeed * deltaTime;
	if (playerRotation >= 6)
		playerRotation = 0;
	else if (playerRotation <= -6)
		playerRotation = 0;
}

void AstroidsGame::CalculateDeltaTime()
{
	float time = SDL_GetTicks();
	deltaTime = time - lastTime;
	lastTime = time;

	if (deltaTime > 1)
		deltaTime = 1;
}

Vector2::Vector2(float xValue, float yValue)
{
	x = xValue;
	y = yValue;
}

Projectile::Projectile(bool state, Vector2 pos, float rot, float speed)
{
	isActive = state;
	position = pos;
	rotation = rot;
	flySpeed = speed;
}

Rock::Rock(bool state, Vector2 pos, float rot, float speed, int form, int rockScale)
{
	isActive = state;
	position = pos;
	rotation = rot;
	flySpeed = speed;
	formation = form;
	scale = rockScale;
}
