#include <../include/raylib.h>
#include <../include/defs.h>
#include <../include/map.h>
#include <../include/player.h>

void updateCameraToMap(Camera2D& camera, Player player, Map map) {
    float width = SCREEN_W / 2.f;
    float height = SCREEN_H / 2.f;
    
    camera.target.x = player.position.x + player.hurtbox.size.x / 2.f;
    camera.target.y = player.position.y - player.hurtbox.size.y / 2.f;
    camera.offset = { width, height };

    float minX = 0.f;
    float minY = 0.f;

    float maxX = map.width * map.tileSize;
    float maxY = map.width * map.tileSize;

    float effectiveWidth = width / camera.zoom;
    float effectiveHeight = height / camera.zoom;

    if (camera.target.x - effectiveWidth < minX) camera.target.x = minX + effectiveWidth;
    if (camera.target.x + effectiveWidth > maxX) camera.target.x = maxX - effectiveWidth;

    if (camera.target.y - effectiveHeight < minY) camera.target.y = minY + effectiveHeight;
    if (camera.target.y + effectiveHeight > maxY) camera.target.y = maxY - effectiveHeight;
}