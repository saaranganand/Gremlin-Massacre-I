import PIL
from PIL import Image

rooms = ["tutorial", "room1", "room2", "room3", "room4", "room5", "room6", "gandalfroom"]

for room in rooms:
    name = room
    dir = "maps/"
    map = Image.open(dir + name + ".png")
    mapPixels = map.load()

    mapData = open(dir + name + ".data", "r")

    mapWidth = int(mapData.readline())
    mapHeight = int(mapData.readline())

    print(mapWidth, mapHeight)

    mapStruct = open("../levels/" + name + ".out", "x")

    for x in range(mapWidth):
        for y in range(mapHeight):
            mapStruct.write(str(mapPixels[x, y]) + ",")

