#include <../include/raylib.h>
#include <../include/map.h>

Tile::Tile(TileType tp, int dmg, bool pog, int hp) {
    type = tp;

    source = { 0, 0, 0, 0 };
    
    damage = dmg;
    health = hp;

    pogo = pog;
    active = true;
}

void Map::writeMap(const char* file) {
    std::ifstream mapData(file);
    int t = 0;

    while (mapData) {
        // Read each line of the map data.
        std::string line;
        
        if (!getline(mapData, line)) break;

        std::istringstream mapStream(line);
        
        while (mapStream) {
            std::string tile;
            // Split map data into comma seperated list.
            if (!getline(mapStream, tile, ',')) break;

            // Map data has tile names seperated by commas.
            // Each tile name has a map to a tile.
            // We take that tile, and add a pointer to it to the tileMap.
            tileMap[t] = &tiles[stoi(tile)];
            t++;
            if (t >= width * height) break;
        }
    }
}

void Map::createTiles() {
    Tile air = Tile(BACKGROUND);
    air.source = {0.f, 0.f, 64.f, 64.f};

    Tile spike = Tile(DAMAGE, 400.f, 1, true);
    spike.source = {64.f, 0.f, 64.f, 64.f};

    Tile wall = Tile(STATIC);
    wall.source = {128.f, 0.f, 64.f, 64.f};

    Tile error = Tile(NONE);
    error.source = {192.f, 0.f, 64.f, 64.f};

    tiles[26] = wall;
    tiles[2] = air;
    tiles[5] = air;
    tiles[27] = spike;
    tiles[11] = air;
    tiles[17] = air;
    tiles[0] = error;
}

Map::Map() {
    instantiated = false;

    width = 0;
    height = 0;

    tileSize = 80;
    startTile = { 0, 0 };

    tileSheet = loadTextureUnloadImage("assets/tileSheet.png");
    createTiles();
}

void Map::kill() {
    if (instantiated) free(tileMap);

    UnloadTexture(tileSheet);
    instantiated = false;
}

void Map::reset() {
    if (instantiated) free(tileMap);
    instantiated = false;
    // do something
}

Tile* Map::getTile(int x, int y) {
    return tileMap[x * height + y];
}

void Map::draw(Camera2D camera) {

    int y = (camera.target.y - camera.offset.y) / tileSize;
    int maxY = (camera.target.y + camera.offset.y) / tileSize;

    int x = (camera.target.x - camera.offset.x) / tileSize;
    int maxX = (camera.target.x + camera.offset.x) / tileSize;

    clamp(x, 0, width - 1);
    clamp(y, 0, height - 1);

    clamp(maxX, 0, width - 1);
    clamp(maxY, 0, height - 1);

    for (; y < maxY + 1; y++) {
        for (int i = x; i < maxX + 1; i++) {
            Rectangle dest = { i * tileSize, y * tileSize, tileSize, tileSize };
            DrawTexturePro(tileSheet, getTile(i, y)->source, dest, ZERO, 0.f, WHITE);
        }
    }
}

void tutorial_assignMap(Map& map) {
    const char* mapName = "levels/tutorial/tutorial.out";

    if (map.instantiated) free(map.tileMap);
    map.instantiated = true;
    map.width = 98;
    map.height = 23;
    map.startTile = { 8, 6 };
    
    map.tileMap = (Tile**) malloc(map.width * map.height * sizeof(Tile*));
    

    // Init map with error tiles, in case loading fails.
    for (int x = 0; x < map.width; x++) {
        for (int y = 0; y < map.height; y++) {
            map.tileMap[x * map.height + y] = &(map.tiles[0]);
        }
    }

    map.writeMap(mapName);
}