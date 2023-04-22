#include <stdio.h>
#include <string.h>
#include <ctype.h>
void codecA(char *argv[], int flag)
{
    char* data =argv[2];
    for (int i = 0; i < sizeof(argv); i++)
    {
        if (isupper(data[i]))
            data[i] -= 32;

        else if (islower(data[i]))
            data[i] +=32;
        else
        {
            continue;
        }
    }
}


