/*
 * Unions.cpp
 *
 *  Created on: Feb 8, 2012
 *      Author: behnam
 *      ref: http://www.cplusplus.com/doc/tutorial/other_data_types/
 */

#include <iostream>
#include <bitset>

union mix_t 
{
    unsigned int ui; // 4 bytes
    struct 
    {
        unsigned short hi; // 2 bytes
        unsigned short lo; // 2 bytes
    } s;
    unsigned char uc[4];// 4x1 bytes
} ;


    

/*
 Union declaration
 
 Unions allow one same portion of memory to be accessed as different data types, since all of them are in fact the same location in memory.
 
 Each one with a different data type. Since all of them are referring to the same location in memory, the modification of one of the elements will affect the value of all of them. We cannot store different values in them independent of each other.
 
 One of the uses a union may have is to unite an elementary type with an array or structures of smaller elements. For example:
 
    union mix_t
 
 defines three names that allow us to access the same group of 4 bytes: mix.l, mix.s and mix.c and which we can use according to
 how we want to access these bytes, as if they were a single long-type data, as if they were two short elements or as an array of char elements, respectively. 
 
 I have mixed types, arrays and structures in the union so that you can see the different ways that we can access the data. For a little-endian system (most PC platforms), this union could be represented as:
     ---------------------    
 mix |    |    |    |    |
     ---------------------
             mix.i
      mix.s.hi   mix.s.hi
      c[0] c[1] c[2] c[4] 
 */

int main()
{
    
    mix_t mix;
    
    //00000001000000010000000100000001
    mix.uc[0]=1;
    mix.uc[1]=1;
    mix.uc[2]=1;
    mix.uc[3]=1;
    
    std::cout<< mix.ui <<std::endl;  
    std::cout<< std::bitset<8*sizeof(mix_t)>(mix.ui) <<std::endl;
    
    std::bitset<8*sizeof(mix_t)> union_bitset;
    union_bitset[0]=1;
    union_bitset[8]=1;
    union_bitset[16]=1;
    union_bitset[24]=1;
    std::cout<< union_bitset.to_ulong() <<std::endl;
    std::cout<< union_bitset.to_string() <<std::endl;
//     std::cout<<sizeof(unsigned int)  <<std::endl;
    return 0;
}


