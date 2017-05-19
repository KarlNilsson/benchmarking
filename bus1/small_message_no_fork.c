#define _GNU_SOURCE
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <linux/bus1.h>
#include <inttypes.h>
#include <unistd.h>

#define MAP_SIZE (16UL * 1024UL)
#define NBR_MSGS (8)
#define VEC_SIZE (4)

int main(int argc, char **argv){
    
    const uint8_t *map;
    uint64_t handle;
    int fd1, fd2;
    char data[VEC_SIZE];
    struct bus1_cmd_handle_transfer cmd_transfer;
    struct bus1_cmd_send cmd_send;
    struct bus1_cmd_recv cmd_recv;
    struct iovec vec;
    int i, r;
    
    fd1 = open("/dev/bus1", O_RDWR | O_CLOEXEC | O_NONBLOCK | O_NOCTTY);
    fd2 = open("/dev/bus1", O_RDWR | O_CLOEXEC | O_NONBLOCK | O_NOCTTY);
    map = mmap(NULL, MAP_SIZE, PROT_READ, MAP_SHARED, fd2, 0);

    cmd_transfer = (struct bus1_cmd_handle_transfer) {
        .flags      = 0,
        .src_handle = 0x100,
        .dst_fd     = fd1,
        .dst_handle = BUS1_HANDLE_INVALID,
    };
    ioctl(fd2, BUS1_CMD_HANDLE_TRANSFER, &cmd_transfer);
    handle = cmd_transfer.dst_handle;
    memset(data, '0', VEC_SIZE);

    i = 0;
    for(; i < NBR_MSGS; ++i){
        vec = (struct iovec) { data, VEC_SIZE };
        
        cmd_send = (struct bus1_cmd_send){
            .flags              = 0,
            .ptr_destinations   = (unsigned long) &handle,
            .ptr_errors         = 0,
            .n_destinations     = 1,
            .ptr_vecs           = (unsigned long) &vec,
            .n_vecs             = 1,
            .ptr_handles        = 0,
            .n_handles          = 0,
            .ptr_fds            = 0,
            .n_fds              = 0,
        };

        r = ioctl(fd1, BUS1_CMD_SEND, &cmd_send);
        assert(r >= 0);
    }

    i = 0;
    for(; i < NBR_MSGS; ++i){
        cmd_recv = (struct bus1_cmd_recv){
            .flags = 0,
            .max_offset = MAP_SIZE,
        };
        r = ioctl(fd2, BUS1_CMD_RECV, &cmd_recv); 
        assert(r >= 0);

        
        r = strncmp(data, (char*) (map + cmd_recv.msg.offset), VEC_SIZE);
        assert(r == 0);

        r = ioctl(fd2, BUS1_CMD_SLICE_RELEASE, &cmd_recv.msg.offset);
        assert(r >= 0);
    }

    munmap((void *)map, MAP_SIZE);
    close(fd1);
    close(fd2);

    return 0;
}
