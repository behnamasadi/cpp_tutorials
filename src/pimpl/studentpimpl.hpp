#include <string>
#include <memory>

class Studentpimpl
{
private:
    class Impl;
    std::unique_ptr<Impl> pimpl;
public:

    Studentpimpl(int id, std::string name);
    Studentpimpl();
    
    ~Studentpimpl();
    std::string getName();
    int getID();
    void setName(std::string name);
    void setID(int ID);
    //copy constructor i.e. ==> Studentpimpl std1=std2; 
    Studentpimpl(const Studentpimpl &other);
    //assignment operator i.e. ==> std1=std2
    Studentpimpl& operator=(Studentpimpl  &rhs);
};
