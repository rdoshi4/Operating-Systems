#ifndef _UNISTD_H
#define _UNISTD_H

#include <sys/defs.h>
extern char **envvar;
/*int open(const char *pathname, int flags);
int close(int fd);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
int unlink(const char *pathname);

int chdir(const char *path);
char *getcwd(char *buf, size_t size);

pid_t fork();
int execvpe(const char *file, char *const argv[], char *const envp[]);
pid_t wait(int *status);
int waitpid(int pid, int *status);

unsigned int sleep(unsigned int seconds);

pid_t getpid(void);
pid_t getppid(void);

// OPTIONAL: implement for ``on-disk r/w file system (+10 pts)''
off_t lseek(int fd, off_t offset, int whence);
int mkdir(const char *pathname, mode_t mode);

// OPTIONAL: implement for ``signals and pipes (+10 pts)''
int pipe(int pipefd[2]);*/
extern void exit(int status);
//extern int chdir(const char *path);
//extern char* getcwd(char * buf, int  size);
//extern int execvp( char *a, char *b[]);
//extern int execvpe( char *command, char *args[],char *env[]);
//int execvpe( char *file_name, char *arg);
//extern int read(int fd,char *buf,int size);
extern int fork();
extern void exit(int status);
extern void chdir(char *argv[]);
extern char* getcwd(char * buf);
extern int execve( char *file_name, char *arg);
extern void readdir(char *argv[]);
extern void ps();
//extern char *getenv(char *varName);
//extern int setenv(char *envname,char *envval, int overwrite);
#endif
