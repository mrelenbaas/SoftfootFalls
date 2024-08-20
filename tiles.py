ROAD = 0
DIRT = 0
HOUSE = 0

tiles = []


class Tile:
    def __init__(self, i, j):
        self.__i = i
        self.__j = j
        self.__has_blob = False
        self.__has_road = False
        self.__has_dirt = False
        self.__has_house = False
        self.__is_full = False
        self.__is_blob_full = False

    def i(self):
        return self.__i

    def j(self):
        return self.__j

    def add_blob(self):
        self.__has_blob = True

    def remove_blob(self):
        self.__has_blob = False

    def has_blob(self):
        return self.__has_blob

    def add_road(self):
        self.__has_road = True

    def remove_road(self):
        self.__has_road = False

    def has_road(self):
        return self.__has_road

    def add_dirt(self):
        self.__has_dirt = True

    def remove_dirt(self):
        self.__has_dirt = False

    def has_dirt(self):
        return self.__has_dirt

    def add_house(self):
        self.__has_house = True

    def remove_house(self):
        self.__has_house = False

    def has_house(self):
        return self.__has_house

    def mark_as_full(self):
        self.__is_full = True

    def mark_as_not_full(self):
        self.__is_full = False

    def is_full(self):
        return self.__is_full

    def mark_as_full_blob(self):
        self.__is_blob_full = True

    def mark_as_not_full_blob(self):
        self.__is_blob_full = False

    def is_blob_full(self):
        return self.__is_blob_full
