/*
 *  V4L2 video capture example
 *
 *  This program can be used and distributed without restrictions.
 *
 *      This program is provided with the V4L2 API
 * see https://linuxtv.org/docs.php for more information
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <linux/videodev2.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

struct buffer {
        void   *start;
        size_t  length;
};

static char            *dev_name;
static int              fd = -1;
struct buffer          *buffers;
static unsigned int     n_buffers;
static int              frame_count = 1;

static void errno_exit(const char *s)
{
        fprintf(stderr, "%s error %d, %s\\n", s, errno, strerror(errno));
        exit(EXIT_FAILURE);
}

static int xioctl(int fh, int request, void *arg)
{
        int r;

        do {
                r = ioctl(fh, request, arg);
        } while (-1 == r && EINTR == errno);

        return r;
}


static int read_frame(void)
{
        struct v4l2_buffer buf;
        unsigned int i;
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        (void) xioctl(fd, VIDIOC_DQBUF, &buf);
        assert(buf.index < n_buffers);
        fwrite(buffers[buf.index].start, buf.bytesused, 1, stdout);
        fflush(stderr);
        fprintf(stderr, ".");
        fflush(stdout);
        (void) xioctl(fd, VIDIOC_QBUF, &buf);
        return 1;
}



int main(int argc, char **argv)
{
        dev_name = "/dev/video0";

        /* OPEN DEVICE */
        struct stat st;
        fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

        /* INIT DEVICE */
        {
        struct v4l2_capability cap;
        struct v4l2_cropcap cropcap;
        struct v4l2_crop crop;
        struct v4l2_format fmt;
        unsigned int min;

        (void) xioctl(fd, VIDIOC_QUERYCAP, &cap);

        /* Select video input, video standard and tune here. */

        CLEAR(cropcap);

        cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        (void) xioctl(fd, VIDIOC_CROPCAP, &cropcap);
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c = cropcap.defrect; /* reset to default */

        CLEAR(fmt);

        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width       = 1280;
        fmt.fmt.pix.height      = 720;
        fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
        (void) xioctl(fd, VIDIOC_S_FMT, &fmt);


        /* INIT MMAP */
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

                buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory      = V4L2_MEMORY_MMAP;
                buf.index       = n_buffers;

                (void) xioctl(fd, VIDIOC_QUERYBUF, &buf);

                buffers[n_buffers].length = buf.length;
                buffers[n_buffers].start =
                        mmap(NULL /* start anywhere */,
                              buf.length,
                              PROT_READ | PROT_WRITE /* required */,
                              MAP_SHARED /* recommended */,
                              fd, buf.m.offset);

                if (MAP_FAILED == buffers[n_buffers].start)
                        errno_exit("mmap");
        }
        }



        /* START CAPTURING */
        enum v4l2_buf_type type;

        for (unsigned int i = 0; i < n_buffers; ++i) {
                struct v4l2_buffer buf;

                CLEAR(buf);
                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;
                buf.index = i;

                (void) xioctl(fd, VIDIOC_QBUF, &buf);
        }
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        (void) xioctl(fd, VIDIOC_STREAMON, &type);



        /* MAIN LOOP */
        {
        unsigned int count;

        count = frame_count;

        while (count-- > 0) {
                for (;;) {
                        fd_set fds;
                        struct timeval tv;
                        int r;

                        FD_ZERO(&fds);
                        FD_SET(fd, &fds);

                        /* Timeout. */
                        tv.tv_sec = 2;
                        tv.tv_usec = 0;

                        r = select(fd + 1, &fds, NULL, NULL, &tv);

                        if (-1 == r) {
                                if (EINTR == errno)
                                        continue;
                                errno_exit("select");
                        }

                        if (0 == r) {
                                fprintf(stderr, "select timeout\\n");
                                exit(EXIT_FAILURE);
                        }

                        if (read_frame())
                                break;
                        /* EAGAIN - continue select loop. */
                }
        }
        }



        /* STOP CAPTURING */
        (void) xioctl(fd, VIDIOC_STREAMOFF, &type);


        /* UNINIT DEVICE */
        for (unsigned int i = 0; i < n_buffers; ++i)
                if (-1 == munmap(buffers[i].start, buffers[i].length))
                        errno_exit("munmap");
        free(buffers);


        /* CLOSE DEVICE */
        if (-1 == close(fd))
                errno_exit("close");
        fd = -1;


        fprintf(stderr, "\\n");
        return 0;
}
