import PIL
from PIL import Image

name = "tutorial"
dir = "maps/tutorial/"
map = Image.open(dir + name + ".png")
mapPixels = map.load()

mapData = open(dir + name + ".data", "r")

mapWidth = int(mapData.readline())
mapHeight = int(mapData.readline())

print(mapWidth, mapHeight)

mapStruct = open("../levels/tutorial/" + name + ".out", "x")

for x in range(mapWidth):
    for y in range(mapHeight):
        mapStruct.write(str(mapPixels[x, y]) + ",")