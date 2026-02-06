#include <fcntl.h>  // For open()
#include <unistd.h> // For close()
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

int fd = open("/dev/i2c-1", O_RDWR);

if (fd < 0) {
    // Failed to open
    perror("Failed to open I2C device");
    return -1;
}

int addr = 0x3C;  // Your OLED's address

if (ioctl(fd, I2C_SLAVE, addr) < 0) {
    perror("Failed to set I2C address");
    close(fd);
    return -1;
}
