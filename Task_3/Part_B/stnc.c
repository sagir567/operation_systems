#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>



#define SIZE 1024*1024*100
#define PATH "sample.txt"

//--------------------------------------------

//--------------------------------------------
bool isServer;
bool isClient;
bool isQuite;
bool isTest;


void generateFile (){
    FILE * file;
    file = fopen("sample.txt","w");
     if(fseek(file,SIZE , SEEK_SET )) perror("ERROR:");
    fputs("this is the end of the file",file);
     fclose (file);
    
}

int main(int argc, char *argv[]){
// this CASE WILL check the input 
// TODO: change to while loop with regeting input from user.
// if (argc < 4 || argc >6){
//     printf("Usage:\n"
//     "client side:\n"
//      "\tstnc -c IP PORT -p <type> <param>\n"
//     "\t-p will indicate to perform the test\n"
//     "\t<type> will be the communication types: so it can be ipv4,ipv6,mmap,pipe,uds\n"
//     "\t<type> will be the communication types: so it can be ipv4,ipv6,mmap,pipe,uds\n"
//     "\tavilable methods:\n"
//     "\t\tIPv4 tcp\n"
//     "\t\tIPv4 udp\n"
//     "\t\tIPv6 tcp\n"
//     "\t\tIPv6 udp\n"
//     "\t\tUDS dgram\n"
//     "\t\tUDS stream\n"
//     "\t\tmmap <file>\n"
//     "\t\tpipe <file>\n"
//     "server side:\n"
//     "\tstnc -s port -p (p for performance test) -q (q for quiet)\n"
//     "\t-p flag will let you know that we are going to test the performance.\n"
//     "\t-q flag will enable quiet mode, in which only testing results will be printed."
//     "\t\tpipe <file>\n"
//     "\t\tpipe <file>\n");
//     exit(1);
// }


    isClient=(*argv[1]=='c')?true:false;
    isServer=(*argv[1]=='s')?true:false;


    if(isClient){

        
    }
     if(isServer){
       
        
     }


    
}