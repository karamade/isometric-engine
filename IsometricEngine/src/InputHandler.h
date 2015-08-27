#pragma once
#include <allegro5/allegro.h>

#include <iostream>

#include "MathHelper.h"
#include "CommandButton.h"
#include "BuildingManager.h"
#include "GameState.h"

enum class InputState { None, PlaceBuilding, PlacePathway, FlattenTerrain, AdjustTile };
enum Keys { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_K, KEY_J };

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
	
	inline bool IsKeyDown(int key) { return keys[key]; }
	inline Point GetMouse() { return mouse; }
	inline bool GetMouseDown() { return mouseDown; }
	inline InputState GetState() { return iState; }
	inline void SetState(InputState state) { iState = state; }

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
	bool mouseDown;
	Point mouseDownPoint;
	Point mouse;

	bool keys[6];

	InputState iState;
	BuildingTemplate* buildingCmd;
};

