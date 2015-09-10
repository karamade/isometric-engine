#include "BuildingManager.h"


BuildingManager::BuildingManager()
{
}


BuildingManager::~BuildingManager()
{
}

// Some gross C code for loading allegro config files. Probably should think
// about moving away from these.
void BuildingManager::LoadBuildingTemplates()
{
	// we get building names from this file
	ALLEGRO_CONFIG *uicfg = al_load_config_file("ui/building_ui.cfg");

	char const *uiSection;
	ALLEGRO_CONFIG_SECTION *iter;
	// we ignore the first global section
	uiSection = al_get_first_config_section(uicfg, &iter);

	while(true)
	{
		uiSection = al_get_next_config_section(&iter);
		if(uiSection == NULL)
			break;
		
		if(strcmp(uiSection, "Terraform")) // don't load the Terraform section
		{
			ALLEGRO_CONFIG_ENTRY *eter;
			char const *entry;
			entry = al_get_first_config_entry(uicfg, uiSection, &eter);
			do
			{
				// load each building name from this section
				m_templates.push_back(std::make_unique<BuildingTemplate>(entry));
				entry = al_get_next_config_entry(&eter);
			} 
			while(entry != NULL);
		}
	}
	
	al_destroy_config(uicfg);

	// load the actual building templates from this file
	ALLEGRO_CONFIG *bcfg = al_load_config_file("buildings/buildings.cfg");

	for(auto it = m_templates.begin(); it != m_templates.end(); it++)
	{
		char const *name = (*it)->Name.c_str();

		// search for the building attributes in the cfg file by name
		const char *temp = al_get_config_value(bcfg, name, "image");

		if(!temp)
			break;
		// we probably shouldn't be loading images here
		(*it)->Texture = al_load_bitmap(temp);

		temp = al_get_config_value(bcfg, name, "width");
		if(!temp)
			break;
		(*it)->Width = atoi(temp);

		temp = al_get_config_value(bcfg, name, "height");
		if(!temp)
			break;
		(*it)->Height = atoi(temp);

	}

	al_destroy_config(bcfg);
}

BuildingTemplate& BuildingManager::GetTemplateByName(std::string& name)
{
	return *std::find_if(m_templates.begin(), m_templates.end(),
		[&name](const std::unique_ptr<BuildingTemplate>& bt)
			{
				return bt->Name == name;
			}
	)->get();
}

Building& BuildingManager::CreateBuilding(BuildingTemplate& btemplate)
{
	m_buildings.push_back(std::make_unique<Building>(btemplate));
	return *m_buildings.back();
}

void BuildingManager::DeleteBuilding(Building& building)
{
	for (auto it = m_buildings.begin(); it != m_buildings.end(); it++)
	{
		if ((*it).get() == &building)
		{
			(*it).reset();
			m_buildings.erase(it);
		}
	}
	// TODO: clean up building references
	throw 0;
}
