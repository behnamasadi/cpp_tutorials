#include <stdlib.h>
#include <iostream>

int mallocExample()
{
	int * x=(int *)malloc(10 * sizeof(int));
	if(x==nullptr)
		exit(1);

	x[0]=12;
	x[1]=2;
	x[2]=4;
	x[3]=6;
	x[4]=27;
	x[5]=9;
	x[6]=11;
    std::cout<<x[6] <<std::endl;
	free(x);
	return 0;
}

int reallocExample()
{
	int * x=(int *)malloc(4 * sizeof(int));
	if(x==nullptr)
		exit(1);

	x[0]=12;
	x[1]=2;
	x[2]=4;
	x[3]=6;
	x=(int *)realloc(x,10 * sizeof(int));
	x[4]=14;
	x[5]=61;

	free(x);
	return 0;
}

int callocExample()
{
	//Allocates memory for an array of num objects of size size and zero-initializes it.
	int numberofElementofArray=10;
	int * x=(int *)calloc(numberofElementofArray,4 * sizeof(int));
	if(x==nullptr)
		exit(1);
	free(x);
	return 0;
}

int main(int argc, char *argv[])
{
    return 0;
}


