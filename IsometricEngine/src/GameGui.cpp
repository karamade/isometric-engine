#include "GameGui.h"

GameGui::GameGui(InputHandler& handler)
	: m_iHandler(handler)
{
	m_renderer = std::make_unique<Gwen::Renderer::Allegro>();
	m_skin = std::make_unique<Gwen::Skin::TexturedBase>(m_renderer.get());
	m_canvas = std::make_unique<Gwen::Controls::Canvas>(m_skin.get());
	m_canvas->SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	m_gwenInput = std::make_unique<Gwen::Input::Allegro>();
	m_gwenInput->Initialize(m_canvas.get());
}


GameGui::~GameGui()
{
}

void GameGui::AllegroInit()
{
	m_skin->Init("images/GUIskin.png");
	m_skin->SetDefaultFont(L"arial.ttf");
}

void GameGui::LoadGui(BuildingManager& manager)
{
	// more gross allegro config file stuff
	// it might be fine to just hardcode this stuff

	auto buildingMenu = new Gwen::Controls::TabControl(m_canvas.get(), "Buildings");
	buildingMenu->SetBounds(SCREEN_WIDTH - 262 - 150,SCREEN_HEIGHT - 135,262,135);

	auto mapBox = new Gwen::Controls::ImagePanel(m_canvas.get(), "Map");
	mapBox->SetBounds(SCREEN_WIDTH - 150, SCREEN_HEIGHT - 150, 150, 150);

	ALLEGRO_CONFIG *uiConfig = al_load_config_file("ui/building_ui.cfg");

	ALLEGRO_CONFIG_SECTION *iter;
	// this first section is the 'global' section, we ignore it
	const char * cfgSection = al_get_first_config_section(uiConfig, &iter);

	const char * cfgEntry;
	ALLEGRO_CONFIG_ENTRY *eIter;

	// load tab pages from building_ui.cfg
	while(true)
	{
		cfgSection = al_get_next_config_section(&iter);

		// did we pass by the last section in the file?
		if(!cfgSection)
			break;

		auto section = std::string(cfgSection);
		Gwen::Controls::TabButton* resExtractors = buildingMenu->AddPage(section);

		cfgEntry = al_get_first_config_entry(uiConfig, cfgSection, &eIter);
		for(int i = 0; cfgEntry; i++)
		{
			auto entry = std::string(cfgEntry);
			CommandButton* but;
			if (section == "Terraform")
				but = CommandButton::CreateTerraformButton(resExtractors->GetPage());
			else if (section == "Transportation")
				but = CommandButton::CreatePathwayButton(resExtractors->GetPage());
			else
				but = CommandButton::CreateBuildingButton(resExtractors->GetPage(), manager.GetTemplateByName(entry));

			but->SetImage(al_get_config_value(uiConfig, cfgSection, cfgEntry));
			but->SetToolTip(cfgEntry);
			but->SetShouldDrawBackground(false);
			but->SetBounds((i/2) * 50, 0, 50, 50);
			but->onPress.Add(&m_iHandler, &InputHandler::OnBuildingButton);
			if(i%2)
				but->SetPos((i/2) * 50, 52);

			cfgEntry = al_get_next_config_entry(&eIter);
		}
	}

	al_destroy_config(uiConfig);
}

void GameGui::Render()
{
	m_canvas.get()->RenderCanvas();
}

void GameGui::HandleEvent(ALLEGRO_EVENT& ev, GameState& state, Point view)
{
	if (m_gwenInput.get()->ProcessMessage(ev))
		return;

	GuiRequest req = m_iHandler.HandleEvent(ev, state, view);

	// currently there's only one type of event that when processed affects
	// the GUI, clicking on a building to open a window
	switch (req.type)
	{
	case GuiRequestType::None:
		break;
	case GuiRequestType::OpenBuildingWindow:
		auto window = req.ptr.building.DisplayGUI(m_canvas.get());
		window->onWindowClosed.Add(this, &GameGui::CloseWindowCallback);
		window->SetDeleteOnClose(true);
		OpenWindow(window);
		break;
	}
}

void GameGui::OpenWindow(Gwen::Controls::WindowControl* window)
{
	m_windows.push_back(window);
}

void GameGui::CloseWindowCallback(Gwen::Controls::Base* window)
{
	CloseWindow(*(Gwen::Controls::WindowControl*)window);
}

void GameGui::CloseWindow(Gwen::Controls::WindowControl& window)
{
	for (auto it = m_windows.begin(); it != m_windows.end(); it++)
	{
		if ((*it) == &window)
		{
			m_windows.erase(it);
			return;
		}
	}
}