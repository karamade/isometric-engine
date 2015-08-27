#include "Game.h"

Game::Game(void) : iHandler(), gui(iHandler)
{
	// allegro init
	if(!al_init())
		throw std::runtime_error("can't initialize allegro");

	if(!al_init_image_addon())
		throw std::runtime_error("can't initialize allegro image addon");

	if(!al_init_primitives_addon())
		throw std::runtime_error("can't intialize allegro primitives addon");

	al_init_font_addon();
	if(!al_init_ttf_addon())
		throw std::runtime_error("can't initalize allegro ttf addon");

	if(!al_install_keyboard())
		throw std::runtime_error("can't install keyboard");

	if(!al_install_mouse())
		throw std::runtime_error("can't install mouse");

	display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
	if(!display)
		throw std::runtime_error("can't create display");

	font = al_load_ttf_font("arial.ttf", 12, 0);
	if(!font)
		throw std::runtime_error("can't load 'arial.ttf'");

	eventQueue = al_create_event_queue();
	if(!eventQueue)
		throw std::runtime_error("can't create allegro event queue");

	timer = al_create_timer(1.0 / 60.0);
	if(!timer)
		throw std::runtime_error("can't create allegro timer");

	al_register_event_source(eventQueue, al_get_display_event_source(display));
	al_register_event_source(eventQueue, al_get_keyboard_event_source());
	al_register_event_source(eventQueue, al_get_mouse_event_source());
	al_register_event_source(eventQueue, al_get_timer_event_source(timer));

	al_start_timer(timer);

	state.GetTerrain().AllegroInit();
	state.GetBuildingManager().LoadBuildingTemplates();

	gui.AllegroInit();
	gui.LoadGui(state.GetBuildingManager());

	view = Point(0, 0);

	fps = 0;
	framesDone = 0;
	oldTime = al_get_time();

	al_start_timer(timer);
}

Game::~Game(void)
{
	al_destroy_display(display);
	al_destroy_event_queue(eventQueue);
	al_destroy_timer(timer);
}

void Game::Run(void)
{
	view = Point(2000, 2000);
	while(1)
	{
		ALLEGRO_EVENT ev;
		ALLEGRO_TIMEOUT timeout;
		al_init_timeout(&timeout, 0.01);

		bool getEvent = al_wait_for_event_until(eventQueue, &ev, &timeout);

		if(getEvent && ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			break;
		
		if(getEvent)
			gui.HandleEvent(ev, state, view);

		if(getEvent && ev.type == ALLEGRO_EVENT_TIMER)
			redraw = true;

		// redraw
		if(redraw && al_is_event_queue_empty(eventQueue))
		{
			view = iHandler.DrawTick(state, view);

			redraw = false;

			std::stringstream ss;
			Point mouse_pt = state.GetTerrain().FineWorldToTile(iHandler.GetMouse() + view);

			al_clear_to_color(al_map_rgb(255,0,255));
			state.GetTerrain().DrawViewport(Rect(view, view + Point(SCREEN_WIDTH, SCREEN_HEIGHT)), true);

			// output
			ss << mouse_pt.x << ", " << mouse_pt.y << ", " << (int)state.GetTerrain().GetTile(mouse_pt).TileType % 16 << ", " << (int)state.GetTerrain().GetTile(mouse_pt).TileType / 16;
			al_draw_text(font, al_map_rgb(255, 255, 255), 10, 12, 0, ss.str().c_str());

			ss.str("");
			ss << fps;
			al_draw_text(font, al_map_rgb(255, 255, 255), 10, 0, 0, ss.str().c_str());

			mouse_pt = iHandler.GetMouse() + view;
			ss.str("");
			ss << mouse_pt.x << ", " << mouse_pt.y;
			al_draw_text(font, al_map_rgb(255, 255, 255), 10, 24, 0, ss.str().c_str());

			gui.Render();
			al_flip_display();

			// fps stuff
			double gameTime = al_get_time();
			if(gameTime - oldTime >= 1.0)
			{
				fps = framesDone / (gameTime - oldTime);
				framesDone = 0;
				oldTime = gameTime;
			}
			framesDone++;
		}
	}
}
