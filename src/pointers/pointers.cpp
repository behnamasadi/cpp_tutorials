/*
 * pointers.cpp
 *
 *  Created on: Jan 4, 2012
 *      Author: behnam
 */

#include <iostream>
#include <memory>
#include <vector>
#include <string>

class person
{
public:
    std::string name;
    int id;
    person()
    {
        std::cout<<"person constructed" <<std::endl;
        id=-1;
    }
    person(int id)
    {
        this->id=id;
        std::cout<<"person with id "<< id<<" constructed" <<std::endl;
    }
    ~person()
    {
        std::cout<< ((id>-1)? std::string("person with ") + std::to_string(this->id)  +std::string(" destructed"):"person destructed"   )<<std::endl;
    }




};

class employee: public person
{
public:
    int id;
};

class manager:public person
{
public:
    std::vector<int> listOfDepartments;
};

void rawPointerExample()
{


/*	here we define a pointer variable, it is located somewhere in the memory that we don't know the value of that,
    it has some random value
*/
    int *pnPtr;
    // is a new keyword introduced in C++11. nullptr is meant as a replacement to NULL
    pnPtr=nullptr;

	int nValue = 5;
/*	here during compliation time, nValue get address and we assign  that address to our pointer variable */
	pnPtr = &nValue;
    std::cout<<"pnPtr is:" <<pnPtr<<std::endl;
    std::cout<<"*pnPtr is:" <<*pnPtr<<std::endl;


/*
    The size of a pointer is dependent upon the architecture of the computer — a 32-bit computer uses 32-bit memory
    addresses — consequently, a pointer on a 32-bit machine is 32 bits (4 bytes). On a 64-bit machine, a pointer
    would be 64 bits (8 bytes).this is true regardless of what is being pointed to
*/


    std::cout<<"sizeof(char *)= " << sizeof(char *)<<std::endl;
    std::cout<<"sizeof(int *)= " <<sizeof(int *)<<std::endl;
    std::cout<<"sizeof(void *)= " <<sizeof(void *)<<std::endl;
    std::cout<<"sizeof(double *)= " <<sizeof(double *)<<std::endl;
    std::cout<<"sizeof(person *)= " <<sizeof(person *)<<std::endl;
    std::cout<<"sizeof(employee *)= " <<sizeof(employee *)<<std::endl;


    manager *coe=new manager;
    delete coe;

    employee* postEmployees=new employee[10];
    std::cout<<"sizeof(postEmployees[0])= " <<sizeof(postEmployees[0])<<std::endl;
    delete[] postEmployees;



    return;
}

void weakPointerExample()
{
//weak pointer, it doens't increase the ref count

std::shared_ptr<person> person0;
{
    std::shared_ptr<person> sharedEntity0(new person(0));
    person0=sharedEntity0;
}
std::cout<<"sharedEntity0 is still alive because of person0=sharedEntity0" <<std::endl;

//if we change the above code to the following the object will die after the {} block

std::weak_ptr<person> person1;
{
    std::shared_ptr<person> sharedEntity1(new person(1));
    person1=sharedEntity1;
}
std::cout<<"sharedEntity1 died befor reaching here since person1 is a weak_ptr" <<std::endl;
}

std::unique_ptr<person> createPersonUnique_ptr()
{
    std::unique_ptr<person> entity=std::make_unique<person>();
    return entity;
}

void uniquePointerExample()
{
/*  unique pointer, they have less overhead
    Unique pointer has no copy constructor Cannot be copied, can be moved*/

    {

        //first way
        //std::unique_ptr<person> entity=std::unique_ptr<person>(new person);

        //second way(recommended) for exception safety
        std::unique_ptr<person> entity=std::make_unique<person>();

        //this will fail because it is a unique pointer, it can not be copied
        //std::unique_ptr<person> secondentity=entity;

        //this will work because because we have move constructor
        std::unique_ptr<person> secondentity=std::move(entity);
    }

    //this will work because because we have move constructor
    std::unique_ptr<person> secondentity=createPersonUnique_ptr();


}

void sharedPointerExample()
{
    //one line
    std::shared_ptr<manager> ceo=std::shared_ptr<manager>(new manager);
    std::shared_ptr<manager> ceo1(new manager);


    //multi line:
    std::shared_ptr<manager> cto;
    cto.reset(new manager);

    //prefered way
    std::shared_ptr<manager> boss=std::make_shared<manager>();


/*  when  you erase items in a vector the occupied space will be freed, but if you have stored pointers
 inside a vector you have to manualy call delete function to free the space.

 The destructor for std::vector<T> ensures that the destructor for T is called for every element stored in the vector.
*/

// (A) create a container of CSample pointers:
    std::vector<std::shared_ptr<manager> > vecOfCTOs;

// (B) add two elements
    std::cout<<"adding three elements to a vector...\n"<<std::endl;
    vecOfCTOs.push_back(std::shared_ptr<manager>(new manager));
    vecOfCTOs.push_back(std::shared_ptr<manager>(new manager));
    vecOfCTOs.push_back(std::shared_ptr<manager>(new manager));


    vecOfCTOs[0]->name="first CTO";
    vecOfCTOs[1]->name="second CTO";
    vecOfCTOs[2]->name="third CTO";

// (C) "keep" a pointer to the second:
    std::cout<<"keeping a reference to the second...\n"<<std::endl;
    std::shared_ptr<manager> anElement = vecOfCTOs[1];

// (D) destroy the vector:
    std::cout<<"destroying the vector releases it's elements...\n"<<std::endl;
    vecOfCTOs.clear();

// (E) the second element still exists
    std::cout<<"referenced elements still exist, however...\n"<<std::endl;
    std::cout<<anElement->name<<std::endl;
    std::cout<<"done. cleanup is automatic\n"<<std::endl;
}

void makesharedExampel()
{
    //shared pointer from normal pointer and exsiting variables

    int *a=new int;
    std::shared_ptr<int> a_ptr(a);



    int b=9;
    std::shared_ptr<int> b_ptr=std::make_shared<int>(b);
}

void pointerCasting()
{
//    std::shared_ptr<Base> b(new Base());
//    std::shared_ptr<Derived> d = std::static_pointer_cast<Derived>(b);

    std::shared_ptr<person> b(new person());
    std::shared_ptr<manager> d = std::static_pointer_cast<manager>(b);
}

void callByRef(std::shared_ptr<person> &sp)
{
    std::cout << "callByRef: sp.use_count() == " << sp.use_count() << '\n';
}

void callByVal(std::shared_ptr<person> sp)
{
    std::cout << "callByVal: sp.use_count() == " << sp.use_count() << '\n';
}

std::weak_ptr<int> gw;

void observe()
{
    std::cout << "use_count == " << gw.use_count() << ": ";
    if (auto spt = gw.lock()) { // Has to be copied into a shared_ptr before usage
    std::cout << *spt << "\n";
    }
    else {
        std::cout << "gw is expired\n";
    }
}

void Weak_ptrLockVsExpiered()
{
    /*
    lock vs expiered:
    https://stackoverflow.com/questions/37679636/what-is-diffrence-between-lock-and-expired-weak-ptr-c
    https://www.internalpointers.com/post/beginner-s-look-smart-pointers-modern-c
    */
    {
        auto sp = std::make_shared<int>(42);
        gw = sp;
        observe();
    }
    observe();

    {
        std::shared_ptr<person> sp1(new person);// = std::make_shared<int>(5);
        std::cout << "sp1.use_count() == " << sp1.use_count() << '\n';
        callByRef(sp1);
        callByVal(sp1);
    }

    {
        std::shared_ptr<person> ptr1 (new person());
        std::cout<<"Use Count: "<<ptr1.use_count()<<std::endl; // 1
        std::shared_ptr<person> ptr2(ptr1);
        std::cout<<"Use Count: "<<ptr2.use_count()<<std::endl; // 2
        std::cout<<"Use Count: "<<ptr1.use_count()<<std::endl; // 2
        std::shared_ptr<person> ptr3(ptr2);
        std::cout<<"Use Count: "<<ptr3.use_count()<<std::endl; // 3
    }


    {
        std::shared_ptr<person> ptr1(new person);
        std::cout<<"Use Count: "<<ptr1.use_count()<<std::endl; // 1
        std::weak_ptr<person> weakt_ptr1=ptr1;
        std::cout<<"Use Count: "<<ptr1.use_count()<<std::endl; // 1
        std::shared_ptr<person> ptr2=ptr1;
        std::cout<<"Use Count: "<<ptr1.use_count()<<std::endl; // 2
    }
}

void pointerVSReference()
{
/*
basically pointers can be reassigned to different location in memory but references can only
stick to one variable.

reference is like an alias for an existing variable. is shares the same address as the original variable.
also a refferece can not be null.

pointer is a variable that store the address of an other avriable.
*/


    int foo=2;
    int &ref=foo;
    int *ptr=&foo;

    std::cout<<"ref is: " << ref <<" &ref is: " << &ref<<" &foo is :"  <<&foo<< " *ptr is: "<<*ptr  <<std::endl;

    int var=10;
    ref=var;
    // since we updated ref and rf is a ref to foo, foo will also change into 10
    std::cout<<"ref is: " << ref <<" var is: " << var<< " foo is "<< foo<<std::endl;

    //this will not effect foo
    int new_foo=20;
    ptr=&new_foo;
    *ptr=30;
}

void addressofExample()
{
}

void reference_wrapperExample()
{
    std::vector<std::reference_wrapper<std::unique_ptr<person>>> people;
    std::unique_ptr empolyee1= std::make_unique<person>();
    people.push_back(empolyee1 );


//    std::vector<const person&> m_people{};

}


person* getperson()
{
    std::unique_ptr<person> person_ptr = std::make_unique<person>();
    return person_ptr.release();
}

int main()
{
    std::cout<<"=================== raw pointer example ===================" <<std::endl;
    pointerVSReference();

    std::cout<<"=================== raw pointer example ===================" <<std::endl;
    rawPointerExample();

    std::cout<<"=================== shared pointer example ===================" <<std::endl;
    sharedPointerExample();

    std::cout<<"=================== unique pointer example ===================" <<std::endl;
    uniquePointerExample();

    std::cout<<"=================== weak pointer example ===================" <<std::endl;
    weakPointerExample();

    std::cout<<"=================== weak pointer lock vs expiered ===================" <<std::endl;
    Weak_ptrLockVsExpiered();

}


