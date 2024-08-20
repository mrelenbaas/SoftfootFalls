import time

class Timer:
    def __init__(self,
                 limit,
                 limit_function,
                 update_function=None,
                 debug=False):
        self.__limit = limit
        self.__limit_function = limit_function
        self.__update_function = update_function
        self.__debug = debug
        self.__previous_time = time.time()
        self.__time = time.time()
        self.__delta = self.__time - self.__previous_time
        self.__time_since_start = self.__delta
        self.__is_percent_complete = False
        self.reset()

    def update(self):
        self.__previous_time = self.__time
        self.__time = time.time()
        self.__delta = self.__time - self.__previous_time
        self.__time_since_start += self.__delta
        if self.__debug:
            print(self.__time_since_start)
        if self.__time_since_start > self.__limit:
            self.__time_since_start -= self.__time_since_start
            self.__limit_function()
            self.__is_percent_complete = True
            if self.__debug:
                print('---')
        if self.__update_function != None:
            self.__update_function()

    def percent(self):
        percent = 0.0
        if self.__is_percent_complete == False:
            percent = 1.0 - (self.__time_since_start / self.__limit)
        if self.__debug:
            print('percent: {}'.format(percent))
        # if percent <= 0.0:
        #     print('---')
        #     self.__is_percent_complete = True
        return percent

    def reset(self, reset_percent=False):
        self.__time_since_start -= self.__time_since_start
        # self.__time_since_start = 0.0
        if reset_percent:
            self.__is_percent_complete = False
            if self.__debug:
                print('RESET')
