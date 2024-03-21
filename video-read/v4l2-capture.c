/* Bachelorgruppe 36, 2024. Gunnar Myhre */


#include <stdint.h>

#include <stdio.h>  // printf
#include <fcntl.h>  // open
#include <errno.h>  // errno
#include <sys/ioctl.h>  // ioctl
#include <sys/mman.h>   // mmap

#include <linux/videodev2.h>

// To demonstrate that the capture was successful, we
// save the image as a png.
#include "lodepng.h"


const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;

uint8_t* image_buffer;


static int xioctl(int fd, int request, void* arg) {
    int r;
    do {
        r = ioctl(fd, request, arg);
        printf(" - ioctl request %d returned %d\n", request, r);
    }
    while (-1 == r && EINTR == errno);
    return r;
}


int main(int argc, char** argv) {
    printf("V4L2 Capture Demo\n");

    /* Open capture device */
    printf("Opening capture device...\n");
    int fd;
    fd = open("/dev/video0", O_RDWR);
    if (fd == -1) {
        perror("could not find capture device /dev/video0");
        return 1;
    }

    /* Query capture device to see if capture is available */
    printf("Querying capture device...\n");
    struct v4l2_capability caps = {0};
    if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &caps)) {
        perror("querying capabilities");
        return 1;
    }

    /* Set image format */
    printf("Setting image format...\n");
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    printf("width: %d\n", fmt.fmt.pix.width);
    printf("height: %d\n", fmt.fmt.pix.height);

    if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt)) {
        perror("setting pixel format");
        return 1;
    }

    /* Request buffers */
    printf("Requesting buffers...\n");
    struct v4l2_requestbuffers req = {0};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
        perror("requesting buffer");
        return 1;
    }

    /* Query buffer */
    printf("Query buffer...\n");
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;
    if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf)) {
        perror("querying buffer");
        return 1;
    }
    image_buffer = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);

    /* Capture image(s ?) */
    printf("Capture image...\n");
    if (-1 == xioctl(fd, VIDIOC_STREAMON, &buf.type)) {
        perror("start capture");
        return 1;
    }

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    struct timeval tv = {0};
    tv.tv_sec = 2;
    int r = select(fd+1, &fds, NULL, NULL, &tv);

    printf("Waiting for frame...\n");
    if (-1 == r) {
        perror("waiting for frame");
        return 1;
    }

    /*
    printf("Retrieving frame...\n");
    if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
        perror("retrieving frame");
        return 1;
    }
    */

    /* Stop capture */
    printf("Stopping capture...\n");
    if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &buf)) {
        perror("stopping capture");
        return 1;
    }

    /* Store data into image file */
    unsigned int error = lodepng_encode32_file("test_image.png", image_buffer, WIDTH, HEIGHT);
    if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

    return 0;
}
