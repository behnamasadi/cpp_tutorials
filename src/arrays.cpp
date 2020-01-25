/*
 * Arrays.cpp
 *
 *  Created on: Jan 4, 2012
 *      Author: behnam
 */
#include <array>
#include <string>
#include <iterator>
#include <iostream>
#include <algorithm>

/*differences between arrays and pointers
what you declare in a parameter list is treated special. There are such situations where things don't make
 sense as a function parameter in C. These are

1)Functions as parameters
2)Arrays as parameters

size of an array dimension is part of the type in C (and an array whose dimension size isn't given has an incomplete type).

*/

void sendingArray(int arrayToPrint[],int size)
{
    std::cout<<"Print Array, in this function we send the size of the array as well"<<std::endl;
	for(int i=0;i<size;i++)
	{
        std::cout<<arrayToPrint[i]<<std::endl;
	}
}

void sendingArray(int arrayToPrint[5])
{
    std::cout<<"Print Array, in This function we have defined the size of the passed array in function definition"<<std::endl;
	for(int i=0;i<5;i++)
	{
        std::cout<<arrayToPrint[i]<<std::endl;
	}
}

void sendingMultidimensionalArray( int x, int y, int z ,int  arrayToPrint[][3][2])
{
	for(int i=0;i<x;i++)
	{
		for(int j=0;j<y;j++)
		{
			for(int k=0;k<z;k++)
			{
                std::cout<<"Row # "<<i <<std::endl;
                std::cout<<"Column # "<<j <<std::endl;
                std::cout<<"Vertex # "<<k <<std::endl;
                std::cout<<arrayToPrint[i][j][k] <<std::endl;
			}
		}
	}
}


int arraysmain()
{

	int FirstArray[] = { 1, 2, 3, 4, 5 };
    std::cout << "sizeof(FirstArray)="<<sizeof(FirstArray)<<std::endl; // prints 20 (5 elements * 4 bytes each)
	const int ArraySize = 5;


    std::cout<<"*FirstArray"<< *FirstArray<<std::endl;
    std::cout<<"*(FirstArray+2)"<< *(FirstArray+2)<<std::endl;

	//Iterators are similar to pointers
    std::cout<<"Iterators are similar to pointers"<<std::endl;
	for (int* p = FirstArray; p != FirstArray+ArraySize; p++)
	{
		//These are equal:
		//FirstArray=&FirstArray[0]
		//FirstArray+ArraySize=&FirstArray[ArraySize]
        std::cout << *p << std::endl;
	}


	int First2DArray[3][5] ={
	{ 1, 2, 3, 4, 5, }, // row 0
	{ 6, 7, 8, 9, 10, }, // row 1
	{ 11, 12, 13, 14, 15 } // row 2
	};

//	Two-dimensional arrays with initializer lists can omit (only) the first size dimension(the most left one):
//C does not have true multidimensional arrays. you can have arrays of arrays, so like
// FirstArray[] = { 1, 2, 3, 4, 5 }  which we didn't declare the dimension, we could have:
	int Second2DArray[][5] ={{ 1, 2, 3, 4, 5, },{ 6, 7, 8, 9, 10, },{ 11, 12, 13, 14, 15 }};


    sendingArray(FirstArray);
    sendingArray(FirstArray,5);
//	It can also be called:
//	SendingArray(&FirstArray[0],5);

	int Second3DArray[2][3][2] ={
			{{1, 2},{3, 4},{5, 6}},
			{{7, 8}, {9, 10}, {11, 12}}
	};
	//If you want to pass a multi-dimensional array to a function, the simplest way is to specify all of the array sizes
	//except the leftmost; e.g.,

    sendingMultidimensionalArray(2,3,2,Second3DArray);

	return 0;
}

void cppArrayExample()
{
//std::array is a container that encapsulates fixed size arrays.                                              // (not needed in C++11 after the revision and in C++14 and beyond)
    std::array<int, 3> a2 = {1, 2, 3};
    std::array<std::string, 2> a3 = { std::string("a"), "b" };

    // container operations are supported
    std::sort(a2.begin(), a2.end());
    std::reverse_copy(a2.begin(), a2.end(), std::ostream_iterator<int>(std::cout, " "));

    std::cout << '\n';


    //make_array
    //to_array
}

void twoDimentionalArray()
{
    int **array;
    array=new int *[3];
    array[0]=new int[2];
    array[1]=new int[4];
    array[2]=new int[3];

    array[0][0]=1;
    array[0][1]=0;

    array[1][0]=5;
    array[1][1]=4;

    std::cout<<array[1][1] <<std::endl;

    delete[] array[0];
    delete[] array[1];
    delete[] array[2];
    delete[] array;
}

int main(int argc, char *argv[])
{

    return 0;
}


