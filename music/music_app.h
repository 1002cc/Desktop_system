#ifndef __MUSIC_APP_H
#define __MUSIC_APP_H

#include "lvgl/examples/lv_examples.h"
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <semaphore.h>

void music_player();
void paly_music(int num);
#endif
