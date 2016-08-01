#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

class Test{
};


class Device{
	public:
		void openDevice()
		{
			fd = open("/dev/input/event1",O_RDONLY);
		}
		void closeDevice()
		{
			::close(fd);
		}

		void close()
		{
		}


	private:
		int fd;

};

#endif
