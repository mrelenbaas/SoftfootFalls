import os
import sys
import pygame


SCALE = 0.5
KERNING = 0.6
PNG_SIZE = 128
LETTER_SIZE = PNG_SIZE * KERNING * SCALE
class Letter:
    def __init__(self, filename):
        self.__filename = filename
        self.__image = pygame.image.load(self.__filename)
        self.__rect = self.__image.get_rect()
        self.__original_rect = self.__rect
        self.__image = pygame.transform.scale(self.__image,
                                              (self.__rect.width * SCALE,
                                               self.__rect.height * SCALE))
        self.__rect = self.__image.get_rect()
        self.__original_width = self.__rect.width
        self.__original_height = self.__rect.height

    def draw(self, screen):
        screen.blit(self.__image, self.__rect)

    def move(self, x, y):
        self.__rect.center = x, y

    def height(self):
        return self.__rect.height

    def width(self):
        return self.__rect.width

    def alpha_percent(self, percent):
        self.__image.set_alpha(256 * percent)


def load_alphabet(filenames):
    alphabet = []
    images = []
    for file in filenames:
        if 'AlphabetUppercase' in file:
            # file = file.split('AlphabetUppercase')[1]
            alphabet.append(file)
    alphabet = sorted(alphabet)
    return alphabet


def draw_title(screen, x, y, letters):
    i = 0
    for letter in letters:
        letter.move(x + (i * LETTER_SIZE), y)
        letter.draw(screen)
        i += 1


def draw_title_fade(screen, x, y, letters, alpha_percent):
    i = 0
    for letter in letters:
        letter.alpha_percent(alpha_percent)
        letter.move(x + (i * LETTER_SIZE), y)
        letter.draw(screen)
        i += 1


def generate_word(word):
    filenames = next(os.walk(os.curdir), (None, None, []))[2]
    alphabet = load_alphabet(filenames)
    letters = []
    i = 0
    for w in word:
        # print(int(ord(w)) - 65)
        i = ord(w) - ord('A')
        # print(i)
        letters.append(Letter(alphabet[i]))
    return letters
