#include "logger.hpp"

void print_msg(const char* msg, const char* file, int line) {
    fprintf(stdout, "ERROR in file: %s | %d --> %s\n", file, line, msg);
}