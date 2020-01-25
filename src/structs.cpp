/*
 * Structs.cpp
 *
 *  Created on: Jan 4, 2012
 *      Author: behnam
 */
#include <iostream>
using namespace std;

/*
 * In C++ the only difference between a class and a struct is that class-members are private by default,
 *  while struct-members default to public. So structures can have constructors,
 *ref: http://www.yolinux.com/TUTORIALS/LinuxTutorialC++Structures.html
 * */

struct DataElement
{
    string SVal;
    int    iVal;
    bool   hasData;

    DataElement()   // Example of a constructor used in a structure.
    {
       iVal=-1;
       hasData=0;
    }
 };

struct Employee
{
    int nID;
    int nAge;
    float fWage;
    //structs can have function as well
    int add(int a, int b)
    {
    	return a+b;
    }
};

int Structsmain()
//int main()
{
//	initializer list. This allows you to initialize some or all the members of a struct at declaration time.
	Employee Joe = {1, 42, 60000.0f};
	Employee sJoe;
	sJoe.nID = 14;
	sJoe.nAge = 32;
	sJoe.fWage = 24.15;
	cout<<sJoe.add(12,12)<<endl;

	cout<<"sum up the bytes of two integers and one float=" <<sizeof(sJoe) <<endl;

	// Example of a constructor used in a structure.
	DataElement myDataElement;
	cout<<myDataElement.iVal<<endl;
	cout<<myDataElement.hasData<<endl;

//	example of  "->" operator, when we have pointer to struct or class, for accessing member we have to use -> operator
	DataElement RealData;
	DataElement *PointerToRealData=&RealData;
	PointerToRealData->iVal=1234;
	cout<<PointerToRealData->iVal<<endl;


	return 0;
}




