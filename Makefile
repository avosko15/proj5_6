simfs_test: simfs_test.o simfs.a mkfs.o
	gcc -o $@ $^

simfs_test.o: simfs_test.c
	gcc -Wall -Wextra -c simfs_test.c -o $@ -DCTEST_ENABLE

simfs.a: block.o free.o image.o inode.o pack.o
	ar rcs $@ $^ 

block.o: block.c block.h
	gcc -Wall -Wextra -c $< -o $@

free.o: free.c free.h
	gcc -Wall -Wextra -c $< -o $@

image.o: image.c image.h
	gcc -Wall -Wextra -c $< -o $@

inode.o: inode.c inode.h
	gcc -Wall -Wextra -c $< -o $@

pack.o: pack.c pack.h
	gcc -Wall -Wextra -c $< -o $@

mkfs.o: mkfs.c block.h image.h
	gcc -Wall -Wextra -c $< -o $@

clean:
	rm -f *.o simfs.a simfs_test

.PHONY: test

test: simfs_test
	./simfs_test