CC=gcc
CFLAGS=-Wall `pkg-config fuse3 --cflags --libs`

all:
	$(CC) main.c fuse_ops.c logic.c file_ops.c $(CFLAGS) -o mini_unionfs

clean:
	rm -f mini_unionfs
