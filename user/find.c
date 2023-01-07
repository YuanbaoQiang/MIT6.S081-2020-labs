#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// cut the space
char*
trim(char *path)
{
    char *p1, *p2;
    p1 = path;
    p2 = path + strlen(path) - 1;
    while (p1 <= p2 && *p1 == ' ') {
        p1++;
    }
    while (p1 <= p2 && *p2 == ' ') {
        p2--;
    }
    *(++p2) = '\0';
    return p1;
}

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

int
matchfile(char *path, char *text)
{
    static char buf[DIRSIZ+1];
    char *p;
    for(p=path+strlen(path); p >= path && *p != '/'; p--);
    p++;
    memmove(buf, p, strlen(p));
    return strcmp(trim(buf), text);
}

void
find(char *path, char *text)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    if(matchfile(fmtname(path), text) == 0)
      printf("%s\n", path);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      if(strcmp(de.name, ".\0") == 0 || strcmp(de.name, "..\0") == 0) {
        continue;
      }
      find(buf, text);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(2, "argc count error");
    }
    find(argv[1], argv[2]);
    exit(0);
}