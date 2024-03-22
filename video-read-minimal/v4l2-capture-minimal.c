#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/videodev2.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))


const int WIDTH = 1280;
const int HEIGHT = 720;


struct buffer {
    void* start;
    size_t length;
};

struct buffer* buffers;
unsigned int n_buffers;


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

    buffers = calloc(req.count, sizeof(*buffers));
    
    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        struct v4l2_buffer buf;
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;
        (void) xioctl(fd, VIDIOC_QUERYBUF, &buf);
        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start = mmap(NULL,
                buf.length,
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                fd, buf.m.offset);
    }

    // Start capture
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    for (int i = 0; i < n_buffers; ++i) {
        struct v4l2_buffer buf;
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        (void) xioctl(fd, VIDIOC_QBUF, &buf);
    }
    (void) xioctl(fd, VIDIOC_STREAMON, &type);
    
    // (trenger kanskje noe fra "mainloop" her)
    unsigned int count = 4;
    struct v4l2_buffer buf;
    while (count-- > 0) {
        fd_set fds;
        struct timeval tv;
        int r;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        r = select(fd + 1, &fds, NULL, NULL, &tv);
        if (-1 == r) {
            if (EINTR == errno)
                continue;
        }

        // Read frame
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        (void) xioctl(fd, VIDIOC_DQBUF, &buf);
    }



    // Process image
    for (int i = 0; i < req.count; i++) {
        fwrite(buffers[i].start, buf.bytesused, 1, stdout);
        fflush(stderr);
        fflush(stdout);
    }

    (void) xioctl(fd, VIDIOC_QBUF, &buf);

    // Stop capturing
    (void) xioctl(fd, VIDIOC_STREAMOFF, &type);

    /*
    // Uninit device
    for (int i = 0; i < req.count; i++) {
        (void) munmap(buffers[i].start, buffers[i].length);
    }
    */

    // Close device
    (void) close(fd);

    //printf("\nIt's over mister Frodo\n");

    return 0;
}
