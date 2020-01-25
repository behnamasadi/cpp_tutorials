#include <iostream>
#include <cstdlib>// for  exit()

/*
return:
    retrun is an instruction of the language that returns from a function call.
exit:
    exit is a system call (not a language statement) that terminates the current process.
    exit() means you want to immediately stops the current process. It can be of some use for
    error management when we encounter some kind of irrecoverable issue that won't allow for your code
    to do anything useful anymore. It is often handy when the control flow is complicated and error codes has to be propagated all way up. But be aware that this is bad coding practice. Silently ending the process is in most case the worse behavior and actual error management should be preferred (or in C++ using exceptions).

*/

class foo
{
    public:
    foo(){std::cout<<"foo constructor" <<std::endl;}

    ~foo(){std::cout<<"foo destructor" <<std::endl;}
};


int retunExample()
{
/*
    if run the program and then
    echo $?
    it wil give you your return value (5) and also we will see the the destructor
*/
    foo fooObject;
    return 5;
}

int exitExample()
{
/*
    if run the program and then
    echo $?
    it wil give you (7) but the destructor won't be called.
*/
    foo fooObject;
    exit(7) ;

}

void abortExample()
{
/*     
    abort sends a SIGABRT signal, exit just closes the application performing normal cleanup.
 */ 
}

int main(int argc, char *argv[])
{

    return exitExample();
    //return retunExample();;
}


