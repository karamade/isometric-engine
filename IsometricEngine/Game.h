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

class Game : public Gwen::Event::Handler
{
public:
	Game(void);
	~Game(void);
	void Run(void);
	
private:
	GameState state;
	InputHandler iHandler;
	GameGui gui;

	ALLEGRO_DISPLAY *display;
	ALLEGRO_EVENT_QUEUE *eventQueue;
	ALLEGRO_TIMER *timer;
	ALLEGRO_FONT *font;

	Point view;

	double fps;
	double framesDone;
	double oldTime;

	bool redraw;
};

