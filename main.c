#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//stdin --> who --> sort --> uniq - c --> sort -nk1 --> file

int main()
{
// prepare pipe
    int pipe_fd[2]; // [1] - write, [0] - read
    pipe(pipe_fd); // connects 2 file descriptors into pipe
//make fork()
    pid_t pid = fork(); //makes a process copy
    if (pid == -1){ // error
        fprintf(stderr, "Errrror while fork");
    }
    else if (pid == 0) { // Child
/* Reorder old pipe, create new pipe, fork, and again*/
    //prepare fake stdin
        close(pipe_fd[1]); // close write end, as we're not going
                           // to write to parent
        close(0); //closes stdin to replace it by fake one (pipe_in)
        dup2(pipe_fd[0], 0); // stdin --> pipe[1] (pipe_wr)
// *** recursive part 1, minimum comments ***

    //prepare pipe
        int pipe_fd[2];
        pipe(pipe_fd);
    //make fork
        pid_t pid = fork();
        if (pid == -1){ // if error
                    fprintf(stderr, "Errrror while fork");
                }
        else if (pid == 0) { // child
        //prepare fake stdin
            close(pipe_fd[1]);
            close(0);
            dup2(pipe_fd[0], 0); // stdin --> pipe[1] (pipe_wr)
// *** recursive part 2 ***

            //prepare pipe
                int pipe_fd[2];
                pipe(pipe_fd);
            //make fork
                pid_t pid = fork();
                if (pid == -1){ // if error
                            fprintf(stderr, "Errrror while fork");
                        }
                else if (pid == 0) { // child
                //prepare fake stdin
                    close(pipe_fd[1]);
                    dup2(pipe_fd[0], 0); // stdin --> pipe[1] (pipe_wr)
                //prepare fake stdout (stdout --> file)
                    int fake_stdout = open("result.out", O_RDWR | O_CREAT,
                                       S_IRWXU | S_IRWXG | S_IRWXO);
                    close(1); //close stdout
                    dup2(fake_stdout, 1); // stdout --> file (pipe_wr) close(1);
//finally the 4-th fork()
                    pid_t pid = fork();
                    if (pid == -1){ // if error
                        fprintf(stderr, "Errrror while fork");
                    }
                    else if (pid == 0) { // child
                        close(fake_stdout);
                    }
                    else { //parent
                        execlp("sort", "-nk1", NULL);
                    }
                }
                else { //parent
                //prepare fake stdout
                    close(pipe_fd[0]); //close pipe_in
                    close(1); // close stdout, to replace it by pipe_out end
                    dup2(pipe_fd[1], 1); // stdout --> pipe[1] (pipe_wr)
                //run
                    execlp("uniq","-c",NULL);
                }
        //**************
        }
        else { //parent
        //prepare fake stdout
            close(pipe_fd[0]); //close pipe_in
            close(1); // close stdout, to replace it by pipe_out end
            dup2(pipe_fd[1], 1); // stdout --> pipe[1] (pipe_wr)
        //run
            execlp("sort","",NULL);
        }
    //***************************
    }
    else { // Parent
        /*Reorder pipe end, and run who*/
//prepare output
        close(pipe_fd[0]); //close pipe_in
        close(1); // close stdout, to replace it by pipe_out end
//        int fake_stdout = // this variable is no needed for this case
                dup2(pipe_fd[1], 1); // stdout --> pipe[1] (pipe_wr)
// run useful program
        execlp("who","",NULL); //

//        printf("Hello");
//        write(pipe_fd[1],"Hello\0",6);

    }
    return 0;
}
