IMGFILE=test_image.jpeg
BIN=capture
SRC=capture.c

all:
	gcc $(SRC) -o $(BIN)

img:
	./$(BIN) -o > $(IMGFILE)
	viewnior $(IMGFILE)

clean:
	rm -f $(BIN) $(IMGFILE)
