#include "AstroidsGame.h"

using namespace std;

int main ( int argc, char* argv[])
{
	AstroidsGame* game = new AstroidsGame();

	game->InitGame();

	while (game->gameIsRunning == true)
	{
		game->CalculateDeltaTime();
		game->UpdateLoop();
	}

	game->QuitGame();

	return 0;
}