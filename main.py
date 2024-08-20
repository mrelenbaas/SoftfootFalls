import os
import sys
import random
import time
import pygame
from blob import *
from grid import *
from player import *
from tiles import *
from timer import *
from utility import *
# from pygame.locals import *

WIDTH = 640 * 2
HEIGHT = 480 * 2
CAPTION = 'SoftfootFalls'


ROAD_FILENAME = 'Road.png'
ROAD_SIZE = 41
class Road:
    def __init__(self, i, j, color, houses, dens):
        self.__i = int(i)
        self.__j = int(j)
        self.__color = color
        self.__houses = houses
        self.__dens = dens
        self.__randStart1 = 9
        self.__randEnd1 = 10
        self.__randStart2 = 13
        self.__randEnd2 = 14
        x = rectXs[self.__i] + random.randint(self.__randStart1,
                                              self.__randEnd1)
        y = rectYs[self.__j] + random.randint(self.__randStart1,
                                              self.__randEnd1)
        self.__width = ROAD_SIZE - random.randint(self.__randStart2,
                                                  self.__randEnd2)
        self.__height = ROAD_SIZE - random.randint(self.__randStart2,
                                                   self.__randEnd2)
        self.__rect = pygame.Rect(x, y, self.__width, self.__height)
        self.__image = pygame.image.load(ROAD_FILENAME)
        self.__image = pygame.transform.scale(self.__image,
                                              (self.__width,
                                               self.__height))
        self.__images = []
        self.__roads = []

    def draw(self, screen):
        '''
        for road in self.__roads:
            pygame.draw.rect(screen,
                             self.__color,
                             road[0])
        pygame.draw.rect(screen,
                         self.__color,
                         self.__rect)
        '''
        for i in range(len(self.__images)):
            screen.blit(self.__images[i], self.__roads[i][0])
        screen.blit(self.__image, self.__rect)

    def move_head(self):
        temp_tiles = []
        # print('---')
        # for i in range(len(self.__roads)):
        #     print('{}: {}, {}'.format(i, self.__roads[i][1], self.__roads[i][2]))
        for i in range(GRID_SIZE - 1):
            for j in range(GRID_SIZE - 1):
                if tiles[i][j].has_road() \
                   and not tiles[i][j].is_full():
                    is_full = True
                    temp_i = i - 1
                    if temp_i >= 0 \
                       and i - 2 >= 0 \
                       and i - 3 >= 0 \
                       and not tiles[temp_i - 1][j].has_road() \
                       and not tiles[temp_i - 2][j].has_road() \
                       and not tiles[temp_i - 3][j].has_road():
                        temp_tiles.append(tiles[temp_i][j])
                        is_full = False
                    else:
                        tiles[temp_i][j].mark_as_full()
                    temp_i = i + 1
                    if temp_i <= 19 \
                       and i + 2 <= 19 \
                       and i + 3 <= 19 \
                       and not tiles[temp_i + 1][j].has_road() \
                       and not tiles[temp_i + 2][j].has_road() \
                       and not tiles[temp_i + 3][j].has_road():
                        temp_tiles.append(tiles[temp_i][j])
                        is_full = False
                    else:
                        tiles[temp_i][j].mark_as_full()
                    temp_j = j - 1
                    if temp_j >= 0 \
                       and j - 2 >= 0 \
                       and j - 3 >= 0 \
                       and not tiles[i][temp_j - 1].has_road() \
                       and not tiles[i][temp_j - 2].has_road() \
                       and not tiles[i][temp_j - 3].has_road():
                        temp_tiles.append(tiles[i][temp_j])
                        is_full = False
                    else:
                        tiles[i][temp_j].mark_as_full()
                    temp_j = j + 1
                    if temp_j <= 19 \
                       and j + 2 <= 19 \
                       and j + 3 <= 19 \
                       and not tiles[i][temp_j + 1].has_road() \
                       and not tiles[i][temp_j + 2].has_road() \
                       and not tiles[i][temp_j + 3].has_road():
                        temp_tiles.append(tiles[i][temp_j])
                        is_full = False
                    else:
                        tiles[i][temp_j].mark_as_full()
                    if is_full:
                        tiles[i][j].mark_as_full()
        temp_tiles_2 = []
        for tile in temp_tiles:
            if tile.has_road() \
               or tile.has_house():
                pass
            else:
                temp_tiles_2.append(tile)
        # for tile in temp_tiles_2:
        #     pass
        if len(temp_tiles_2) == 0:
            return;
        tile = temp_tiles_2[random.randint(0, len(temp_tiles_2) - 1)]
        choice = random.randint(0, 9)
        if choice != 0 \
           and choice != 1 \
           and choice != 2:
            self.__roads.append((self.__rect, self.__i, self.__j))
            self.__images.append(pygame.image.load(ROAD_FILENAME))
            self.__images[-1] = pygame.transform.scale(self.__images[-1],
                                                       (self.__width,
                                                        self.__height))
            self.__i = tile.i()
            self.__j = tile.j()
            tiles[self.__i][self.__j].add_road()
            x = rectXs[self.__i]
            y = rectYs[self.__j]
            self.__rect = pygame.Rect(x + random.randint(self.__randStart1,
                                                         self.__randEnd1),
                                      y + random.randint(self.__randStart1,
                                                         self.__randEnd1),
                                      ROAD_SIZE - random.randint(self.__randStart2,
                                                                 self.__randEnd2),
                                      ROAD_SIZE - random.randint(self.__randStart2,
                                                                 self.__randEnd2))
        else:
            self.__houses.append(House(tile.i(), tile.j(), (62, 35, 14)))
            tiles[tile.i()][tile.j()].add_house()


TERRAIN_FILENAME = 'TerrainRocks.png'
TERRAIN_SIZE = 41
class Terrain:
    def __init__(self, i, j, color):
        self.__i = int(i)
        self.__j = int(j)
        self.__color = color
        x = rectXs[self.__i] + 2
        y = rectYs[self.__j] + 2
        self.__rect = pygame.Rect(x + random.randint(-1, 1),
                                  y + random.randint(-1, 1),
                                  TERRAIN_SIZE, TERRAIN_SIZE)
        self.__image = pygame.image.load(TERRAIN_FILENAME)
        self.__image = pygame.transform.scale(self.__image,
                                              (TERRAIN_SIZE,
                                               TERRAIN_SIZE))

    def draw(self, screen):
        '''
        pygame.draw.rect(screen,
                         self.__color,
                         self.__rect)
        '''
        screen.blit(self.__image, self.__rect)

    def compare(self, i, j):
        if i == self.__i and j == self.__j:
            return True
        return False


HOUSE_FILENAME = 'HouseHaunt.png'
TEMPLE_SPEED = 0.1
HOUSE_SIZE = 30
class House:
    def __init__(self, i, j, color, houses=None, dens=None, is_den=False):
        self.__i = int(i)
        self.__j = int(j)
        self.__color = color
        self.__houses = houses
        self.__dens = dens
        x = rectXs[self.__i] + 8
        y = rectYs[self.__j] + 8
        self.__rect = pygame.Rect(x, y, HOUSE_SIZE, HOUSE_SIZE)
        self.__image = pygame.image.load(HOUSE_FILENAME)
        self.__image = pygame.transform.scale(self.__image,
                                              (HOUSE_SIZE,
                                               HOUSE_SIZE))

    def draw(self, screen):
        '''
        pygame.draw.rect(screen,
                         self.__color,
                         self.__rect)
        '''
        screen.blit(self.__image, self.__rect)

    def first_road(self):
        return Road(self.__i,
                    self.__j,
                    (208, 237, 242),
                    self.__houses,
                    self.__dens)

    def i(self):
        return self.__i

    def j(self):
        return self.__j


class Main:
    def __init__(self):
        pygame.init();
        self.__screen = pygame.display.set_mode((WIDTH, HEIGHT))
        title = generate_word('SOFTFOOTFALLS')
        title_x = (WIDTH / 2)
        title_x -= (len(title) * LETTER_SIZE / 2)
        title_x += (LETTER_SIZE / 2)
        title_y = (HEIGHT / 2)
        title_y -= (LETTER_SIZE / 2)
        self.__title_is_on = True
        self.__title_timer = Timer(3.0, self.teardown_title)
        self.__gui = generate_word('ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF')
        gui_max_len = len(self.__gui)
        gui_x = (WIDTH / 2)
        gui_x -= (gui_max_len * LETTER_SIZE / 2)
        gui_x += (LETTER_SIZE / 2)
        gui_y = HEIGHT - LETTER_SIZE
        self.__gui = generate_word('')
        self.__fps_counter = 0
        fps_timer = Timer(1.0, self.display_fps, self.count_fps)
        self.__debug = False
        done = False
        bg = (73, 136, 217)
        # bg = (25, 80, 62)
        self.__i = 0
        self.__j = 0
        pygame.display.set_caption(CAPTION)
        # deprecated
        # pygame.mouse.set_system_cursor(pygame.SYSTEM_CURSOR_CROSSHAIR)
        pygame.mouse.set_cursor(pygame.SYSTEM_CURSOR_CROSSHAIR)
        BOTTOM = HEIGHT - LETTER_SIZE
        GRID_HEIGHT = (HEIGHT - (PNG_SIZE / 2)) / GRID_SIZE
        x_offset = WIDTH - (GRID_HEIGHT * GRID_SIZE)
        for i in range(GRID_SIZE + 1):
            x1s.append((x_offset / 2) + (i * GRID_HEIGHT))
            x2s.append((x_offset / 2) + (i * GRID_HEIGHT))
            y1s.append(0)
            y2s.append(GRID_SIZE * GRID_HEIGHT)
            rectXs.append(x1s[-1])
            temp_tiles = []
            for j in range(GRID_SIZE + 1):
                x3s.append(x_offset / 2)
                x4s.append((x_offset / 2) + (GRID_SIZE * GRID_HEIGHT))
                y3s.append(j * GRID_HEIGHT)
                y4s.append(j * GRID_HEIGHT)
                rectYs.append(y3s[-1])
                temp_tiles.append(Tile(i, j))
            tiles.append(temp_tiles)
        self.__terrain = []
        for i in range(GRID_SIZE):
            for j in range(GRID_SIZE):
                self.__terrain.append(Terrain(i,
                                              j,
                                              (211, 118, 18)))
                # tiles.append(Tile(i, j, rectXs[-1], rectYs[-1]))
                # tiles[-1].add_dirt()
                tiles[i][j].add_dirt()
        # self.__road = Road((len(rectXs) / 2) - 1,
        #                    len(rectYs) / 2,
        #                    (208, 237, 242))
        self.__i = 19
        self.__j = 19
        self.__blob = Blob(self.__i,
                           self.__j,
                           (25, 80, 62),
                           rectXs,
                           rectYs,
                           GRID_SIZE)
        self.__blob_timer = Timer(BLOB_SPEED, self.start_blob_round)
        tiles[self.__i][self.__j].add_blob()
        self.__houses = []
        '''
        self.__i = int(len(rectXs) / 2)
        self.__j = int(len(rectYs) / 2)
        self.__houses.append(House(self.__i - 3,
                                   self.__j - 2,
                                   (62, 35, 14)))
        self.__houses.append(House(self.__i - 2,
                                   self.__j - 2,
                                   (62, 35, 14)))
        self.__houses.append(House(self.__i - 1,
                                   self.__j - 2,
                                   (62, 35, 14)))
        '''
        '''
        tiles[self.__i][self.__j - 210].add_house()
        tiles[self.__i][self.__j - 210].add_house()
        tiles[self.__i][self.__j - 210].add_house()
        '''
        self.__dens = []
        self.__dens.append(House((len(rectXs) / 2) - 9,
                                 (len(rectYs) / 2) - 9,
                                 (162, 35, 214),
                                 is_den=True))
        self.__dens.append(House((len(rectXs) / 2) + 8,
                                 (len(rectYs) / 2) + 8,
                                 (162, 35, 214),
                                 is_den=True))
        self.__dens.append(House((len(rectXs) / 2) - 10,
                                 (len(rectYs) / 2) + 9,
                                 (162, 35, 214),
                                 is_den=True))
        self.__i = int(len(rectXs) / 2)
        self.__j = int(len(rectYs) / 2)
        self.__temple = House(self.__i,
                              self.__j,
                              (212, 185, 164),
                              self.__houses,
                              self.__dens)
        self.__temple_timer = Timer(TEMPLE_SPEED, self.start_round)
        tiles[self.__i][self.__j - 210].add_house()
        self.__i = int(len(rectXs) / 2)
        self.__j = int(len(rectYs) / 2)
        self.__road = self.__temple.first_road()
        tiles[self.__i][self.__j - 210].add_road()
        self.__enemies = []
        self.__enemies.append(Player('CharacterSaucerBone.png',
                                     len(rectXs) / 2,
                                     len(rectYs) / 2,
                                     (255, 255, 0)))
        self.__enemies.append(Player('CharacterSaucerHowdy.png',
                                     len(rectXs) / 2,
                                     len(rectYs) / 2,
                                     (0, 255, 255))),
        self.__enemies.append(Player('CharacterSaucerOverhead.png',
                                     len(rectXs) / 2,
                                     len(rectYs) / 2,
                                     (255, 0, 255)))
        self.__enemy_timers = []
        self.__enemy_timers.append(Timer(ENEMY_SPEED_0,
                                         self.move_enemy_0))
        self.__enemy_timers.append(Timer(ENEMY_SPEED_1,
                                         self.move_enemy_1))
        self.__enemy_timers.append(Timer(ENEMY_SPEED_2,
                                         self.move_enemy_2))
        self.__player = Player('CharacterBirdBirdly.png',
                               len(rectXs) / 2,
                               len(rectYs) / 2,
                               (0, 255, 0),
                               is_enemy=False)
        self.__player_timer = Timer(PLAYER_SPEED, self.move_player)
        # self.__player_timer = Timer(PLAYER_SPEED, self.move_enemy)
        self.__screen.fill((0, 0, 0))
        keys = []
        self.__events = None
        while not done:
            self.__events = pygame.event.get()
            for event in self.__events:
                keys = pygame.key.get_pressed()
                result_a = False
                result_d = False
                result_w = False
                result_s = False
                if keys[pygame.K_a]:
                    pass
                else:
                    result_a = True
                if keys[pygame.K_d]:
                    pass
                else:
                    result_d = True
                if keys[pygame.K_w]:
                    pass
                else:
                    result_w = True
                if keys[pygame.K_s]:
                    pass
                else:
                    result_s = True
                if result_a and result_d and result_w and result_s:
                    self.__player.reset_movement()
                if(event.type == pygame.QUIT):
                    done = True
                    pygame.quit()
                    sys.exit()
                if event.type == pygame.KEYDOWN:
                    key=pygame.key.name(event.key)
                    if key == 'p':
                        if self.__debug:
                            self.__gui = generate_word('RESET')
                            self.__debug = False
                        else:
                            self.__debug = True
                    if key == 'a':
                        self.__player.move_left()
                        self.__player_timer.reset(reset_percent=True)
                        tiles[self.__player.i()][self.__player.j()-210].remove_blob()
                        tiles[self.__player.i()][self.__player.j()-210].mark_as_not_full_blob()
                        self.__blob.remove(self.__player.i(),
                                           self.__player.j()-210)
                    if key == 'd':
                        self.__player.move_right()
                        self.__player_timer.reset(reset_percent=True)
                        tiles[self.__player.i()][self.__player.j()-210].remove_blob()
                        tiles[self.__player.i()][self.__player.j()-210].mark_as_not_full_blob()
                        self.__blob.remove(self.__player.i(),
                                           self.__player.j()-210)
                    if key == 'w':
                        self.__player.move_up()
                        self.__player_timer.reset(reset_percent=True)
                        tiles[self.__player.i()][self.__player.j()-210].remove_blob()
                        tiles[self.__player.i()][self.__player.j()-210].mark_as_not_full_blob()
                        self.__blob.remove(self.__player.i(),
                                           self.__player.j()-210)
                    if key == 's':
                        self.__player.move_down()
                        self.__player_timer.reset(reset_percent=True)
                        tiles[self.__player.i()][self.__player.j()-210].remove_blob()
                        tiles[self.__player.i()][self.__player.j()-210].mark_as_not_full_blob()
                        self.__blob.remove(self.__player.i(),
                                           self.__player.j()-210)
                    # print(key, 'Key is pressed')
                if event.type == pygame.KEYUP:
                    key=pygame.key.name(event.key)
                    # print(key, 'Key is released')
                if event.type == pygame.MOUSEBUTTONDOWN:
                    pos=pygame.mouse.get_pos()
                    btn=pygame.mouse
                    # print('x = {}, y = {}'.format(pos[0], pos[1]))
                if event.type == pygame.MOUSEMOTION:
                    pos=event.pos
            self.__screen.fill(bg)
            draw_lines(self.__screen)
            draw_boxes(self.__screen)
            for tile in self.__terrain:
                tile.draw(self.__screen)
            self.__road.draw(self.__screen)
            self.__temple.draw(self.__screen)
            for house in self.__houses:
                house.draw(self.__screen)
            for den in self.__dens:
                den.draw(self.__screen)
            self.__blob.draw(self.__screen)
            for i in range(len(self.__enemies)):
                if self.__enemy_timers[i].percent() <= 0.0:
                    self.__enemy_timers[i].reset(reset_percent=True)
                    # print('{}, {}'.format(self.__enemies[i].i(), self.__enemies[i].j()-210))
                    # tiles[self.__enemies[i].i()][self.__enemies[i].j()-210].remove_blob()
                    # tiles[self.__enemies[i].i()][self.__enemies[i].j()-210].mark_as_not_full_blob()
                    # self.__blob.remove(self.__enemies[i].i(),
                    #                    self.__enemies[i].j()-210)
                self.__enemies[i].draw(self.__screen,
                                       self.__enemy_timers[i].percent())
            self.__player.draw(self.__screen, self.__player_timer.percent())
            if self.__title_timer != None:
                if self.__title_is_on:
                    draw_title(self.__screen, title_x, title_y, title)
                else:
                    draw_title_fade(self.__screen,
                                    title_x,
                                    title_y,
                                    title,
                                    self.__title_timer.percent())
                self.__title_timer.update()
            draw_title(self.__screen, gui_x, gui_y, self.__gui)
            self.__temple_timer.update()
            self.__blob_timer.update()
            self.__enemy_timers[0].update()
            self.__enemy_timers[1].update()
            self.__enemy_timers[2].update()
            self.__player_timer.update()
            fps_timer.update()
            # pygame.draw.polygon(screen, blue, ((25, 75), (76, 125), (275, 200), (350, 25), (60, 280)))
            pygame.display.update()

    def start_round(self):
        self.__road.move_head()
        self.__temple_timer.reset(reset_percent=True)

    def start_blob_round(self):
        result = self.__gui = self.__blob.move_head()
        if result == None:
            self.__gui = generate_word('WIN')
        elif result[1] == 0:
            self.__gui = result[0]
        elif result[1] == 2:
            self.__gui = generate_word('LOSE')

    def move_player(self):
        pass

    def move_enemy_0(self):
        tiles[self.__enemies[0].i()][self.__enemies[0].j()-210].remove_blob()
        tiles[self.__enemies[0].i()][self.__enemies[0].j()-210].mark_as_not_full_blob()
        self.__blob.remove(self.__enemies[0].i(),
                           self.__enemies[0].j()-210)
        self.__enemies[0].move_enemy()

    def move_enemy_1(self):
        tiles[self.__enemies[1].i()][self.__enemies[1].j()-210].remove_blob()
        tiles[self.__enemies[1].i()][self.__enemies[1].j()-210].mark_as_not_full_blob()
        self.__blob.remove(self.__enemies[1].i(),
                           self.__enemies[1].j()-210)
        self.__enemies[1].move_enemy()

    def move_enemy_2(self):
        tiles[self.__enemies[2].i()][self.__enemies[1].j()-210].remove_blob()
        tiles[self.__enemies[2].i()][self.__enemies[1].j()-210].mark_as_not_full_blob()
        self.__blob.remove(self.__enemies[2].i(),
                           self.__enemies[2].j()-210)
        self.__enemies[2].move_enemy()

    def teardown_title(self):
        self.__title_is_on = False
        self.__title_timer = Timer(3.0, self.delete_title, self.fade_title)

    def fade_title(self):
        pass

    def delete_title(self):
        self.__title_timer = None
        self.__gui = generate_word('FADECOMPLETE')

    def display_fps(self):
        # print(self.__fps_counter)
        if self.__debug:
            self.__gui = generate_word(str(self.__fps_counter))
        self.__fps_counter = 0

    def count_fps(self):
        self.__fps_counter += 1

if __name__ == '__main__':
    main = Main()
