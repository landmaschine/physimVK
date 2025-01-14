#pragma once
#include <stdio.h>

void print_msg(const char* msg, const char* file, int line);

#define ERRLOG(msg) print_msg(msg, __FILE__, __LINE__);