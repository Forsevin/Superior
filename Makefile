make:
	gcc superior.c $(shell pkg-config --libs --cflags libcurl) -o superior
