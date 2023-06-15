#ifndef IMAGE_H
#define IMAGE_H

extern int image_fd; // External declaration of the image file descriptor

int image_open(char *filename, int truncate);
int image_close(void);

#endif
