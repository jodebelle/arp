#include "./../include/inspection_utilities.h"
#include <curses.h>
#include <stdio.h> // printf
#include <string.h> // strlen
#include <fcntl.h> // O_RDWR
#include <sys/stat.h> // PIPE
#include <sys/types.h> // PIPE
#include <unistd.h> // write and close
#include <stdlib.h> // EXIT_SUCCESS
#include <signal.h> 
#include <sys/wait.h>

// GLobal variables
int fd_xi, fd_zi;
int fd_IX, fd_IZ;
char command[80];
int str[5];
float ee_x, ee_y;


// PIPE 
char * myfifo_xi = "/tmp/myfifo_xi"; 
char * myfifo_zi = "/tmp/myfifo_zi";
char * myfifo_IX = "/tmp/myfifo_IX"; 
char * myfifo_IZ = "/tmp/myfifo_IZ"; 


    


static void signal_handler(int sig){
    if (sig == SIGUSR1) {     
        // Open PIPE
        fd_IX = open(myfifo_IX, O_RDWR); 
    	init_console_ui();
        read(fd_IX, &ee_x, sizeof(float));
	    update_console_ui(&ee_x, &ee_y);

        // Take pid
        pid_t wd = str[4];
        kill(wd, SIGUSR2);
        close(fd_IX); 
    }

    else if (sig == SIGUSR2) {
        fd_IZ = open(myfifo_IZ, O_RDWR); 
    	init_console_ui();
        read(fd_IZ, &ee_y, sizeof(float));
	    update_console_ui(&ee_x, &ee_y);

        // Take pid
        pid_t wd = str[4];
        kill(wd, SIGUSR2);
        close(fd_IZ); 
    }

    else if (sig == SIGALRM) {
        printf("\n\nWatchdog alarm detected, motors are been reset!!\n\nCommand: "); fflush(stdout);
    } 

    else if (sig == SIGINT) {
        exit(EXIT_SUCCESS);
    }
     
}

int main() {

        
	   
        // Read signal from motor x
        signal(SIGUSR1, signal_handler);
        // Read signal from motor z
        signal(SIGUSR2, signal_handler);
        // Read signal from the watchdog
        signal(SIGALRM, signal_handler);
        // Read signal from the command console to exit
        signal(SIGINT, signal_handler);
        
        
        /*
     * 1. Open PID.txt file that contain the pid for each process
     * 2. Write the pid of this one
     * 3. Read the pids of other processes
     */
    // 1.
    char *filename = "/tmp/PID.txt";
    FILE *file = fopen (filename, "a");
    int pid = getpid();

    // 2.
    fprintf(file, "%d\n", pid);
    fclose(file);

    sleep(4);

    // 3.
    FILE *file_r = fopen (filename, "r");
    fscanf(file_r, "%d %d %d %d %d", &str[0], &str[1], &str[2], &str[3], &str[4]);
    pid_t mx = str[2];
    pid_t mz = str[3];
    pid_t wd = str[4]; 

    
    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;
    

    while(TRUE) {
        

        // Get mouse/resize commands in non-blocking mode...
        int cmd = getch();
        kill(wd, SIGUSR2);

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

            // Check which button has been pressed...
            if(getmouse(&event) == OK) {

                // STOP button pressed
                if(check_button_pressed(stp_button, &event)) {
                    command[0] == 'S';
                    // Send STOP signal to the motor x and z
                    
                    mvprintw(LINES - 1, 1, "STP button pressed");
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }

                // RESET button pressed
                else if(check_button_pressed(rst_button, &event)) {
                    command[0] = 'R';
                    // Send signal to the motor x and z
                    kill(mx, SIGUSR2);
                    kill(mz, SIGUSR2);
                    // Open PIPE to send a Reset or a Stop signal
                    fd_xi = open(myfifo_xi, O_RDWR); 
                    fd_zi = open(myfifo_zi, O_RDWR); 

                    write(fd_xi, command, strlen(command)+1); 
                    write(fd_zi, command, strlen(command)+1); 

                    // Close PIPE
                    close(fd_xi); 
                    close(fd_zi); 
                    mvprintw(LINES - 1, 1, "RST button pressed");
                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }
            }
        }
          /*
        // To be commented in final version...
        switch (cmd)
        {
            case KEY_LEFT:
                ee_x--;
                break;
            case KEY_RIGHT:
                ee_x++;
                break;
            case KEY_UP:
                ee_y--;
                break;
            case KEY_DOWN:
                ee_y++;
                break;
            default:
                break;
        }
*/
        
        
        // Update UI
        
	}

    // Terminate
    unlink(myfifo_xi);
    unlink(myfifo_zi);
    unlink(myfifo_IX);
    unlink(myfifo_IZ);
    unlink(filename);

    endwin();
    return 0;
}





