/*
 * bitwiseshift.cpp
 *
 *  Created on: Sep 7, 2012
 *      Author: behnam
 */

#include <iostream>
#include <bitset>
/*

&	bitwise AND
|	bitwise inclusive OR
^	bitwise XOR (eXclusive OR)
<<	left shift 
>>	right shift
~	bitwise NOT (one's complement) (unary)
 
&=	bitwise AND assignment
|=	bitwise inclusive OR assignment
^=	bitwise exclusive OR assignment
<<=	left shift assignment
>>=	right shift assignment



Bitwise	Logical
a & b	a && b
a | b	a || b
a ^ b	a != b
~a	!a

 
*/


void bitwiseshiftPOW2(int number=2,int power=5)
{
//	[variable]<<[number of places to shift]
    
    int result=number<<(power-1);
	std::cout <<result<<std::endl;
}

void evenOdd(int num=6)
{
    int result=num&1;
    std::cout<<num << (result ? " is odd":" is even")<<std::endl;
}

void shiftBits()
{
    int a,b, result;
    a=13;//1101
    b=2;
    result=(a>>b);//0011
    std::cout <<result<<std::endl;
    
    //a >>= 2 means "set a to itself shifted by two bit to the right"
    
    std::cout <<(a >>= 2) <<std::endl;
}

//https://www.geeksforgeeks.org/bitwise-algorithms/

int main()
{
     //bitwiseshiftPOW2();
     //evenOdd(8);
    shiftBits();
}

