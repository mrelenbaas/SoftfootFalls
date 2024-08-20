import pygame


GRID_SIZE = 20
THICKNESS = 1
MIN_X = 0
MAX_X = GRID_SIZE - 1
MIN_Y = 210
MAX_Y = 229

x1s = []
x2s = []
x3s = []
x4s = []
y1s = []
y2s = []
y3s = []
y4s = []
rectXs = []
rectYs = []


def draw_lines(screen):
    for i in range(GRID_SIZE + 1):
        pygame.draw.line(screen,
                         (255, 255, 255),
                         (x1s[i], y1s[i]),
                         (x2s[i], y2s[i]),
                         THICKNESS)
        for j in range(GRID_SIZE + 1):
            pygame.draw.line(screen,
                             (255, 255, 255),
                             (x3s[i], y3s[i]),
                             (x4s[i], y4s[i]),
                             THICKNESS)


def draw_boxes(screen):
    for i in range(GRID_SIZE):
            for j in range(GRID_SIZE):
                    pygame.draw.rect(screen,
                                     (25, 80, 62),
                                     pygame.Rect(rectXs[i] + 1,
                                                 rectYs[j] + 1,
                                                 43,
                                                 43))
