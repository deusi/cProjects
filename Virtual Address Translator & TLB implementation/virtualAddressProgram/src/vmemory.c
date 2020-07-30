//Implement the API modeling the translation of virtual page address to a
//physical frame address. We assume a 32 bit virtual memory and physical memory.
//Access to the page table is only via the CR3 register.
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "vmemory.h"

#define OUT_TLB "../bin/tlb_out.txt"
#define MAX 8

bool FIFO_policy = true;
int **cr3;
//
// More static functions can be implemented here
//
// count number of element in tlb
int count = 0;
// tlb array
int tlb[8][2];
// LRU struct for
int time = 0;
// lru array tracks access time of entries in tlb
int lru[8];

void init_lru() {
  for(int i = 0; i < 8; i++) {
    lru[i] = -1;
  }
}

void replace_lru(int v_addr, int p_addr) {
  int min = lru[0];
  int min_idx = 0;
  if(count < 8)
  {
    tlb[count][1] = p_addr;
    tlb[count][0] = v_addr;
    lru[count] = time;
    time++;
    count++;
    return;
  }
  else
  {
    for(int i = 0; i < 8; i++) {
      if(lru[i] < min)
      {
        min = lru[i];
        min_idx = i;
      }
    }
    tlb[min_idx][1] = p_addr;
    tlb[min_idx][0] = v_addr;
    lru[min_idx] = time;
    time++;
    return;
  }
}

void lru_hit(int tlb_entry) {
  for(int i = 0; i < 8; i++)
  {
    if(tlb[i][1] == tlb_entry)
    {
      lru[i] = time;
      time++;
      return;
    }
  }
}

// TLB miss count & total request
int tlb_miss = 0;
int tlb_total = 0;

// FIFO Queue for TLB
int front = 0;
int rear = -1;

void init_tlb() {
  for(int i = 0; i < 8; i++) {
    tlb[i][0] = -1;
    tlb[i][1] = -1;
  }
}

bool isEmpty() {
  return count;
}

bool isFull() {
  if(count >= 8)
  return true;
  else
  return false;
}

void enqueue(int v_addr, int p_addr) {
  if(!isFull()) {
    if(rear == MAX-1) {
      rear = -1;
    }
    tlb[++rear][1] = p_addr;
    tlb[rear][0] = v_addr;
    count++;
  }
}

void dequeue() {
  front++;
  if(front == MAX) {
    front = 0;
  }
  count--;
}

static char* bin_to_hex(long int bin_num)
{
  long int hexdec_num = 0, i = 1, remainder;
  static char buffer[13];
  while (bin_num != 0)
  {
    remainder = bin_num % 10;
    hexdec_num = hexdec_num + remainder * i;
    i = i * 2;
    bin_num = bin_num / 10;
  }
  sprintf(buffer, "%lX", hexdec_num);
  return buffer;
}

char* hex_to_bin(char *hex_num, char *bin)
{
  char *hex = strndup(hex_num+2, 8);
  for(int i=0; i < 8; i++)
  {
    switch(hex[i])
    {
      case '0':
      strcat(bin, "0000");
      break;
      case '1':
      strcat(bin, "0001");
      break;
      case '2':
      strcat(bin, "0010");
      break;
      case '3':
      strcat(bin, "0011");
      break;
      case '4':
      strcat(bin, "0100");
      break;
      case '5':
      strcat(bin, "0101");
      break;
      case '6':
      strcat(bin, "0110");
      break;
      case '7':
      strcat(bin, "0111");
      break;
      case '8':
      strcat(bin, "1000");
      break;
      case '9':
      strcat(bin, "1001");
      break;
      case 'a':
      case 'A':
      strcat(bin, "1010");
      break;
      case 'b':
      case 'B':
      strcat(bin, "1011");
      break;
      case 'c':
      case 'C':
      strcat(bin, "1100");
      break;
      case 'd':
      case 'D':
      strcat(bin, "1101");
      break;
      case 'e':
      case 'E':
      strcat(bin, "1110");
      break;
      case 'f':
      case 'F':
      strcat(bin, "1111");
      break;
      default:
      printf("Invalid hexadecimal input.");
    }
  }
}

int bin_to_dec(long int bin_num)
{
  int dec = 0, i = 1, remainder;
  static char buffer[13];
  while (bin_num > 0)
  {
    remainder = bin_num % 10;
    dec = dec + remainder * i;
    i = i * 2;
    bin_num = bin_num / 10 ;
  }
  return dec;
}

// The implementation of get_vpage_cr3 is provided in
// an object file, so no need to re-implement it
void initialize_vmanager(int policy)
{
  // Set LRU policy when passsed as a parameter
  if (policy)
  FIFO_policy = false;
  cr3 = get_vpage_cr3();
}

//
// The implementation of following functions is required
//
int translate_virtual_address(unsigned int v_addr)
{
  // get first and second pt index from v_addr
  int first_pt_idx = v_addr >> 22;
  int second_pt_idx = v_addr << 10 >> 22;

  if(*(cr3 + first_pt_idx) == NULL)
  return -1;
  else
  {
    int frame = *(*(cr3 + first_pt_idx) + second_pt_idx);
    return frame;
  }
}

void print_physical_address(int frame, int offset)
{
  if(frame == -1)
  printf("-1");
  else {
    // right zero pad to 8 digits by mutiplying 4096
    // and add offset
    int p_addr = frame * 4096 + offset;
    printf("0x%08x\n", p_addr);
  }
}

int get_tlb_entry(int n)
{
  tlb_total++;
  for(int i = 0; i < 8; i++)
  {
    if(tlb[i][0] == n)
    return tlb[i][1];
  }
  tlb_miss++;
  return -1;
}

void populate_tlb(int v_addr, int p_addr)
{
  // populate with LRU
  // populate with FIFO
  if(isFull())
  {
    dequeue();
    enqueue(v_addr, p_addr);
  }
  else
  enqueue(v_addr, p_addr);
  return;
}

float get_hit_ratio()
{
  return 1 - (float)tlb_miss / tlb_total;
}

//Write to the file in OUT_TLB
void print_tlb()
{
  FILE *fp;
  fp = fopen("tlb_out.txt", "a");
  if(fp == NULL) {
    perror("Coudn't open tlb_out.txt\n");
    return;
  }
  fprintf(fp, "\n");
  for(int i = 0; i < 8; i++)
  {
    if(tlb[i][0] == -1) {

      fprintf(fp, "-1 -1\n");
    }
    else {
      fprintf(fp, "0x%05x 0x%05x\n" , tlb[i][0], tlb[i][1]);
    }
  }
  fclose(fp);
  return;
}
