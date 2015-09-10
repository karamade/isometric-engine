#pragma once

#include <Gwen/Controls/Button.h>

#include "Building.h"

enum class CommandType { Building, Pathway, Terraform };

union CommandPtr
{
	// This union will have more than one item with other commands are ready for implementation
	BuildingTemplate& building;

	CommandPtr() {};
	CommandPtr(BuildingTemplate& building) : building(building) {};
};

/*
GWEN button subclass that stores a reference to the object it should build or
action it should execute when clicked on.
*/
class CommandButton : public Gwen::Controls::Button
{
public:
	CommandButton(Gwen::Controls::Base *p, CommandType type, CommandPtr item)
		: Gwen::Controls::Button(p)
		, m_type(type)
		, m_item(item)
	{};

	static CommandButton* CreateBuildingButton(Gwen::Controls::Base *p, BuildingTemplate& building)
	{
		return new CommandButton(p, CommandType::Building, building);
	};

	static CommandButton* CreatePathwayButton(Gwen::Controls::Base *p)
	{
		return new CommandButton(p, CommandType::Pathway, CommandPtr());
	};

	static CommandButton* CreateTerraformButton(Gwen::Controls::Base *p)
	{
		return new CommandButton(p, CommandType::Terraform, CommandPtr());
	};

	CommandType GetType() { return m_type; };
	CommandPtr GetItem() { return m_item; };

private:
	CommandType m_type;
	CommandPtr m_item;
};