//main.c program to translate the virtual addresses from the virtual.txt
//into the physical frame address. Project 3 - CSci 4061

#define _GNU_SOURCE
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include "vmemory.h"

#define INPUT_FILE "../bin/virtual.txt"

int main(int argc, char* argv[])
{
	// what we expect to see when we run main
	if (argc > 2)
	{
		printf("Too many arguments, enter up to one argument\n");
		exit(-1);
	}
	int policy = 0;
	if(argc == 2) {
		if(strcmp(argv[1], "-lru") == 0)
		policy = 1;
		else
		{
			printf("Invalid option. The option should be -lru.\n");
			exit(-1);
		}
	}
	//int policy = (argc == 2) ? 1:0;
	initialize_vmanager(policy);

	FILE *fp;
	fp = fopen(INPUT_FILE, "r");
	if(fp == NULL)
	{
		perror("No such file\n");
		return 0;
	}

	// remove output file is present
	remove("tlb_out.txt");

	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	init_tlb();
	print_tlb();
	// read line by line
	while((read = getline(&line, &len, fp)) != -1)
	{
		// convert line from hexdec to decimal integer
		char *bin = malloc(sizeof(char) * 32);
		*bin = '\0';
		hex_to_bin(line, bin);

		long int bin_line = strtol(bin, NULL, 2);

		// get offset
		char *offset = strndup(bin+20, 12);
		int offset_int = strtol(offset, NULL, 2);
		// get 20MSB of virtual address
		char *MSB_20 = strndup(bin, 20);
		int MSB_20_int = strtol(MSB_20, NULL, 2);

		int tlb_entry;

		if((tlb_entry = get_tlb_entry(MSB_20_int)) != -1)
		{
			if(policy == 1)
			{
				lru_hit(tlb_entry);
			}

			print_physical_address(tlb_entry, offset_int);
		}
		else
		{
			// if not exist, do virtual address translation
			// get base frame address
			int base_frame = translate_virtual_address(bin_line);
			if(policy == 1)
			replace_lru(MSB_20_int, base_frame);
			else
			populate_tlb(MSB_20_int, base_frame);

			print_physical_address(base_frame, offset_int);
		}
		free(bin);
	}
	print_tlb();
	printf("Hit rate of the cache is %f\n", get_hit_ratio());
	fclose(fp);
	free(line);
	//Free the page table
	free_resources();
	return 0;
}
