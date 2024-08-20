import random
import pygame
from main import *

PLAYER_SIZE = 43 / 2
NORTH = -PLAYER_SIZE
SOUTH = PLAYER_SIZE
WEST = -PLAYER_SIZE
EAST = PLAYER_SIZE
NO_DIRECTION = 0
PLAYER_SPEED = 0.5
ENEMY_SPEED_0 = 1.1
ENEMY_SPEED_1 = 1.5
ENEMY_SPEED_2 = 1.0
class Player:
    def __init__(self, filename, i, j, color, is_enemy=True):
        self.__filename = filename
        self.__i = int(i)
        self.__j = int(j)
        self.__is_enemy = is_enemy
        self.__previous_i = self.__i
        self.__previous_j = self.__j
        self.__color = color
        self.__modifier_x = NO_DIRECTION
        self.__modifier_y = NO_DIRECTION
        self.__image = pygame.image.load(self.__filename)
        self.__image = pygame.transform.scale(self.__image,
                                              (PLAYER_SIZE * 4.0,
                                               PLAYER_SIZE * 4.0))

    def i(self):
        return self.__i

    def j(self):
        return self.__j

    def draw(self, screen, percent):
        # pygame.draw.rect(screen,
        #                  self.__color,
        #                  pygame.Rect(rectXs[self.__i] + 2,
        #                              rectYs[self.__j] + 2,
        #                              PLAYER_SIZE - 2,
        #                              PLAYER_SIZE - 2))
        animation_x = rectXs[self.__previous_i] + PLAYER_SIZE + 1
        animation_x -= rectXs[self.__i] + PLAYER_SIZE + 1
        animation_x *= percent
        circle_x = rectXs[self.__i] + PLAYER_SIZE + 1
        animation_y = rectYs[self.__previous_j] + PLAYER_SIZE + 1
        animation_y -= rectYs[self.__j] + PLAYER_SIZE + 1
        animation_y *= percent
        circle_y = rectYs[self.__j] + PLAYER_SIZE + 1
        randomize = 0.0
        x = 0.0
        y = 0.0
        if self.__previous_i < self.__i:
            pass
        else:
            pass
        if self.__is_enemy:
            x = circle_x + animation_x - PLAYER_SIZE + randomize
            y = circle_y + animation_y
            '''
            pygame.draw.ellipse(screen,
                                self.__color,
                                (x,
                                 y,
                                 PLAYER_SIZE * 2,
                                 PLAYER_SIZE),
                                2)
            '''
            if (random.randint(0, 9) > 7):
                randomize = random.randint(-1, 1)
            x = rectXs[self.__i] + animation_x + PLAYER_SIZE + randomize
            y = rectYs[self.__j] + animation_y + PLAYER_SIZE + 1
            '''
            pygame.draw.circle(screen,
                               self.__color,
                               (x, y),
                               PLAYER_SIZE / 2)
            '''
            screen.blit(self.__image, pygame.Rect(x - (PLAYER_SIZE * 2),
                                                  y - (PLAYER_SIZE * 2),
                                                  PLAYER_SIZE,
                                                  PLAYER_SIZE))
        else:
            '''
            pygame.draw.circle(screen,
                               self.__color,
                               (circle_x + animation_x,
                                circle_y + animation_y),
                               PLAYER_SIZE,
                               2)
            pygame.draw.circle(screen,
                               self.__color,
                               (rectXs[self.__i] + PLAYER_SIZE + 1 + self.__modifier_x,
                                rectYs[self.__j] + PLAYER_SIZE + 1 + self.__modifier_y),
                               PLAYER_SIZE / 2)
            '''
            screen.blit(self.__image, pygame.Rect(circle_x + animation_x - (PLAYER_SIZE * 2),
                                                  circle_y + animation_y - (PLAYER_SIZE * 2),
                                                  PLAYER_SIZE,
                                                  PLAYER_SIZE))

    def move_left(self):
        self.__modifier_x = WEST
        self.__modifier_y = NO_DIRECTION
        self.__previous_i = self.__i
        self.__previous_j = self.__j
        self.__i -= 1
        if self.__i < MIN_X:
            self.__i = MIN_X

    def move_right(self):
        self.__modifier_x = EAST
        self.__modifier_y = NO_DIRECTION
        self.__previous_i = self.__i
        self.__previous_j = self.__j
        self.__i += 1
        if self.__i > MAX_X:
            self.__i = MAX_X

    def move_up(self):
        self.__modifier_y = NORTH
        self.__modifier_x = NO_DIRECTION
        self.__previous_i = self.__i
        self.__previous_j = self.__j
        self.__j -= 1
        if self.__j < MIN_Y:
            self.__j = MIN_Y

    def move_down(self):
        self.__modifier_y = SOUTH
        self.__modifier_x = NO_DIRECTION
        self.__previous_i = self.__i
        self.__previous_j = self.__j
        self.__j += 1
        if self.__j > MAX_Y:
            self.__j = MAX_Y

    def dont_move(self):
        self.__modifier_y = NO_DIRECTION
        self.__modifier_x = NO_DIRECTION
        self.__previous_i = self.__i
        self.__previous_j = self.__j

    def move_enemy(self):
        choice = random.randint(0, 4)
        if choice == 0:
            self.move_left()
        if choice == 1:
            self.move_right()
        if choice == 2:
            self.move_up()
        if choice == 3:
            self.move_down()
        if choice == 4:
            self.dont_move()

    def reset_movement(self):
        self.__modifier_x = NO_DIRECTION
        self.__modifier_y = NO_DIRECTION

    def display(self):
        print('{}, {}'.format(self.__i, self.__j))
