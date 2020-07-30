#include "client.h"
#include <errno.h>

#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>

#define MSG_SIZE 512

// count number of existing txt files
int txt_count = 0;
// store partition number for each client
int partition = 0;
// store remainder of txt_count / num_clients
int leftover = 0;
// store number of path in current client txt
int num_path = 0;
// store current client number
int cur_client = 0;

struct msg_buffer {
  long msg_type;
  char msg_text[MSG_SIZE];
} msg;

void ignore_newline(char *str) {
  for(int i=0; str[i] != '\0';i++) {
    if(str[i] == '\n') {
      str[i] = '\0';
    }
  }
}

void count_txt_path(char *path) {
  struct dirent *de;
  char new_path[256];
  new_path[0] = '\0';
  DIR *dir = opendir(path);
  if(dir == NULL) {				// error checking

    perror("opendir");
    return;
  }

  char* new_path_arr[100] = {0};

  while((de = readdir(dir)) != NULL) {
    if(strcmp(de->d_name,".") != 0 &&
    strcmp(de->d_name,"..") != 0 &&
    de->d_name[0] != '.') {
      // check extension to be txt
      char *ext;
      char *txt_ext = ".txt";
      if((ext = strrchr(de->d_name,'.')) != NULL) {
        if(strcmp(ext, txt_ext) == 0)
        txt_count++;
      }
    }
  }

  rewinddir(dir);
  // recursion
  while((de = readdir(dir)) != NULL) {
    if(strcmp(de->d_name,".") != 0 &&
    strcmp(de->d_name,"..") != 0 &&
    de->d_name[0] != '.') {
      if(de->d_type == DT_DIR) {
        strcpy(new_path, path);
        strcat(new_path, "/");
        strcat(new_path, de->d_name);
        // recurse
        count_txt_path(new_path);
      }
    }
  }
  closedir(dir);
}

void assign_path(char *path) {
  struct dirent *de;
  char new_path[256];
  new_path[0] = '\0';
  DIR *dir = opendir(path);
  if(dir == NULL) {				// error checking
    perror("opendir");
    return;
  }
  else {
    char *new_path_arr[100] = {0};
    while((de = readdir(dir)) != NULL) {
      if(strcmp(de->d_name,".") != 0 &&
      strcmp(de->d_name,"..") != 0 &&
      de->d_name[0] != '.') {

        // store txt path to corresponding client
        // get path of current client
        char *client_path = (char *)malloc(100 * sizeof(char));
        snprintf(client_path , 100, "./ClientInput/Client%d.txt", cur_client);
        // write valid txt path to current client
        char *ext;
        char *txt_ext = ".txt";
        if((ext = strrchr(de->d_name,'.')) != NULL) {
          if(strcmp(ext, txt_ext) == 0) {
            FILE *fp;
            fp = fopen(client_path, "a");
            if(fp == NULL) {
              perror("fopen");
              return;
            }
            strcpy(new_path, path);
            strcat(new_path, "/");
            strcat(new_path, de->d_name);
            fprintf(fp, "%s\n", new_path);
            num_path++;
            free(client_path);
            fclose(fp);
          }
        }
        // if there's a remainder, put additional address at the end
        // of the current client and reduce the remainder count
        if((num_path == partition) && (leftover > 0)) {
          leftover--;
          continue;
        }

        // if number of path is bigger than partition number
        // go to next client and set number of path to 0
        else if(num_path >= partition) {
          cur_client++;
          num_path = 0;
        }
      }
    }
    rewinddir(dir);
    // recursion
    while((de = readdir(dir)) != NULL) {
      if(strcmp(de->d_name,".") != 0 &&
      strcmp(de->d_name,"..") != 0 &&
      de->d_name[0] != '.') {
        if(de->d_type == DT_DIR) {
          strcpy(new_path, path);
          strcat(new_path, "/");
          strcat(new_path, de->d_name);
          // recurse
          assign_path(new_path);
        }
      }
    }
    closedir(dir);
  }
}

int main(int argc, char** argv) {
  // get current date and time
  time_t t;
  time(&t);
  char *date_time = ctime(&t);
  // remove '\n' from date_time
  ignore_newline(date_time);

  // others;
  struct dirent *de;
  char folder_path[256];
  int num_clients;
  // get path of folder
  if(argc != 3) {
    printf("Wrong number of arguments.\nRun using the following example: \n%s input_folder_path num_clients\n", argv[0]);
    return 1;
  }
  else if(atoi(argv[2]) <= 0) {
    printf("Number of client should not be less than 1, please input a client number bigger than 0\n");
    return 1;
  }
  else {
    memcpy(folder_path, argv[1], sizeof(folder_path));
    num_clients = atoi(argv[2]);

    printf("[%s]Client starts...\n", date_time);
    printf("[%s]Directory %s traversal and file partitioning...\n", date_time, folder_path);

    DIR *dir = opendir(folder_path);
    if(dir == NULL) {
      printf("[%s]The %s folder is empty\n", date_time, folder_path);
      printf("[%s]Client ends...\n", date_time);
      return 1;
    }
  }
  // clean txt files in ClientInput and Output directory before running
  DIR *in_dir = opendir("./ClientInput");
  DIR *out_dir = opendir("./Output");
  if(in_dir && out_dir) {
    system("exec rm -r ./ClientInput/*");
    system("exec rm -r ./Output/*");
    closedir(in_dir);
    closedir(out_dir);
  } else if (ENOENT == errno) {
    printf("Failed to open ./ClientInput and ./Output\n");
  } else {
    perror("opendir");
    return 0;
  }

  // create Clienti.txt and Clienti_out.txt files
  for(int i = 0; i < num_clients; i++) {
    char *fname = (char *)malloc(100 * sizeof(char));
    char *fname_out = (char *)malloc(100 * sizeof(char));
    mkdir("ClientInput", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    mkdir("Output", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    snprintf(fname , 100, "./ClientInput/Client%d.txt", i);
    snprintf(fname_out , 100, "./Output/Client%d_out.txt", i);
    FILE *fp;
    fp = fopen(fname, "w");
    if(fp == NULL) {
      perror("fopen");
      return 0;
    }
    FILE *fp_out;
    fp_out = fopen(fname_out, "w");
    if(fp_out == NULL) {
      perror("fopen");
      return 0;
    }
    fclose(fp_out);
    free(fname_out);
  }

  // store subfolder txt path to Clienti.txt
  count_txt_path(folder_path);
  if (txt_count <= 0) {
    printf("[%s]The %s folder is empty\n", date_time, folder_path);
    printf("[%s]Client ends...\n", date_time);
    return 0;
  }
  // calc partition number
  partition = txt_count / num_clients;

  leftover = txt_count % num_clients;

  // assign path to each client txt file
  assign_path(folder_path);
  // forking client processes, i represents serial number of process
  int tag_ack = num_clients;
  int tag_glob = 2 * num_clients;
  int status = 0;
  pid_t pid;

  key_t key;
  int msgid;
  key = ftok("../progfile", 65);
  msgid = msgget(key, 0666 | IPC_CREAT);

  for(int i = 0; i < num_clients; i++) {
    tag_ack += 1;
    tag_glob += 1;
    pid = fork();
    if(pid == 0) {
      char *line = NULL;
      size_t len = 0;
      ssize_t read;
      char *client_path = (char *)malloc(100 * sizeof(char));
      snprintf(client_path , 100, "./ClientInput/Client%d.txt", i);
      FILE *fp = fopen(client_path, "r");
      if(fp == NULL){
        perror("fopen");
        return 0;
      }
      // read by lines
      while ((read = getline(&line, &len, fp)) != -1) {
        msg.msg_type = i + 1;
        // send read line to msg_text
        ignore_newline(line);
        strncpy(msg.msg_text, line, MSG_SIZE);
        // send message
        printf("[%s]Sending %s from client process %d\n", date_time, msg.msg_text, i);
        if((msgsnd(msgid, (void*)&msg, sizeof(msg.msg_text), 0)) == -1) {
          printf("msgid2: %ld\n", msg.msg_type);
          printf("error is %s", strerror(errno));
          fflush(stdout);
          perror("msgsnd");
          return 0;
        }
        // receive ACK
        if((msgrcv(msgid, &msg, sizeof(msg.msg_text), tag_ack, 0)) == -1) {
          perror("msgrcv");
          return 0;
        }
        if(strcmp(msg.msg_text, "ACK") == 0){
          printf("[%s]Client process %d received ACK from server for %s\n", date_time, i, line);
          continue;
        }
      }
      fclose(fp);
      msg.msg_type = i + 1;
      strncpy(msg.msg_text, "END", MSG_SIZE);
      printf("[%s]Sending END from client process %d\n", date_time, i);
      if((msgsnd(msgid, &msg, sizeof(msg.msg_text), 0)) == -1) {
        perror("msgsnd");
        return 0;
      }
      // receive global result
      if((msgrcv(msgid, &msg, sizeof(msg), tag_glob, 0)) == -1) {
        perror("msgrcv");
        return 0;
      }
      // print global result
      printf("[%s]Client process %d received |||%s||| from server\n", date_time, i, msg.msg_text);
      // write global count to Clienti_out.txt
      char *out_path = (char *)malloc(100 * sizeof(char));
      snprintf(out_path , 100, "./Output/Client%d_out.txt", i);
      FILE *fp_out = fopen(out_path, "w");
      if(fp_out == NULL) {
        perror("fopen");
        return 0;
      }
      fprintf(fp_out, "%s\n", msg.msg_text);
      // free memory
      free(out_path);
      fclose(fp_out);
      exit(0);
    }
  }
  pid_t pid_w;
  while((pid_w = wait(&status)) > 0);
  msgctl(msgid, IPC_RMID, NULL);
  printf("[%s]Client ends...\n", date_time);
  return 0;
}
