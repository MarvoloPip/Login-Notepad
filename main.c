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
#define BUF_CMD 10

#define VIEW 0
#define ADD 1
#define QUIT 2

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

    fprintf(stderr, msg);
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

int main (int argc, char ** argv) {

    // variables 
    char buf[2];


    // usage ./a.out [uname] 

    if (argc != 2) {
        // fprintf(stderr, "Usage: ./note username (uname has to be <= 20 chars)");
        // exit(1);
        e("Usage: ./note username");
    } 

    // open file 
    // int data_fd;
    FILE * data_f;
    if (data_f = fopen("./data", "ab+") == NULL){
        // file no exist or cant open
        e("error opening/creating file");
    }

    // open notes file
    FILE * p_note_f;
    if (p_note_f = fopen("./notes", "ab+") == NULL){
        // file no exist or cant open
        e("error opening/creating file");
    }

    // open secondary file
    FILE * s_note_f;
    if (s_note_f = fopen("./notes", "ab+") == NULL){
        // file no exist or cant open
        e("error opening/creating file");
    }

    // ask user for name
    struct user * u = malloc(sizeof(struct user));
    int num_read;
    int num_write;
    int buf_dist = 0;
    int buf_u = 0;
    int buf_name = 0;
    int buf_new = 0;
    int num_usr = 0;

    fprintf(stdout, "Please enter a user name (max 19 characters): ");
    fflush(stdout);

    if (num_read = read(STDIN_FILENO, u->name, MAX_NAME) == 0) {
        e("error processing name ");
    } else if (num_read == MAX_NAME) {
        u->name[MAX_NAME-1] = '\0';
    } else {
        u->name[num_read] = '\0';
    }


    // read from file  
    if (num_read = fread(&num_usr, sizeof(int), 1, data_f) == 0) {

        // EOF (first time) 
        num_usr = 1;
        //write that new user has joined
        if (num_write = fwrite(&num_usr, sizeof(int), 1, data_f) != sizeof(int)) {
            e("error writing to data file for first time");
        }

        // data file format (in regex)

        // [num usrs (int)][usr data structs]+

        // start asking for user input and wrwite to file
        // need function for this


        // still need to write user data


    } else { // already exists 

        
        int found = 0;
        struct user_data * temp = malloc(sizeof(struct user_data)); 
        


        // reading num_usr times to see if usr alreayd exists
        for (int i = 0; i < num_usr; i++) {

            num_read = fread(temp, sizeof(struct user_data), 1, data_f);

            // found name
            if (strcmp(temp->name, u->name) == 0) {
                found = 1;
                buf_u = temp->buf;
                buf_name = strlen(temp->name) + 1;
                break;
            }
            else {
                buf_dist += temp->buf;
            }
        }

        // add new user
        if (!found) {

            num_usr ++;
            // go to start of data file
            fseek(data_f, 0, SEEK_SET);

            if (num_write = fwrite(&num_usr, sizeof(int), 1, data_f) != sizeof(int)) {
                e("error updating user count in data file");
            }
            
            // go to end of data file
            // to add user_data struct

            fseek(data_f, 0, SEEK_END);

            buf_new = edit_file(p_note_f, s_note_f, buf_dist, 0, 0, u);

            temp->buf = buf_new;


            

        } else { // older user 

            // go back one struct cuz we gotta overwrite
            fseek(data_f, 0.0-sizeof(struct user_data), SEEK_CUR);

            buf_new = edit_file(p_note_f, s_note_f, buf_dist, buf_u, buf_name, u);

            temp->buf = buf_new;

        }


        // wwrite to data file
        if (num_write = fwrite(temp, sizeof(struct user_data), 1, data_f) != sizeof(struct user_data)) {
            e("error adding new user to data file");
        }

        // final finish

        free(u);
        free(temp);
        fclose(p_note_f);
        fclose(s_note_f);
        

    }

}

// function that allows user to edit files 
// returns new buf_u value (how many bytes does the users name+data take up)
int edit_file(FILE * p_note_f, FILE * s_note_f, int buf_dist, int buf_u, int buf_name, struct user * u) {

    char in[BUF_CMD];
    

    // first need to travel to point in primary notes
    int num_read;
    int num_write;

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
    if (num_read = fread(u->data, sizeof(char),buf_u - buf_name, p_note_f) != buf_u-buf_name) {
        e("error reading user old data");
    }

    char new_notes[MAX_NAME - strlen(u->data)];


    // copy rest of file into secondary file 
    char c = fgetc(p_note_f);
    while (c != EOF) {
        fputc(c, s_note_f);
        c = fgetc(p_note_f);
    }

    // print menu. 
    print_opt();

    while (1) {

        print_prompt();

        if (num_read = read(STDIN_FILENO, in, BUF_CMD) == 0) {
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
            fprintf(stdout, u->data);
            continue;
        }
        else if (p == ADD) {

            if (strlen(u->data) >= MAX_DATA-1) {
                fprintf(stdout, "\n no more space left");
            }
            else {
                fprintf(stdout, "\n");

                fflush(stdout);

                num_read += read(STDIN_FILENO, new_notes, sizeof(new_notes) - (strlen(new_notes)+1));
                new_notes[num_read] = '\0';
                
                strncat(u->data, new_notes, sizeof(u->data)-strlen(u->data));
            } 

            continue;

        }
        else if (p == QUIT) {
            
            num_write = 0;
            // copy new notes to user notes 

            // to the old notes start
            fseek(p_note_f, buf_dist, SEEK_SET);

            // write name
            num_write = fwrite(u->name, sizeof(char), strlen(u->name)+1, p_note_f);
            if (num_write != strlen(u->name) + 1) {
                e("Error rewriting name to file");
            }

            // write data
            num_write += fwrite(u->data, sizeof(char), strlen(u->data)+1, p_note_f);
            if (num_write != (strlen(u->data) +strlen(u->name) + 2)) {
                e("Error rewriting notes to file");
            }

            // copy old notes from s_notes to p_notes
            // copy rest of file into secondary file 
            char c = fgetc(s_note_f);
            while (c != EOF) {
                fputc(c, p_note_f);
                c = fgetc(s_note_f);
            }

            return num_write;
        }

    }



}