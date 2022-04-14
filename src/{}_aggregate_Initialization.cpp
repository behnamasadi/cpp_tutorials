#include <string>
#include <iostream>
#include <vector>
#include <map>

class entity {
public:
    int x;
    std::string y;
};

class student {
public:
    int id;
    std::string name;
    student(int id, std::string name) :id{ id }, name{ name } 
    {
        std::cout << "id :"<<id<< " name is: "<<name << std::endl; 
    }
};

class intArray
{
public:
	int size;
	int * data;

	intArray(int size)
	{
		size = size; 
		data = new int[size];
	}

	~intArray() 
	{
		delete[] data;
	}

	intArray(std::initializer_list<int> list)
	{
		size = list.size();
		data = new int[list.size()];

		//std::initializer_list doesn't provide a subscript operator

		int count{ 0 };
		for (auto element : list)
		{
			data[count] = element;
			++count;
		}

	}
};

int main() 
{

/*
Aggregate Initialization:
It means the use of brace-enclosed initializer lists to initialize all members of an aggregate
(i.e. an array or struct). This type of initialization safe guards one from over stepping the boundary and provides for initializing an array with set values.
*/

    char Array1[5] = {};      // char Array2[5] = {'\0', '\0', '\0', '\0', '\0'}
    char* Array2[5] = {};     //char * Array3[5] = {nullptr, nullptr, nullptr, nullptr, nullptr};
    int i{};                // i becomes 0
    std::string s{};        // s becomes ""
    std::vector<float> v{}; // v becomes an empty vector
    double d{};             // d becomes 0.0

    std::vector<int> vec1{ 1,2,3,4,5 };
    std::map<std::string, int> myMap{ {"Scott",1976}, {"Dijkstra",1972} };

/*
    We can use that to initialize class member variables and constructors as well member initialization list.
*/
    //Initializations using the constructor
    student first_student(10,"mumbo");
    student second_student { 11, "jumbo" };

    // Aggregate initialization (Initializations using public attributes)	
    entity entity1{ 2011,"name" };
    entity entity2 = { 2011,"name"};


    /*
	initializer_list
	so far the only way to use our array is like this:
	*/

	intArray array1(4);
	array1.data[0] =7 ;
	array1.data[1] = 3;
	array1.data[2] = 5;
	array1.data[3] = 6;

	/*
	with initializer_list, we can send an array directly:
	*/


	intArray array2({ 1,2,3,4,5,6,7,8 });
	std::cout << array2.data[0] << std::endl;
	std::cout << array2.data[1] << std::endl;
	std::cout << array2.data[2] << std::endl;
}
