#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "ctest.h"
#include "image.h"
#include "block.h"
#include "free.h"
#include "inode.h"
#include "mkfs.h"
#define BLOCK_SIZE 4096
#define FS_SIZE 1024

#ifdef CTEST_ENABLE

// Test the mkfs function
void test_mkfs(void) {
    printf("Test 1: Testing mkfs function\n");
    char* pname = "image_name.img";
    int fd = image_open(pname, 1);
    CTEST_ASSERT(fd != -1, "Image was not created or opened properly by mkfs");
    mkfs();
    off_t size = lseek(fd, 0, SEEK_END);
    CTEST_ASSERT(size != FS_SIZE * BLOCK_SIZE, "Image size does not match FS_SIZE * BLOCK_SIZE");
    close(fd);
}

// Test the image_open function
void test_image_open(void) {
    printf("Test 2: Testing image_open function\n");
    char* pname = "test_pic.jpeg";
    CTEST_ASSERT(image_open(pname, 1) != -1, "Image failed to open");
}

// Test the alloc function
void test_alloc(void) {
    printf("Test 3: Testing alloc function\n");
    int block_num = alloc();
    CTEST_ASSERT(block_num != -1, "Block allocation failed");
}

// Test the bread and bwrite functions
void test_bread_bwrite(void) {
    printf("Test 4: Testing bread and bwrite functions\n");
    unsigned char original_block[BLOCK_SIZE];
    for(int i = 0; i < BLOCK_SIZE; i++) {
        original_block[i] = i % 256;
    }
    bwrite(0, original_block);
    unsigned char read_block[BLOCK_SIZE];
    bread(0, read_block);
    CTEST_ASSERT(memcmp(original_block, read_block, BLOCK_SIZE) == 0, "Bread and bwrite do not match");
}

// Test the set_free and find_free functions
void test_set_and_find_free(void) {
    printf("Test 5: Testing set_free and find_free functions\n");
    unsigned char block[64];
    memset(block, 0xFF, sizeof(block));
    int num = 8;  
    set_free(block, num, 0);
    int free_num = find_free(block);
    CTEST_ASSERT(free_num == num, "Incorrect free block found after clearing bit");
}

// Test the find_incore_free function
void test_find_incore_free(void) {
    printf("Test 6: Testing find_incore_free function\n");
    struct inode *in = find_incore_free();
    CTEST_ASSERT(in != NULL, "No free incore found");
}

// Test the find_incore function
void test_find_incore(void) {
    printf("Test 7: Testing find_incore function\n");
    struct inode *in = iget(0);
    struct inode *found = find_incore(0);
    CTEST_ASSERT(found == in, "Incore inode not found");
    iput(in);
}

// Test the read_inode and write_inode functions
void test_read_write_inode(void) {
    printf("Test 8: Testing read_inode and write_inode functions\n");
    struct inode in;
    read_inode(&in, 0);
    in.size++;
    write_inode(&in);
    struct inode in2;
    read_inode(&in2, 0);
    CTEST_ASSERT(in.size == in2.size, "Read/write inode mismatch");
}

// Test the iget and iput functions
void test_iget_iput(void) {
    printf("Test 9: Testing iget and iput functions\n");
    struct inode *in = iget(0);
    unsigned int ref_count = in->ref_count;
    iput(in);
    CTEST_ASSERT(in->ref_count == ref_count - 1, "iput did not decrease ref_count");
}

// Test the ialloc function
void test_ialloc(void) {
    printf("Test 10: Testing ialloc function\n");
    struct inode *in = ialloc();
    CTEST_ASSERT(in != NULL, "ialloc did not allocate inode");
    CTEST_ASSERT(in->ref_count == 1, "ialloc did not set ref_count to 1");
    iput(in);
}

// Test the image_close function
void test_image_close(void){
    printf("Test 11: Testing image_close function\n");
    CTEST_ASSERT(image_close() != -1, "Failed to close the image");
}

int main(void) {
    // Run the tests
    test_mkfs();
    test_image_open();
    test_alloc();
    test_bread_bwrite();
    test_set_and_find_free();
    test_find_incore_free();
    test_find_incore();
    test_read_write_inode();
    test_iget_iput();
    test_ialloc();
    test_image_close();

    return 0;
}

#endif