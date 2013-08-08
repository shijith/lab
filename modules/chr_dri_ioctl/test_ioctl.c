/* Program to clear device buffer starting from the offset 
*/
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <error.h>
#include "cdev_ioctl.h"

main(int argc, char **argv)
{
	int fd;
	unsigned long offset;
	if(argc < 2) 
		error(-1, 0, "missing argument");
	sscanf(argv[1], "%lu", &offset);
	fd = open("/dev/chr_dev", O_RDWR);
	if(ioctl(fd, CDEV_CLEAR, offset) == 0)
		return 0;
	perror("error");
	return -1;

}
