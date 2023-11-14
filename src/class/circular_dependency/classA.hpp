#ifndef CLASSA_H
#define CLASSA_H

// Forward declaration of ClassB
class ClassB;

class ClassA {
public:
  ClassA();
  void setClassB(ClassB *b);
  void doSomethingWithB();
  void exampleMethod();

private:
  ClassB *b;
};

#endif // CLASSA_H
