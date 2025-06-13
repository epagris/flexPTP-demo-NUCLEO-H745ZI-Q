#include "standard_output.h"

#include <stdarg.h>
#include <stdint.h>
#include <string.h>

#include <embfmt/embformat.h>
//#include <flatUSB/class/cdc.h>

#include <ICC/icc.h>

void icc_write(char * str, uint16_t len) {
    ICCQueue * q = icc_get_outbound_pipe();
    for (uint16_t i = 0; i < len; i++) {
        if (!iccq_push(q, str + i)) {
            break;
        }
    }
    icc_notify();
}

#define STDIO_OUTPUT_LINEBUF_LEN (2048)
static char lineBuf[STDIO_OUTPUT_LINEBUF_LEN + 1];

static uint32_t insert_carriage_return(char *str, uint32_t maxLen) {
    uint32_t len = strlen(str);
    for (uint32_t i = 0; (i < len) && (len <= maxLen); i++) {
        // if a \n is found and no \r precedes it or it is the first character os the string
        if ((str[i] == '\n') && ((i == 0) || (str[i - 1] != '\r'))) {
            len++;                               // increase length
            for (uint32_t k = len; k > i; k--) { // copy each character one position right 
                str[k] = str[k - 1];
            }
            str[i] = '\r'; // insert '\r'
            i++; // skip the current \r\n block
        }
    }
    str[len] = '\0';
    return len;
}

static const char core_prefix[] = "[" ANSI_COLOR_YELLOW "M4" ANSI_COLOR_RESET "] ";
static char last_char = '\n';

void MSG(const char *format, ...) {
    va_list vaArgP;
    va_start(vaArgP, format);
    uint32_t lineLen = vembfmt(lineBuf, STDIO_OUTPUT_LINEBUF_LEN, format, vaArgP);
    va_end(vaArgP);
    lineLen = insert_carriage_return(lineBuf, STDIO_OUTPUT_LINEBUF_LEN); // insert carriage returns
    if (lineLen > 0) {
        if (last_char == '\n') {
            icc_write(core_prefix, sizeof(core_prefix));
        }
        icc_write(lineBuf, lineLen);
        last_char = lineBuf[lineLen - 1];
    }
}

void MSGchar(int c) {
    icc_write(&c, 1);
}

void MSGraw(const char *str) {
    icc_write(str, strlen(str));
}
