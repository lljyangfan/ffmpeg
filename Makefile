#gcc test.c -g -I../include/ -L../lib/ -lavfilter  -lavformat -lavcodec  -lavutil -lm -lpthread  -lz -lswresample -lswscale
CC = gcc

LIBS = -L/home/yangfan/Devel/FFmpeg/.ffmpeg/lib -lavfilter  -lavformat -lavcodec  -lavutil -lm -lpthread  -lz -lswresample -lswscale
INCS = -I/home/yangfan/Devel/FFmpeg/.ffmpeg/include

CFLAGS = -g -O0

all: 2yuv

test: test.c
	${CC} $< ${LIBS} ${CFLAGS} ${INCS} -o $@

2yuv: 2yuv.c
	${CC} $< ${LIBS} ${CFLAGS} ${INCS} -o $@

.PHONY: clean
clean:
	rm -rf *.o *.out test 2yuv
