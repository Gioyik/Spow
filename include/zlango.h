#ifndef ZL_H
#define ZL_H

#define ZLANGO_VERSION "v0.7.0"

#include "types.h"
#include "colors.h"

/* system functions */
void run_scripts(zlenv* e, int argc, char** argv);
void setup_zl(void);
void teardown_zl(void);
char* get_zl_version(void);

#endif
