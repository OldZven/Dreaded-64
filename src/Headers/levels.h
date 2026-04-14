#pragma once
#include <vector>
#include <iostream>


class levels {
public:
	void Levels();

	int loadLevelOne(int counter);
	int loadLevelTwo(int counter);
	int loadLevelThree(int counter);
	int loadLevelFour(int counter);
	int loadLevelFive(int counter);

	std::vector<float> enemySpawnX;
	std::vector<float> enemySpawnY;

	int mapWidth;
	int mapLength;
	int mapLevel[256];

	int getCurrentLevel() const { return currentLevel; }
private:

	int currentLevel;
};
