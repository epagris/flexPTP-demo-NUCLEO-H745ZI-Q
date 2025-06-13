#ifndef SERIALIO_SERIAL_IO
#define SERIALIO_SERIAL_IO
#ifndef STANDARD_OUTPUT_SERIAL_OUTPUT
#define STANDARD_OUTPUT_SERIAL_OUTPUT

#include <stdint.h>

void serial_io_init();
void serial_io_write(const char * str, uint32_t len);

#endif /* STANDARD_OUTPUT_SERIAL_OUTPUT */


#endif /* SERIALIO_SERIAL_IO */
