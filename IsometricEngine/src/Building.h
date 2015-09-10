#pragma once

#include <allegro5\allegro.h>
#include <allegro5\allegro_image.h>

#include <Gwen/Gwen.h>
#include <Gwen/Controls.h>
#include <Gwen/Controls/WindowControl.h>

#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sstream>

#include "Resources.h"
#include "MathHelper.h"

/*
A template used to create buildings from, these are loaded from cfg files and store basic building attributes
*/
struct BuildingTemplate
{
	std::string Name;
	ALLEGRO_BITMAP *Texture;
	unsigned char Width;
	unsigned char Height;

	Point Size()
	{
		return Point(Width, Height);
	}

	// accepts a cstring argument because cfg file contents are cstrings
	// perhaps this should be changed
	BuildingTemplate(const char *cname)
		: Name(std::string(cname))
		, Texture(nullptr)
	{
	}

	BuildingTemplate(BuildingTemplate&& other)
		: Name(std::move(other.Name))
		, Texture(other.Texture)
		, Width(other.Width)
		, Height(other.Height)
	{
		other.Texture = nullptr;
	}

	~BuildingTemplate()
	{
		if (Texture != nullptr)
			al_destroy_bitmap(Texture);
	}
};

// This is super barebones stuff for displaying building sprites and placeholder windows when buildings get clicked on.
// Nothing actually happens.
// Fleshing out buildings is probably the next step.
class Building
{
public:
	Building(BuildingTemplate& btemplate);
	~Building(void);

	void Tick();

	/*
	Returns a new window for this building.
	*/
	Gwen::Controls::WindowControl* DisplayGUI(Gwen::Controls::Canvas *c);

	unsigned char GetWidth() { return m_width; };
	unsigned char GetHeight() { return m_height; };

	ALLEGRO_BITMAP* GetTexture() { return m_texture; };

	/*
	Has resource input, not implemented.
	*/
	bool HasInput() { return false; };

private:
	std::string m_name;

	unsigned char m_width;
	unsigned char m_height;

	ALLEGRO_BITMAP* m_texture;

	std::vector<Resource> m_input;
	std::vector<Resource> m_output;
};
