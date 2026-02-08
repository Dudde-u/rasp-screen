#include <fcntl.h> // For open()
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h> // For close()
const int OLED_ADRESS = 0x3C;
int send_commands_i2c(int fd, uint8_t *data, int len);
void draw_test_pattern(int fd);
void draw_test_pattern(int fd);
int send_data_i2c(int fd, uint8_t *data, int len);
void clear_display(int fd);
void draw_digit(int fd, int num);
int init(int fd);

int main() {
  int fd = open("/dev/i2c-1", O_RDWR);

  if (fd < 0) {
    // Failed to open
    perror("Failed to open I2C device");
    return -1;
  }
  init(fd);
  draw_digit(fd, 4);
  // draw_test_pattern(fd);
  // clear_display(fd);
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

int send_commands_i2c(int fd, uint8_t *commands, int len) {
  uint8_t buffer[len + 1];
  buffer[0] = 0x00; // Control byte for commands

  // Copy commands after control byte
  for (int i = 0; i < len; i++) {
    buffer[i + 1] = commands[i];
  }

  return write(fd, buffer, len + 1);
}

int send_data_i2c(int fd, uint8_t *data, int len) {
  uint8_t buffer[len + 1];
  buffer[0] = 0x40; // Control byte for data

  for (int i = 0; i < len; i++) {
    buffer[i + 1] = data[i];
  }

  return write(fd, buffer, len + 1);
}
const uint8_t font_digits[9][5] = {
    // 1
    {0x00, 0x42, 0x7F, 0x40, 0x00},
    // 2
    {0x62, 0x51, 0x49, 0x49, 0x46},
    // 3
    {0x21, 0x41, 0x49, 0x4D, 0x33},
    // 4
    {0x18, 0x14, 0x12, 0x7F, 0x10},
    // 5
    {0x27, 0x45, 0x45, 0x45, 0x39},
    // 6
    {0x3C, 0x4A, 0x49, 0x49, 0x31},
    // 7
    {0x03, 0x01, 0x71, 0x09, 0x07},
    // 8
    {0x36, 0x49, 0x49, 0x49, 0x36},
    // 9
    {0x26, 0x49, 0x49, 0x29, 0x1E}};

void clear_display(int fd) {
  uint8_t empty[128];
  memset(empty, 0x00, 128);

  for (int page = 0; page < 8; page++) {
    send_data_i2c(fd, empty, 128);
  }
}

void draw_test_pattern(int fd) {
  uint8_t line[128];
  for (int i = 0; i < 128; i++) {
    line[i] = 0x01; // Light up top pixel
  }

  // Write to all 8 pages
  for (int page = 0; page < 8; page++) {
    send_data_i2c(fd, line, 128);
  }
}

void draw_digit(int fd, int number) {
  if (number < 1 || number > 9) {
    printf("Error: number must be between 1 and 9\n");
    return;
  }

  // Clear display first
  clear_display(fd);

  // Calculate center position (128 cols / 2 - 5 cols / 2 = ~61)
  int start_col = 61;

  // Create a buffer for one page (128 bytes)
  uint8_t line[128];
  memset(line, 0x00, 128);

  // Copy the 5 columns of the digit into the buffer at center position
  for (int i = 0; i < 5; i++) {
    line[start_col + i] = font_digits[number - 1][i];
  }

  // We'll draw on page 3 (roughly middle of display vertically)
  // First, send empty pages 0-2
  uint8_t empty[128];
  memset(empty, 0x00, 128);

  for (int page = 0; page < 3; page++) {
    send_data_i2c(fd, empty, 128);
  }

  // Send the digit on page 3
  send_data_i2c(fd, line, 128);

  // Send empty pages 4-7
  for (int page = 4; page < 8; page++) {
    send_data_i2c(fd, empty, 128);
  }
}
