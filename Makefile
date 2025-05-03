# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
LDFLAGS = -lGL -lGLU -lglut -lm

# Directories
SRCDIR = src
OBJDIR = obj
BINDIR = bin
INCLUDEDIR = include

# Source files
MAIN_SOURCES = $(SRCDIR)/main.c $(SRCDIR)/gui.c 
PLAYER_SOURCE = $(SRCDIR)/player.c



MAIN_OBJECTS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(MAIN_SOURCES))
PLAYER_OBJECT = $(OBJDIR)/player.o



EXECUTABLE = $(BINDIR)/tug_of_war
PLAYER_EXEC = $(BINDIR)/player

# Targets
all: $(EXECUTABLE) $(PLAYER_EXEC)

$(EXECUTABLE): $(MAIN_OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(PLAYER_EXEC): $(PLAYER_OBJECT)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -rf $(OBJDIR)/*.o $(BINDIR)/*

.PHONY: all clean run
