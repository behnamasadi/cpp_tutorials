/*
Abstract class: class that has at least one pure virtual function
Interface: class that has only pure virtual functions and no data members
*/

class MyInterface {
public:
  // Empty virtual destructor for proper cleanup
  virtual ~MyInterface() {}

  virtual void Method1() = 0;
  virtual void Method2() = 0;
};

class MyAbstractClass {
public:
  virtual ~MyAbstractClass();

  virtual void Method1() {
    // some code here
  }
  virtual void Method2();
  void Method3();

  virtual void Method4() = 0; // make MyAbstractClass not instantiable
};

int main() {}
