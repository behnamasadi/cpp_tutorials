//https://akrzemi1.wordpress.com/2014/04/24/noexcept-what-for/
//https://stackoverflow.com/questions/10787766/when-should-i-really-use-noexcept
/*
noexcept
1)noexcept specifier:
This means if a function specified with noexcept it shouldn't throw exeption.
In the case of excpetion std::sterminate will be called

2)noexcept operator
It tests if a function noexcept specification evalute to true or false at complie time.
noexcept(some compile time expression) and this returns a boolan



when should we use it?
1)When using c++ functions in c
2)when c++ standard requires us.
*/

/*//////////////////////////// noexcept specifier ///////////////////////////*/
void func1() noexcept //equals to noexcept(true),this func can not throw exception
{

}

void func2() noexcept (false)//equals to not using noexcept, means this func can throw exception
{

}



int main()
{

}
