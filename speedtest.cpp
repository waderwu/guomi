#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define SIZE 1073741824

int main(int argc, char *argv[])
{
  char * filename = argv[1];
  char * buf = malloc(SIZE);
  int fd = open(filename,O_RDONLY);
  if (!fd)
  {
    printf("%s\n", "open file error");
    exit(-1);
  }


  int haveread;
  while ((haveread=read(fd,buf,SIZE)) > 0)
  {

  }


  return 0;
}
