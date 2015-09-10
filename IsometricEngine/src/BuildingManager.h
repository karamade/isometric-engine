#pragma once

#include <vector>
#include <memory>

#include "Building.h"

/*
This class maintains a collection of exisiting buildings in game,
and a collection of building templates used to create new buildings from.
Buildings are created using the CreateBuilding factory method.

This class is also responsible for loading and parsing the building templates
config file, eventually this should probably be moved to some kind of
resource manager class that handles images and config files. (sound too eventually)
*/
class BuildingManager
{
public:
	BuildingManager();
	~BuildingManager();

	void LoadBuildingTemplates();

	BuildingTemplate& GetTemplateByName(std::string& name);

	/*
	Create a new building from a building template.
	*/
	Building& CreateBuilding(BuildingTemplate& btemplate);

	void DeleteBuilding(Building& building);
	// Eventually there will be some other methods that iterate through the buildings.

private:
	// performance impact of O(n) deletion from this list should be
	// marginal for reasonable numbers of buildings.
	std::vector<std::unique_ptr<Building>> m_buildings;
	std::vector<std::unique_ptr<BuildingTemplate>> m_templates;
	// references to elements of the above lists can be stored in
	// other parts of the game state, it is the responsibility of this
	// manager to clean up those references when buildings are deleted.
	//
	// unique_ptr vectors are used even though they aren't actually
	// the only references of the elements to represent ownership
	// of the objects.
};

