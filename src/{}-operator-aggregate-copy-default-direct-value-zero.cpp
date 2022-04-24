#include <string>
#include <iostream>
#include <vector>
#include <map>

class Foo
{
    public: int i;
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

int main() 
{



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


	
	
	



    std::cout<<"value initialization" <<std::endl;
    Foo* inst3 = new Foo();
    std::cout<<inst3->i <<std::endl;


    std::cout<<"copy-initialization" <<std::endl;
    Foo inst4 = Foo();


    std::cout<<inst4.i <<std::endl;

    std::cout<<"direct-initialization" <<std::endl;
    Foo inst5((Foo()));
    std::cout<<inst5.i <<std::endl;
	
	
}





