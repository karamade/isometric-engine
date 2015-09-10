#pragma once

#include <memory>

#include "Terrain.h"
#include "BuildingManager.h"

// maintains the state of the actual game, not UI stuff
class GameState
{
public:
	GameState() {};
	~GameState() {};

	inline Terrain& GetTerrain() { return m_terrain; }
	inline BuildingManager& GetBuildingManager() { return m_buildingManager; }

private:
	Terrain m_terrain;
	BuildingManager m_buildingManager;
};

