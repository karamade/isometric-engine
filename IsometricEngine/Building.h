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

	BuildingTemplate(char const *cname) : Name(std::string(cname)), Texture(nullptr)
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

class Building
{
public:
	Building(BuildingTemplate& btemplate);
	~Building(void);

	void Tick();
	Gwen::Controls::WindowControl* DisplayGUI(Gwen::Controls::Canvas *c);

	inline unsigned char GetWidth() { return width; };
	inline unsigned char GetHeight() { return height; };

	inline ALLEGRO_BITMAP* GetTexture() { return texture; };

	inline bool HasInput() { return false; };

private:
	std::string name;

	unsigned char width;
	unsigned char height;

	ALLEGRO_BITMAP* texture;

	std::vector<Resource> input;
	std::vector<Resource> output;
};
