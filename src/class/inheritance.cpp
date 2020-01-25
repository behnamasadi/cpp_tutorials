/*
 * Inheritance.cpp
 *
 *  Created on: Feb 29, 2012
 *      Author: behnam
 */

class Base
{
public:
    int m_nPublic; // can be accessed by anybody
private:
    int m_nPrivate; // can only be accessed by Base member functions (but not derived classes)
protected:
    int m_nProtected; // can be accessed by Base member functions, or derived classes.
};
//http://www.learncpp.com/cpp-tutorial/115-inheritance-and-access-specifiers/
//There are three different ways for classes to inherit from other classes: public, private, and protected.
//If you do not choose an inheritance type, C++ defaults to private inheritance

/* Inherit from Base publicly
 Public inheritance is by far the most commonly used type of inheritance.
class Pub: public Base
{
};

// Inherit from Base privately
class Pri: private Base
{
};

// Inherit from Base protectedly
class Pro: protected Base
{
};

class Def: Base // Defaults to private inheritance
{
};
*/
class Derived: public Base
{
public:
    Derived()
    {
        // Derived's access to Base members is not influenced by the type of inheritance used,
        // so the following is always true:

        m_nPublic = 1; // allowed: can access public base members from derived class
        //m_nPrivate = 2; // not allowed: can not access private base members from derived class
        m_nProtected = 3; // allowed: can access protected base members from derived class
    }
};


int Inheritancemain()
//int main()
{
    Base cBase;
    cBase.m_nPublic = 1; // allowed: can access public members from outside class
    //cBase.m_nPrivate = 2; // not allowed: can not access private members from outside class
    //cBase.m_nProtected = 3; // not allowed: can not access protected members from outside class


}
