#pragma once

#include <Gwen/Gwen.h>
#include <Gwen/Skins/Simple.h>
#include <Gwen/Skins/TexturedBase.h>
#include <Gwen/Input/Allegro.h>
#include <Gwen/Renderers/Allegro.h>
#include <Gwen/Controls.h>
#include <Gwen/Controls/WindowControl.h>

#include <allegro5/allegro.h>

#include <memory>

#include "CommandButton.h"
#include "InputHandler.h"
#include "MathHelper.h"
#include "BuildingManager.h"

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

class GameGui : public Gwen::Event::Handler
{
public:
	GameGui(InputHandler& handler);
	~GameGui();

	/*
	GUI skin and image loading, requires allegro to be initialized.
	*/
	void AllegroInit();

	/*
	Loads GUI config files.
	*/
	void LoadGui(BuildingManager& manager);

	/*
	Draw the GUI.
	*/
	void Render();

	/*
	Handles allegro input events, passes events to the terrain if not consumed by GUI elements.
	*/
	void HandleEvent(ALLEGRO_EVENT& ev, GameState& state, Point view);

private:
	void OpenWindow(Gwen::Controls::WindowControl* window);
	void CloseWindowCallback(Gwen::Controls::Base* window);
	void CloseWindow(Gwen::Controls::WindowControl& window);

	// Things GWEN needs to draw a canvas
	std::unique_ptr<Gwen::Renderer::Allegro> m_renderer;
	std::unique_ptr<Gwen::Skin::TexturedBase> m_skin;
	std::unique_ptr<Gwen::Controls::Canvas> m_canvas;
	std::unique_ptr<Gwen::Input::Allegro> m_gwenInput;

	std::vector<Gwen::Controls::WindowControl*> m_windows;

	InputHandler& m_iHandler;
};

