class foo
{
public:

/*
=0
C++ uses the special syntax = 0; to indicate pure virtual functions instead of adding a new keyword to the language 
*/  
    void virtual pureVirtualFunction()=0;
/*
=default
It means that you want to use the compiler-generated version of that function, so you don't need to specify a body.
*/
    ~foo()=default;
/*
= delete 
    It means that the compiler will not generate those constructors for you. This is only allowed on copy constructor and assignment operator
*/
    foo(const foo &t) = delete;
    foo& operator = (const foo &t)= delete ;
};

int main()
{
    //foo
}
