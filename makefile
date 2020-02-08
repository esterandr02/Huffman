HEADER = header.h
COMPRESS = compress.h compress.c
DECOMPRESS = decompress.h decompress.c
MAIN = main.c

compile_huffman:
	gcc $(HEADER) $(MAIN) $(COMPRESS) $(DECOMPRESS) -o out