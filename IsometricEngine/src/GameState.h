#pragma once

#include <memory>

#include "Terrain.h"
#include "BuildingManager.h"

class GameState
{
public:
	GameState() {};
	~GameState() {};

	inline Terrain& GetTerrain() { return terrain; }
	inline BuildingManager& GetBuildingManager() { return buildingManager; }

private:
	Terrain terrain;
	BuildingManager buildingManager;
};

