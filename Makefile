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
CFLAGS=-std=c99 -Wpedantic -Wextra -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wno-unused-parameter -Wno-cast-qual -O2
DEPFLAGS=-MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
LDFLAGS=libs/libui.a `pkg-config --cflags --libs gtk+-3.0` -ldl -lm -pthread -lcurl
PREFIX=/usr
.PHONY: all clean run install uninstall

all: $(BIN) 

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

install:
	install -Dm 0755 build/bin/twitch-downloader-gui $(DESTDIR)$(PREFIX)/bin/twitch-downloader-gui
	for _size in 128 16 22 24 256 32 48 512 64 96 ; do \
		install -Dm 644 data/icons/$${_size}x$${_size}/twitch-downloader-gui.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/$${_size}x$${_size}/apps/twitch-downloader-gui.png; \
	done
	install -Dm 644 data/applications/twitch-downloader-gui.desktop $(DESTDIR)$(PREFIX)/share/applications/twitch-downloader-gui.desktop

uninstall:
	rp $(DESTDIR)$(PREFIX)/bin/twitch-downloader-gui
	for _size in 128 16 22 24 256 32 48 512 64 96 ; do \
		rm $(DESTDIR)$(PREFIX)/share/icons/hicolor/$${_size}x$${_size}/apps/twitch-downloader-gui.png; \
	done
	rm $(DESTDIR)$(PREFIX)/share/applications/twitch-downloader-gui.desktop

$(DEPS):

include $(wildcard $(DEPS))
