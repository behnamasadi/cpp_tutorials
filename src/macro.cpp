/*
 * MacroExample.cpp
 *
 *  Created on: Jan 4, 2012
 *      Author: behnam
 */
#include <iostream>

/*
1)First Example
*/
#define SOFTWARE "Software Version= "
#define VERSION 10

/*
2)in the CMakeLists.txt ADD_DEFINITIONS(-DLOGING=0) or ADD_DEFINITIONS(-DLOGING=1)
*/

#if LOGING==1
    #define LOG(x) std::cout<< x <<" has happened" <<std::endl
#else
    #define LOG(x)
#endif

/*
3)Since the macro will replace the text where they used, they are like inline functions and they will speed up code.
If you have multi line put a \
*/

#define ADD(a, b){\
    (a + b);\
    }

#define SWAP(a, b) {\
    (a) ^= (b);\
    (b) ^= (a);\
    (a) ^= (b);\
    }


int main(int argc, char *argv[])
{

    std::cout << SOFTWARE  << VERSION << std::endl;

    int x,y;
    x=2;
    y=3;
    ADD(x,y);
    SWAP(x,y);

    LOG("loading");
    return 0;
}




