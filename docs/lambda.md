# Lambda Expression

## syntax:
```cpp
[ captures ] ( params ) { body }
[ captures ] ( params ) -> ret { body }
```

##  inline:

```cpp
std::sort(v1.begin(),v1.end(),[](int x,int y){ return (x<y); }  );
```

##  outside:

```cpp

    auto increaser=[](student &s)
    {
        s.x++;
        return;
    };
    
    std::vector<student> students;
    std::for_each(students.begin(),students.end(),increaser);
```


##  outside as a function:

```cpp

auto increaserFunction=[](student &s)
{
    s.x++;
    return;
};

int main()
{
    std::for_each(students.begin(),students.end(),increaserFunction);
}

```

## Captures
To access other variables other than what was passed to lambda within it, we can use capture-clause `[]`. You can capture by both **reference** and **value**, which you can specify using & and = respectively:

If you don't specify the default capture mode, no variables from the enclosing scope will be captured automatically. Each variable that you wish to use inside the lambda must be explicitly captured.

There are two main default capture modes in C++ lambda expressions:

1. Capture by value `[=]`: This mode captures all local variables used in the lambda by value. It creates a copy of each variable as it exists at the point where the lambda is defined.

2. Capture by reference `[&]`: This mode captures all local variables used in the lambda by reference. The lambda will operate on the actual variables from the enclosing scope, and any modifications inside the lambda will affect the originals.

If neither of these is specified, you must list each variable you want to capture in the capture list. For example:

- `[&x, y]`: captures `x` by reference and `y` by value.
- `[]`: captures nothing; no access to variables from the enclosing scope unless passed as arguments.
- `[&var]`: capture var by reference
- `[&, var]`: specify that the default way of capturing is by reference and we want to capture var
- `[=]`: it means that all local variables used inside the lambda function are captured by value. 
- `[=, &var]`: capture the variables in scope by value by default, but capture var using reference instead
- `[x, y]`: If you specify individual variable names in the capture list without a preceding `&`, those variables are captured by value. For example, `[x, y]` means `x` and `y` are captured by value. Changes to `x` and `y` inside the lambda will not affect the original variables outside the lambda.

### by value and mutable
`[=]` capture all variables within scope by value:

```cpp
    int a;
    a=10;
    auto lambda = [=] ()  mutable { ++a; std::cout<<a<<std::endl; };
    lambda();
    std::cout<<a <<std::endl;
```

When a variable is captured by value in a lambda expression in C++, a copy of the variable is made and used within the lambda. This copy is independent of the original variable. By default, this copy is `const`, meaning it cannot be modified within the lambda.


However, when you use the `mutable` keyword with a lambda, it allows this copied variable to be modified within the lambda. It's important to understand that this modification affects only the lambda's internal copy of the variable, not the original variable outside the lambda. The original variable remains unchanged regardless of any modifications made to the copy within the lambda.

In this code, lambda has its own internal copy of `a`. When lambda is called, it modifies this copy (due to **mutable**), but the original `a` in the outer scope remains unchanged at `10`.




## Functions or classes that accept lambdas

In C++, you can write functions or classes that accept lambdas by using templates or `std::function`. Here's an example of both approaches:

### 1. Using Templates to Accept a Lambda

Templates in C++ allow you to write a function that can accept any callable object, including lambdas. Here's an example:

```cpp
#include <iostream>

// Function template that accepts any callable object (including lambdas)
template<typename Func>
void execute(Func f) {
    f();  // Call the passed function
}

int main() {
    // Lambda that prints a message
    auto lambda = []() {
        std::cout << "Hello from lambda!" << std::endl;
    };

    // Pass the lambda to the execute function
    execute(lambda);

    // You can also pass the lambda directly
    execute([]() {
        std::cout << "Direct lambda call!" << std::endl;
    });

    return 0;
}
```

### 2. Using `std::function` to Accept a Lambda

`std::function` is a more flexible way to accept a callable object, but it might involve some overhead compared to templates. Here’s an example:

```cpp
#include <iostream>
#include <functional>

// Function that accepts a std::function<void()> (any callable object that takes no arguments and returns void)
void execute(const std::function<void()>& func) {
    func();  // Call the passed function
}

int main() {
    // Lambda that prints a message
    auto lambda = []() {
        std::cout << "Hello from lambda!" << std::endl;
    };

    // Pass the lambda to the execute function
    execute(lambda);

    // You can also pass the lambda directly
    execute([]() {
        std::cout << "Direct lambda call!" << std::endl;
    });

    return 0;
}
```


[source code](../src/lambda.cpp)
