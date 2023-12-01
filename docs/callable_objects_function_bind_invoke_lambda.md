# Callable Objects
In C++, callable objects are entities that can be called as if they are functions. This includes a variety of types. Here's a list of the most common callable objects in C++:

1. **Functions**: The most basic callable entities. They can be regular functions, including those defined at the global scope.

2. **Function Pointers**: Pointers that point to functions. They can be used to invoke the function they point to.

3. **Lambda Expressions (C++11 and later)**: Anonymous function objects defined in-place. They are useful for short snippets of code that are passed to algorithms or used for event handling.

4. **Functors (Function Objects)**: Objects of a class that has the `operator()` defined. This operator makes an object of the class behave like a function.

5. **Member Functions**: Functions that are members of a class. To call these, you typically need an object (or a reference/pointer to an object) and the function name.

6. **Member Function Pointers**: Pointers to member functions of a class. These pointers need to be invoked on an object of the class they belong to.

7. **std::function (C++11 and later)**: A template class provided by the standard library that can wrap any callable object, including functions, lambda expressions, function objects, and member function pointers. It provides a level of abstraction and flexibility in handling callable entities.

8. **std::bind (C++11 and later)**: Produces a new callable object by binding arguments to parameters of another callable object. It's often used in conjunction with `std::function`.

9. **Packaged Tasks (C++11 and later)**: These are template classes that encapsulate a callable object and allow its result to be retrieved asynchronously.

10. **Coroutines (C++20 and later)**: Introduced in C++20, coroutines are functions that can suspend execution to be resumed later. They are callable and can be used in asynchronous programming.

11. **Auto-generated operator() from a Captureless Lambda (C++20 and later)**: Captureless lambdas can be implicitly converted to a function pointer, making them callable in a variety of contexts.

Each of these callable types serves different purposes and has different uses depending on the context. Understanding how to use them effectively is a key part of mastering C++.


## Examples
Let's go through each type of callable object in C++ with an example:

1. **Functions**:
   ```cpp
   void helloWorld() {
       std::cout << "Hello, World!" << std::endl;
   }

   // Usage
   helloWorld();
   ```

2. **Function Pointers**:
   ```cpp
   void greet() {
       std::cout << "Hello!" << std::endl;
   }

   void (*funcPtr)() = greet;

   // Usage
   funcPtr();
   ```

3. **Lambda Expressions**:
   ```cpp
   auto lambda = []() { std::cout << "Hello from lambda!" << std::endl; };

   // Usage
   lambda();
   ```

4. **Functors (Function Objects)**:
   ```cpp
   struct Functor {
       void operator()() const {
           std::cout << "Functor called" << std::endl;
       }
   };

   Functor functor;

   // Usage
   functor();
   ```

5. **Member Functions**:
   ```cpp
   class MyClass {
   public:
       void memberFunc() {
           std::cout << "Member function called" << std::endl;
       }
   };

   MyClass obj;

   // Usage
   obj.memberFunc();
   ```

6. **Member Function Pointers**:
   ```cpp
   class MyClass {
   public:
       void display() const {
           std::cout << "Display method" << std::endl;
       }
   };

   void (MyClass::*funcPtr)() const = &MyClass::display;
   MyClass obj;

   // Usage
   (obj.*funcPtr)();
   ```

7. **std::function**:
   ```cpp
   std::function<void()> func = []() { std::cout << "std::function" << std::endl; };

   // Usage
   func();
   ```

8. **std::bind**:
   ```cpp
   void printNumber(int number) {
       std::cout << "Number: " << number << std::endl;
   }

   auto boundFunc = std::bind(printNumber, 42);

   // Usage
   boundFunc();
   ```

9. **Packaged Tasks**:
   ```cpp
   std::packaged_task<int(int)> task([](int x) { return x * x; });
   std::future<int> result = task.get_future();

   // Usage
   task(2);
   std::cout << "Result: " << result.get() << std::endl; // Outputs 4
   ```

10. **Coroutines (C++20 and later)**:
    Coroutines are more complex and require a supporting infrastructure. Here's a simple example:
    ```cpp
    std::generator<int> simpleCoroutine() {
        co_yield 1;
        co_yield 2;
        co_yield 3;
    }

    // Usage
    for (int value : simpleCoroutine()) {
        std::cout << value << std::endl;
    }
    ```

11. **Auto-generated operator() from a Captureless Lambda**:
    ```cpp
    auto lambda = []() { std::cout << "Lambda as function pointer" << std::endl; };
    void (*funcPtr)() = lambda;

    // Usage
    funcPtr();
    ```

These examples illustrate the basic usage of each callable object type in C++. Note that for more advanced features like coroutines and generators, additional setup and understanding of the C++20 standard are required.


[code](../src/bind.cpp)
