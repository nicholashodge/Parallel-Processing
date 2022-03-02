#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <unistd.h>

double evalfunc(int nodeid, int *expandid1, int *expandid2);

struct node {

	int id;
	struct node *next;

};

int main(int argc, char *argv[]){

	int expandid1, expandid2, nThreads, nSolutions, s;
	double rc;
	struct node *currnode, *tofree, *tailnode, *expandnode1, *expandnode2;

	currnode = (struct node *) malloc(sizeof(struct node));
	currnode->id = 0;
	currnode->next = NULL;
	tailnode = currnode;

	nThreads = atoi(argv[1]);
	nSolutions = atoi(argv[2]);
	s = 0;

	#pragma omp parallel shared(s) num_threads(nThreads)
	{

		int threadid;
	        threadid = omp_get_thread_num();

		alarm(300);
		while(currnode && !(s >= 4 || s >= nSolutions)){
			
			#pragma sections
			{
			rc = evalfunc(currnode->id, &expandid1, &expandid2);
			if(rc > 1.0){

				printf("answer: %d %lf\n", currnode->id, rc);
				s++;

			}
			#pragma section
			{
				if(expandid1 >= 0){

					expandnode1 = (struct node *) malloc(sizeof(struct node));
					expandnode1->id = expandid1;
					expandnode1->next = NULL;

				}
				else
				{
					expandnode1 = NULL;
				}

			}
			#pragma section
			{
				if(expandnode2 >= 0){

					expandnode2 = (struct node *) malloc(sizeof(struct node));
					expandnode2->id = expandid2;
					expandnode2->next = NULL;

				}
				else
				{
					expandnode2 = NULL;
				}
			}

			if(expandnode1)
			{

				tailnode->next = expandnode1;
				tailnode = expandnode1;

			}
			if(expandnode2)
			{

				tailnode->next = expandnode2;
				tailnode = expandnode2;

			}

			tofree = currnode;
			currnode = currnode->next;
			free(tofree);
			}

		}

	}

}
