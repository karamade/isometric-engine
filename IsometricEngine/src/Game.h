#pragma once

#include <Gwen/Gwen.h>
#include <Gwen/Skins/Simple.h>
#include <Gwen/Skins/TexturedBase.h>
#include <Gwen/Input/Allegro.h>
#include <Gwen/Renderers/Allegro.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

#include <Gwen/Controls.h>
#include <Gwen/Controls/WindowControl.h>

#include <cstring>
#include <sstream>
#include <iostream>

#include "GameState.h"
#include "Terrain.h"
#include "CommandButton.h"
#include "InputHandler.h"
#include "GameState.h"
#include "GameGui.h"

class Game
{
public:
	Game(void);
	~Game(void);

	/*
	Main game loop
	*/
	void Run(void);
	
private:
	GameState m_state;
	// InputHandler could be combined with GameGui probably
	InputHandler m_iHandler;
	GameGui m_gui;

	ALLEGRO_DISPLAY *m_display;
	ALLEGRO_EVENT_QUEUE *m_eventQueue;
	ALLEGRO_TIMER *m_timer;
	ALLEGRO_FONT *m_font;

	Point m_view;

	double m_fps;
	double m_framesDone;
	double m_oldTime;

	bool m_redraw;
};

