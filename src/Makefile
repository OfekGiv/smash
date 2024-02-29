# Makefile for the smash program
CC = gcc
CFLAGS = -std=c99 -g -Wall -Werror -pedantic-errors -DNDEBUG
CCLINK = $(CC)
OBJS = smash.o commands.o signals.o linkedList.o
RM = rm -f
SOURCE_DIR := /media/sf_sharedFolder
DEST_DIR := /home/os/workspace/smash/src
SRC_FILES := commands.c commands.h linkedList.c linkedList.h signals.c signals.h smash.c

# Creating the  executable
smash: copy_files $(OBJS)
	$(CCLINK) -o smash $(OBJS)

# copy files
copy_files:
	@echo "Copying files from $(SOURCE_DIR) to $(DEST_DIR)"
	@cp $(addprefix $(SOURCE_DIR)/,$(SRC_FILES)) $(DEST_DIR)

# Creating the object files
commands.o: commands.c commands.h
linkedList.o: linkedList.c linkedList.h
smash.o: smash.c commands.h
signals.o: signals.c signals.h

# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.*

