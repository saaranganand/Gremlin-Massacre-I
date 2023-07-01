import PIL
from PIL import Image

name = "test"
dir = "maps/"
map = Image.open(dir + name + ".png")
mapPixels = map.load()

mapData = open(dir + name + ".data", "r")

mapWidth = int(mapData.readline())
mapHeight = int(mapData.readline())

tileDict = {
    1: "air",
    2: "wall",
    3: "spike"
}

mapStruct = open(dir + name + ".out", "x")

for x in range(mapWidth):
    for y in range(mapHeight):
        mapStruct.write(tileDict[mapPixels[x, y]])
        if (x * mapWidth + y < mapWidth * mapHeight - 1):
            mapStruct.write(",")