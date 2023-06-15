#ifndef INODE_H
#define INODE_H

#define INODE_MAP_BLOCK 1
#define MAX_SYS_OPEN_FILES 64
#define INODE_PTR_COUNT 16
struct inode* ialloc(void);
struct inode {
    unsigned int size;
    unsigned short owner_id;
    unsigned char permissions;
    unsigned char flags;
    unsigned char link_count;
    unsigned short block_ptr[INODE_PTR_COUNT];
    unsigned int ref_count;
    unsigned int inode_num;
};

struct inode *find_incore_free(void);
struct inode *find_incore(unsigned int inode_num);
void read_inode(struct inode *in, int inode_num);
void write_inode(struct inode *in);
struct inode *iget(int inode_num);
void iput(struct inode *in);

#endif
