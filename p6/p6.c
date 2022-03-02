//Author: Nicholas Hodge
//Due Date: 4/28/21
//Project: p6

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

void main(int argc, char *argv[]){

	//"-hw" command line argument
	if(! strncmp(argv[1], "-hw", 3)){

		printf("hello world\n");
		exit(0);

	}

	void *handle;
	char *error;
	void (*func)(char *, int);
	int arg2 = atoi(argv[3]);

	//load the shared library
	handle = dlopen("/nfshome/rbutler/public/courses/cs/p6testfiles/libp6.so", RTLD_LAZY);
	if(!handle){

		fprintf(stderr, "%s\n", dlerror());
		exit(1);

	}

	//get a pointer to the function
	func = dlsym(handle, argv[1]);
	if((error = dlerror()) != NULL){

		fprintf(stderr, "%s\n", error);
		exit(1);

	}

	//call the function loaded
	func(argv[2], arg2);

	if(dlclose(handle) < 0){

		fprintf(stderr, "%s\n", dlerror());
		exit(1);

	}

	return;

}
