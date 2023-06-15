#include <unistd.h>
#include <stdio.h>
#include "block.h"
#include "image.h"

#define FS_SIZE 1024
#define BLOCK_SIZE 4096

int mkfs(void) {
    char* pname = "test_pic.jpeg";
    int fd = image_open(pname, 1);  //Open the image file in read-write mode (and truncate if it exists)
    off_t initial_size = lseek(fd, 0, SEEK_END);  //Get the initial size

    // Write blocks filled with zeros to the image file
    for (int i = 0; i < FS_SIZE; i++) {
        char zeros[BLOCK_SIZE] = {0};
        write(fd, zeros, BLOCK_SIZE);
    }

    // Allocate additional blocks
    for (int i = 0; i < 7; i++) {
        alloc();
    }

    off_t final_size = lseek(fd, 0, SEEK_END);  // Get the final size

    // Check if the image size matches the expected size
    if (final_size - initial_size != FS_SIZE * BLOCK_SIZE) {
        printf("Image size does not match FS_SIZE * BLOCK_SIZE\n");
    }

    close(fd);

    return (final_size - initial_size);
}
