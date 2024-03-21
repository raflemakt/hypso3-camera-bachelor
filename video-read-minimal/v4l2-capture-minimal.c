#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))


const int WIDTH = 1280;
const int HEIGHT = 720;

char* image_buffer;


static int xioctl(int fh, int request, void *arg) {
    int r;

    do {
	    r = ioctl(fh, request, arg);
    } while (-1 == r && EINTR == errno);

    return r;
}


int main(int argc, char** argv) {
    // Open device
    int fd = open("/dev/video0", O_RDWR | O_NONBLOCK, 0);

    // Query device for its capabilities
    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_format fmt;

    (void) xioctl(fd, VIDIOC_QUERYCAP, &cap);
    CLEAR(cropcap);
    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    (void) xioctl(fd, VIDIOC_S_CROP, &cropcap);
    crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    crop.c = cropcap.defrect;
    (void) xioctl(fd, VIDIOC_S_CROP, &crop);

    CLEAR(fmt);
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
    (void) xioctl(fd, VIDIOC_S_FMT, &fmt);

    // Init device. Here we choose the MMAP way
    struct v4l2_requestbuffers req;
    CLEAR(req);
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    (void) xioctl(fd, VIDIOC_REQBUFS, &req);
    
    struct v4l2_buffer buf;
    CLEAR(buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;
    (void) xioctl(fd, VIDIOC_QUERYBUF, &buf);

    image_buffer = mmap(NULL,
            buf.length,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            fd, buf.m.offset);

    // Start capture
    CLEAR(buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;
    (void) xioctl(fd, VIDIOC_QBUF, &buf);
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    (void) xioctl(fd, VIDIOC_STREAMON, &type);

    // (trenger kanskje noe fra "mainloop" her)

    // Read frame
    CLEAR(buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    (void) xioctl(fd, VIDIOC_DQBUF, &buf);

    // Process image
    int size = WIDTH*HEIGHT;
    fwrite(image_buffer, size, 1, stdout);
    fflush(stderr);
    fflush(stdout);

    (void) xioctl(fd, VIDIOC_QBUF, &buf);

    // Stop capturing
    (void) xioctl(fd, VIDIOC_STREAMOFF, &type);

    // Uninit device
    //(void) munmap(buf, size);
    //free(buf);

    // Close device
    (void) close(fd);


    return 0;
}
