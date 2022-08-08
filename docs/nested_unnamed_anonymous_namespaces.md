# Unnamed Namespaces

## Translation unit
A translation unit (aka compilation unit) is the ultimate input to a C or C++ compiler.  It consists of the contents of a single source file, plus the contents of any header files directly or indirectly included by it, minus conditional preprocessing statements (i.e. `#ifndef`). A single translation unit can be compiled into an object file, library, or executable program.

## Program linkage
Linkage determines whether identifiers that have identical names refer to the same object, function, or other entity, even if those identifiers appear in different translation units. There are three types of linkages:

- Internal linkage : identifiers can only be seen **within a translation unit**.
- External linkage : identifiers can be seen (and referred to) in other translation units.
- No linkage: identifiers can only be seen in the scope in which they are defined.

## Internal linkage
The following kinds of identifiers have internal linkage:
- Objects, references, or functions explicitly declared static
- Objects or references declared in namespace scope (or global scope in C) with the specifier const or constexpr and neither explicitly declared extern, nor previously declared to have external linkage
- Data members of an anonymous union
- C++ Function templates explicitly declared static
- C++ Identifiers declared in the unnamed namespace


Unnamed namespaces are a utility to make an identifier translation unit local. Having something in an anonymous namespace means it's local to this translation unit (.cpp file and all its includes) this means that if another symbol with the same name is defined elsewhere there will not be a violation of the [One Definition Rule](https://en.wikipedia.org/wiki/One_Definition_Rule).

Unnamed namespace limits access of class,variable,function and objects to the file in which it is defined. All anonymous namespaces in the same file are treated as the same namespace and all anonymous namespaces in different files are distinct. An anonymous namespace is the equivalent of:


```cpp
namespace __unique_compiler_generated_identifer 
{
    ...
}
using namespace __unique_compiler_generated_identifer;
```

You can extend an unnamed namespace within the same translation unit. For example:

```cpp
#include <iostream>
namespace 
{
   int variable;
   void funct (int);
}

namespace 
{
   void funct (int i) { std::cout << i << std::endl; }
}

int main()
{
   funct(variable);
   return 0;
}
```

If you're trying to replicate the behavior of a static class, just using a regular namespace works better since it allows you to access the functions from other files. An anonymous namespace will limit the scope of the function to just one file but why can't you just declare the function outside of a class instead?

It will have **external linkage** if you do that (assuming you didn't declare it static.) The whole point is to avoid that. There are numerous advantages. The compiler can optimize better, since it knows that it can see all call sites/uses of the function; and it's a good way to avoid accidentally exporting unnecessary dynamic symbols when creating shared libraries.

It will also  protects you against ODR violations, if you use the global namespace or a named namespace you may clash with another type/function/variable in another file.

Refs: [1](https://www.ibm.com/docs/en/i/7.3?topic=linkage-program#cplr080), [2](https://stackoverflow.com/questions/357404/why-are-unnamed-namespaces-used-and-what-are-their-benefits), [3](https://www.ibm.com/docs/en/i/7.3?topic=only-unnamed-namespaces-c), [4](https://www.reddit.com/r/cpp/comments/4ukhh5/what_is_the_purpose_of_anonymous_namespaces/)


# Nested Namespaces, 
Below is the syntax to use the nested namespace in one line 

```cpp
namespace Game { 
  
    namespace Graphics { 
  
        namespace Physics { 
  
           class 2D { 
           }; 
        } 
    } 
}


  
namespace Game::Graphics::Physics { 
  
    class 2D { 

    }; 
} 
```



[code](../src/nested_namespaces.cpp)
