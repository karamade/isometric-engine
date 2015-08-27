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

struct Tile
{
	unsigned char TileType;
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
		TileType = tileType;
		Elevation = elevation;
		NorthPoint = northPoint;
		DryRunElevation = -1;

		LockedCorner = false;
		Building = NULL;
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
	inline Tile& GetTile(const Point point) { return map[point.y * pmw + point.x]; };
	inline int GetElevation(const Point point) { return GetTile(point).Elevation; };

	/*
	Returns of the pixel coordinates of the tile at the given tile coordinates.
	This calculation incorporates the tile elevation.
	*/
	inline Point FineTileToWorld(const Point point) { return FineTileToWorld(point, GetElevation(point)); };

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
	Point FinePointToWorld(const Point point) { return Point((height - point.y) * 32 + point.x * 32, (point.x + point.y) * 16 + 16 - map[point.y * pmw + point.x].NorthPoint * PPUE); };

	inline void Select(const Rect r) { selection = r; }

	void AdjustSelectedElevation(int change);
	bool Flatten(const Rect region, int targetElevation);
	bool FlattenDryRun(const Rect region, int targetElevation);
	void FinalizeElevationChange();
	void RevertElevationChange();

	bool PlaceBuilding(int x, int y, Building& building);
	bool IsBuilding(int x, int y) { return map[y * pmw + x].Building != NULL; };
	Building* GetBuilding(int x, int y) { return map[y * pmw + x].Building; };

	

private:
	// tile width/height
	int width;
	int height;

	// point map width/height
	int pmw;
	int pmh;

	std::vector<Tile> map;
	ALLEGRO_BITMAP *tileMap;
	std::vector<ALLEGRO_BITMAP*> tileBitmaps;

	// selected x/y ranges
	Rect selection;
	
	/*
	Performs an elevation-ignoring conversion of tile coordinates to absolute pixel coordinates.
	Used in determining the location of a map tile on the screen.
	*/
	inline Point RoughTileToWorld(const Point point) { return Point((height - point.y) * 32 + point.x * 32 - 32, (point.x + point.y) * 16 - 16); };

	/*
	Returns of the pixel coordinates of the tile at the given tile coordinates.
	This calculation assumes the tile is at the given elevation.
	*/
	inline Point FineTileToWorld(const Point point, int elevation) { Point world = RoughTileToWorld(point); world.y -= elevation * PPUE; return world; };

	/*
	Performs an elevation-ignoring conveersion of absolute pixel coordinates to tile coordinates.
	Used in determining which tile the mouse is over.
	*/
	inline Point RoughWorldToTile(const Point world) { return Point((world.y - 16 + (world.x - width * 32) / 2) / 32, (world.y - 16 - (world.x - width * 32) / 2) / 32); };

	inline int RoughWorldToTileY(const Point world) { return (world.y - (world.x - width*32)/2)/32; };
	inline int RoughWorldToTileX(const Point world) { return (world.y + (world.x - width*32)/2)/32; };

	void LoadMapFromHeightmap(ALLEGRO_BITMAP *heightMap);
	void GenerateTilesFromPoints();

	/*
	Given a pixel coordinate point local to the top-left corner of a tile,
	determines if the point is above the tile, and if not, which neighboring
	tile it is most likely to be above.

	The return value is a tile coordinate delta value. (e.g. -1,0)
	*/
	Point MouseMapClamp(const Point mmPoint, unsigned int tileType);
};