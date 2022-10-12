#
# Makefile
#CC = gcc  
CC = arm-linux-gcc
LVGL_DIR_NAME ?= lvgl
LVGL_DIR = ${shell pwd}
CFLAGS = -O3 -g0   -I$(LVGL_DIR)/ -Wall -std=gnu99
LDFLAGS ?= -lm -lfreetype -L. -llvgl
BIN = ../Desktop/app/file

MAINSRC = ./main.c  


OBJEXT ?= .o
MAINOBJ = $(MAINSRC:.c=$(OBJEXT))


all:default

%.o: %.c
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "CC $<"
    
%.o: %.c
	@$(CC)  $(CFLAGS) -c $< -o $@
	@echo "CC $<"
    
default: $(MAINOBJ) 
	$(CC)  $(MAINOBJ)   -o  $(BIN)  $(LDFLAGS)

clean: 
	@rm  $(BIN) $(MAINOBJ) 





