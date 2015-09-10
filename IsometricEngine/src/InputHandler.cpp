#include "InputHandler.h"


InputHandler::InputHandler()
{
	m_iState = InputState::None;
	m_mouseDown = false;

	for (int i = 0; i < 4; i++)
		m_keys[i] = false;
}


InputHandler::~InputHandler()
{
}

GuiRequest InputHandler::HandleEvent(ALLEGRO_EVENT& event, GameState& state, Point view)
{
	// key down
	if(event.type == ALLEGRO_EVENT_KEY_DOWN)
	{
		switch(event.keyboard.keycode)
		{
		case ALLEGRO_KEY_UP:
			m_keys[KEY_UP] = true;
			break;
		case ALLEGRO_KEY_DOWN:
			m_keys[KEY_DOWN] = true;
			break;
		case ALLEGRO_KEY_LEFT:
			m_keys[KEY_LEFT] = true;
			break;
		case ALLEGRO_KEY_RIGHT:
			m_keys[KEY_RIGHT] = true;
			break;
		case ALLEGRO_KEY_K:
			state.GetTerrain().AdjustSelectedElevation(1);
			break;
		case ALLEGRO_KEY_J:
			state.GetTerrain().AdjustSelectedElevation(-1);
			break;
		case ALLEGRO_KEY_ESCAPE:
			m_iState = InputState::None;
			break;
		}
	}

	// key up
	else if (event.type == ALLEGRO_EVENT_KEY_UP)
	{
		switch(event.keyboard.keycode)
		{
		case ALLEGRO_KEY_UP:
			m_keys[KEY_UP] = false;
			break;
		case ALLEGRO_KEY_DOWN:
			m_keys[KEY_DOWN] = false;
			break;
		case ALLEGRO_KEY_LEFT:
			m_keys[KEY_LEFT] = false;
			break;
		case ALLEGRO_KEY_RIGHT:
			m_keys[KEY_RIGHT] = false;
			break;
		}
	}
	else if (event.type == ALLEGRO_EVENT_MOUSE_AXES ||
			 event.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY)
	{
		m_mouse = state.GetTerrain().FineWorldToTile(Point(event.mouse.x, event.mouse.y) + view);
	}
	else if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
	{
		// left mouse button has been pressed

		m_mouseDown = true;
		m_mouseDownPoint = m_mouse;

		if (m_iState == InputState::PlaceBuilding)
		{
			m_iState = InputState::None;
			m_mouseDown = false; // 'consume' the mouse down/up event
			Building& b = state.GetBuildingManager().CreateBuilding(*m_buildingCmd);
			if (!state.GetTerrain().PlaceBuilding(m_mouseDownPoint.x, m_mouseDownPoint.y, b))
				state.GetBuildingManager().DeleteBuilding(b);
		}
	}
	else if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP && m_mouseDown == true)
	{
		// left mouse button has been let up

		m_mouseDown = false;
		Point mouseUpPoint = m_mouse;

		if (state.GetTerrain().IsBuilding(mouseUpPoint.x, mouseUpPoint.y) && m_mouseDownPoint == mouseUpPoint)
			return GuiRequest(GuiRequestType::OpenBuildingWindow, GuiRequestPtr(*state.GetTerrain().GetBuilding(mouseUpPoint.x, m_mouseDownPoint.y)));

		else if (m_iState == InputState::None)
			state.GetTerrain().Flatten(Rect::Bounding(mouseUpPoint, m_mouseDownPoint), state.GetTerrain().GetElevation(m_mouseDownPoint));

		else if (m_iState == InputState::PlacePathway)
		{
			m_iState = InputState::None;
			state.GetTerrain().Flatten(Rect::Bounding(mouseUpPoint, m_mouseDownPoint), state.GetTerrain().GetElevation(m_mouseDownPoint));
		}
	}

	return GuiRequest(GuiRequestType::None, GuiRequestPtr());
}

Point InputHandler::DrawTick(GameState& state, Point view)
{
	if(m_keys[KEY_UP])
		view.y -= 10;
	if(m_keys[KEY_DOWN])
		view.y += 10;
	if(m_keys[KEY_LEFT])
		view.x -= 10;
	if(m_keys[KEY_RIGHT])
		view.x += 10;

	if (m_iState == InputState::PlaceBuilding)
	{
		// draw building outline
		state.GetTerrain().Select(Rect(m_mouse, m_mouse + m_buildingCmd->Size()));
	}
	else if (m_iState == InputState::PlacePathway && m_mouseDown)
	{
		// draw path outline
		if (AbsoluteValue(m_mouseDownPoint.x - m_mouse.x) < AbsoluteValue(m_mouseDownPoint.y - m_mouse.y))
			state.GetTerrain().Select(Rect::Bounding(m_mouseDownPoint, Point(m_mouseDownPoint.x, m_mouse.y)));
		else
			state.GetTerrain().Select(Rect::Bounding(m_mouseDownPoint, Point(m_mouse.x, m_mouseDownPoint.y)));
	}
	else if (m_iState == InputState::None && m_mouseDown)
	{
		// draw drag select box
		state.GetTerrain().Select(Rect::Bounding(m_mouseDownPoint, m_mouse));
	}
	else
	{
		// highlight the tile we are over
		state.GetTerrain().Select(Rect::Bounding(m_mouse, m_mouse));
	}

	return view;
}

void InputHandler::OnBuildingButton(Gwen::Controls::Base *c)
{
	std::cout << c->GetName() << std::endl;
	switch(((CommandButton*)c)->GetType())
	{
	case CommandType::Building:
		m_iState = InputState::PlaceBuilding;
		m_buildingCmd = &(((CommandButton*)c)->GetItem().building);
		break;
	case CommandType::Pathway:
		m_iState = InputState::PlacePathway;
		break;
	}
}