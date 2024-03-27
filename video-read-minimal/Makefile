SOURCES=v4l2-capture-minimal.c
IMGFILE=test_image.jpeg
BINARY=capture

IMGVIEWER=viewnior


all:
	gcc $(SOURCES) -o $(BINARY)

img:
	./$(BINARY) -o > $(IMGFILE)
	$(IMGVIEWER) $(IMGFILE)

debug:
	gcc $(SOURCES) -g -o $(BINARY)
	gdb $(BINARY)

clean:
	rm -f $(BINARY) $(IMGFILE)
