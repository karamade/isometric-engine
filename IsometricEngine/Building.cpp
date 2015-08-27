#include "Building.h"

std::vector<BuildingTemplate*> Buildings;


Building::Building(BuildingTemplate& btemplate) : width(btemplate.Width), height(btemplate.Height), name(btemplate.Name), texture(btemplate.Texture)
{
	this->input.push_back(Resource(1, 1));
	this->input.push_back(Resource(2, 2));

	this->output.push_back(Resource(3, 3));
	this->output.push_back(Resource(4, 4));
	this->output.push_back(Resource(5, 5));
}

Building::~Building(void)
{
}

Gwen::Controls::WindowControl* Building::DisplayGUI(Gwen::Controls::Canvas *c)
{
	auto window = new Gwen::Controls::WindowControl(c, name);

	window->SetTitle(name);
	//window->SetPos(100, 100);
	window->SetSize(200, 200);
	window->SetTitle(name);

	auto resourceLabel = new Gwen::Controls::Label(window);
	resourceLabel->Dock(Gwen::Pos::Fill);
	resourceLabel->SetWrap(true);

	std::stringstream ss;
	ss << "Input:\n";
	for(auto i = input.begin(); i < input.end(); i++)
		ss << "    " << i->Amount << " units of " << (int)i->Code << "\n";
	ss << "Output:\n";
	for(auto i = output.begin(); i < output.end(); i++)
		ss << "    " << i->Amount << " units of " << (int)i->Code << "\n";

	resourceLabel->SetText(ss.str());

	return window;
}