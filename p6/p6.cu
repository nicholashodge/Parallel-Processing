//Nicholas Hodge

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CUDA_CALL(x){ \
	if ( (x) != cudaSuccess ) { \
		printf("CUDA error at %s:%d\n",__FILE__,__LINE__); \
		printf("  %s\n", cudaGetErrorString(cudaGetLastError())); \
		exit(EXIT_FAILURE); \
	} \
}

__global__
void func(double *result, double *input, double *weights, int in){
	
	float calc = 0.0;
	int id = blockIdx.x * blockDim.x + threadIdx.x;

	//apply weights to inputs
	for(int i = 0; i < in; i++){

		calc += input[i] * weights[id + i];

	}

	calc = (1.0)/(1.0 + exp(-calc));

	result[id] = calc;


}


int main(int argc, char *argv[]){

	int fd;
	double fl;
	double *result, *input, *weights;
	double *dev_results, *dev_input, *dev_weights;
	int rc, numBlocks, numThreads, numInputs, numNeurons, numWeights;


	fd = open(argv[1], O_RDONLY);
	if(fd < 0){

		printf("**** failed to open file :%s:\n", argv[1]);
		exit(-1);

	}


	//get numNeurons and numInputs
	rc = read(fd, &fl, sizeof(double));
	numNeurons = (int) fl;
	rc = read(fd, &fl, sizeof(double));
	numInputs = (int) fl;

	//get inputs
	input = (double *)malloc(numInputs * sizeof(double));
	rc = read(fd, input, numInputs * sizeof(double));

	//get weights
	numWeights = numInputs * numNeurons;
	weights = (double *)malloc(numWeights * sizeof(double));
	rc = read(fd, weights, numWeights * sizeof(double));

	close(fd);

	if((numInputs * numNeurons) < 32){

                numThreads = numInputs * numNeurons;

        }else{

                numThreads = 32;

        }


	numBlocks = numWeights/numThreads;
	if(numBlocks == 0){
		numBlocks = 1;
	}

	result = (double *)malloc(numNeurons * sizeof(double));

	CUDA_CALL(cudaMalloc((void**) &dev_results, numNeurons * sizeof(double)));
	CUDA_CALL(cudaMalloc((void**) &dev_input, numInputs * sizeof(double)));
	CUDA_CALL(cudaMalloc((void**) &dev_weights, numWeights * sizeof(double)));


	CUDA_CALL(cudaMemcpy(dev_results, result, numNeurons * sizeof(double), cudaMemcpyHostToDevice));
	CUDA_CALL(cudaMemcpy(dev_input, input, numInputs * sizeof(double), cudaMemcpyHostToDevice));
	CUDA_CALL(cudaMemcpy(dev_weights, weights, numWeights * sizeof(double), cudaMemcpyHostToDevice));


	func<<<numBlocks, numThreads>>>(dev_results, dev_input, dev_weights, numInputs);


	CUDA_CALL(cudaMemcpy(result, dev_results, numNeurons * sizeof(double), cudaMemcpyDeviceToHost));

	

	if(argc < 3){

		for(int i = 0; i < numNeurons; i++){

			printf("%d  %f\n", i, result[i]);

		}
	}



	double total = 0;
	for(int i = 0; i < numNeurons; i++){

		total+= result[i];

	}

	printf("total %f\n", total);

	free(input);
	free(weights);
	free(result);
	cudaFree(dev_results);
	cudaFree(dev_input);
	cudaFree(dev_weights);

}
