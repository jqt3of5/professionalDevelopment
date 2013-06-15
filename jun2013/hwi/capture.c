#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "common.h"


#define MAX_SAMPLES 10000

static int fd;
static char *fileName = "/dev/i2c-1";
static int address = 0x68;

int writeReg(int fd, char reg, char value);
int readReg(int fd, char reg, char * value, int size);
void swap(union sensor_raw * data);

int main()
{
  if ((fd = open(fileName, O_RDWR)) < 0) {
      printf("Failed to open the i2c bus error?%d\n ", errno);
        return 0;
    }

    if (ioctl(fd,I2C_SLAVE,address) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
	return 0;
    }

    //==== Configuration =================================

    //Clear sleep bits
    writeReg(fd, 0x6B, 00);
    writeReg(fd, 0x6C, 00);

    //other stuff?
  

    union sensor_raw bufx[MAX_SAMPLES] = {0};
    char reg = 0x3B;

    int i = 0;
    while(1)
      {
	if (write(fd, &reg, 1) != 1)
    	  {
	    fprintf(stderr, "Failed to write\n");
    	    return 0;
    	  }
    	if (read(fd, &bufx[i], sizeof(union sensor_raw)) != sizeof(union sensor_raw))
    	  {
	    fprintf(stderr, "Failed to read\n");
    	    return 0;
    	  }
    	  swap(&bufx[i]);

	  printf("%d %d %d %d %d %d\n", 
		 bufx[i].reg.accx, bufx[i].reg.accy, bufx[i].reg.accz, 
		 bufx[i].reg.gyrox, bufx[i].reg.gyroy, bufx[i].reg.gyroz);

	  i += 1;
	  i = i%MAX_SAMPLES;
      }
}

void swap(union sensor_raw * data)
{
  int i = 0;
  char temp = 0;
  for (i = 0; i < 14; i += 2)
    {
      temp = data->buf[i];
      data->buf[i] = data->buf[i+1];
      data->buf[i+1] = temp;
    }
}

int writeReg(int fd, char reg, char value)
{
  char buf[2] = {reg, value};
    if (write(fd, buf, 2) != 2)
      {
	return 0;
      }
    return 1;

}

int readReg(int fd, char reg, char * value, int size)
{
    if (write(fd, &reg, 1) != 1)
      {
	return 0;
      }

    if (read(fd, value, size) != size)
      {
	return 0;
      }
    return 1;
}
