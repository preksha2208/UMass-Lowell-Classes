#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define READ_END 0
#define WRITE_END 1

int main(int argc, char *argv[])
{

    // need to pull in Notes.txt somehow
    // create first pipe fd1
    int fd1[2];
    if (pipe(fd1) == -1)
    {
        fprintf(stderr, "Pipe Failed\n");
        return 1;
    }
    // fork first child
    pid_t pid;
    pid = fork(); // create first child for sort

    if (pid < 0)
    {
        fprintf(stderr, "Fork Failed\n");
        return 1;
    }
    if (pid == 0)
    {                                         // first child process, run sort
        dup2(fd1[WRITE_END], 1);              // tie write end of pipe fd1 to standard output (file descriptor 1)
        close(fd1[READ_END]);                 // close read end of pipe fd1
        execlp("usr/bin/sort", "sort", NULL); // start the sort command using execlp
        printf("Should not be here after execlp to sort\n"); // should not get here
    }
    //create second pipe fd2
    int fd2[2];
    if (pipe(fd2) == -1)
    {
        fprintf(stderr, "Pipe Failed\n");
        return 1;
    }
    // fork second child
    pid = fork(); // create second child for uniq
    if (pid < 0)
    {
        fprintf(stderr, "Fork Failed\n");
        return 1;
    }
    if (pid == 0)
    {                                         // second child process, run uniq
        dup2(fd1[READ_END], 0);               // tie read end of fd1 to standard input (file descriptor 0)
        dup2(fd2[WRITE_END], 1);              // tie write end of fd2 to standard output (file descriptor 1)
        close(fd1[WRITE_END]);                // close write end of pipe fd1
        close(fd2[READ_END]);                 // close read end of pipe fd2
        execlp("usr/bin/uniq", "uniq", NULL); // start the uniq command using execlp
                printf("Should not be here after execlp to uniq\n"); // should not get here
// should not get here
    }
    // fork third child
    pid = fork(); // create third child for wc -l
    if (pid < 0)
    {
        fprintf(stderr, "Fork Failed\n");
        return 1;
    }
    if (pid == 0)
    {                                     // third child process, run wc -l
        dup2(fd2[READ_END], 0);           // tie read end of fd2 to standard input (file descriptor 0)
        close(fd2[WRITE_END]);            // close write end of pipe fd2
        close(fd1[READ_END]);             // close read end of pipe fd1
        close(fd1[WRITE_END]);            // close write end of pipe fd1
        execlp("usr/bin/wc", "wc", "-l"); // start the uniq command using execlp// start the wc -l command using execlp
        // should not get here
    }
    // parent process code
    // close both ends of pipes fd1 and fd2
    close(fd1[READ_END]);
    close(fd2[READ_END]);
    close(fd1[WRITE_END]);
    close(fd2[WRITE_END]);
    // wait for third process to end.
    wait(pid);
    return 0;
}