#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <fcntl.h>
#include <sys/stat.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAX_DATA 2048
#define MAX_NAME 20
#define MAX_IP 10
#define BUF_CMD 10

#define VIEW 0
#define ADD 1
#define QUIT 2

#ifndef PORT
  #define PORT 55555
#endif

/*

LOGIN BASED NOTEPAD

Exit statuses: 

0 - All good 
1 - Error

*/

struct user {

    // note file structs

    char name[MAX_NAME];
    char data[MAX_DATA]; // each user only gets 2048 chars

};

struct user_data {

    // data file structs
    char name[MAX_NAME];
    int buf;


};

// function to wrwite to stderr
void e(char* msg) {

    
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

// function to print options to stdout
void print_opt() {
    fprintf(stdout, "The following operations are available:\n");
    fprintf(stdout, "    view\n");
    fprintf(stdout, "    add \n");
    fprintf(stdout, "    quit\n");
}


// function to print prompt to stdout
void print_prompt() {
    fprintf(stdout, "Enter new command: ");
    fflush(stdout);
}

// function to parse user input and return the 
// appropriate number that represents that cmd. 
// -1 for invalid cmd
int parse_cmd(char * input) {

    if (strncmp(input, "view", strlen("view")) == 0) {
        return VIEW;
    }
    else if (strncmp(input, "add", strlen("add")) == 0) {
        return ADD;
    }
    else if (strncmp(input, "quit", strlen("quit")) == 0) {
        return QUIT;
    }
    else {
        return -1;
    }

}

// function that allows user to edit files 
// returns new buf_u value (how many bytes does the users name+data take up)
int edit_file(FILE * p_note_f, FILE * s_note_f, int buf_dist, int buf_u, int buf_name, struct user * u) {

    char in[BUF_CMD];
    int second_start;
    int prim_len;

    // copy rest of file into secondary file 
    char c = fgetc(p_note_f);
    while (c != EOF) {
        fputc(c, s_note_f);
        c = fgetc(p_note_f);
    }
    

    // first need to travel to point in primary notes
    int num_read;
    int num_write = 0;
    int total_write = 0;

    // get length of p_note_f
    fseek(p_note_f, 0, SEEK_END);
    prim_len = ftell(p_note_f);

    // reset both to 0
    fseek(p_note_f, 0, SEEK_SET);
    fseek(s_note_f, 0, SEEK_SET);

    // read point where users notes start
    fseek(p_note_f, buf_dist, SEEK_SET);

    // go forward to the end of their notes
    //fseek(p_note_f, buf_u, SEEK_CUR);

    // go forward to the end of their name
    fseek(p_note_f, buf_name, SEEK_CUR);

    // store previous data in u
    num_read = fread(u->data, sizeof(char),buf_u - buf_name, p_note_f);
    if (num_read != buf_u-buf_name) {
        e("error reading user old data");
    }

    char new_notes[MAX_DATA - (strlen(u->data))];
    second_start = ftell(p_note_f);


    // print menu. 
    print_opt();

    while (1) {

        print_prompt();
        num_read = read(STDIN_FILENO, in, BUF_CMD);
        if (num_read == 0) {
            break;
        }   
        else if (num_read == BUF_CMD){
            in[BUF_CMD-1] = '\0';
        }
        else {
            in[num_read] = '\0';
        }

        int p = parse_cmd(in);

        if (p == VIEW) {
            fprintf(stdout, "%s\n", u->data);
            continue;
        }
        else if (p == ADD) {

            if (strlen(u->data) >= MAX_DATA-1) {
                fprintf(stdout, "\n no more space left");
            }
            else {
                fprintf(stdout, "\n");

                fflush(stdout);
                
                int len_read1 = sizeof(new_notes);
                int len_read2 = strlen(new_notes);
                int len_read3 = strlen(u->data);

                num_read = read(STDIN_FILENO, new_notes, sizeof(new_notes) - strlen(new_notes));
                new_notes[num_read] = '\0';
                
                strncat(u->data, new_notes, sizeof(u->data)-strlen(u->data));
            } 

            continue;

        }
        else if (p == QUIT) {

            if (strlen(new_notes) > 0) {
            
                num_write = 0;
                // copy new notes to user notes 

                // to the old notes start
                fseek(p_note_f, buf_dist, SEEK_SET);

                
                // write name
                num_write = fwrite(u->name, sizeof(char), strlen(u->name), p_note_f);
                if (num_write != strlen(u->name)) {
                    e("Error rewriting name to file");
                }
                total_write += num_write;

                // write data
                num_write = fwrite(u->data, sizeof(char), strlen(u->data), p_note_f);
                // if (num_write != (strlen(u->data) +strlen(u->name) + 2)) {
                //     e("Error rewriting notes to file");
                // }
                total_write += num_write;

                // // copy old notes from s_notes to p_notes
                // // copy rest of file into secondary file 
                // char c = fgetc(s_note_f);
                // while (c != EOF) {
                //     fputc(c, p_note_f);
                //     c = fgetc(s_note_f);
                // }
                fseek(s_note_f, second_start, SEEK_SET);

                char second_notes[prim_len - second_start];

                num_read = fread(second_notes, sizeof(char), sizeof(second_notes), s_note_f);
                
                fwrite(second_notes, sizeof(char), sizeof(second_notes), p_note_f);


                return total_write;
            }
            else {
                return 0;
            }

        }

    }

    return total_write;

}

int main (int argc, char ** argv) {

    // variables 
    //char buf[2];

    int fd[3]; // 0- notes, 1- s_notes, 2- data

    // usage ./a.out [uname] 

    if (argc != 1) {
        // fprintf(stderr, "Usage: ./main username (uname has to be <= 20 chars)");
        // exit(1);
        e("Usage: ./main");
    } 

    // open files
    if ((fd[2] = open("./data", O_CREAT) == -1) || 
            (fd[1] = open("./s_notes", O_CREAT) == -1) || 
            (fd[0] = open("./note", O_CREAT) == -1)) {
        e("error creating file/s");
    }
    FILE * data_f;
    data_f = fopen("./data", "r+");
    if (data_f == NULL){
        // file no exist or cant open
        e("error opening file");
    }

    // open notes file
    FILE * p_note_f;
    p_note_f = fopen("./note", "r+");
    if (p_note_f == NULL){
        // file no exist or cant open
        e("error opening/creating file");
    }

    // open secondary file
    FILE * s_note_f;
    s_note_f = fopen("./s_notes", "r+");
    if (s_note_f == NULL){
        // file no exist or cant open
        e("error opening/creating file");
    }

    // ask user for name
    struct user * u = malloc(sizeof(struct user));
    struct user_data * temp = malloc(sizeof(struct user_data)); 

    int num_read;
    int num_write;
    int buf_dist = 0;
    int buf_u = 0;
    int buf_name = 0;
    int buf_new = 0;
    int num_usr = 0;

    int temp_tell = 0;
    int found = 0;


    fprintf(stdout, "Please enter a user name (max 19 characters): ");
    fflush(stdout);

    num_read = read(STDIN_FILENO, u->name, MAX_NAME);
    if (num_read == 0) {
        e("error processing name ");
    } else if (num_read == MAX_NAME) {
        u->name[MAX_NAME-1] = '\0';
    } else {
        u->name[num_read] = '\0';
    }


    // read from file  
    num_read = fread(&num_usr, sizeof(int), 1, data_f);
    if (num_read == 0) {

        // EOF (first time) 
        num_usr = 1;
        //write that new user has joined
        num_write = fwrite(&num_usr, sizeof(int), 1, data_f);
        // if (num_write != sizeof(int)) {
        //     e("error writing to data file for first time");
        // }

        // data file format (in regex)

        // [num usrs (int)][usr data structs]+

        // start asking for user input and wrwite to file
        // need function for this


        // still need to write user data
        fseek(data_f, 0, SEEK_END);

        buf_new = edit_file(p_note_f, s_note_f, buf_dist, 0, 0, u);

        strcpy(temp->name, u->name);
        temp->buf = buf_new;


    } else { // already exists 

        
        


        // reading num_usr times to see if usr alreayd exists
        for (int i = 0; i < num_usr; i++) {

            num_read = fread(temp, sizeof(struct user_data), 1, data_f);

            // found name
            if (strcmp(temp->name, u->name) == 0) {
                found = 1;
                buf_u = temp->buf;
                buf_name = strlen(temp->name);
                break;
            }
            else {
                temp_tell++;
                buf_dist += temp->buf;
            }
        }

        // add new user
        if (!found) {

            num_usr ++;
            // go to start of data file
            fseek(data_f, 0, SEEK_SET);

            num_write = fwrite(&num_usr, sizeof(int), 1, data_f);
            // if (num_write != sizeof(int)) {
            //     e("error updating user count in data file");
            // }
            
            // go to end of data file
            // to add user_data struct

            fseek(data_f, 0, SEEK_END);

            buf_new = edit_file(p_note_f, s_note_f, buf_dist, 0, 0, u);

            temp->buf = buf_new;


            

        } else { // older user 

            // go back one struct cuz we gotta overwrite
            fseek(data_f, 0.0-sizeof(struct user_data), SEEK_CUR);

            buf_new = edit_file(p_note_f, s_note_f, buf_dist, buf_u, buf_name, u);

            if (buf_new != 0){ 
                temp->buf = buf_new;
            }
            

        }


       

    }

    if (!found) {
        fseek(data_f, 0, SEEK_END);
        strcpy(temp->name, u->name);
    }
    else {
        fseek(data_f, sizeof(int) + (temp_tell * sizeof(struct user_data)), SEEK_SET);
    }
    
    if (buf_new > 0) {
        // wwrite to data file
        num_write = fwrite(temp, sizeof(struct user_data), 1, data_f);
        // if (num_write != sizeof(struct user_data)) {
        //     e("error adding new user to data file");
        // }

        // final finish
    }

    free(u);
    free(temp);
    fclose(p_note_f);
    fclose(s_note_f);

    // calling update
    
    return 0;

}

// function that handles updating the top 10 ips in config.txt
void server_bizness() {

    int num_ip = 0; 
    int port = PORT;
    char buf[BUF_CMD];
    
    // tdodo
    // set up server
    int note_client;
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("server socket:");
        e("");
    }

    fprintf(stdout, "which port do you want to use to update your notes? (default = 55555)\n");
    fflush(stdout);

    int num_read = read(STDIN_FILENO, buf, BUF_CMD);

    port = atoi(buf);

    // Set information about the port (and IP) we want to be connected to.
    struct sockaddr_in server, client;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    // This sets an option on the socket so that its port can be reused right
    // away. Since we are likely to run, stop, edit, compile and rerun your
    // server fairly quickly, this will mean you can reuse the same port.
    int on = 1;
    int status = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (const char *) &on, sizeof(on));
    if (status == -1) {
        perror("setsockopt -- REUSEADDR");
    }

    // This should always be zero. On some systems, it won't error if you
    // forget, but on others, you'll get mysterious errors. So zero it.
    memset(&server.sin_zero, 0, 8);

    // Bind the selected port to the socket.
    if (bind(sock_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("server: bind");
        close(sock_fd);
        e("");
    }

    // Announce willingness to accept connections on this socket.
    if (listen(sock_fd, MAX_IP) < 0) {
        perror("server: listen");
        close(sock_fd);
        e("");
    }

    // dont need to use select for the server side because we are not 
    // reading from any of the ips but we will use it on the socket

    int max_fd = sock_fd;
    fd_set all;
    FD_ZERO(&all);
    FD_SET(sock_fd, &all);
    fd_set all_cpy = all;

    if (select(max_fd+1, &all_cpy, NULL, NULL, NULL) == -1) {
        perror("server: select");
        e("");
    }
    
    // an ip wants to connect
     
    int c_size = sizeof(client); 
    if (FD_ISSET(sock_fd, &all_cpy)) {

        note_client = accept(sock_fd, (struct sockaddr*)&client, &c_size);
        if (note_client > 0) {
            fprintf(stdout, "Accepted the note-taking client\n");
        
            if (note_client > sock_fd) {
                max_fd = note_client;
            }
            
            FD_SET(note_client, &all);

            all_cpy = all;

            conduct_bizness(sock_fd, max_fd, note_client, all);

        } 

    }



    // connect to client

    // copy main file
    // copy notes 
    // copy data

    // strat;
    // give servre 
    // 4 byte file name indicator
    // sizeof(int) size of file/bytes to allocate
    // send those bytes

    fprinttf(stdout, "Updating %d number of Machines", num_ip);


}

// conduct business with the client. Client will be sending the following 2 times
//  
// sizeof(int) size of file/bytes to allocate
// those bytes
//
//  for the three files (in this order): data,note,
void conduct_bizness(int sock_fd, int max_fd, int note_client, fd_set all) {

    fd_set all_cpy = all;
    int num_read = 0, num_write = 0;
    int num_bytes;

    char buf[BUF_CMD];
    char * data_buf = NULL;
    char * note_buf = NULL;

    if (select(max_fd+1, &all_cpy, NULL, NULL, NULL) == -1) {
        perror("server: select");
        e("");
    }

        
    // create ze files     

    // the data file
    FILE * data_f = fopen("./data", "wb+");
    if (data_f == NULL) {
        e("problems with data file");
    } 

    // the note file
    FILE * p_note_f = fopen("./note", "wb+");
    if (p_note_f == NULL) {
        e("problems with note file");
    }


    // client ready to sned over the goodies
    if (FD_ISSET(note_client, &all_cpy)) {


        // read int
        num_read = read(sock_fd, buf, BUF_CMD);

        if (num_read == BUF_CMD) {
            buf[num_read-1] = '\0';
        }
        else {
            buf[num_read] = '\0';
        }

        num_bytes = atoi(buf);

        // allocate that number of bytes
        data_buf = malloc(num_bytes);

    }

    // reset the fd set copy
    all_cpy = all;

    // client ready to send data file bytes
    if ((FD_ISSET(note_client, &all_cpy)) && data_buf != NULL) {

        // read the contents of data file
        num_read = read(sock_fd, data_buf, num_bytes);  
        // write to the data file
        num_write = fwrite(data_buf, sizeof(data_buf) , 1, data_f);

        fclose(data_f);

        fprintf(stdout, "sucessfully cloned the data file\n");

    }

    // reset the fd set copy
    all_cpy = all;
    
    // client ready to sned over the goodies
    if (FD_ISSET(note_client, &all_cpy)) {


        // read int
        num_read = read(sock_fd, buf, BUF_CMD);

        if (num_read == BUF_CMD) {
            buf[num_read-1] = '\0';
        }
        else {
            buf[num_read] = '\0';
        }

        num_bytes = atoi(buf);

        // allocate that number of bytes
        note_buf = malloc(num_bytes);

    }

    // reset the fd set copy
    all_cpy = all;

    // client ready to send data file bytes
    if ((FD_ISSET(note_client, &all_cpy)) && note_buf != NULL) {

        // read the contents of notes file
        num_read = read(sock_fd, note_buf, num_bytes);  
        // write to the data file
        num_write = fwrite(note_buf, sizeof(note_buf) , 1, p_note_f);

        fclose(p_note_f);

        fprintf(stdout, "sucessfully cloned the notes\n");


    }

    fprintf(stdout, "transfer complete with client\n");

}

void client_bizness() {

    char line[256];
    char ips[MAX_IP][20];
    int ports[MAX_IP];

    // read the ips and ports from config file
    FILE * config_f = fopen("./config.txt", "r");
    if (config_f == NULL) {
        perror("config file:");
        e("nowhere to update the notes");
    }
    else {

        // read the file line by line
        int num_ips = 0;
        while (fgets(line, sizeof(line), config_f)) {

            if (num_ips >= 10) {
                fprintf(stdout, "Only attempt to update the first 10 ips\n");
                break;
            }

            
            char * real_line = strtok(line, '\n');

            char * addy = strtok(real_line, ',');
            char * port = strtok(addy, ',');

            strncpy(ips[num_ips], addy, 20);
            ports[num_ips] = atoi(port);

        }

        // either 10 ips read or no more left to read

        


    }


}
