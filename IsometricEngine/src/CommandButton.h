#pragma once

#include <Gwen/Controls/Button.h>

#include "Building.h"

enum class CommandType { Building, Pathway, Terraform };

union CommandPtr
{
	BuildingTemplate& building;

	CommandPtr() {};
	CommandPtr(BuildingTemplate& building) : building(building) {};
};

class CommandButton : public Gwen::Controls::Button
{
public:
	CommandButton(Gwen::Controls::Base *p, CommandType type, CommandPtr item) : Gwen::Controls::Button(p), type(type), item(item) {};

	static CommandButton* CreateBuildingButton(Gwen::Controls::Base *p, BuildingTemplate& building) { return new CommandButton(p, CommandType::Building, building); };
	static CommandButton* CreatePathwayButton(Gwen::Controls::Base *p) { return new CommandButton(p, CommandType::Pathway, CommandPtr()); };
	static CommandButton* CreateTerraformButton(Gwen::Controls::Base *p) { return new CommandButton(p, CommandType::Terraform, CommandPtr()); };

	CommandType GetType() { return type; };
	CommandPtr GetItem() { return item; };

private:
	CommandType type;
	CommandPtr item;
};