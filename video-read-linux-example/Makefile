all:
	gcc capture_example_linux.c -o example

img:
	./example -o > test_image.jpeg
	viewnior test_image.jpeg

clean:
	rm -f example test_image.jpeg
