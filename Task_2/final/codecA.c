#include <stdio.h>
#include <string.h>
#include <ctype.h>

void codecA(char data[], int size) {



    for (int i = 0; i < size ;i++) {
        if (isupper(data[i])) data[i] += 32;

        else if (islower(data[i])) data[i] -= 32;

        else {
            continue;
        }
    }

}


