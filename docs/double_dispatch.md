# Double Dispatch
Double dispatch is a design pattern used in programming, particularly in languages like C++, to achieve dynamic polymorphism for methods. It's a way to resolve a call to a function or method at runtime based on the runtime types of two objects involved in the call. This pattern is often used when a single operation needs to be performed differently based on the combination of types of two objects.

Here's a basic explanation using C++:

1. **The Problem**: C++ supports polymorphism, but it's typically single dispatch polymorphism. This means the method that gets called is determined by the runtime type of a single object (usually the object on which the method is called). But sometimes you need to choose a method based on the runtime types of two objects.

2. **First Dispatch**: The first dispatch is typically achieved via a virtual function call. This allows the system to determine the actual type of one object at runtime.

3. **Second Dispatch**: The second dispatch involves calling a function that is based on the runtime type of the second object. This is where the "double" in double dispatch comes in.

### Example in C++:

Imagine you have a class hierarchy for different `Shape` objects and you want to implement a collision detection system. You might have classes like `Circle`, `Rectangle`, and so on, all inheriting from `Shape`.

1. **Abstract Base Class**:
   ```cpp
   class Shape {
   public:
       virtual void collideWith(Shape& other) = 0;
   };
   ```

2. **Derived Classes**:
   ```cpp
   class Circle : public Shape {
   public:
       void collideWith(Shape& other) override;
   };

   class Rectangle : public Shape {
   public:
       void collideWith(Shape& other) override;
   };
   ```

3. **Implementing Double Dispatch**:
   In the `collideWith` method of each derived class, you would then call a method that is specific to the combination of shapes. This is often done through a visitor pattern or by dynamic casting to the specific type.

   ```cpp
   void Circle::collideWith(Shape& other) {
       // Call a method that is specific to Circle colliding with other shape
       other.collideWithCircle(*this);
   }

   void Rectangle::collideWith(Shape& other) {
       // Call a method that is specific to Rectangle colliding with other shape
       other.collideWithRectangle(*this);
   }
   ```

4. **Adding Collision Logic**:
   You would then need to add methods to handle each specific collision type. For example, `collideWithCircle`, `collideWithRectangle`, etc., each handling the logic for collision between different pairs of shapes.

5. **Handling New Shapes**:
   If a new shape is added, you would need to update each class with a new method for colliding with the new shape, which can lead to scalability issues.

In summary, double dispatch in C++ is a way to resolve method calls involving two objects based on their runtime types. It's particularly useful for operations that depend on the combination of types of two objects, like collision detection in a simulation or game. The implementation often involves a mix of polymorphism and method overloading.


## Scenario: Document Printing System

Let's consider a real-life scenario in a printing system where you have different printers and different documents, and each type of document requires a different printing technique depending on the printer.


1. **Classes and Hierarchy**:
   - You have a base class `Printer` and derived classes like `InkjetPrinter`, `LaserPrinter`, etc.
   - Similarly, you have a base class `Document` with derived classes like `PDFDocument`, `WordDocument`, etc.

2. **The Goal**:
   - Your goal is to print documents correctly based on the combination of the printer and document type. For instance, a `PDFDocument` might need a different printing approach on an `InkjetPrinter` compared to a `LaserPrinter`.

3. **Double Dispatch Implementation**:

   - **First Dispatch (Document Side)**: When you initiate printing, the document's print method is called, which then delegates to a second dispatch based on the printer type.

     ```cpp
     class Document {
     public:
         virtual void printOn(Printer& printer) = 0;
     };

     class PDFDocument : public Document {
     public:
         void printOn(Printer& printer) override;
     };

     class WordDocument : public Document {
     public:
         void printOn(Printer& printer) override;
     };
     ```

   - **Second Dispatch (Printer Side)**: Each document type then calls an appropriate method on the printer object to handle that specific document.

     ```cpp
     void PDFDocument::printOn(Printer& printer) {
         printer.printPDF(*this);
     }

     void WordDocument::printOn(Printer& printer) {
         printer.printWord(*this);
     }
     ```

4. **Printer Class Hierarchy**:

   - The `Printer` class and its derived classes implement the methods to handle different document types.

     ```cpp
     class Printer {
     public:
         virtual void printPDF(PDFDocument& doc) = 0;
         virtual void printWord(WordDocument& doc) = 0;
     };

     class InkjetPrinter : public Printer {
         void printPDF(PDFDocument& doc) override;
         void printWord(WordDocument& doc) override;
     };

     class LaserPrinter : public Printer {
         void printPDF(PDFDocument& doc) override;
         void printWord(WordDocument& doc) override;
     };
     ```

5. **Printing Logic**:
   - In each printer class, the specific logic for printing each type of document is implemented. For example, `InkjetPrinter` might handle a `PDFDocument` differently than a `LaserPrinter`.

6. **Adding New Printer or Document Types**:
   - If you introduce a new type of printer or document, you'd need to update the printing methods across the classes to accommodate the new combinations, which is a common challenge with the double dispatch pattern.

In this scenario, double dispatch allows you to decouple the logic of printing documents from the types of printers and documents themselves. Each printer knows how to handle each type of document, and each document type knows how to be printed on different types of printers. This results in a flexible and extendable system that can handle various combinations of printers and documents.

