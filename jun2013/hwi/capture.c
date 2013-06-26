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

char myError[200];

bool init(FILE &* fd)
{
    if ((fd = open(fileName, O_RDWR)) < 0) {
        sprintf(myError,"Failed to open the i2c bus error?%d\n ", errno);
        return false;
    }

    if (ioctl(fd,I2C_SLAVE,address) < 0) {
        sprintf(myError,"Failed to acquire bus access and/or talk to slave.\n");
	return false;
    }

    return true;
}

void readMeasurement(FILE * i2cFD, union sensor_raw * data)
{
    char reg = 0x3B;
    if (write(i2cFD, &reg, 1) != 1)
    	  {
	    fprintf(stderr, "Failed to write\n");
    	    return 0;
    	  }
    	if (read(i2cFD, data, sizeof(union sensor_raw)) != sizeof(union sensor_raw))
    	  {
	    fprintf(stderr, "Failed to read\n");
    	    return 0;
    	  }
    	  swap(&bufx[i]);
}

int main()
{
    FILE *i2cFD;
    //==== Configuration =================================
    if (!init(i2cFD))
    {
    	printf(myError);
    	return 1;
    }
    //Clear sleep bits
    writeReg(i2cFD, 0x6B, 00);
    writeReg(i2cFD, 0x6C, 00);
    //other stuff?


    union sensor_raw bufx[MAX_SAMPLES] = {0};
   

    int i = 0;
    while(1)
      {
	
          readMeasurement(i2cFD, &bufx[i])

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
