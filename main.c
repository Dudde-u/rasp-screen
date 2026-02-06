#include <fcntl.h>  // For open()
#include <unistd.h> // For close()
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdint.h>
const int OLED_ADRESS = 0x3C;
int send_commands_i2c(int fd, uint8_t* data, int len);
int send_data_i2c(int fd, uint8_t* data, int len);
int init(int fd);

int main(){
int fd = open("/dev/i2c-1", O_RDWR);

if (fd < 0) {
    // Failed to open
    perror("Failed to open I2C device");
    return -1;
}
init(fd);
close(fd);
return 0;

}

int init(int fd)
{

if (ioctl(fd, I2C_SLAVE, OLED_ADRESS) < 0) {
    perror("Failed to set I2C address");
    close(fd);
    return -1;
}
uint8_t init_data[] = {0x00, 0xAF};

send_commands_i2c(fd, init_data, 2);
}


int send_data_i2c(int fd, uint8_t* data, int len)
{
        write(fd, data, len);
}
int send_commands_i2c(int fd, uint8_t* data, int len)
{
        write(fd, data, len);
}
