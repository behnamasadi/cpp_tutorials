Stack unwinding in C++ is a process that occurs during exception handling. When an exception is thrown, the C++ runtime system unwinds the stack, which means it pops function calls off the stack until it finds a try block with a catch handler that can handle the thrown exception. Here's a step-by-step explanation:

1. **Exception Throwing**: An exception is thrown using the `throw` keyword. This can happen anywhere in the code where an error or unexpected situation is detected.

2. **Stack Unwinding Process**:
   - When an exception is thrown, the C++ runtime starts to unwind the stack.
   - This involves exiting the current function and destroying all local objects that were created inside the function.
   - The destruction of local objects is done in the reverse order of their creation, ensuring proper release of resources.
   - The runtime system then moves to the previous function in the call stack and repeats this process.

3. **Search for a Catch Block**:
   - The stack unwinding continues until the runtime system finds a function with a try block that encloses the point where the exception was thrown.
   - The catch clauses associated with this try block are examined to see if any of them can handle the exception (based on the exception's type).

4. **Handling the Exception**:
   - If a suitable catch block is found, the exception is considered handled, and the execution continues from the catch block.
   - If no suitable catch block is found in the call stack, the program typically terminates, and an unhandled exception error may be reported.

5. **Resource Management**:
   - Stack unwinding ensures that resources (like memory, file handles, etc.) are properly released even when an exception occurs.
   - This is critical in C++ because it doesn't have garbage collection like some other languages. It relies on deterministic destruction (via destructors) for resource management.

6. **Examples**:
   Here's a simple example to illustrate stack unwinding:

   ```cpp
   #include <iostream>
   #include <exception>

   void funcC() {
       throw std::runtime_error("Error occurred");
   }

   void funcB() {
       funcC();
       // More code...
   }

   void funcA() {
       try {
           funcB();
       } catch (const std::exception& e) {
           std::cout << "Caught exception: " << e.what() << std::endl;
       }
   }

   int main() {
       funcA();
       return 0;
   }
   ```

   In this example, `funcC` throws an exception. The stack unwinding process begins, and `funcB` is exited. The runtime system then checks `funcA`, finds the try-catch block, and executes the catch block since it can handle the exception type.

Remember, proper use of stack unwinding is crucial for writing robust C++ programs, especially for managing resources and ensuring consistent program state even in the face of errors.

[code](../src/stack_unwinding.cpp)
