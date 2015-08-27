#include "GameGui.h"

GameGui::GameGui(InputHandler& handler) : iHandler(handler)
{
	renderer = std::make_unique<Gwen::Renderer::Allegro>();
	skin = std::make_unique<Gwen::Skin::TexturedBase>(renderer.get());
	canvas = std::make_unique<Gwen::Controls::Canvas>(skin.get());
	canvas->SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	gwenInput = std::make_unique<Gwen::Input::Allegro>();
	gwenInput->Initialize(canvas.get());
}


GameGui::~GameGui()
{
}

void GameGui::AllegroInit()
{
	skin->Init("images/GUIskin.png");
	skin->SetDefaultFont(L"arial.ttf");
}

void GameGui::LoadGui(BuildingManager& manager)
{
	auto buildingMenu = new Gwen::Controls::TabControl(canvas.get(), "Buildings");
	buildingMenu->SetBounds(SCREEN_WIDTH - 262 - 150,SCREEN_HEIGHT - 135,262,135);

	auto mapBox = new Gwen::Controls::ImagePanel(canvas.get(), "Map");
	mapBox->SetBounds(SCREEN_WIDTH - 150, SCREEN_HEIGHT - 150, 150, 150);

	ALLEGRO_CONFIG *uiConfig = al_load_config_file("ui/building_ui.cfg");

	ALLEGRO_CONFIG_SECTION *iter;
	// this first section is the 'global' section, we ignore it
	char const * cfgSection = al_get_first_config_section(uiConfig, &iter);

	char const * cfgEntry;
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
			but->onPress.Add(&iHandler, &InputHandler::OnBuildingButton);
			if(i%2)
				but->SetPos((i/2) * 50, 52);

			cfgEntry = al_get_next_config_entry(&eIter);
		}
	}

	al_destroy_config(uiConfig);
}

void GameGui::Render()
{
	canvas.get()->RenderCanvas();
}

void GameGui::HandleEvent(ALLEGRO_EVENT& ev, GameState& state, Point view)
{
	if (gwenInput.get()->ProcessMessage(ev))
		return;

	GuiRequest req = iHandler.HandleEvent(ev, state, view);

	switch (req.type)
	{
	case GuiRequestType::None:
		break;
	case GuiRequestType::OpenBuildingWindow:
		auto window = req.ptr.building.DisplayGUI(canvas.get());
		window->onWindowClosed.Add(this, &GameGui::CloseWindowCallback);
		window->SetDeleteOnClose(true);
		windows.push_back(window);
		break;
	}
}

void GameGui::OpenWindow(Gwen::Controls::WindowControl* window)
{
	windows.push_back(window);
}

void GameGui::CloseWindowCallback(Gwen::Controls::Base* window)
{
	CloseWindow(*(Gwen::Controls::WindowControl*)window);
}

void GameGui::CloseWindow(Gwen::Controls::WindowControl& window)
{
	for (auto it = windows.begin(); it != windows.end(); it++)
	{
		if ((*it) == &window)
		{
			windows.erase(it);
			return;
		}
	}
}