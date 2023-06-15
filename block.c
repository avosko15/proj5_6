#include <unistd.h>
#include "block.h"
#include "image.h"
#include "free.h"
#include <stdio.h>

unsigned char *bread(int block_num, unsigned char *block) {
    // Seek to the specified block in the image file
    off_t ret = lseek(image_fd, block_num * BLOCK_SIZE, SEEK_SET);
    if (ret == (off_t) -1) {
        perror("lseek failed");
        return NULL;
    }

    // Read the block from the image file
    ssize_t bytes_read = read(image_fd, block, BLOCK_SIZE);
    if (bytes_read < 0) {
        perror("bread read failed");
        return NULL;
    }
    
    return block;
}

void bwrite(int block_num, unsigned char *block) {
    // Seek to the specified block in the image file
    off_t ret = lseek(image_fd, block_num * BLOCK_SIZE, SEEK_SET);
    if (ret == (off_t) -1) {
        perror("lseek failed");
        return;
    }
    
    // Write the block to the image file
    ssize_t bytes_written = write(image_fd, block, BLOCK_SIZE);
    if (bytes_written < 0) {
        perror("bwrite write failed");
    }
}

int alloc(void) {
    unsigned char block[BLOCK_SIZE];
    // Read the block map block from the image file
    bread(BLOCK_MAP_BLOCK, block);
    
    // Find a free block
    int block_num = find_free(block);
    if (block_num == -1) {
        return -1;
    }

    // Mark the allocated block as used
    set_free(block, block_num, 1);
    
    // Write the updated block map block to the image file
    bwrite(BLOCK_MAP_BLOCK, block);
    
    return block_num;
}