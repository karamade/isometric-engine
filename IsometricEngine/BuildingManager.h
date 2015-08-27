#pragma once

#include <vector>
#include <memory>

#include "Building.h"

class BuildingManager
{
public:
	BuildingManager();
	~BuildingManager();

	void LoadBuildingTemplates();

	BuildingTemplate& GetTemplateByName(std::string& name);

	Building& CreateBuilding(BuildingTemplate& btemplate);
	void DeleteBuilding(Building& building);

private:
	// performance impact of O(n) deletion from this list should be
	// marginal for reasonable numbers of buildings.
	std::vector<std::unique_ptr<Building>> buildings;
	std::vector<std::unique_ptr<BuildingTemplate>> templates;
	// references to elements of the above lists can be stored in
	// other parts of the game state, it is the responsibility of this
	// manager to clean up those references when buildings are deleted.
	//
	// unique_ptr vectors are used even though they aren't actually
	// the only references of the elements to represent ownership
	// of the objects.
};

