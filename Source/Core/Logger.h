#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdio.h>

extern FILE *gpfile;

bool initializeLogger(void);
void uninitializeLogger(void);

#endif