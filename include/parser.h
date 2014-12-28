#ifndef ZL_PARSER_H
#define ZL_PARSER_H

#include <stdbool.h>

#include "types.h"

void setup_parser(void);
void teardown_parser(void);

bool zlval_parse(const char* input, zlval** v, char** err);
bool zlval_parse_file(const char* file, zlval** v, char** err);

#endif
