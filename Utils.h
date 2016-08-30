#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdbool.h>

bool read_bytes(int socket_fd, unsigned int bytes_to_read, void *buffer);
  
#endif // __UTILS_H__
