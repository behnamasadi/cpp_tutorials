#include <iostream>
#include <string>
#include "studentpimpl.hpp"
#include "student.hpp"


int main()
{
    
    std::cout<<"============Student============" <<std::endl;
    Student student(34,"Behnam");

    std::cout<<student.getID() <<std::endl;
    std::cout<<student.getName() <<std::endl;
    student.setID(12);
    student.setName("Bamboo");
    std::cout<<student.getID() <<std::endl;
    std::cout<<student.getName() <<std::endl;
    
    std::cout<<"============Studentpimpl============" <<std::endl;

    Studentpimpl studentpimpl(34,"Behnam");
    std::cout<<studentpimpl.getID() <<std::endl;
    std::cout<<studentpimpl.getName() <<std::endl;
    studentpimpl.setID(12);
    studentpimpl.setName("Bamboo");
    std::cout<<studentpimpl.getID() <<std::endl;
    std::cout<<studentpimpl.getName() <<std::endl;
    
    std::cout<<"============Studentpimpl Copy Constructor============" <<std::endl;
    Studentpimpl studentpimpl1= studentpimpl;
    std::cout<<studentpimpl.getID() <<std::endl;
    std::cout<<studentpimpl.getName() <<std::endl;
    
    std::cout<<studentpimpl1.getID() <<std::endl;
    std::cout<<studentpimpl1.getName() <<std::endl;
    
    std::cout<<"============Studentpimpl Assignment============" <<std::endl;
    Studentpimpl studentpimpl2(35,"Mumbo");
    std::cout<<studentpimpl2.getID() <<std::endl;
    std::cout<<studentpimpl2.getName() <<std::endl;
    studentpimpl2=studentpimpl;
    std::cout<<studentpimpl2.getID() <<std::endl;
    std::cout<<studentpimpl2.getName() <<std::endl;
    
} 

