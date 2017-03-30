/* The purpose of this program is to measure the performance
 * of UDS during IPC.
 * Note: MSG_SIZE > 65536 doesn't work */
#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#ifdef PRINTOUT
#define PARENT_COLOR_YELLOW   "\x1b[33;1m"
#define PARENT_COLOR_PURPLE "\x1b[35;1m"
#define RESET_COLOR         "\x1b[0m"
#endif

int main(int argc, char** argv){

    int i, socks[2];
    size_t nbr_msgs, msg_size;

    if (argc == 3){
        nbr_msgs = atoi(argv[1]);
        msg_size = atoi(argv[2]);
    } else {
        printf("Usage: './uds_1-1 nbr_msgs msg_size'\n"
                "Defaulting to nbr_msgs = 8, msg_size = 512\n");
        nbr_msgs = 8;
        msg_size = 512;
    }

    char msg[msg_size];


    if (socketpair(AF_UNIX, SOCK_STREAM, 0, socks) < 0){
        perror("Error creating socket pair");
        goto exit_error;
    }
    
    if((memset(&msg, '\0', sizeof(msg))) == NULL){
        perror("Error using memset");
        goto exit_error;
    }

    if (fork() == 0){
        close(socks[1]);
        for (i = 0; i < nbr_msgs; ++i){

            if (read(socks[0], msg, msg_size) < 0){
                perror("Error using read(socks[0])");
                goto exit_error;
            }
            
            #ifdef PRINTOUT
            printf(PARENT_COLOR_YELLOW "PONG (%s)\n" RESET_COLOR, (char*) &msg); 
            sprintf(msg, "%d", -(i+1));
            #endif

            if (write(socks[0], msg, msg_size) < 0){
                perror("Error using write(socks[0])");
                goto exit_error;
            }
            #ifdef PRINTOUT
            printf(PARENT_COLOR_YELLOW "PING (%s)\n" RESET_COLOR, (char*) &msg);
            #endif

        }
        close(socks[0]);

    } else {
        close(socks[0]);
        for (i = 0; i < nbr_msgs; ++i){
            
            #ifdef PRINTOUT
            sprintf(msg, "%d", i+1);
            printf(PARENT_COLOR_PURPLE "PING (%s)\n" RESET_COLOR, (char*) &msg);
            #endif

            if (write(socks[1], msg, msg_size) < 0){
                perror("Error using write(socks[1])");
                goto exit_error;
            }

            if (read(socks[1], msg, msg_size) < 0){
                perror("Error using read(socks[1])");
                goto exit_error;
            }

            #ifdef PRINTOUT
            printf(PARENT_COLOR_PURPLE "PONG (%s)\n" RESET_COLOR, (char*) &msg);
            #endif

        }
        close(socks[1]);
    }

    return 0;

exit_error:
        exit(1);
}
