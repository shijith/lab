#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>

#define PAGE_SIZE 4096

main ()
{
	int fd;
	char *virt = NULL;
	
	fd = open("/dev/chr_dev", O_RDWR);
	if(fd < 0) {
		perror("open");
		return -1;
	}
	virt = mmap(NULL, PAGE_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(virt == MAP_FAILED) {
		perror("mmap");
		return -1;
	}
	printf("address: %p\n", virt);
	printf("buffer before changing: %s\n", virt);
	memcpy(virt, "new letters :)", 14);
	printf("buffer after  changing: %s\n", virt);

	close(fd);	
	return 0;
}
