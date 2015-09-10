#pragma once
#include <allegro5/allegro.h>

#include <iostream>

#include "MathHelper.h"
#include "CommandButton.h"
#include "BuildingManager.h"
#include "GameState.h"

enum class InputState { None, PlaceBuilding, PlacePathway, FlattenTerrain, AdjustTile };
enum Keys { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_K, KEY_J };

// A GUI request is created when a user action not directed at the GUI
// requires the GUI to react. For example, clicking on a building on the
// terrain requires a window to open.

// A GUI request has a type that corresponds to a union field.
enum class GuiRequestType { None, OpenBuildingWindow };

union GuiRequestPtr
{
	Building& building;

	GuiRequestPtr() {};
	GuiRequestPtr(Building& building) : building(building) {};
};

struct GuiRequest
{
	GuiRequestType type;
	GuiRequestPtr ptr;

	GuiRequest(GuiRequestType type, GuiRequestPtr ptr) : type(type) , ptr(ptr) {};
};

class InputHandler : public Gwen::Event::Handler
{
public:
	InputHandler();
	~InputHandler();
	
	bool IsKeyDown(int key) { return m_keys[key]; }
	Point GetMouse() { return m_mouse; }
	bool GetMouseDown() { return m_mouseDown; }
	InputState GetState() { return m_iState; }
	void SetState(InputState state) { m_iState = state; }

	/*
	Handles events not handled by the GameGui, and returns a request for GUI elements to be modified.
	(e.g. open a building window when a building is clicked on.
	*/
	GuiRequest HandleEvent(ALLEGRO_EVENT& event, GameState& state, Point view);

	/*
	Updates tile selection before the terrain is drawn.
	*/
	Point DrawTick(GameState& state, Point view);

	/*
	Building button click callback.
	*/
	void OnBuildingButton(Gwen::Controls::Base *c);

private:
	bool m_mouseDown;
	Point m_mouseDownPoint;
	Point m_mouse;

	bool m_keys[6];

	InputState m_iState;
	/*
	Tracks what building to build after a building button has been pressed
	Perhaps using a proper command pattern would be better
	*/
	BuildingTemplate* m_buildingCmd;
};

