#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>


int spawn(const char * program, char * arg_list[]) {

  pid_t child_pid = fork();

  if(child_pid < 0) {
    perror("Error while forking...");
    return 1;
  }

  else if(child_pid != 0) {
    return child_pid;
  }

  else {
    if(execvp (program, arg_list) == 0);
    perror("Exec failed");
    return 1;
  }
}

int main() {
/*
  remove("./../logs/log-old.txt");
  rename("./../logs/log.txt", "./../logs/log-old.txt");
  FILE *log = fopen("./../logs/log.txt", "w");
  fclose(log);
  */
  
  char * arg_list_command[] = { "/usr/bin/konsole", "-e", "./bin/command", NULL };
  char * arg_list_inspection[] = { "/usr/bin/konsole", "-e", "./bin/inspection", NULL };
  char * arg_list_motx[] = { "/usr/bin/konsole", "-e", "./bin/motX", NULL };
  char * arg_list_motz[] = { "/usr/bin/konsole", "-e", "./bin/motZ", NULL };
  char * arg_list_watchdog[] = { "/usr/bin/konsole", "-e", "./bin/watchdog", NULL };


  pid_t wd = spawn("/usr/bin/konsole", arg_list_watchdog);
  pid_t cmd = spawn("/usr/bin/konsole", arg_list_command);
  pid_t ic = spawn("/usr/bin/konsole", arg_list_inspection);
  pid_t mx = spawn("/usr/bin/konsole", arg_list_motx);
  pid_t mz = spawn("/usr/bin/konsole", arg_list_motz);
  
  char *tab[6];
  tab[0] = wd;
  tab[1] = cmd;
  tab[2] = ic;
  tab[3] = mx;
  tab[4] = mz;
  
  int i = 0;
  int fd = fopen("/tmp/PID.txt", O_RDWR);
  while (i < 6)
  {
    write(fd, tab[i], strlen(tab[i]));
    i++;
  }
  fclose(fd);

  int status;
  waitpid(cmd, &status, 0);
  waitpid(ic, &status, 0);
  waitpid(mx, &status, 0);
  waitpid(mz, &status, 0);
  waitpid(wd, &status, 0);
  
  printf ("Main program exiting with status %d\n", status);
}

