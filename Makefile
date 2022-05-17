CC=gcc
SRCDIR=src
OBJDIR=build/obj
LIBDIR=libs
LIBOBJDIR=build/libobj
DEPDIR=build/dep
BINDIR=build/bin
SRCS=$(wildcard $(SRCDIR)/*.c)
OBJS=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS)) 
LIBS=$(wildcard $(LIBDIR)/*.c)
LIBSOBJS=$(patsubst $(LIBDIR)/%.c, $(LIBOBJDIR)/%.o, $(LIBS)) 
DEPS=$(patsubst $(SRCDIR)/%.c, $(DEPDIR)/%.d, $(SRCS))
BIN=$(BINDIR)/twitch-downloader-gui
CFLAGS=-std=c99 -Wswitch-enum -Wpedantic -Wextra -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wno-unused-parameter -Wno-cast-qual -O2 
DEPFLAGS=-MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
LDFLAGS=libs/libui.a `pkg-config --cflags --libs gtk+-3.0` -ldl -lm -pthread -lcurl
.PHONY: all clean

all: clean $(BIN) 

debug: CFLAGS += -ggdb3 -fsanitize=address,undefined,leak -Og
debug: LDFLAGS = libs/libuiDebug.a `pkg-config --cflags --libs gtk+-3.0` -ldl -lm -pthread -lcurl
debug: $(BIN)

$(BIN): $(OBJS) $(LIBSOBJS) | $(BINDIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR) $(DEPDIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

$(LIBOBJDIR)/%.o: $(LIBDIR)/%.c | $(LIBOBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR) $(LIBOBJDIR) $(BINDIR) $(DEPDIR):
	@mkdir -p $@

clean:
	rm -rf build

run:
	$(BIN)

$(DEPS):

include $(wildcard $(DEPS))
