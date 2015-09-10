#include "Building.h"

std::vector<BuildingTemplate*> Buildings;


Building::Building(BuildingTemplate& btemplate)
	: m_width(btemplate.Width)
	, m_height(btemplate.Height)
	, m_name(btemplate.Name)
	, m_texture(btemplate.Texture)
{
	// Populate resource queues with dummy values until actually implemented
	m_input.push_back(Resource(1, 1));
	m_input.push_back(Resource(2, 2));

	m_output.push_back(Resource(3, 3));
	m_output.push_back(Resource(4, 4));
	m_output.push_back(Resource(5, 5));
}

Building::~Building(void)
{
}

Gwen::Controls::WindowControl* Building::DisplayGUI(Gwen::Controls::Canvas *c)
{
	auto window = new Gwen::Controls::WindowControl(c, m_name);

	// Some kind of window placement system is needed
	window->SetPos(100, 100);
	window->SetSize(200, 200);
	window->SetTitle(m_name);

	// Populate the window with the resource queue contents for now, until we get some real functionality implemented
	auto resourceLabel = new Gwen::Controls::Label(window);
	resourceLabel->Dock(Gwen::Pos::Fill);
	resourceLabel->SetWrap(true);

	std::stringstream ss;
	ss << "Input:" << std::endl;
	for(auto i = m_input.begin(); i != m_input.end(); i++)
		ss << "    " << i->Amount << " units of " << (int)i->Code << std::endl;

	ss << "Output:" << std::endl;
	for(auto i = m_output.begin(); i != m_output.end(); i++)
		ss << "    " << i->Amount << " units of " << (int)i->Code << std::endl;

	resourceLabel->SetText(ss.str());

	return window;
}