/* hello_signal.c */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char * argv[])
{
  while (1){
    printf("Process is active!\n");
    sleep(4);
  }
  return 0;
}