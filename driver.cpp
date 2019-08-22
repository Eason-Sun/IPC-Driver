#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <fcntl.h>
#include <vector>
#include <string>

using namespace std;

int main(int argc, char** argv) {
    int opt, maxNumOfSt = 10, maxNumOfSeg = 5, maxWaitTime = 5, maxNumOfCord = 20;
    vector<pid_t> proc;
    while ((opt = getopt(argc, argv, "s:n:l:c:")) != EOF) {
        switch (opt) {
            case 's':
                //To do; Add error handling for minimum input values
                if (optarg)maxNumOfSt = atoi(optarg);
                break;
            case 'n':
                if (optarg)maxNumOfSeg = atoi(optarg);
                break;
            case 'l':
                if (optarg)maxWaitTime = atoi(optarg);
                break;
            case 'c':
                if (optarg)maxNumOfCord = atoi(optarg);
                break;
        }
    }
    int pipe1[2];
    int pipe2[2];
    pipe(pipe1);
    pipe(pipe2);
    pid_t p1 = fork();
    if (p1 == 0) {
        dup2(pipe1[1], STDOUT_FILENO);
        close(pipe1[0]);
        close(pipe2[0]);
        close(pipe2[1]);
        close(pipe1[1]);
        execl("./rgen", "rgen", "-s", to_string(maxNumOfSt).c_str(), "-n", to_string(maxNumOfSeg).c_str(), "-l", to_string(maxWaitTime).c_str(), "-c", to_string(maxNumOfCord).c_str(), NULL);
    } else {
        proc.push_back(p1);
        pid_t p2 = fork();
        if (p2 == 0) {
            dup2(pipe2[1], STDOUT_FILENO);
            close(pipe2[0]);
            close(pipe2[1]);
            dup2(pipe1[0], STDIN_FILENO);
            close(pipe1[0]);
            close(pipe1[1]);
            execl("/usr/bin/python3", "python3", "streetGraphGen.py", NULL);
        } else {
            pid_t p3 = fork();
            if (p3 == 0) {
                dup2(pipe2[0], STDIN_FILENO);
                close(pipe1[0]);
                close(pipe1[1]);
                close(pipe2[0]);
                close(pipe2[1]);
                execl("./bfs", "bfs", NULL);
            } else {
                pid_t p4 = fork();
                if (p4 == 0) {
                    dup2(pipe2[1], STDOUT_FILENO);
                    close(pipe1[0]);
                    close(pipe1[1]);
                    close(pipe2[0]);
                    close(pipe2[1]);
                    while (true) {
                        string line;
                        getline(cin, line);
                        if (cin.eof()) {
                            exit(0);
                        }
                        cout << line << endl;
                    }

                } else {
                    proc.push_back(p4);
                    int status = 1;
                    while (1) {
                        for (auto it = proc.begin(); it != proc.end(); ++it) {
                            waitpid((*it), &status, WNOHANG);
                            if (status == 0) {
                                if ((*it) == p1) {
                                    kill(p2, SIGTERM);
                                    kill(p3, SIGTERM);
                                    kill(p4, SIGTERM);
                                    return 0;
                                } else {
                                    kill(p2, SIGTERM);
                                    kill(p3, SIGTERM);
                                    kill(p1, SIGTERM);
                                    return 0;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
