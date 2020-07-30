#include "server.h"

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
#include <pthread.h>

#define MSG_SIZE 512

int letter_count[26] = {0};
int end_count = 0;

key_t key;
int msgid;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct msg_buffer {
  long msg_type;
  char msg_text[MSG_SIZE];
} msg;

// structure for counting attribures of server
typedef struct argstruct {
  int tag_line;
  int tag_ack;
  int tag_glob;
  int num_threads;
} args_t;

void *msgread(void *args) {
  struct msg_buffer msg_new;
  // get current date and time
  time_t t;
  time(&t);
  char *date_time = ctime(&t);

  // remove '\n' from date_time
  for(int i=0; date_time[i] != '\0';i++) {
    if(date_time[i] == '\n') {
      date_time[i] = '\0';
    }
  }

  args_t *input = (struct argstruct *) args;
  // receive requests from message queue and count words
  for( ; ; ) {
    printf("[%s]Waiting to rcv from client process %d\n", date_time, input->tag_line-1);
    int tag = input->tag_line;
    msg_new.msg_type = input->tag_line;

    if((msgrcv(msgid, &msg_new, sizeof(msg_new.msg_text), tag, 0)) == -1) {
      perror("msgrcv");
      return 0;
    }

    // if "END" received, break the loop
    if(strcmp(msg_new.msg_text, "END") == 0) {
      // mutex
      pthread_mutex_lock(&mutex);
      end_count++;
      pthread_mutex_unlock(&mutex);
      printf("[%s]Thread %d received END from client process %d\n", date_time, input->tag_line-1, input->tag_line-1);
      break;
    }
    char *fname = (char *)malloc(512 * sizeof(char));
    printf("[%s]Thread %d received %s from client process %d\n", date_time, input->tag_line-1, msg_new.msg_text, input->tag_line-1);
    // copy received message as file name
    memcpy(fname, msg_new.msg_text, MSG_SIZE);

    // open file, read it line by line and count letter
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    FILE *fp;
    fp = fopen(fname, "r");
    if(fp == NULL) {
      perror("fopen");
      return 0;
    }
    while ((read = getline(&line, &len, fp)) != -1) {
      pthread_mutex_lock(&mutex);
      int idx = 0;
      // uppercase
      if(line[0] >= 'A' && line[0] <= 'Z') {
        idx = line[0] - 'A';
        letter_count[idx]++;
      }
      // lowercase
      else if(line[0] >= 'a' && line[0] <= 'z') {
        idx = line[0] - 'a';
        letter_count[idx]++;
      }
      pthread_mutex_unlock(&mutex);
    }
    fclose(fp);
    // send ACK after the file read completed
    strcpy(msg_new.msg_text, "ACK");
    printf("[%s]Thread %d sending ACK to client %d for %s\n", date_time, input->tag_line-1, input->tag_line-1, fname);
    msg_new.msg_type = input->tag_ack;
    msgsnd(msgid, &msg_new, sizeof(msg_new.msg_text), 0); // send message "Acknowledgement"
    free(fname);
  }

  pthread_mutex_lock(&mutex);
  // cond with broadcast
  while(end_count != input->num_threads) {
    pthread_cond_wait(&cond, &mutex);
  }
  pthread_cond_broadcast(&cond);
  pthread_mutex_unlock(&mutex);
  // construct char array
  char *char_arr = (char *)malloc(256 * sizeof(char));
  char *first_count = (char *)malloc(256 * sizeof(char));;
  sprintf(first_count, "%d", letter_count[0]);
  strcpy(char_arr, first_count);
  for(int i = 1; i < 26; i++) {
    char *add = (char *)malloc(50 * sizeof(char));
    snprintf(add , 50, "#%d", letter_count[i]);
    strcat(char_arr, add);
    free(add);
  }
  // send char array
  strcpy(msg.msg_text, char_arr);
  msg.msg_type = input->tag_glob;
  msgsnd(msgid, &msg, sizeof(msg), 0);
  printf("[%s]Thread %d sending final letter count to client process %d\n", date_time, input->tag_line-1, input->tag_line-1);
  free(char_arr);
}

int main(int argc, char** argv) {
  key_t key = ftok("../progfile", 65);
  if((msgid = msgget(key, 0666 | IPC_CREAT)) < 0){
    printf("error");
    exit(1);
  }
  else {
    msgctl(msgid, IPC_RMID, NULL);
    if((msgid = msgget(key, 0666 | IPC_CREAT)) < 0){
      printf("error");
      return 1;
    }
  }

  int num_threads = 0;
  if(argc != 2) {
    printf("Wrong number of arguments.\nRun using the following example: \n%s num_clients\n", argv[0]);
    return 1;
  }
  else if(atoi(argv[1]) <= 0) {
    printf("Number of client should not be less than 1, please input a client number bigger than 0\n");
    return 1;
  }
  else {
    num_threads = atoi(argv[1]);
  }
  // get current date and time
  time_t t;
  time(&t);
  char *date_time = ctime(&t);

  // remove '\n' from date_time
  for(int i=0; date_time[i] != '\0';i++) {
    if(date_time[i] == '\n') {
      date_time[i] = '\0';
    }
  }

  printf("[%s]Server starts...\n", date_time);

  pthread_t m_threads[num_threads];
  args_t m_args[num_threads];

  for(int i = 0; i < num_threads; i++) {
    m_args[i].tag_line = i + 1;
    m_args[i].tag_ack = i + 1 + num_threads;
    m_args[i].tag_glob = i + 1 + 2 * num_threads;
    m_args[i].num_threads = num_threads;
    pthread_create(&m_threads[i], NULL, msgread, (void *)&m_args[i]);
  }

  for(int j = 0; j < num_threads; j++) {
    pthread_join(m_threads[j], NULL);
  }

  printf("[%s]Server ends...\n", date_time);
}
