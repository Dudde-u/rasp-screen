#include <fcntl.h> // For open()
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h> // For close()
const int OLED_ADRESS = 0x3C;
int send_commands_i2c(int fd, uint8_t *data, int len);
void draw_test_pattern(int fd); 
void draw_test_pattern(int fd);
int send_data_i2c(int fd, uint8_t *data, int len);
int init(int fd);

int main() {
  int fd = open("/dev/i2c-1", O_RDWR);

  if (fd < 0) {
    // Failed to open
    perror("Failed to open I2C device");
    return -1;
  }
  init(fd);
  draw_test_pattern(fd);
  close(fd);



  return 0;

}

int init(int fd) {

  if (ioctl(fd, I2C_SLAVE, OLED_ADRESS) < 0) {
    perror("Failed to set I2C address");
    close(fd);
    return -1;
  }
    uint8_t init_cmds[] = {
      0xAE,       // Display OFF
      0xD5, 0x80, // Set display clock
      0xA8, 0x3F, // Set multiplex ratio
      0xD3, 0x00, // Set display offset
      0x40,       // Set start line
      0x8D, 0x14, // Enable charge pump
      0x20, 0x00, // Memory addressing mode
      0xA1,       // Segment remap
      0xC8,       // COM scan direction
      0xDA, 0x12, // COM pins config
      0x81, 0xCF, // Set contrast
      0xD9, 0xF1, // Set precharge
      0xDB, 0x40, // Set VCOMH
      0xA4,       // Display from RAM
      0xA6,       // Normal display (not inverted)
      0xAF        // Display ON
  };

  send_commands_i2c(fd, init_cmds, sizeof(init_cmds));
}

int send_commands_i2c(int fd, uint8_t* commands, int len) {
    uint8_t buffer[len + 1];
    buffer[0] = 0x00;  // Control byte for commands
    
    // Copy commands after control byte
    for(int i = 0; i < len; i++) {
        buffer[i + 1] = commands[i];
    }
    
    return write(fd, buffer, len + 1);
}

int send_data_i2c(int fd, uint8_t* data, int len) {
    uint8_t buffer[len + 1];
    buffer[0] = 0x40;  // Control byte for data
    
    for(int i = 0; i < len; i++) {
        buffer[i + 1] = data[i];
    }
    
    return write(fd, buffer, len + 1);
}


void draw_test_pattern(int fd) {
  // Fill entire display with 0x01 (top row lit on each page)
  uint8_t line[128];
  for (int i = 0; i < 128; i++) {
    line[i] = 0x01; // Light up top pixel
  }

  // Write to all 8 pages
  for (int page = 0; page < 8; page++) {
    send_data_i2c(fd, line, 128);
  }
}
void draw_test_pattern(int fd) {
    // Fill entire display with 0x01 (top row lit on each page)
    uint8_t line[128];
    for(int i = 0; i < 128; i++) {
        line[i] = 0x01;  // Light up top pixel
    }
    
    // Write to all 8 pages
    for(int page = 0; page < 8; page++) {
        send_data_i2c(fd, line, 128);
    }
}
