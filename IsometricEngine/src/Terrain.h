#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include <vector>
#include <fstream>
#include <queue>

#include "MathHelper.h"
#include "Building.h"
#include "Pathway.h"

// Pixels Per Unit (of) Elevation
const int PPUE = 8;
const int TILE_SIZE = 64;
const int TILE_X = 64;
const int TILE_Y = 64;

/*
Represents tiles in the game map
*/
struct Tile
{
	/*
	Each tile has a tile type, which represents the physical shape of the tile.
	This type is derived from the relative elevation of each of its corners.
	It is used to index the tile map bitmap array.
	It is also used for tile traversing when converting pixel coords to tile
	coordinates.
	*/
	struct TileType
	{
		// NN EE SS WW
		// 00 00 00 00
		unsigned char value;

		unsigned char GetNorth() const
		{
			return value >> 6;
		}

		unsigned char GetEast() const
		{
			return (value >> 4) & 3;
		}

		unsigned char GetSouth() const
		{
			return (value >> 2) & 3;
		}

		unsigned char GetWest() const
		{
			return value & 3;
		}

		TileType()
		{
			value = 0;
		}

		TileType(unsigned char north, unsigned char east, unsigned char south, unsigned char west)
		{
			value = (north << 6) | (east << 4) | (south << 2) | west;
		}
	};
	TileType Type;
	unsigned char Elevation;
	unsigned char NorthPoint;
	short DryRunElevation;

	bool LockedCorner;
	Building* Building;
	bool BuildingCorner;

	bool PathCorner;
	unsigned char PathType;
	unsigned char PathOrientation;

	Tile(unsigned char tileType, unsigned char elevation, unsigned char northPoint)
	{
		Elevation = elevation;
		NorthPoint = northPoint;
		DryRunElevation = -1;

		LockedCorner = false;
		Building = nullptr;
		BuildingCorner = false;

		PathCorner = false;
	}
};

class Terrain
{
public:
	Terrain();
	~Terrain(void);
	void AllegroInit();

	void DrawViewport(const Rect viewWorld, bool drawSelected);
	inline Tile& GetTile(const Point point) { return m_map[point.y * m_pmw + point.x]; };
	inline int GetElevation(const Point point) { return GetTile(point).Elevation; };

	/*
	Returns of the pixel coordinates of the tile at the given tile coordinates.
	This calculation incorporates the tile elevation.
	*/
	Point FineTileToWorld(const Point point) { return FineTileToWorld(point, GetElevation(point)); };

	/*
	Returns the coordinates of the tile that the given pixel coordinates is over.
	This calculations incorporates tile elevation and is somewhat expensive.
	*/
	Point FineWorldToTile(const Point world);

	/*
	This is like FineWorldToTile, but instead of getting the tile coordinates, it returns the nearest tile corner.
	*/
	Point FineWorldToPoint(const Point world);

	/*
	Returns the world coordinates of the given tile corner point.
	*/
	Point FinePointToWorld(const Point point)
	{
		return Point((m_height - point.y) * 32 + point.x * 32,
			(point.x + point.y) * 16 + 16 - GetTile(point).NorthPoint * PPUE);
	}

	void Select(const Rect r) { m_selection = r; }

	void AdjustSelectedElevation(int change);
	bool Flatten(const Rect region, int targetElevation);
	bool FlattenDryRun(const Rect region, int targetElevation);
	void FinalizeElevationChange();
	void RevertElevationChange();

	bool PlaceBuilding(int x, int y, Building& building);
	bool IsBuilding(int x, int y) { return m_map[y * m_pmw + x].Building != nullptr; };
	Building* GetBuilding(int x, int y) { return m_map[y * m_pmw + x].Building; };

	

private:
	// tile width/height
	int m_width;
	int m_height;

	// point map width/height
	int m_pmw;
	int m_pmh;

	std::vector<Tile> m_map;
	ALLEGRO_BITMAP *m_tileMap;
	std::vector<ALLEGRO_BITMAP*> m_tileBitmaps;

	// selected x/y ranges
	Rect m_selection;
	
	/*
	Performs an elevation-ignoring conversion of tile coordinates to absolute pixel coordinates.
	Used in determining the location of a map tile on the screen.
	*/
	Point RoughTileToWorld(const Point point)
	{
		return Point((m_height - point.y) * 32 + point.x * 32 - 32, (point.x + point.y) * 16 - 16);
	}

	/*
	Returns of the pixel coordinates of the tile at the given tile coordinates.
	This calculation assumes the tile is at the given elevation.
	*/
	Point FineTileToWorld(const Point point, int elevation)
	{
		Point world = RoughTileToWorld(point);
		world.y -= elevation * PPUE;
		return world;
	}

	/*
	Performs an elevation-ignoring conveersion of absolute pixel coordinates to tile coordinates.
	Used in determining which tile the mouse is over.
	*/
	Point RoughWorldToTile(const Point world)
	{
		return Point((world.y - 16 + (world.x - m_width * 32) / 2) / 32,
			(world.y - 16 - (world.x - m_width * 32) / 2) / 32);
	}

	int RoughWorldToTileY(const Point world)
	{
		return (world.y - (world.x - m_width*32)/2)/32;
	}
	
	int RoughWorldToTileX(const Point world)
	{
		return (world.y + (world.x - m_width*32)/2)/32;
	}

	void LoadMapFromHeightmap(ALLEGRO_BITMAP *heightMap);
	void GenerateTilesFromPoints();

	/*
	Given a pixel coordinate point local to the top-left corner of a tile,
	determines if the point is above the tile, and if not, which neighboring
	tile it is most likely to be above.

	The return value is a tile coordinate delta value. (e.g. -1,0)
	*/
	Point MouseMapClamp(const Point mmPoint, const Tile::TileType tileType);
};