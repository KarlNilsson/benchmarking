/* The purpose of this program is to measure the IPC performance
 * of UDS.
 * Note: MSG_SIZE > 65536 doesn't work */

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/un.h>
#include<string.h>
#include<pthread.h>

#define SOCK_PATH ".socket"

#ifdef PRINTOUT
#define CHILD_COLOR_YELLOW   "\x1b[33;1m"
#define PARENT_COLOR_PURPLE "\x1b[35;1m"
#define RESET_COLOR         "\x1b[0m"
#endif

/* Using global variables here as they will
 * only be set once and are used globally,
 * so I don't need to pass them everywhere.
 * Not using global variables is not the main purpose here. */

size_t nbr_msgs, msg_size;

void *server_thread(void *sock_desc){
    int sock = *(int*) sock_desc;
    char msg[msg_size];
    memset(&msg, '\0', sizeof(msg));
    int i;
    for (i = 0; i < nbr_msgs; ++i){
        #ifdef PRINTOUT
        sprintf(msg, "%d", i+1);
        printf(PARENT_COLOR_PURPLE "PING (%s)\n" RESET_COLOR, &msg);
        fflush(stdout);
        #endif

        if ((write(sock, msg, msg_size) < 0)){
            perror("Error sending message on a server thread");
        }

        if ((read(sock, msg, msg_size) < 0)){
            perror("Error receiving message on a server thread");
            pthread_exit(NULL);
        }

        #ifdef PRINTOUT
        printf(PARENT_COLOR_PURPLE "PING (%s)\n" RESET_COLOR, &msg);
        fflush(stdout);
        #endif

    }


    return 0;
}

int main(int argc, char** argv){

    int i;
    size_t nbr_children;
    pid_t child;

    if (argc == 3){
        nbr_msgs = atoi(argv[1]);
        msg_size = atoi(argv[2]);
        nbr_children = atoi(argv[3]);
    } else {
        printf("Usage: './uds_1-n nbr_msgs msg_size nbr_children'\n"
                "Defaulting to nbr_msgs = 8, msg_size = 512 nbr_children = 4\n");
        nbr_msgs = 2;
        msg_size = 512;
        nbr_children = 4;
    }


    int t, server_sock;
    socklen_t len;
    struct sockaddr_un local, remote;
    server_sock = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (server_sock < 0){
        perror("Error creating parent socket");
        goto exit_error;
    }

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, SOCK_PATH);
    unlink(local.sun_path);
    len = strlen(local.sun_path) + sizeof(local.sun_family);

    if ((bind(server_sock, (struct sockaddr *)&local, len)) < 0){
        perror("Error binding socket");
        goto exit_error;
    }

    /* Setting remote path once, since its the same for everyone */
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCK_PATH);
    
    if ((listen(server_sock, 5)) < 0){
        perror("Error listening for connections");
        goto exit_error;
    }

    for (i = 0; i < nbr_children; ++i){
        child = fork();
        if (child == 0)
            break;
    }

    if (child == 0){
        int child_id = i+1;
        char msg[msg_size];
        if ((memset(&msg, '\0', sizeof(msg))) == NULL){
            perror("Error using memset");
            goto exit_error;
        }

        int child_sock;
        if ((child_sock = socket(AF_UNIX, SOCK_SEQPACKET, 0)) < 0){
           perror("Error creating child socket");
           goto exit_error;
        }

        len = strlen(remote.sun_path)+ sizeof(remote.sun_family);
        if (connect(child_sock, (struct sockaddr*)&remote, len) < 0){
            perror("Error connecting from child");
            goto exit_error;
        }

        for (i = 0; i < nbr_msgs; ++i){
            if (read(child_sock, msg, msg_size) < 0){
                printf("Problem for child #%d\n"
                        "----------\n", child_id);
                perror("Error using read(child_sock)");
                printf("----------\n");
                goto exit_error;
            } 
            #ifdef PRINTOUT
            printf(CHILD_COLOR_YELLOW "PONG (%s) - Child #%d\n" RESET_COLOR, &msg, child_id); 
            sprintf(msg, "%d", -(i+1));
            printf(CHILD_COLOR_YELLOW "PING (%s) - Child #%d\n" RESET_COLOR, &msg, child_id);
            fflush(stdout);
            #endif

            if (write(child_sock, msg, msg_size) < 0){
                perror("Error using write(child_sock)");
                goto exit_error;
            }

        }

    } else {

        int ret_cd, conn_socks[nbr_children];
        pthread_t threads[nbr_children];

        t = sizeof(remote);
        for (i = 0; i < nbr_children; ++i){
            
            conn_socks[i] = accept(server_sock, (struct sockaddr*) &local, &t);
            if (conn_socks[i] < 0){
                perror("Error accepting connection");
                goto exit_error;
            }
            ret_cd = pthread_create(&threads[i], NULL, server_thread, &conn_socks[i]); 
            if (ret_cd < 0){
                perror("Error creating thread");
                goto exit_error;
            }
        }
        for (i = 0; i < nbr_children; ++i){
            pthread_join(threads[i], NULL);
        }

    }

    unlink(SOCK_PATH);
    return 0;

exit_error:
    unlink(SOCK_PATH);

    exit(1);
}
