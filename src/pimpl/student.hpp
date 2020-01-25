#include <string>

#ifndef STUDENT_H
#define STUDENT_H
class Student
{
private:
    int ID;
    std::string name;
public:
    Student(int id, std::string name):ID(id),name(name){}
    std::string getName()
    {
        return name;
    }
    int getID()
    {
        return ID;
    }
    void setName(std::string name)
    {
        this->name=name;
    }
    void setID(int ID)
    {
        this->ID=ID;
    }
};
#endif
