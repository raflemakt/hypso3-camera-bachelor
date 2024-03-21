KVER=$(shell uname -r)
INCDIR=/usr/lib/modules/$(KVER)/build/include

all:
	gcc v4l2-capture.c lodepng.c -o v4l2-capture #-I$(INCDIR) 

listen:
	sudo strace -p $(shell pidof v4l2-capture)

img:
	viewnior test_image.png

kill:
	kill -9 $(shell pidof v4l2-capture)

clean:
	rm -f v4l2-capture test_image.png
