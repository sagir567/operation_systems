#include <stdio.h>
#include <string.h>
#include<ctype.h>
#include <stdbool.h>

void codecB(bool flag,char data[],int size){
    int increment = (flag==true)?3:-3;
    printf("------------");
    for (int i=0;i<size;i++){
        printf("%d\n",i);
        data[i]+=increment;
    }

}