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


// this one expect exactly 3x4
void foo(int a[4][3])
{
}


//we can omit (only) the first size dimension(the most left one)
void foo(int a[][3], int size)
{
}

void foo(int *a[10], int size)
{
}

void foo(int **a, int m, int n)
{
}

void sendingArraytoFunctions()
{
    int array[2][3];
    foo(array,3);


    int *p_array[10];
    for(int i = 0; i < 10; i++)
        p_array[i] = new int[4];
    foo(p_array,4);


    int **pp_array;
    int m,  n;

    m=4;
    n=5;

    pp_array = new int *[n];
    for(int i = 0; i <m; i++)
        pp_array[i] = new int[n];

    int first2DArray[3][5] ={
    { 1, 2, 3, 4, 5, }, // row 0
    { 6, 7, 8, 9, 10, }, // row 1
    { 11, 12, 13, 14, 15 } // row 2
    };

    foo((int**)&first2DArray[0][0], 3, 5);

}


void accessingArrayElements()
{
    int array[] = { 1, 2, 3, 4, 5 };
    std::cout << "sizeof(array)="<<sizeof(array)<<std::endl; // prints 20 (5 elements * 4 bytes each)
    const int number_of_elemenets = sizeof(array)/sizeof(int);
    std::cout << "number of elemenets = "<<number_of_elemenets<<std::endl;

    //These are equal:
    //array<=>&array[0]
    //array+index<=>&array[index]
    std::cout<<"*array="<< *array<<std::endl;
    std::cout<<"*(array+2)="<< *(array+2)<<std::endl;

    for (int* p = array; p != array+number_of_elemenets; p++)
    {
        std::cout << *p << std::endl;
    }
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

}



struct Foo {};

void checkingIsArray()
{
    const char *s  = "Behnam";
    std::cout << std::boolalpha;
    std::cout << std::is_array<Foo>::value << '\n';
    std::cout << std::is_array<Foo[]>::value << '\n';
    std::cout << std::is_array<Foo[3]>::value << '\n';
    std::cout << std::is_array<float>::value << '\n';
    std::cout << std::is_array<int>::value << '\n';
    std::cout << std::is_array<int[]>::value << '\n';
    std::cout << std::is_array<int[3]>::value << '\n';
    std::cout << std::is_array<std::array<int, 3>>::value << '\n';


}

int main(int argc, char *argv[])
{
    checkingIsArray();
    return 0;
}


