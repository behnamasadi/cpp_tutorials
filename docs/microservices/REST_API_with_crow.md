### Microservice Example with Crow

First source:
```
./vcpkg/bootstrap-vcpkg.sh
```
and then build it

```
cmake -S . -B build  -G "Ninja Multi-Config" -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake
```

#### 1. **User Service** (user_service.cpp)

```cpp
#include "crow.h"
#include <string>
std::string getUserInfo() {
  return "{\"name\": \"John Doe\", \"email\": \"john@example.com\"}";
}

int main() {

  crow::SimpleApp app;

  CROW_ROUTE(app, "/user")
  ([]() { return crow::response(getUserInfo()); });

  app.port(18080).multithreaded().run();
}
```
the run:

```bash
curl http://localhost:18080/user`
```

#### 2. **Product Service** (product_service.cpp)

```cpp
#include "crow.h"
#include <string>

std::string getProductCatalog() {
    return "[{\"name\": \"Laptop\", \"price\": 1200.50}, {\"name\": \"Headphones\", \"price\": 200.99}]";
}

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/products")
    ([]() {
        return crow::response(getProductCatalog());
    });

    app.port(18081).multithreaded().run();
}
```
the run:

```bash
curl http://localhost:18081/products
```

#### 3. **Order Service** (order_service.cpp)

```cpp
#include "crow.h"
#include <iostream>

void createOrder() {
    std::cout << "Order created for John Doe with Laptop and Headphones" << std::endl;
}

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/order")
    ([]() {
        createOrder();
        return crow::response(200);
    });

    app.port(18082).multithreaded().run();
}
```
the run:

```bash
curl http://localhost:18082/order
```


#### 4. **Payment Service** (payment_service.cpp)

```cpp
#include "crow.h"

void processPayment(double amount) {
    std::cout << "Processing payment of $" << amount << std::endl;
}

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/payment/<double>")
    ([](double amount) {
        processPayment(amount);
        return crow::response(200);
    });

    app.port(18083).multithreaded().run();
}
```
the run:

```bash
curl http://localhost:18083/payment/10
```


### Running Services

- **User Service** runs on port 18080
- **Product Service** runs on port 18081
- **Order Service** runs on port 18082
- **Payment Service** runs on port 18083

Each service runs independently and listens for HTTP requests. They can be deployed separately, scaled, or updated independently without affecting other services.
