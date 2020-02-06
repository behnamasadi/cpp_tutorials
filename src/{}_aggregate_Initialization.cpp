#include <string>
#include <iostream>
#include <vector>
#include <map>


class student {
public:
    int id;
    std::string name;
    student(int id, std::string name) :id{ id }, name{ name } 
    {
        std::cout << "id :"<<id<< " name is: "<<name << std::endl; 
    }
};

class entity {
public:
    int x;
    std::string y;
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
    //Initializations of an arbitrary object using the constructor
    student first_student(10,"mumbo");
    student second_student { 11, "jumbo" };

    // Initializations of an arbitrary object using public attributes	
    entity entity1{ 2011,"name" };
    entity entity2 = { 2011,"name"};
}
