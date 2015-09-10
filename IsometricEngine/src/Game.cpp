#include "Game.h"

Game::Game(void)
	: m_iHandler()
	, m_gui(m_iHandler)
{
	// allegro init stuff, kinda annoying
	if(!al_init())
		// don't worry about failing nice, just kick up a big fuss for now
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

	m_display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
	if(!m_display)
		throw std::runtime_error("can't create display");

	m_font = al_load_ttf_font("arial.ttf", 12, 0);
	if(!m_font)
		throw std::runtime_error("can't load 'arial.ttf'");

	m_eventQueue = al_create_event_queue();
	if(!m_eventQueue)
		throw std::runtime_error("can't create allegro event queue");

	// Redraw timer that fires for every frame
	m_timer = al_create_timer(1.0 / 60.0);
	if(!m_timer)
		throw std::runtime_error("can't create allegro timer");

	al_register_event_source(m_eventQueue, al_get_display_event_source(m_display));
	al_register_event_source(m_eventQueue, al_get_keyboard_event_source());
	al_register_event_source(m_eventQueue, al_get_mouse_event_source());
	al_register_event_source(m_eventQueue, al_get_timer_event_source(m_timer));

	// Other game objects that load allegro resources
	m_state.GetTerrain().AllegroInit();
	m_state.GetBuildingManager().LoadBuildingTemplates();

	m_gui.AllegroInit();
	m_gui.LoadGui(m_state.GetBuildingManager());

	m_view = Point(0, 0);

	// fps counting
	m_fps = 0;
	m_framesDone = 0;
	m_oldTime = al_get_time();

	al_start_timer(m_timer);
}

Game::~Game(void)
{
	al_destroy_display(m_display);
	al_destroy_event_queue(m_eventQueue);
	al_destroy_timer(m_timer);
	al_destroy_font(m_font);
}

void Game::Run(void)
{
	m_view = Point(2000, 2000);

	ALLEGRO_EVENT ev;
	ALLEGRO_TIMEOUT timeout;
	al_init_timeout(&timeout, 0.01);

	std::stringstream ss;
	ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);

	while(true) // game loop
	{
		// user input or redraw timer event
		bool getEvent = al_wait_for_event_until(m_eventQueue, &ev, &timeout);

		// game closed
		if(getEvent && ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			break;
		
		// timer event requesting frame redraw
		if(getEvent && ev.type == ALLEGRO_EVENT_TIMER)
			m_redraw = true;

		// handle user input
		if(getEvent)
			m_gui.HandleEvent(ev, m_state, m_view);

		// preform redraw only if event queue is empty
		// aka consume all user input events before wasting all our CPU time
		// drawing and letting input events stack up in the queue
		if(m_redraw && al_is_event_queue_empty(m_eventQueue))
		{
			m_redraw = false;

			m_view = m_iHandler.DrawTick(m_state, m_view);

			// clear background
			al_clear_to_color(al_map_rgb(255,0,255));
			// draw terrain
			m_state.GetTerrain().DrawViewport(Rect(m_view, m_view + Point(SCREEN_WIDTH, SCREEN_HEIGHT)), true);

			// find what tile the mouse is over for debugging
			Point mouse_pt = m_state.GetTerrain().FineWorldToTile(m_iHandler.GetMouse() + m_view);

			// show fps
			ss.str("");
			ss.clear();
			ss << m_fps;
			auto fps_text = ss.str();

			ss.str("");
			ss.clear();
			ss << mouse_pt.x << ", " << mouse_pt.y << ", "
				// display tile map coords of the tile the mouse is over, helps debugging
				<< (int)m_state.GetTerrain().GetTile(mouse_pt).TileType % 16 << ", "
				<< (int)m_state.GetTerrain().GetTile(mouse_pt).TileType / 16;
			auto mouse_tile_coords = ss.str();

			mouse_pt = m_iHandler.GetMouse() + m_view;
			ss.str("");
			ss.clear();
			// mouse pixel coords
			ss << mouse_pt.x << ", " << mouse_pt.y;
			auto mouse_pixel_coords = ss.str();

			// actually draw the debugging text
			al_draw_text(m_font, white, 10, 0, 0, fps_text.c_str());
			al_draw_text(m_font, white, 10, 12, 0, mouse_tile_coords.c_str());
			al_draw_text(m_font, white, 10, 24, 0, mouse_pixel_coords.c_str());

			// draw the rest of the frame and display
			m_gui.Render();
			al_flip_display();

			// fps upkeep stuff
			double gameTime = al_get_time();
			if(gameTime - m_oldTime >= 1.0)
			{
				m_fps = m_framesDone / (gameTime - m_oldTime);
				m_framesDone = 0;
				m_oldTime = gameTime;
			}
			m_framesDone++;
		}
	}
}
