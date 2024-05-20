CC = gcc
CFLAGS = -Wall -Wextra -g

# Directories
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

SERVER_SRC = $(OBJDIR)/server.c $(SRCDIR)/rw_http.c $(SRCDIR)/utils.c
SERVER_HDR = $(INCDIR)/rw_http.h $(INCDIR)/utils.h
SERVER_OBJS = $(OBJDIR)/server.o $(OBJDIR)/rw_http.o $(OBJDIR)/utils.o

# Targets
SERVER_EXEC = $(BINDIR)/server

all: $(SERVER_EXEC)
server: $(SERVER_EXEC)

# Targets rules
$(SERVER_EXEC): $(SERVER_OBJS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(SERVER_HDR)
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c -o $@ $<


.PHONY: clean

clean:
	rm -rf $(OBJDIR) $(BINDIR)
