import random
import pygame
from tiles import *
from utility import *


BLOB_FILENAME = 'PlantPalm.png'
BLOB_SIZE = 40
BLOB_SPEED = 2.0 # 0.5
class Blob:
    def __init__(self, i, j, color, rectXs, rectYs, grid_size):
        self.__i = int(i)
        self.__j = int(j)
        self.__color = color
        self.__grid_size = grid_size
        self.__rectXs = rectXs
        self.__rectYs = rectYs
        self.__x_offset = 3
        self.__y_offset = 3
        x = self.__rectXs[self.__i] + self.__x_offset
        y = self.__rectYs[self.__j] + self.__y_offset
        self.__rect = pygame.Rect(x,
                                  y,
                                  BLOB_SIZE,
                                  BLOB_SIZE)
        self.__image = pygame.image.load(BLOB_FILENAME)
        self.__image = pygame.transform.scale(self.__image,
                                              (BLOB_SIZE * 2,
                                               BLOB_SIZE * 2))
        # self.__images = []
        self.__roads = []

    def draw(self, screen):
        s = pygame.Surface((BLOB_SIZE, BLOB_SIZE))
        s.set_alpha(200)
        s.fill(self.__color)
        for road in self.__roads:
            # pygame.draw.rect(screen,
            #                  self.__color,
            #                  road[0])
            # screen.blit(road[3], road[0])
            screen.blit(s, (road[0].x, road[0].y))
        # pygame.draw.rect(screen,
        #                  self.__color,
        #                  self.__rect)
        # screen.blit(self.__image, self.__rect)
        screen.blit(s, (self.__rect.x, self.__rect.y))
        
        # for i in range(len(self.__images)):
        #     screen.blit(self.__images[i], self.__roads[i][0])

    def move_head(self):
        temp_tiles = []
        # print('---')
        # for i in range(len(self.__roads)):
        #     print('{}: {}, {}'.format(i, self.__roads[i][1], self.__roads[i][2]))
        blob_count = 0
        for i in range(self.__grid_size):
            for j in range(self.__grid_size):
                # if tiles[i][j].has_blob() and not tiles[i][j].is_full_blob():
                if tiles[i][j].has_blob() and not tiles[i][j].is_blob_full():
                    blob_count += 1
                    is_full = True
                    temp_i = i - 1
                    if temp_i >= 0:
                        temp_tiles.append(tiles[temp_i][j])
                        is_full = False
                    temp_i = i + 1
                    if temp_i < 20:
                        temp_tiles.append(tiles[temp_i][j])
                        is_full = False
                    temp_j = j - 1
                    if temp_j >= 0:
                        temp_tiles.append(tiles[i][temp_j])
                        is_full = False
                    temp_j = j + 1
                    if temp_j < 20:
                        temp_tiles.append(tiles[i][temp_j])
                        is_full = False
                    if is_full:
                        tiles[i][j].mark_as_full_blob()
        temp_tiles_2 = []
        for tile in temp_tiles:
            if tile.has_blob():
                pass
            else:
                temp_tiles_2.append(tile)
        # for tile in temp_tiles_2:
        #     pass
        temp_tiles_2 = list(set(temp_tiles_2))
        if len(temp_tiles_2) == 0:
            return;
        tile = temp_tiles_2[random.randint(0, len(temp_tiles_2) - 1)]
        # self.__roads.append((self.__rect, self.__i, self.__j))
        self.__image = pygame.image.load(BLOB_FILENAME)
        self.__image = pygame.transform.scale(self.__image,
                                              (BLOB_SIZE * 2,
                                               BLOB_SIZE * 2))
        # self.__images.append(self.__image)
        self.__roads.append((self.__rect,
                             self.__i,
                             self.__j,
                             self.__image))
        self.__i = tile.i()
        self.__j = tile.j()
        tiles[self.__i][self.__j].add_blob()
        x = self.__rectXs[self.__i] + self.__x_offset
        y = self.__rectYs[self.__j] + self.__y_offset
        self.__rect = pygame.Rect(x,
                                  y,
                                  BLOB_SIZE,
                                  BLOB_SIZE)
        result = 0
        if blob_count <= 0:
            result = 1
        if len(temp_tiles_2) <= 0:
            result = 2
        return (generate_word(str(blob_count)), result)

    def compare(self, i, j):
        for blob in self.__roads:
            if i == blob[1] and j == blob[2]:
                return True
        return False

    def remove(self, i, j):
        for blob in self.__roads:
            if i == blob[1] and j == blob[2]:
                self.__roads.remove(blob)
