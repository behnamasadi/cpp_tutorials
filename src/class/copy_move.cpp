#include <iostream>
#include <cstring>
#include <istream>
#include <random>
#include <vector>


struct S
{
    S(){std::cout<<"constructor" <<std::endl;}
    ~S(){std::cout<<"deconstructor" <<std::endl;}


    /* copy constructor: it happens at  S s2 = s1; or  S s3(s2);*/
    S(const S& rhs){std::cout<<"copy constructor" <<std::endl;}

    /* assignment operator    s2 = s1;*/
    // Use *this from within a function of a class to get a reference to the current object
    S& operator = (const S& rhs){std::cout<<"copy assignment operator" <<std::endl;}


    /* it happens at: S s2(std :: move(s1));    or   S s2 = std :: move(s1) */
    S(S&& rhs){std::cout<<"move constructor" <<std::endl;}

    /* it happens at: s1 = std :: move(s2); */
    S& operator = (S&& rhs){std::cout<<"move assignment operator" <<std::endl;}



};




class money
{
public:
    int size=10;
    int value;
    int *data;

money(int value=10)
{
    this->value=value;
    std::cout<< "Constructor money with value: "<< this->value<<std::endl;

    data=new int [size];

    int upperBound=20;
    int lowerBound=0;
    int numberOfRandomNumbers=size;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(lowerBound, upperBound);

    for (int n=0; n<numberOfRandomNumbers; ++n)
       data[n]= dis(gen);
};


/***************************************Copy Constructor***************************************/

/*
Copy constructor: money m2 = m1; or money m3(m2); */

money(const money &rhs)
{
    std::cout<<"Copy constructor called "<<std::endl;
    //rhs is pointing to the object that we want to copy from
    data=new int [size];
    //memcpy(dst,src,size)
    memcpy(data,rhs.data,size*sizeof(rhs.data[0]) );
    value=rhs.value;
}

/*
second implementation
money(const money &rhs)
{
   //rhs is pointing to the object that we want to copy from
   data=new int [10];
   memcpy(this, &rhs,sizeof(rhs));
}


if we want disable copy constructor called
money(const money &other)= delete;
*/


/***************************************Move Constructor***************************************/

money(money &&rhs)// rvalue refrence
{
    std::cout<<"move constructor called "<<std::endl;
    //rhs is pointing to the object that we want to copy from
    data=rhs.data;
    value=rhs.value;
    rhs.data=nullptr;
}

/***************************************Copy Assignment operator***************************************/
money& operator = (const money &rhs)
{
    std::cout<<"Copy assignment operator called "<<std::endl;
    memcpy(data,rhs.data,size*sizeof(rhs.data[0]) );
    value=rhs.value;
    return *this;
}


/***************************************Move Assignment operator***************************************/

/* In C++0x, the compiler chooses between the copy constructor and the move constructor based on whether the argument to the assignment operator is an lvalue or an rvalue.
*/

money& operator=(money &&rhs)
{
    std::cout<<"Move assignment operator called "<<std::endl;
    std::swap(data, rhs.data);
    return *this;
}

~money()
{
    //std::cout<< "Destructor money with value: "<< this->value <<std::endl;
    delete []data;
}

friend std::ostream& operator<<(std::ostream& os, const money& t);
friend money operator + (const money &t1,const money &t2);
};

std::ostream& operator<<(std::ostream& os, const money& t)
{
    for(int i=0;i<t.size;i++)
        os << t.data[i]<<" ";
    return os;
}

money operator + (const money &t1,const money &t2)
{
    std::cout<<"+ operator called "<<std::endl;

    money m=money(t1.value+t2.value);
    for (int i=0;i<m.size;i++  )
        m.data[i]=t1.data[i]+t2.data[i];
    return m;
}




struct nonmovable
{
    nonmovable() = default;
    nonmovable(const nonmovable&) { std::cout << "copy\n"; }
    nonmovable& operator = (const nonmovable&) { std::cout << "asign\n"; return *this; }
    nonmovable(nonmovable&&) = delete;
    nonmovable& operator = (nonmovable&&)  = delete;
};

struct noncopyable
{
    noncopyable() = default;
    // Deletion of copy constructor and copy assignment makes the class
    // non-movable, too.
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator = (const noncopyable&) = delete;

    // Move construction and move assignment must be enabled explicitly, if desiered.
    noncopyable(noncopyable&&) = default;
    noncopyable& operator = (noncopyable&&)  = default;
};

struct X : nonmovable {};

void non_copy_non_moveable()
{

    nonmovable n0;
    nonmovable n1(n0);

    // error: use of deleted function ‘nonmovable::nonmovable(nonmovable&&)’:
    //nonmovable n2(std::move(n0));

    X x0;
    X x1(x0);
    // However, X has a copy constructor not applying a move.
    X x2(std::move(x0));

}


int main()
{

    std::cout<< "===========================Assignment operator======================"<<std::endl;
    money  money3(3);
    money  money4(4);
    std::cout<<"money3 is: " <<money3 <<std::endl;
    std::cout<<"money4 is: " <<money4 <<std::endl;

    money4=money3;
    std::cout<<"after money4=money3, money4 is: " <<money4 <<std::endl;


    std::cout<< "===========================Copy operator======================"<<std::endl;

    money  money5=money3;
    money3.data[0]=-1;
    money5.data[2]=-2;
    std::cout<< "money5 after updating money5.data[2]=-2 : "<< money5<<std::endl;
    std::cout<< "money3 after updating money3.data[0]=-1 : "<<money3 <<std::endl;


    money  money6(money5);
    money5.data[0]=-3;
    money6.data[2]=-4;
    std::cout<< "money5 after updating money5.data[2]=-3 : "<< money5<<std::endl;
    std::cout<< "money6 after updating money3.data[0]=-4 : "<<money6 <<std::endl;

    std::cout<< "===========================move constructor======================"<<std::endl;
    money  money12(std::move(money(12)+money(45)));


    std::cout<< "===========================move assignment======================"<<std::endl;
    money money20;
    money20=std::move(money(45));

}


