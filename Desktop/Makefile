#
# Makefile
#-L/home/gec/freetype-2.12.1/tmp/lib -I/home/gec/freetype-2.12.1/tmp/include/freetype2
CC = arm-linux-gcc
LVGL_DIR_NAME ?= lvgl
LVGL_DIR ?= ${shell pwd}
CFLAGS ?= -O3 -g0   -I$(LVGL_DIR)/ -I./include/ -Wall  -std=gnu99
LDFLAGS ?= -lm -lfreetype -L. -llvgl -lpthread
BIN =Desktop


#Collect the files to compile
MAINSRC = ./main.c ${shell find ./src/*.c} #./src/app_data.c ./src/desktop_ui.c ./src/show_time.c
 #
OBJEXT ?= .o

MAINOBJ = $(MAINSRC:.c=$(OBJEXT))

## MAINOBJ -> OBJFILES


all: default

%.o: %.c
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "CC $<"

default: $(MAINOBJ)
	$(CC) -o $(BIN) $(MAINOBJ) $(LDFLAGS)

clean: 
	rm -f $(BIN) $(MAINOBJ)

