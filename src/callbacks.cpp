//https://stackoverflow.com/questions/2298242/callback-functions-in-c

#include <functional>
#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <string>
#include <memory>

/*
std::invoke is a generic way to activate any callable

std::invoke takes something callable, and arguments to call it with, and does the call. std::invoke( f, args... ) is a slight generalization of typing f(args...) that also handles a few additional cases.

*/




/*
1)Function pointers (including pointers to member functions)
2)std::function objects
3)Lambda expressions
4)Bind expressions
5)Function objects (classes with overloaded function call operator operator())
*/

/**************************** 1)Function Pointer ****************************/

namespace FunctionPointer
{

typedef int (*caclculator_fn_ptr)(int,int);

int adder(int x,int y)
{
    return x+y;
}

int subtractor(int x,int y)
{
    return x-y;
}

//int caclculator(int a, int b, int (*calcus_operation)(int,int)) is also correct
int caclculator(int a, int b, caclculator_fn_ptr calcus_operation)
{
    return calcus_operation(a,b);
}
}

/**************************** 2)Pointer to Member Function ****************************/
namespace PointertoMemberFunction
{

class mathOperations
{
public:
    int adder(int x,int y)
    {
       return x+y;
    }

    int subtractor(int x,int y)
    {
       return x+y;
    }
};

int calculator(int x, int y, mathOperations &c, int(mathOperations::*func_ptr)(int,int)  )
{
    return (c.*func_ptr)(x,y);
}

}

/**************************** 3)std::function ****************************/
namespace stdFunction
{

int calculator(int x,int y, std::function<int(int,int)> calcus_operation)
{
    return calcus_operation(x,y);
}

int calculator(int x,int y, PointertoMemberFunction::mathOperations &c, std::function<int(PointertoMemberFunction::mathOperations &,int,int)> calcus_operation)
{
    return calcus_operation(c,x,y);
}

class FunctionObject
{
    public:
    int operator()(int x, int y)
    {
        return x+y;
    }
};

}


/***************************** 4)Templated callback type ******************************/

/*
templates are a compile-time feature and are a design tool for compile-time polymorphism.
If runtime dynamic behaviour is to be achieved through callbacks, templates will help but they won't induce runtime dynamics.
*/

namespace TemplatedCallback
{
template<class R, class T>
void stdf_transform_every_int_templ(int * v,
  unsigned const n, std::function<R(T)> fp)
{
  for (unsigned i = 0; i < n; ++i)
  {
    v[i] = fp(v[i]);
  }
}

#if defined(__GNUC__) || defined(__GNUG__)
#include <cxxabi.h>
template <class T>
std::string type_name()
{
  typedef typename std::remove_reference<T>::type TR;
  std::unique_ptr<char, void(*)(void*)> own
    (abi::__cxa_demangle(typeid(TR).name(), nullptr,
    nullptr, nullptr), std::free);
  std::string r = own != nullptr?own.get():typeid(TR).name();
  if (std::is_const<TR>::value)
    r += " const";
  if (std::is_volatile<TR>::value)
    r += " volatile";
  if (std::is_lvalue_reference<T>::value)
    r += " &";
  else if (std::is_rvalue_reference<T>::value)
    r += " &&";
  return r;
}
#endif


#if defined(__GNUC__) || defined(__GNUG__)
template<class F>
void transform_every_int_templ(int * v,
  unsigned const n, F f)
{
  std::cout << "transform_every_int_templ<"
    << type_name<F>() << ">\n";
  for (unsigned i = 0; i < n; ++i)
  {
    v[i] = f(v[i]);
  }
}
#endif

int foo (int x) { return 2+x; }
int muh (int const &x) { return 3+x; }
int & woof (int &x) { x *= 4; return x; }

void print_int(int * p, unsigned const n)
{
  bool f{ true };
  for (unsigned i = 0; i < n; ++i)
  {
    std::cout << (f ? "" : " ") << p[i];
    f = false;
  }
  std::cout << "\n";
}


struct Meow
{
  int y = 0;
  Meow(int y_) : y(y_) {}
  int operator()(int x) { return y * x; }
};

}

// a function object class:
struct third_t {
  int operator()(int x) {return x/3;}
};

int main()
{

/*************************** 1)Function Pointer ****************************/
    {
        int a=10;
        int b=12;
        int c;

        // first way
        int(*adder_fn_ptr)(int,int) =FunctionPointer::adder;//int(*adder_fn_ptr)(int,int) =&adder; is also okay
        c=FunctionPointer::caclculator(a, b, adder_fn_ptr );
        std::cout<<c <<std::endl;


        // second way

        FunctionPointer::caclculator_fn_ptr subtractor_fn_ptr=FunctionPointer::subtractor;
        c=FunctionPointer::caclculator(a, b, subtractor_fn_ptr );
        std::cout<<c <<std::endl;


        // third way

        using f_int_t = int(*)(int,int);
        f_int_t an_other_subtractor_fn_ptr=FunctionPointer::subtractor;
        c=FunctionPointer::caclculator(a, b, an_other_subtractor_fn_ptr);
        std::cout<<c <<std::endl;

    }

/**************************** 2)Pointer to member function ****************************/

    {
        int a=10;
        int b=12;
        int c;
        PointertoMemberFunction::mathOperations my_mathOperations;
        //int (calc::*)(int,int) adder_fn_ptr;
        c=PointertoMemberFunction::calculator(a,b ,my_mathOperations, &PointertoMemberFunction::mathOperations::adder );

        std::cout<<c <<std::endl;

    }


/**************************** 3) std::function ****************************/
//The std::function class is a polymorphic function wrapper to store, copy or invoke callables.


    {
        int a=10;
        int b=12;
        int c;

        std::function <int(int,int)> std_func_ptr_adder=FunctionPointer::adder;
        std::function<int (PointertoMemberFunction::mathOperations &C,int,int  )> std_func_class_member_adder=&PointertoMemberFunction::mathOperations::adder;

        //std::function

        c=stdFunction::calculator(a,b,std_func_ptr_adder);
        std::cout<<c <<std::endl;



        //function pointer
        c=stdFunction::calculator(a,b,&FunctionPointer::adder);
        std::cout<<c <<std::endl;

        //pointer to member function
        PointertoMemberFunction::mathOperations C;
        c=stdFunction::calculator(a,b,C,&PointertoMemberFunction::mathOperations::adder);
        std::cout<<c <<std::endl;

        //Lambda expressions
        c=stdFunction::calculator(a,b,[](int x ,int y){return x+y;});
        std::cout<<c <<std::endl;

        //std::bind expressions
        c=stdFunction::calculator(a,b, std::bind(FunctionPointer::adder,std::placeholders::_1, std::placeholders::_2)   );
        std::cout<<c <<std::endl;

        //function object
        c=stdFunction::calculator(a,b,stdFunction::FunctionObject());
        std::cout<<c <<std::endl;
    }


/********************************* Templated callback type *****************************************/
    {

        using namespace TemplatedCallback;

        int a[5] = {1, 2, 3, 4, 5};
        stdf_transform_every_int_templ<int,int>(&a[0], 5, &foo);
        // a == {3, 4, 5, 6, 7}
        stdf_transform_every_int_templ<int, int const &>(&a[0], 5, &muh);
        // a == {6, 7, 8, 9, 10}
        stdf_transform_every_int_templ<int, int &>(&a[0], 5, &woof);



        print_int(a, 5);
        transform_every_int_templ(&a[0], 5, foo);
        print_int(a, 5);
        transform_every_int_templ(&a[0], 5, muh);
        print_int(a, 5);
        transform_every_int_templ(&a[0], 5, woof);
        print_int(a, 5);
        transform_every_int_templ(&a[0], 5, [](int x) -> int { return x + x + x; });
        print_int(a, 5);
        transform_every_int_templ(&a[0], 5, Meow{ 4 });
        print_int(a, 5);
        using std::placeholders::_1;
//        transform_every_int_templ(&a[0], 5, std::bind(foo_2, _1, 3));
//        print_int(a, 5);
        transform_every_int_templ(&a[0], 5, std::function<int(int)>{&foo});

    }

    std::function<int(int)> fn3 = third_t();               // function object
    std::cout << "fn3(60): " << fn3(60) << '\n';



}


template
<typename Callable, typename ... Param>
auto bind_values(Callable callable, Param ...param )
{
    return [=](){return callable(param...);};
}

int add(int lhs,int rhs)
{
    return lhs+ rhs;
}

int main1()
{
//    int nSum = Sum(5, 1, 2, 3, 4, 5);
//    LogError(_T("Failed - %s, ErrorCode - %d"), "AccessDenied", 111);

//    auto bound=bind_values(add,1,2);

}
