SRC = app.c ChatDownloader.c ChatRender.c ClipDownloader.c VodDownloader.c utils.c
HEADERS = ChatDownloader.h ChatRender.h ClipDownloader.h  VodDownloader.h utils.h
LIBS = ../libs/cJSON.c ../libs/libui.a `pkg-config --cflags --libs gtk+-3.0` 
OPTIONS = -ldl -lm -pthread -lcurl -std=c99 -Wswitch-enum -Wpedantic -Wextra -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wno-unused-parameter -Wno-cast-qual -ggdb3  -fsanitize=address,undefined,leak 
BINARY = ../build/app

${BINARY}: ${SRC} ${HEADERS}
	cc ${SRC} ${LIBS} ${OPTIONS} -o ${BINARY} 
run:
	${BINARY}