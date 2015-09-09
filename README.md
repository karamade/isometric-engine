# Isometric Engine

![](https://github.com/rjwilson/isometric-engine/blob/master/screenie.png)

This is a small isometric game engine written in C++ using the Allegro game library that I've been experimenting with in my free time for fun. The interesting feature is the isometric tile map with tiles of variable height, similar to old isometric sim games like Transport Tycoon, Sim City, and Roller Coaster Tycoon.

The representation of a number of different tile elevations and images has proven to be an fun challenge. Particularly the algorithms for flattening large areas of tiles and determining which tile the mouse is over with regard to elevation.

The most interesting code is located in Terrain.cpp/Terrain.h
