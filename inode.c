#include "inode.h"
#include "free.h"
#include "block.h"
#include "image.h"
#include "pack.h"
#include <stddef.h>

#define INODE_MAP_BLOCK 1
#define BLOCK_SIZE 4096
#define INODE_SIZE 64
#define INODE_FIRST_BLOCK 3
#define INODES_PER_BLOCK (BLOCK_SIZE / INODE_SIZE)

struct inode incore[MAX_SYS_OPEN_FILES];  // Array to hold in-core inodes

struct inode *find_incore_free(void) {
    for (int i = 0; i < MAX_SYS_OPEN_FILES; ++i) {  // Iterate over the in-core inode array
        if (incore[i].ref_count == 0) {  // Check if the inode is free
            return &incore[i];  // Return the free inode
        }
    }
    return NULL;  // Return NULL if no free inode is found
}

struct inode *find_incore(unsigned int inode_num) {
    for (int i = 0; i < MAX_SYS_OPEN_FILES; ++i) {  // Iterate over the in-core inode array
        if (incore[i].ref_count > 0 && incore[i].inode_num == inode_num) {  // Check if the inode is in use and matches the given inode number
            return &incore[i];  // Return the matching inode
        }
    }
    return NULL;  // Return NULL if no matching inode is found
}

void read_inode(struct inode *in, int inode_num) {
    int block_num = inode_num / INODES_PER_BLOCK + INODE_FIRST_BLOCK;  // Calculate the block number where the inode is stored
    int block_offset = (inode_num % INODES_PER_BLOCK) * INODE_SIZE;  // Calculate the offset within the block where the inode is located

    unsigned char block[BLOCK_SIZE];  // Buffer to hold the block data
    bread(block_num, block);  // Read the block containing the inode

    // Read the inode data from the block
    in->size = read_u32(block + block_offset);
    in->owner_id = read_u16(block + block_offset + 4);
    in->permissions = read_u8(block + block_offset + 6);
    in->flags = read_u8(block + block_offset + 7);
    in->link_count = read_u8(block + block_offset + 8);

    for (int i = 0; i < INODE_PTR_COUNT; i++) {  // Iterate over the inode block pointers
        in->block_ptr[i] = read_u16(block + block_offset + 9 + (2 * i));  // Read the block pointers
    }

    in->inode_num = inode_num;  // Set the inode number
}

void write_inode(struct inode *in) {
    int block_num = in->inode_num / INODES_PER_BLOCK + INODE_FIRST_BLOCK;  // Calculate the block number where the inode is stored
    int block_offset = (in->inode_num % INODES_PER_BLOCK) * INODE_SIZE;  // Calculate the offset within the block where the inode is located

    unsigned char block[BLOCK_SIZE];  // Buffer to hold the block data
    bread(block_num, block);  // Read the block containing the inode

    // Write the inode data to the block
    write_u32(block + block_offset, in->size);
    write_u16(block + block_offset + 4, in->owner_id);
    write_u8(block + block_offset + 6, in->permissions);
    write_u8(block + block_offset + 7, in->flags);
    write_u8(block + block_offset + 8, in->link_count);

    for (int i = 0; i < INODE_PTR_COUNT; i++) {  // Iterate over the inode block pointers
        write_u16(block + block_offset + 9 + (2 * i), in->block_ptr[i]);  // Write the block pointers
    }

    bwrite(block_num, block);  // Write the block back to the disk
}

struct inode *iget(int inode_num) {
    struct inode *inode = find_incore(inode_num);  // Check if the inode is already in the in-core inode array
    if (inode) {
        inode->ref_count++;  // Increase the reference count
        return inode;
    }

    inode = find_incore_free();  // Find a free inode in the in-core inode array
    if (!inode) {
        return NULL;  // Return NULL if no free inode is available
    }

    read_inode(inode, inode_num);  // Read the inode data from disk
    inode->ref_count = 1;  // Set the reference count to 1
    inode->inode_num = inode_num;  // Set the inode number
    return inode;
}

void iput(struct inode *in) {
    if (in->ref_count == 0) {  // Check if the inode is already free
        return;
    }

    in->ref_count--;  // Decrease the reference count
    if (in->ref_count == 0) {  // Check if the reference count reaches zero
        write_inode(in);  // Write the inode back to disk
    }
}

struct inode *ialloc(void) {
    unsigned char block[BLOCK_SIZE];  // Buffer to hold the block data
    bread(INODE_MAP_BLOCK, block);  // Read the inode map block
    int inode_num = find_free(block);  // Find a free inode number
    if (inode_num == -1) {
        return NULL;  // Return NULL if no free inode is available
    }

    set_free(block, inode_num, 1);  // Set the inode as used in the inode map
    bwrite(INODE_MAP_BLOCK, block);  // Write the updated inode map block to disk
    struct inode *in = iget(inode_num);  // Get the in-core inode
    if (in == NULL) {
        return NULL;  // Return NULL if the in-core inode is not available
    }

    // Initialize the inode fields
    in->size = 0;
    in->owner_id = 0;
    in->permissions = 0;
    in->flags = 0;
    for (int i = 0; i < INODE_PTR_COUNT; i++) {
        in->block_ptr[i] = 0;
    }

    in->inode_num = inode_num;  // Set the inode number
    write_inode(in);  // Write the inode back to disk
    return in;
}