#include "Terrain.h"
#include <iostream>

Terrain::Terrain()
{
}

Terrain::~Terrain(void)
{
	al_destroy_bitmap(m_tileMap);
}

void Terrain::AllegroInit()
{
	// load tile images
	m_tileMap = al_load_bitmap("images/tiles2.png");
	if(!m_tileMap)
		throw std::runtime_error("can't load tilemap");

	// split tile images into individual tiles
	for (int tt = 0; tt < 256; tt++)
	{
		m_tileBitmaps.push_back(al_create_sub_bitmap(m_tileMap, TILE_X * (tt % 16), TILE_Y * (tt / 16), TILE_X, TILE_Y));
	}

	ALLEGRO_BITMAP *hm = al_load_bitmap("images/200px-Heightmap.png");
	LoadMapFromHeightmap(hm);
	al_destroy_bitmap(hm);
}

Point Terrain::FineWorldToTile(const Point world)
{
	Point local;
	int mmWidth = 1024;
	int count = 0;

	Point tile = RoughWorldToTile(world);

	while (true)
	{
		if (tile.x >= m_width || tile.y >= m_height)
		{
			tile.x = Clamp(tile.x, 0, m_width - 1);
			tile.y = Clamp(tile.y, 0, m_height - 1);
			return tile;
		}

		Point clamped = Point(ClampPositive(tile.x), ClampPositive(tile.y));

		if (count > 100)
		{
			// prevent infinite loops, this should never be reached if this function is working correctly though.
			return tile;
		}
		if (count == 90)
			// breakpoint this line to identify tile walking loops
			count = count;

		// find the corner of the rough tile we are on
		local = RoughTileToWorld(tile);

		// translate to the tile's local coordinates
		local = world - local;

		unsigned char tileType;
		unsigned char elevation;
		if (tile.x < 0 || tile.y < 0)
		{
			// the rough tile is outside the bounds of the map.
			// we want to mimic the slope that is parallel to the edge of the
			// map of each tile for each fake tile outside the the bounds of the map.
			// we can then proceed to tile walk over those fake tiles until we
			// either get back inside the map or we match a tile outside
			// the map, which means the cursor is pointed outside the map.
			tileType = GetTile(clamped).TileType;
			elevation = GetTile(clamped).Elevation;

			// mask the corners of each tile to determine tile type
			// NN EE SS WW
			// 00 00 00 00
			// mask = (n << 6) | (e << 4) | (s << 2) | w;

			unsigned char top = (tileType >> 6) & 3;
			unsigned char side;

			if (tile.x < tile.y)
				side = tileType & 3;
			else
				side = (tileType >> 4) & 3;

			unsigned char min = Min(top, side);
			elevation += min;
			top -= min;
			side -= min;

			// clamp side based on if the tile is on the north-west or
			// north-east side
			if (tile.x < tile.y)
				tileType = (top << 6) | (top << 4) | (side << 2) | side;
			else
				tileType = (top << 6) | (side << 4) | (side << 2) | top;
		}
		else
		{
			tileType = GetTile(tile).TileType;
			elevation = GetTile(tile).Elevation;
		}

		// adjust for elevation
		local.y += elevation * PPUE;

		count++;

		Point modifier;

		if (local.x < 0 || local.x >= TILE_X || local.y < 0 || local.y >= TILE_Y)
		{
			// perform bounds checking
			if (local.x >= TILE_X && local.y >= TILE_Y)
				modifier = Point(1, 0);
			else if (local.x < 0 && local.y >= TILE_Y)
				modifier = Point(0, 1);
			else if (local.x >= TILE_X && local.y < 0)
				modifier = Point(0, -1);
			else if (local.x < 0 && local.y < 0)
				modifier = Point(-1, 0);
			else if (local.x >= TILE_X)
				modifier = Point(1, -1);
			else if (local.y >= TILE_Y)
				modifier = Point(1, 1);
			else if (local.x < 0)
				modifier = Point(-1, 1);
			else if (local.y < 0)
				modifier = Point(-1, -1);
			else
				modifier = Point(0, 0);
		}
		else
		{
			// within tile bounding rect, check actual tile bounds
			modifier = MouseMapClamp(local, tileType);
			if (modifier.x == 0 && modifier.y == 0)
				return tile;
		}

		if (modifier.x == 0 && tile.y <= 0 || modifier.y == 0 && tile.x <= 0)
		{
			return clamped;
		}
		tile += modifier;
	}
}

Point Terrain::MouseMapClamp(const Point mmPoint, unsigned int tileType)
{
	// mask the corners of each tile to determine tile type
	// NN EE SS WW
	// 00 00 00 00
	// mask = (n << 6) | (e << 4) | (s << 2) | w;
	int topOffset   = (tileType >> 6) & 3;
	int rightOffset = (tileType >> 4) & 3;
	int botOffset   = (tileType >> 2) & 3;
	int leftOffset  = tileType & 3;

	topOffset =   TILE_Y     / 2 - topOffset   * PPUE;
	leftOffset =  TILE_Y * 3 / 4 - leftOffset  * PPUE;
	rightOffset = TILE_Y * 3 / 4 - rightOffset * PPUE;
	botOffset =   TILE_Y         - botOffset   * PPUE;

	Point top   = Point(TILE_X / 2, topOffset);
	Point left  = Point(0,          leftOffset);
	Point right = Point(TILE_X,     rightOffset);
	Point bot   = Point(TILE_X / 2, botOffset);

	if (mmPoint.x >= 32)
	{
		if (AboveLine(top, top + Point(31, -15), mmPoint) < 0)
			return Point(-1, -1);
		else if (AboveLine(top, right + Point(-1, -1), mmPoint) <= 0)
			return Point(0, -1);
		else if (AboveLine(bot, bot + Point(31, 15), mmPoint) > 0)
			return Point(1, 1);
		else if (AboveLine(bot, right + Point(-1, 1), mmPoint) >= 0)
			return Point(1, 0);
		else
			return Point(0, 0);
	}
	else
	{
		top += Point(-1, 0);
		bot += Point(-1, 0);
		if (AboveLine(top + Point(-31, -15), top, mmPoint) < 0)
			return Point(-1, -1);
		else if (AboveLine(left + Point(0, -1), top, mmPoint) <= 0)
			return Point(-1, 0);
		else if (AboveLine(bot + Point(-31, 15), bot, mmPoint) > 0)
			return Point(1, 1);
		else if (AboveLine(left + Point(0, 1), bot, mmPoint) >= 0)
			return Point(0, 1);
		else
			return Point(0, 0);
	}
}

Point Terrain::FineWorldToPoint(const Point world)
{
	// find the tile we are over, then we test its endpoints
	Point point = FineWorldToTile(world);

	// we are outside the bounds of the map
	if(point.x < 0 || point.x >= m_width || point.y < 0 || point.y >= m_height)
		return Point(-1, -1);

	Point top = FinePointToWorld(point);
	Point left = FinePointToWorld(point + Point(0, 1));
	Point right = FinePointToWorld(point + Point(1, 0));
	Point bot = FinePointToWorld(point + Point(1, 1));

	if (AboveLine((bot + left) / 2, (right + top) / 2, world) > 0)
		point.x += 1;
	if (!(AboveLine((bot + right) / 2, (left + top) / 2, world) > 0))
		point.y += 1;

	return point;
}

void Terrain::DrawViewport(const Rect viewWorld, bool drawSelected)
{
	Rect viewTile = Rect(
		RoughWorldToTileX(viewWorld.TopLeft()),
		RoughWorldToTileY(viewWorld.TopRight()),
		FineWorldToTile(viewWorld.BotRight()).x,
		FineWorldToTile(viewWorld.BotLeft()).y);

	viewTile.Clamp(0, 0, m_width, m_height);

	al_hold_bitmap_drawing(true);
	for(int y = viewTile.top; y <= viewTile.bot; y++)
	{
		for(int x = viewTile.left; x <= viewTile.right; x++)
		{
			Point fine = FineTileToWorld(Point(x, y)) - viewWorld.TopLeft();

			int tt = GetTile(Point(x, y)).TileType;

			if (m_selection.PointInside(Point(x, y)) && drawSelected)
				al_draw_tinted_bitmap(m_tileBitmaps[tt], al_map_rgba_f(1, 1, 1, .5), fine.x, fine.y, 0);
			else
				al_draw_bitmap(m_tileBitmaps[tt], fine.x, fine.y, 0);
		}
	}
	al_hold_bitmap_drawing(false);


	// draw the point the mouse is currently over
	if(!drawSelected)
	{
		Point fine = FinePointToWorld(m_selection.TopLeft()) - viewWorld.TopLeft();
		al_draw_filled_circle(fine.x, fine.y, 2, al_map_rgb(255, 255, 255));
	}

	// draw buildings
	for(int y = viewTile.top; y <= viewTile.bot; y++)
	{
		for(int x = viewTile.left; x <= viewTile.right; x++)
		{
			Point xy = Point(x, y);
			if(GetTile(xy).BuildingCorner)
			{
				Point fine = FineTileToWorld(xy) - viewWorld.TopLeft();
				Building* building = GetTile(xy).Building;
				int w = building->GetWidth();
				int h = building->GetHeight();
				Rect footprint = Rect(xy, xy + Point(w, h));

				// is the building selected?
				if(footprint.PointInside(m_selection.TopLeft()))
					al_draw_tinted_bitmap(building->GetTexture(), al_map_rgba_f(1, 0, .5, .5), fine.x - h * 32 + 32, fine.y + 32, 0);
				else
					al_draw_bitmap(building->GetTexture(), fine.x - h * 32 + 32, fine.y + 32, 0);
			}
		}
	}
}

void Terrain::LoadMapFromHeightmap(ALLEGRO_BITMAP *heightMap)
{
	std::vector<unsigned char> pointMap;

	m_pmw = al_get_bitmap_width(heightMap);
	m_pmh = al_get_bitmap_height(heightMap);

	ALLEGRO_LOCKED_REGION *region = al_lock_bitmap(heightMap, ALLEGRO_PIXEL_FORMAT_ANY, 0);

	// read colors into the corner elevation map
	ALLEGRO_COLOR color;
	for(int y = 0; y < m_pmh; y++)
	{
		for(int x = 0; x < m_pmw; x++)
		{
			color = al_get_pixel(heightMap, x, y);
			pointMap.push_back((int)(color.r * 255));
		}
	}
	al_unlock_bitmap(heightMap);

	// clamp values to ensure adjacent points do not experience an elevation change of more than 3 units
	int min;
	bool flag;
	do
	{
		flag = false;
		
		for(int y = 1; y < m_pmh - 1; y++)
		{
			for(int x = 1; x < m_pmw - 1; x++)
			{
				min = pointMap[(y-1) * m_pmw + x];               // up
				min = Min(pointMap[(y-1) * m_pmw + (x+1)], min); // up-right
				min = Min(pointMap[(y  ) * m_pmw + (x+1)], min); // right
				min = Min(pointMap[(y+1) * m_pmw + (x+1)], min); // down-right
				min = Min(pointMap[(y+1) * m_pmw + (x  )], min); // down
				min = Min(pointMap[(y+1) * m_pmw + (x-1)], min); // down-left
				min = Min(pointMap[(y  ) * m_pmw + (x-1)], min); // left
				min = Min(pointMap[(y-1) * m_pmw + (x-1)], min); // up-left

				if(pointMap[y * m_pmw + x] > (min + 3))
				{
					flag = true;
					pointMap[y * m_pmw + x] = min + 3;
				}
			}
		}
	} while(flag);

	// initalize the map
	m_map.clear();
	for(int y = 1; y < m_pmh - 1; y++)
		for(int x = 1; x < m_pmw - 1; x++)
			m_map.push_back(Tile(0, 0, pointMap[y * m_pmw + x]));

	// these represent the dimensions of the tiles actually used
	m_width = m_pmw - 3;
	m_height = m_pmh - 3;

	// these variables become the 'actual' height and width of map
	m_pmw = m_width + 1;
	m_pmh = m_height + 1;

	// generate tiletypes given point data
	GenerateTilesFromPoints();
}

void Terrain::GenerateTilesFromPoints()
{
	unsigned int mask, n, s, e, w, min;

	// determine tile types
	for(int y = 0; y < m_height; y++)
	{
		for(int x = 0; x < m_width; x++)
		{
			n = m_map[(y  ) * m_pmw + (x  )].NorthPoint;
			e = m_map[(y  ) * m_pmw + (x+1)].NorthPoint;
			w = m_map[(y+1) * m_pmw + (x  )].NorthPoint;
			s = m_map[(y+1) * m_pmw + (x+1)].NorthPoint;
			min = Min(Min(n, s), Min(e, w));
			n -= min;
			s -= min;
			e -= min;
			w -= min;

			// mask the corners of each tile to determine tile type
			// NN EE SS WW
			// 00 00 00 00
			mask = (n << 6) | (e << 4) | (s << 2) | w;
			m_map[y * m_pmw + x].Elevation = min;
			m_map[y * m_pmw + x].TileType = mask;
		}
	}
}

bool Terrain::PlaceBuilding(int x, int y, Building& building)
{
	// is the land we are building on occupied?
	for (int y1 = y; y1 < y + building.GetHeight() + 1; y1++)
	{
		for (int x1 = x; x1 < x + building.GetWidth() + 1; x1++)
		{
			if (GetTile(Point(x1, y1)).LockedCorner)
			{
				return false;
			}
		}
	}

	// if possible, flatten the building land. otherwise, cancel construction
	if(!Flatten(Rect(x, y, building.GetWidth() + x, building.GetHeight() + y), -2))
		return false;

	GetTile(Point(x, y)).BuildingCorner = true;
	
	for(int y1 = y ; y1 < y + building.GetHeight(); y1++)
	{
		for(int x1 = x ; x1 < x + building.GetWidth(); x1++)
		{
			GetTile(Point(x1, y1)).Building = &building;
		}
	}

	for(int y1 = y + 1; y1 < y + building.GetHeight(); y1++)
	{
		for(int x1 = x + 1; x1 < x + building.GetWidth(); x1++)
		{
			GetTile(Point(x1, y1)).LockedCorner = true;
		}
	}

	return true;
}

// elevation adjustment queue node, used to flatten regions
struct EAQN
{
	int x;
	int y;
	int last; // elevation of the last altered node

	EAQN(int x, int y, int last)
	{
		this->x = x;
		this->y = y;
		this->last = last;
	}
};

// change the elevation of (sx1, sy1) by change
void Terrain::AdjustSelectedElevation(int change)
{
	Flatten(m_selection, GetTile(m_selection.TopLeft()).NorthPoint + change);
}

// flatten a rectangle defined by the corners (x1, y1) and (x2, y2)
// targetElevation is the elevation the rectangle will be flattened to
// if targetElevation is -1, it will default to the north point of (x1, y1)
// if targetElevation is -2, it will default to the average elevation of the rectangle
// return value true means the elevation change was successful, false means you must
// revert the changes
bool Terrain::Flatten(const Rect region, int targetElevation)
{
	if(FlattenDryRun(region, targetElevation))
	{
		FinalizeElevationChange();
		return true;
	}
	else
	{
		RevertElevationChange();
		return false;
	}
}

// flatten a rectangle defined by the corners (x1, y1) and (x2, y2)
// targetElevation is the elevation the rectangle will be flattened to
// if targetElevation is -1, it will default to the north point of (x1, y1)
// if targetElevation is -2, it will default to the average elevation of the rectangle
// return value true means the elevation change was successful, false means you must
// revert the changes
bool Terrain::FlattenDryRun(const Rect region, int targetElevation)
{
	// we will select the elevation of the top corner for the targetElevation
	if(targetElevation == -1)
	{
		targetElevation = GetTile(region.TopLeft()).NorthPoint;
	}

	// set targetElevation to the average elevation of the rectangle
	if(targetElevation == -2)
	{
		int totalElevation = 0;
		for(int y = region.top; y < region.bot; y++)
			for(int x = region.left; x < region.right; x++)
				totalElevation += GetTile(Point(x, y)).NorthPoint;

		targetElevation = totalElevation / region.Area();
	}
	
	// setup queue stuff
	std::queue<EAQN> q;
	bool flag = true;
	EAQN n(0, 0, 0);

	// adjust the height of all corners within the rectangle
	for(int y = region.top; y <= region.bot; y++)
	{
		for(int x = region.left; x <= region.right; x++)
		{
			if (GetTile(Point(x, y)).LockedCorner && GetTile(Point(x, y)).NorthPoint != targetElevation)
				return false;
			GetTile(Point(x, y)).DryRunElevation = targetElevation;
		}
	}	

	// west
	if(region.left - 1 >= 0)
		for(int y = region.top; y <= region.bot; y++)
			q.push(EAQN(region.left - 1, y, targetElevation));
	// north
	if(region.top - 1 >= 0)
		for(int x = region.left; x <= region.right; x++)
			q.push(EAQN(x, region.top - 1, targetElevation));
	// east
	if(region.right + 1 < m_pmw)
		for(int y = region.top; y <= region.bot; y++)
			q.push(EAQN(region.right + 1, y, targetElevation));
	// south
	if(region.bot + 1 < m_pmh)
		for(int x = region.left; x <= region.right; x++)
			q.push(EAQN(x, region.bot + 1, targetElevation));

	// corners
	q.push(EAQN(region.left - 1, region.top - 1, targetElevation));
	q.push(EAQN(region.right + 1, region.bot + 1, targetElevation));
	q.push(EAQN(region.left - 1, region.bot + 1, targetElevation));
	q.push(EAQN(region.right + 1, region.top - 1, targetElevation));

	int currentElevation;
	// work through queue
	while(!q.empty())
	{
		n = q.front();
		q.pop();

		if(n.x < 0 || n.y < 0 || n.x >= m_pmw || n.y >= m_pmh)
			continue;

		if (GetTile(Point(n.x, n.y)).DryRunElevation != -1)
			currentElevation = GetTile(Point(n.x, n.y)).DryRunElevation;
		else
			currentElevation = GetTile(Point(n.x, n.y)).NorthPoint;

		if (currentElevation != n.last && GetTile(Point(n.x, n.y)).LockedCorner)
			return false;

		// our elevation is more than 3 units less than n.last
		if(n.last - currentElevation > 3)
		{
			GetTile(Point(n.x, n.y)).DryRunElevation = n.last - 3;
			currentElevation = n.last - 3;
		}
		// our elevation is more than 3 units more than n.last
		else if(currentElevation - n.last > 3)
		{
			GetTile(Point(n.x, n.y)).DryRunElevation = n.last + 3;
			currentElevation = n.last + 3;
		}
		else if(flag == false)
			continue;

		q.push(EAQN(n.x-1, n.y-1, currentElevation));
		q.push(EAQN(n.x, n.y-1, currentElevation));
		q.push(EAQN(n.x-1, n.y, currentElevation));
		q.push(EAQN(n.x+1, n.y+1, currentElevation));
		q.push(EAQN(n.x+1, n.y, currentElevation));
		q.push(EAQN(n.x, n.y+1, currentElevation));
		q.push(EAQN(n.x+1, n.y-1, currentElevation));
		q.push(EAQN(n.x-1, n.y+1, currentElevation));

		flag = false;
	}

	return true;
}

void Terrain::FinalizeElevationChange()
{
	for(int y = 0; y < m_pmh; y++)
	{
		for(int x = 0; x < m_pmw; x++)
		{
			if (GetTile(Point(x, y)).DryRunElevation != -1)
			{
				GetTile(Point(x, y)).NorthPoint = GetTile(Point(x, y)).DryRunElevation;
				GetTile(Point(x, y)).DryRunElevation = -1;
			}
		}
	}

	GenerateTilesFromPoints();
}

void Terrain::RevertElevationChange()
{
	for(int y = 0; y < m_pmh; y++)
	{
		for(int x = 0; x < m_pmw; x++)
		{
			GetTile(Point(x, y)).DryRunElevation = -1;
		}
	}
}