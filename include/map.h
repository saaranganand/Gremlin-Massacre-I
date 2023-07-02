#ifndef MAP_H_
#define MAP_H_

#include <raylib.h>
#include <collider.h>
#include <vector>
#include <map>
#include <string>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

enum TileType { STATIC = 0, DAMAGE, BACKGROUND, BONFIRE, NONE };

struct Tile {
    TileType type;

    Rectangle source;

    int damage;
    int health;

    bool pogo;
    bool active;

    Tile(TileType tp = NONE, int dmg = 0, bool pog = false, int hp = 10);
};

struct Map {
    bool instantiated;

    float tileSize;
    int width;
    int height;
    Vector2 startTile;

    Tile** tileMap;
    std::map<int, Tile> tiles;
    Texture2D tileSheet;

    Map();

    void writeMap(const char* file);
    void createTiles();

    void kill();
    void reset();

    Tile* getTile(int x, int y);

    void draw(Camera2D camera);
};

void tutorial_assignMap(Map& map);


#endif