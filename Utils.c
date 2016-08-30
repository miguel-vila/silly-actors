#include <unistd.h>
#include <stdbool.h>

bool read_bytes(int socket_fd, unsigned int bytes_to_read, void *buffer) {
  unsigned int bytes_read = 0;
  int result;
  while(bytes_read < bytes_to_read) {
    result = read(socket_fd, buffer + bytes_read, bytes_to_read - bytes_read);
    if(result < 1) {
      return false;
    }
    bytes_read += result;
  }
  return true;
}
