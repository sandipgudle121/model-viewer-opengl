#include "Logger.h"

FILE *gpfile = NULL;

bool initializeLogger(void)
{
    if (fopen_s(&gpfile, "Log.txt", "w") != 0)
    {
        return false;
    }
    fprintf(gpfile, "Program Started Successfully\n");
    return true;
}

void uninitializeLogger(void)
{
    if (gpfile)
    {
        fprintf(gpfile, "Program Ended Successfully\n");
        fclose(gpfile);
        gpfile = NULL;
    }
}