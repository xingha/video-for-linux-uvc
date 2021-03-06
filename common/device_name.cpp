extern "C"{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <assert.h>
	#include <fcntl.h>              /* low-level i/o */
	#include <unistd.h>
	#include <errno.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <sys/time.h>
	#include <sys/mman.h>
	#include <sys/ioctl.h>
	#include <asm/types.h>
	#include <linux/videodev2.h>
}

#include <string>

int device_open(char *dev_name)
{
	struct stat st;
	int fd = -1;

	if (-1 == stat(dev_name, &st)) {
		fprintf(stderr, "Cannot identify '%s': %d, %s\n", dev_name, errno, strerror(errno));
		return fd;
	}
	if (!S_ISCHR(st.st_mode)) {
		fprintf(stderr, "%s is no device\n", dev_name);
		return fd;
	}
	fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);
	if (-1 == fd) {
		fprintf(stderr, "Cannot open '%s': %d, %s\n", dev_name, errno, strerror(errno));
		return fd;
	}

	printf("%s %d : Open %s successfully. fd = %d\n", __func__, __LINE__, dev_name, fd);
	return fd;
}

int device_close(int fd)
{
	if (-1 == close(fd)) {
		// printf("\tdevice close failed.\n");
		return -1;
	} else {
		printf("%s %d : devices close successfully\n", __func__, __LINE__);
	}

	return 0;
}

int device_query(char *dev_name, int fd, struct v4l2_capability *cap)
{
	/* query v4l2-devices's capability */
	if (-1 == ioctl(fd, VIDIOC_QUERYCAP, cap)) {
		if (EINVAL == errno) {
			printf("%s is no V4L2 device\n", dev_name);
			exit(EXIT_FAILURE);
		} else {
			printf("\tvideo ioctl_querycap failed.\n");
			exit(EXIT_FAILURE);
		}
	} else {
		printf("\n\tdriver name : %s\n\tcard name : %s\n\tbus info : %s\n\tdriver version : %u.%u.%u\n\n",
			cap->driver, cap->card, cap->bus_info,(cap->version >> 16) & 0XFF,
			(cap->version >> 8) & 0XFF, cap->version & 0XFF);
	}
	return 0;
}

std::string dev_list[] = {
	"/dev/video0",	"/dev/video1",	"/dev/video2",	"/dev/video3",	"/dev/video4", // 00 - 04
	"/dev/video5",	"/dev/video6",	"/dev/video7",	"/dev/video8",	"/dev/video9", // 05 - 09
	"/dev/video10",	"/dev/video11",	"/dev/video12",	"/dev/video13",	"/dev/video14", // 10 - 14
};

char* get_device(const char *name)
{
	int i = 0;
	struct v4l2_capability cap;
	memset(&cap, 0, sizeof(cap));
	std::string videodev = "/dev/video0";
	int fd = device_open((char*)videodev.c_str());
	device_query((char*)videodev.c_str(),fd,&cap);
	device_close(fd);
	/*
	for( i = 0; i < sizeof(dev_list)/sizeof(*dev_list); ++i) {
		int fd = device_open((char*) dev_list[i].c_str());

		// no video device, break 
		if (fd == -1) {
			printf("no vide device, quit");
			return 0;
		}
		device_query((char*) dev_list[i].c_str(), fd, &cap);
		device_close(fd);
		char need_find[]={};
		char body[]={};
		sprintf(body,"%s",cap.driver);
		sprintf(need_find,"%s",name);
		// printf("[1]body:%s\n",body);
		// printf("[2]need_find:%s\n",need_find);
		// printf("[3]Found:%d\n", std::string(body).find(std::string(need_find)));
		if (std::string(body).find(std::string(need_find))!=std::string::npos){
			printf("get device %s\n", dev_list[i].c_str());
			return (char*) dev_list[i].c_str();
		}
	}
	*/
	return (char*)videodev.data();
}

