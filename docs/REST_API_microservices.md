# Microservices
C++ microservices are less common compared to microservices built using languages like Java, Python, or Node.js, which are more popular for web applications and cloud-native services. However, C++ is still used for microservices, especially in scenarios that require high performance, low latency, or extensive computational power, such as:

1. **Finance**: High-frequency trading systems or any system that demands low-latency communication can benefit from C++'s performance.
2. **Telecommunications**: Systems like network equipment or infrastructure services may use C++ for efficiency.
3. **Embedded and IoT**: C++ is used in devices where hardware-level interaction and performance optimization are critical.



### Monolithic Software Example in C++

In a monolithic architecture, all components of the application are tightly coupled and run as a single unit. Let's assume we are building a simple e-commerce application. In a monolithic design, all components—user authentication, order management, product catalog, and payment processing—are integrated into one executable.

#### Monolithic Architecture (C++)

```cpp
#include <iostream>
#include <string>
#include <vector>

class User {
public:
    std::string name;
    std::string email;

    User(std::string name, std::string email) : name(name), email(email) {}

    void displayUserInfo() {
        std::cout << "User: " << name << ", Email: " << email << std::endl;
    }
};

class Product {
public:
    std::string name;
    double price;

    Product(std::string name, double price) : name(name), price(price) {}

    void displayProduct() {
        std::cout << "Product: " << name << ", Price: $" << price << std::endl;
    }
};

class Order {
public:
    User user;
    std::vector<Product> products;

    Order(User user, std::vector<Product> products) : user(user), products(products) {}

    void displayOrder() {
        std::cout << "Order placed by " << user.name << " for:" << std::endl;
        for (auto &product : products) {
            product.displayProduct();
        }
    }
};

class Payment {
public:
    void processPayment(double amount) {
        std::cout << "Processing payment of $" << amount << std::endl;
    }
};

class ECommerceApp {
public:
    void run() {
        // User registration
        User user("John Doe", "john@example.com");

        // Product catalog
        Product product1("Laptop", 1200.50);
        Product product2("Headphones", 200.99);

        // Create order
        std::vector<Product> products = {product1, product2};
        Order order(user, products);
        order.displayOrder();

        // Payment processing
        Payment payment;
        payment.processPayment(product1.price + product2.price);
    }
};

int main() {
    ECommerceApp app;
    app.run();
    return 0;
}
```

#### Key Aspects:
- All functionalities (user management, product catalog, order management, and payment) are part of a single executable.
- Any change requires recompiling and redeploying the whole application.
- Hard to scale individual components independently.

---

### Microservice Alternative in C++

In a microservices architecture, each functionality is broken down into small, independently deployable services. Let's split the monolithic application into different services.

1. **User Service** (Handles user authentication and management)
2. **Product Service** (Handles product catalog)
3. **Order Service** (Handles orders)
4. **Payment Service** (Processes payments)

Each service can be written in C++ but will run as a separate process, possibly communicating via HTTP (e.g., using REST APIs).

#### Example: Microservices Setup

##### 1. User Service (user_service.cpp)

```cpp
#include <iostream>
#include <string>

class UserService {
public:
    std::string getUserInfo() {
        return "John Doe, john@example.com";
    }
};

int main() {
    UserService userService;
    std::cout << "User info: " << userService.getUserInfo() << std::endl;
    return 0;
}
```

##### 2. Product Service (product_service.cpp)

```cpp
#include <iostream>
#include <string>

class ProductService {
public:
    std::string getProductCatalog() {
        return "Product: Laptop, Price: $1200.50\nProduct: Headphones, Price: $200.99";
    }
};

int main() {
    ProductService productService;
    std::cout << "Product Catalog:\n" << productService.getProductCatalog() << std::endl;
    return 0;
}
```

##### 3. Order Service (order_service.cpp)

```cpp
#include <iostream>
#include <string>

class OrderService {
public:
    void createOrder() {
        std::cout << "Order created for: John Doe\nProducts: Laptop, Headphones" << std::endl;
    }
};

int main() {
    OrderService orderService;
    orderService.createOrder();
    return 0;
}
```

##### 4. Payment Service (payment_service.cpp)

```cpp
#include <iostream>

class PaymentService {
public:
    void processPayment(double amount) {
        std::cout << "Processing payment of $" << amount << std::endl;
    }
};

int main() {
    PaymentService paymentService;
    paymentService.processPayment(1401.49);
    return 0;
}
```

### Microservices Communication

In the microservice architecture, services would typically communicate using HTTP or gRPC. For example, `OrderService` might call `UserService` to retrieve user info, or `PaymentService` might be called to process payments after an order is placed.

The implementation of communication could be handled using REST API frameworks like:
- **CppRestSDK** (for RESTful communication)
- **gRPC** (for high-performance communication)



