#include "./../include/command_utilities.h"
#include <fcntl.h> // O_WRONLY
#include <sys/stat.h> // PIPE
#include <sys/types.h> // PIPE
#include <unistd.h> // write and close
#include <stdlib.h> // EXIT_SUCCESS
#include <asm-generic/errno-base.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/resource.h>

static void signal_handler(int sig) {
    if (sig == SIGALRM){
        printf("\n\nWatchdog alarm detected, motors are been reset!!\n\nCommand: ");
        fflush(stdout);
    }
}

int main() {
    // Variables
    int fd_x;
    int fd_z;
    char command[80];

    /*
     * 1. Open PID.txt file that contain the pid for each process
     * 2. Write the pid of this one
     * 3. Read the pids of other processes
     */
    // 1.

    char *filename = "/tmp/PID.txt";
    FILE *file = fopen (filename, "w");
    int pid = getpid();

    // 2.
    fprintf(file, "%d\n", pid);
    fclose(file);

    sleep(1);

    // 3.
    FILE *file_r = fopen (filename, "r");
    int *str[5];
    fscanf(file_r, "%d", &str[0]);
    
    // Take pids
    pid_t ic = str[1]; // inspection console
    pid_t mx = str[2]; // motor_x
    pid_t mz = str[3]; // motor_z
    pid_t wd = str[4]; // watchdog

    // PIPE 
    char * myfifo_x = "/tmp/myfifo_x"; 
    mkfifo(myfifo_x, 0666);
    char * myfifo_z = "/tmp/myfifo_z"; 
    mkfifo(myfifo_z, 0666);
    char * myfifo_xi = "/tmp/myfifo_xi"; 
    mkfifo(myfifo_xi, 0666);
    char * myfifo_zi = "/tmp/myfifo_zi"; 
    mkfifo(myfifo_zi, 0666); 
    char * myfifo_IX = "/tmp/myfifo_IX"; 
    mkfifo(myfifo_IX, 0666);
    char * myfifo_IZ = "/tmp/myfifo_IZ"; 
    mkfifo(myfifo_zi, 0666); 
    

    printf("Commands:\n1 - X axis increase\n2 - X axis decrease\n3 - X axis stop\n4 - Z axis increase\n5 - Z axis decrease\n6 - Z axis stop\nPress 'q' to quit\n"); fflush(stdout);


    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;

    // Initialize User Interface 
    init_console_ui();
    
    // Infinite loop
    while(TRUE)
	{	

        signal(SIGALRM, signal_handler);
        // Get mouse/resize commands in non-blocking mode...
        int cmd = getch();

        // If user resizes screen, re-draw UI
        if(cmd == KEY_RESIZE) {
            if(first_resize) {
                first_resize = FALSE;
            }
            else {
                reset_console_ui();
            }
        }
        // Else if mouse has been pressed
        else if(cmd == KEY_MOUSE) {

            kill(wd, SIGUSR1);

            // Check which button has been pressed...
            if(getmouse(&event) == OK) {

                // Vx++ button pressed
                if(check_button_pressed(vx_decr_btn, &event)) {
                    mvprintw(LINES - 1, 1, "Horizontal Speed Decreased");
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                    kill(mx, SIGUSR1);
                    command[0] = '1';
                    fd_x = open(myfifo_x, O_RDWR); 
                    write(fd_x, command, strlen(command)+1);
                }

                // Vx-- button pressed
                else if(check_button_pressed(vx_incr_btn, &event)) {
                    mvprintw(LINES - 1, 1, "Horizontal Speed Increased");
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                    kill(mx, SIGUSR1);
                    command[0] = '2';
                    fd_x = open(myfifo_x, O_RDWR); 
                    write(fd_x, command, strlen(command)+1);
                }

                // Vx stop button pressed
                else if(check_button_pressed(vx_stp_button, &event)) {
                    mvprintw(LINES - 1, 1, "Horizontal Motor Stopped");
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                    kill(mx, SIGUSR1);
                    command[0] = '3';
                    fd_x = open(myfifo_x, O_RDWR); 
                    write(fd_x, command, strlen(command)+1);
                }

                // Vz++ button pressed
                else if(check_button_pressed(vz_decr_btn, &event)) {
                    mvprintw(LINES - 1, 1, "Vertical Speed Decreased");
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                    kill(mz, SIGUSR1);
                    command[0] = '4';
                    fd_z = open(myfifo_z, O_RDWR); 
                    write(fd_z, command, strlen(command)+1); 
                }

                // Vz-- button pressed
                else if(check_button_pressed(vz_incr_btn, &event)) {
                    mvprintw(LINES - 1, 1, "Vertical Speed Increased");
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                    kill(mz, SIGUSR1);
                    command[0] = '5';
                    fd_z = open(myfifo_z, O_RDWR); 
                    write(fd_z, command, strlen(command)+1); 
                }

                // Vz stop button pressed
                else if(check_button_pressed(vz_stp_button, &event)) {
                    mvprintw(LINES - 1, 1, "Vertical Motor Stopped");
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                    kill(mz, SIGUSR1);
                    command[0] = '6';
                    fd_z = open(myfifo_z, O_RDWR); 
                    write(fd_z, command, strlen(command)+1); 
                }
                close(fd_x); 
                close(fd_z);
            }
        }

        refresh();
	}

    // Terminate
    endwin();

    unlink(myfifo_xi);
    unlink(myfifo_x);
    unlink(myfifo_zi);
    unlink(myfifo_z);
    unlink(filename);
    return 0;
}






