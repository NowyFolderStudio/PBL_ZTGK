#include "Engine.hpp"
#include <iostream>

int main() {
	Engine gameEngine;

	if (!gameEngine.Init()) {
		std::cerr << "Failed to initialize the game engine!" << std::endl;
		return -1;
	}

	gameEngine.Run();

	return 0;
}