//Author: Nicholas Hodge
//Due Date: 3/31/21

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void main(int argc, char **argv){

	alarm(90);

	//"-hw" command line argument
	if (! strncmp(argv[1], "-hw", 3)){

		printf("hello world\n");
		exit(0);

	}

	int i, j, rc;
	int stdout_pipe_fds[2];
	char *args[argc + 1];
	char buff[1000];
	char* token;

	//fill in the arguments for the execve call
	args[0] = "/usr/bin/shuf";
	for(i = 1; i < argc; i++){

		args[i] = argv[i];

	}
	args[argc] = NULL;

	pipe(stdout_pipe_fds);
	if( (rc = fork()) == -1){
	
		printf("fork failed\n");
		exit(-1);

	//child
	}else if(rc == 0){

		alarm(90);

		close(stdout_pipe_fds[0]);

		dup2(stdout_pipe_fds[1], STDOUT_FILENO);
		close(stdout_pipe_fds[1]);

		execvp("/usr/bin/shuf", args);

		//if we reach this point there is an error
		perror("exec failed");
		exit(-1);

	}
	//parent
	
	close(stdout_pipe_fds[1]);

	j = 0;


	while((i = read(stdout_pipe_fds[0], buff, 1000)) > 0){

		buff[i] = 0;

		token = strtok(buff, "\n");

		while(token != 0){

			j++;
			printf("%d: %s\n", j, token);
			token =  strtok(0, "\n");

		}


	}

}
