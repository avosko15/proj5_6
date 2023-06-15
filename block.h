#ifndef BLOCK_H
#define BLOCK_H

#define BLOCK_SIZE 4096
#define BLOCK_MAP_BLOCK 2

// Function to read a block from the image file
unsigned char *bread(int block_num, unsigned char *block);

// Function to write a block to the image file
void bwrite(int block_num, unsigned char *block);

// Function to allocate a free block
int alloc(void);

#endif

