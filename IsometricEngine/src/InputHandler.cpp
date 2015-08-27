#include "InputHandler.h"


InputHandler::InputHandler()
{
	iState = InputState::None;
	mouseDown = false;

	for (int i = 0; i < 4; i++)
		keys[i] = false;
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
			keys[KEY_UP] = true;
			break;
		case ALLEGRO_KEY_DOWN:
			keys[KEY_DOWN] = true;
			break;
		case ALLEGRO_KEY_LEFT:
			keys[KEY_LEFT] = true;
			break;
		case ALLEGRO_KEY_RIGHT:
			keys[KEY_RIGHT] = true;
			break;
		case ALLEGRO_KEY_K:
			state.GetTerrain().AdjustSelectedElevation(1);
			break;
		case ALLEGRO_KEY_J:
			state.GetTerrain().AdjustSelectedElevation(-1);
			break;
		case ALLEGRO_KEY_ESCAPE:
			iState = InputState::None;
			break;
		}
	}

	// key up
	else if (event.type == ALLEGRO_EVENT_KEY_UP)
	{
		switch(event.keyboard.keycode)
		{
		case ALLEGRO_KEY_UP:
			keys[KEY_UP] = false;
			break;
		case ALLEGRO_KEY_DOWN:
			keys[KEY_DOWN] = false;
			break;
		case ALLEGRO_KEY_LEFT:
			keys[KEY_LEFT] = false;
			break;
		case ALLEGRO_KEY_RIGHT:
			keys[KEY_RIGHT] = false;
			break;
		}
	}
	else if (event.type == ALLEGRO_EVENT_MOUSE_AXES ||
			 event.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY)
	{
		mouse = state.GetTerrain().FineWorldToTile(Point(event.mouse.x, event.mouse.y) + view);
	}
	else if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
	{
		// left mouse button has been pressed

		mouseDown = true;
		mouseDownPoint = mouse;

		if (iState == InputState::PlaceBuilding)
		{
			iState = InputState::None;
			mouseDown = false; // 'consume' the mouse down/up event
			Building& b = state.GetBuildingManager().CreateBuilding(*buildingCmd);
			if (!state.GetTerrain().PlaceBuilding(mouseDownPoint.x, mouseDownPoint.y, b))
				state.GetBuildingManager().DeleteBuilding(b);
		}
	}
	else if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP && mouseDown == true)
	{
		// left mouse button has been let up

		mouseDown = false;
		Point mouseUpPoint = mouse;

		if (state.GetTerrain().IsBuilding(mouseUpPoint.x, mouseUpPoint.y) && mouseDownPoint == mouseUpPoint)
			return GuiRequest(GuiRequestType::OpenBuildingWindow, GuiRequestPtr(*state.GetTerrain().GetBuilding(mouseUpPoint.x, mouseDownPoint.y)));

		else if (iState == InputState::None)
			state.GetTerrain().Flatten(Rect::Bounding(mouseUpPoint, mouseDownPoint), state.GetTerrain().GetElevation(mouseDownPoint));

		else if (iState == InputState::PlacePathway)
		{
			iState = InputState::None;
			state.GetTerrain().Flatten(Rect::Bounding(mouseUpPoint, mouseDownPoint), state.GetTerrain().GetElevation(mouseDownPoint));
		}
	}

	return GuiRequest(GuiRequestType::None, GuiRequestPtr());
}

Point InputHandler::DrawTick(GameState& state, Point view)
{
	if(keys[KEY_UP])
		view.y -= 10;
	if(keys[KEY_DOWN])
		view.y += 10;
	if(keys[KEY_LEFT])
		view.x -= 10;
	if(keys[KEY_RIGHT])
		view.x += 10;

	if (iState == InputState::PlaceBuilding)
	{
		// draw building outline
		state.GetTerrain().Select(Rect(mouse, mouse + buildingCmd->Size()));
	}
	else if (iState == InputState::PlacePathway && mouseDown)
	{
		// draw path outline
		if (AbsoluteValue(mouseDownPoint.x - mouse.x) < AbsoluteValue(mouseDownPoint.y - mouse.y))
			state.GetTerrain().Select(Rect::Bounding(mouseDownPoint, Point(mouseDownPoint.x, mouse.y)));
		else
			state.GetTerrain().Select(Rect::Bounding(mouseDownPoint, Point(mouse.x, mouseDownPoint.y)));
	}
	else if (iState == InputState::None && mouseDown)
	{
		// draw drag select box
		state.GetTerrain().Select(Rect::Bounding(mouseDownPoint, mouse));
	}
	else
	{
		// highlight the tile we are over
		state.GetTerrain().Select(Rect::Bounding(mouse, mouse));
	}

	return view;
}

void InputHandler::OnBuildingButton(Gwen::Controls::Base *c)
{
	std::cout << c->GetName() << std::endl;
	switch(((CommandButton*)c)->GetType())
	{
	case CommandType::Building:
		iState = InputState::PlaceBuilding;
		buildingCmd = &(((CommandButton*)c)->GetItem().building);
		break;
	case CommandType::Pathway:
		iState = InputState::PlacePathway;
		break;
	}
}