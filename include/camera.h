#ifndef CAMERA_H_
#define CAMERA_H_

#include <raylib.h>
#include <map.h>
#include <player.h>

void updateCameraToMap(Camera2D& camera, Player followPos, Map map);

#endif