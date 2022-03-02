//Author: Nicholas Hodge
//Due Date: 03/01/21
//Project: p2

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

void Insertmmap(void *region, char *key, char *value, int size);
void Deletemmap(void *region, char *key, int size);
void PrintKey(void *region, char *key, int size);
void PrintAll(void *region, int size);

int main(int argc, char **argv){

	int pgsz, pgnum, pgtot;
	unsigned char *cp;
	void *region;
	char buff[37], buff2[32], k[16], v[16];
	FILE *fd;

	//"-hw" command line argument
	if(! strncmp(argv[1], "-hw", 3)){

		printf("hello world\n");
		exit(0);

	}

	//get the page size
	pgsz = getpagesize();

	//get the number of pages entered on cma
	pgnum = atoi(argv[1]);

	//get the total number of bytes
	pgtot = pgsz * pgnum;

	//initialize the region
	region = mmap(NULL, pgtot, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	if (region == ((void *) -1)){

		perror("mmap failed to initialize");
		return 1;

	}

	//initialize the entire page with null characters
	memset(region, ' ', pgtot);

	//attempt to open the file
	fd = fopen(argv[2], "r");

	while(fgets(buff, 37, fd)){

		//skip comments
		if(buff[0] == '#'){

			continue;

		}

		//insert command
		if(strncmp(buff, "isrt", 4)){

			//take the key and value from the buffer
			memcpy(k, &buff[5], 16);
			memcpy(v, &buff[21], 16);
			Insertmmap(region, k, v, pgtot);
								

		}

		//delete command
		if(strncmp(buff, "delt", 4)){

			memcpy(k, &buff[5], 16);
			Deletemmap(region, k, pgtot);

		}

		//print line command
		if(strncmp(buff, "prtt", 4)){

			memcpy(buff2, &buff[5], 32);
			printf("%s\n", buff2);

		}

		//print key command
		if(strncmp(buff, "prtk", 4)){

			memcpy(k, &buff[5], 16);
			PrintKey(region, k, pgtot);

		}

		//print all command
		if(strncmp(buff, "prta", 4)){

			PrintAll(region, pgtot);

		}

		//blank command
		if(strncmp(buff, "blnk",4)){

			printf("\n");

		}


	}

	//close the file when we are done with it	
	fclose(fd);

	return 0;

}

void Insertmmap(void *region, char *key, char *value, int size){

	//set the character pointer to the beginning of the mmap
	unsigned char *cp;
	unsigned char *location;
	cp = (unsigned char *) region;
	bool keyExists = false;
	bool firstEmpty = false;


	//since key/value pairs consist of 32 bits, increment by 32 to get to the start of the next node
	for(int i = 0; i < size; i += 32){

		//compare the current location in the mmap with the given key
		if(! strncmp(*(cp + i), key, 16)){

			// if they are identical, save their location
			location = *(cp + i);
			keyExists = true;

		}else{

			//save the first open spot to place the key if it is empty
			if(!keyExists){

				if(!firstEmpty){

					firstEmpty = true;
					location = *(cp + i);

				}
		
			}

		}

	}

	//if the key was never found, save the input key to the first available spot
	if(firstEmpty){

		*(location) = key;

	}

	//save the value too
	*(location + 16) = value;

	return;
}

void Deletemmap(void *region, char *key, int size){

	unsigned char *cp;
	cp = (unsigned char *) region;
	bool keyExists = false;

	for(int i = 0; i < size; i += 32){

		if(! strncmp(*(cp + i), key, 16)){

			//replace the key and value with a blank string
			keyExists = true;
			*(cp + i) = "                                ";

		}

	}

	if(! keyExists){

		printf("key does not exist\n");

	}

	return;

}

void PrintKey(void *region, char *key, int size){

	unsigned char *cp;
	cp = (unsigned char *) region;
	bool keyExists = false;

	for(int i = 0; i < size; i += 32){

		if(! strncmp(*(cp + i), key, 16)){

			keyExists = true;
			for(int i = 0; i < 32; i++){

				//print the newline at the end of the input and break
				if(! strncmp(*(cp + i), '\n', 1)){

					printf('\n');
					break;

				}
				printf("%c", *(cp + i));

			}

		}

	}

	if(!keyExists){

		printf("KEY DOES NOT EXIST %s\n", key);

	}

	return;

}

void PrintAll(void *region, int size){

	unsigned char *cp;
	cp = (unsigned char *) region;

	for(int i = 0; i < size; i += 32){

		if(strncmp(*(cp + i), "                ", 16)){

			for(int j = 0; j < 16; j++){

				//if there is a space, that means the key is done, so we stop reading for the key
				if(strncmp(*(cp + i + j), " ", 1)){

					break;

				}

				//if there is no space, print out the character
				printf("%c", *(cp + i + j));

			}

			//print the double space between key and value
			printf("  ");

			for(int j = 0; j < 16; j++){

				//if the value is done being input, quit reading this pair
				if(strncmp(*(cp + i + j), "\n", 1)){
				
					printf("\n");
					break;
				
				}

				//print out the value characters until we hit a newline
				printf("%c", *(cp + i + j));

			}

		}

	}

	return;

}
