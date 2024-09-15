# gRPC

## Calculator Application Example

Let's create a simple gRPC-based calculator application in C++ using CMake. We'll have a `CalculatorService` that performs basic operations like addition and subtraction. The server will expose this service, and the client will call the service to request operations.

### Installation
Installation has been done with `vcpk`. Please check the corresponding files.
 
The content of `vcpkg.json`:

```
 {
  "name": "microservices",
  "version-string": "1.1.0",
  "dependencies": [
     { "name": "grpc" }
  ]
}
```
 
and `CMakeLists.txt`:
 
 
```
 cmake_minimum_required(VERSION 3.14)
project(microservices CXX)

if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE PATH "toolchain file")
endif()
message("toolchain file: ${CMAKE_TOOLCHAIN_FILE}")

# Find Protobuf and gRPC packages
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

message("gRPC_FOUND: "${gRPC_FOUND})
message("gRPC_VERSION: "${gRPC_VERSION})


message("Protobuf_FOUND: "${Protobuf_FOUND})
message("Protobuf_VERSION: "${Protobuf_VERSION})
```
 
Now run: 
 
```
cmake -S . -B build  -G "Ninja Multi-Config" -DCMAKE_TOOLCHAIN_FILE=$PWD/vcpkg/scripts/buildsystems/vcpkg.cmake
```

### Project Structure:
```
.
├── CMakeLists.txt
├── proto
│   └── calculator.proto
├── src
│   ├── client.cpp
│   └── server.cpp
└── vcpkg.json

```

### Step 1: Define the `calculator.proto` file
This `.proto` file defines the service and message types that gRPC will use to generate code for both the server and client.

**calculator.proto**:
```proto
syntax = "proto3";

package calculator;

// The request message containing two numbers.
message CalcRequest {
    double number1 = 1;
    double number2 = 2;
}

// The response message containing the result.
message CalcResponse {
    double result = 1;
}

// The Calculator service definition.
service CalculatorService {
    // Performs addition of two numbers.
    rpc Add(CalcRequest) returns (CalcResponse);

    // Performs subtraction of two numbers.
    rpc Subtract(CalcRequest) returns (CalcResponse);
}
```

- **message CalcRequest**: This message defines two numbers (`number1` and `number2`) for the input.
- **message CalcResponse**: This message will hold the result of the calculation.
- **CalculatorService**: Defines two RPC methods, `Add` and `Subtract`, which accept a `CalcRequest` and return a `CalcResponse`.

### Step 2: Generate gRPC Code
Using the `.proto` file, you will generate C++ classes for gRPC. Assuming you have the `protoc` and gRPC plugins available, run the following commands in your terminal:


The `vcpkg` build the `protoc` in `build/vcpkg_installed/x64-linux/tools/protobuf`, and `grpc_cpp_plugin` in `build/vcpkg_installed/x64-linux/tools/grpc/` so from the root of the project, add them to the path:

```
export PATH=$PWD/build/vcpkg_installed/x64-linux/tools/protobuf:$PWD/build/vcpkg_installed/x64-linux/tools/grpc/:$PATH
```

Then go to `proto` directory

```bash
cd proto
protoc -I=. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` calculator.proto
protoc -I=. --cpp_out=. calculator.proto
```

This will generate `calculator.grpc.pb.cc`, `calculator.grpc.pb.h`, `calculator.pb.cc`, and `calculator.pb.h` files, which we will use in our client and server code.

copy these file into your `generated` directory:

```
.
├── CMakeLists.txt
├── generated
│   ├── calculator.grpc.pb.cc
│   ├── calculator.grpc.pb.h
│   ├── calculator.pb.cc
│   └── calculator.pb.h
├── proto
│   └── calculator.proto
├── src
│   ├── client.cpp
│   └── server.cpp
└── vcpkg.json

```


### Step 3: Write the Server

**server.cpp**:
```cpp
// Implementation of the Calculator Service
class CalculatorServiceImpl final : public CalculatorService::Service {
    Status Add(ServerContext* context, const CalcRequest* request, CalcResponse* response) override {
        double sum = request->number1() + request->number2();
        response->set_result(sum);
        return Status::OK;
    }

    Status Subtract(ServerContext* context, const CalcRequest* request, CalcResponse* response) override {
        double diff = request->number1() - request->number2();
        response->set_result(diff);
        return Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    CalculatorServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main() {
    RunServer();
    return 0;
}
```

- **CalculatorServiceImpl**: Implements the service defined in the `proto` file. It overrides the `Add` and `Subtract` methods to provide logic for the operations.
- **RunServer**: Sets up and starts the gRPC server on port `50051`.

### Step 4: Write the Client

**client.cpp**:
```cpp
class CalculatorClient {
public:
    CalculatorClient(std::shared_ptr<Channel> channel)
        : stub_(CalculatorService::NewStub(channel)) {}

    double Add(double num1, double num2) {
        CalcRequest request;
        request.set_number1(num1);
        request.set_number2(num2);

        CalcResponse response;
        ClientContext context;

        Status status = stub_->Add(&context, request, &response);

        if (status.ok()) {
            return response.result();
        } else {
            std::cout << "RPC failed" << std::endl;
            return 0.0;
        }
    }

    double Subtract(double num1, double num2) {
        CalcRequest request;
        request.set_number1(num1);
        request.set_number2(num2);

        CalcResponse response;
        ClientContext context;

        Status status = stub_->Subtract(&context, request, &response);

        if (status.ok()) {
            return response.result();
        } else {
            std::cout << "RPC failed" << std::endl;
            return 0.0;
        }
    }

private:
    std::unique_ptr<CalculatorService::Stub> stub_;
};

int main() {
    CalculatorClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

    double num1 = 10.0;
    double num2 = 5.0;

    std::cout << "Add: " << client.Add(num1, num2) << std::endl;
    std::cout << "Subtract: " << client.Subtract(num1, num2) << std::endl;

    return 0;
}
```

- **CalculatorClient**: Defines a client that connects to the server and calls the `Add` and `Subtract` methods. It sets up an RPC request, sends it to the server, and retrieves the result.

### Step 5: CMakeLists.txt

Here's how you can set up the CMake build file.

```cmake
set(GENERATED_DIR     "${CMAKE_SOURCE_DIR}/generated" )
set(PROTO_SRCS     "${GENERATED_DIR}/calculator.pb.cc" )
set(GRPC_SRCS     "${GENERATED_DIR}/calculator.grpc.pb.cc")

include_directories(${GENERATED_DIR})


add_executable(server src/server.cpp ${PROTO_SRCS} ${GRPC_SRCS})
add_executable(client src/client.cpp ${PROTO_SRCS} ${GRPC_SRCS})

target_link_libraries(server PRIVATE gRPC::grpc++ protobuf::libprotobuf)
target_link_libraries(client PRIVATE gRPC::grpc++ protobuf::libprotobuf)
```


### Step 6: Build and Run
In the root of the project, run:

```
cmake -S . -B build  -G "Ninja Multi-Config" -DCMAKE_TOOLCHAIN_FILE=$PWD/vcpkg/scripts/buildsystems/vcpkg.cmake

```

Start the server in one terminal:
```bash
./server
```

Run the client in another terminal:
```bash
./client
```

[code](../src/microservices/grpc/)



## Payment Application Example


Let's reimplement the previous microservices example using **gRPC** in C++. We'll define service interfaces using **Protocol Buffers** (`.proto` files), generate the necessary C++ code, and implement each service.

### Overview of gRPC in C++

**gRPC** is a high-performance, open-source RPC framework that uses **Protocol Buffers** as its interface definition language and data serialization mechanism. It allows defining services, and the method parameters and return types are specified as Protocol Buffer message types.

---

### Microservices Architecture with gRPC

We'll recreate the four services:

1. **User Service**: Provides user information.
2. **Product Service**: Provides product catalog information.
3. **Order Service**: Creates orders by communicating with the User and Product services.
4. **Payment Service**: Processes payments.

---

### Prerequisites

- **C++ Compiler**: Supports C++11 or higher.
- **Protocol Buffers Compiler (`protoc`)**: To compile `.proto` files.
- **gRPC C++ Libraries**: Installed and configured.
- **CMake**: For building the project.

---

### Step 1: Define Protocol Buffers for Each Service

We'll start by creating `.proto` files for each service.

#### 1. `user_service.proto`

```proto
syntax = "proto3";

package userservice;

// The user service definition.
service UserService {
  rpc GetUserInfo(UserRequest) returns (UserResponse);
}

// The request message containing the user ID.
message UserRequest {
  string user_id = 1;
}

// The response message containing user information.
message UserResponse {
  string name = 1;
  string email = 2;
}
```

#### 2. `product_service.proto`

```proto
syntax = "proto3";

package productservice;

service ProductService {
  rpc GetProductCatalog(ProductRequest) returns (ProductResponse);
}

message ProductRequest {
}

message Product {
  string name = 1;
  double price = 2;
}

message ProductResponse {
  repeated Product products = 1;
}
```

#### 3. `order_service.proto`

```proto
syntax = "proto3";

package orderservice;

service OrderService {
  rpc CreateOrder(OrderRequest) returns (OrderResponse);
}

message OrderRequest {
  string user_id = 1;
  repeated string product_names = 2;
}

message OrderResponse {
  string order_id = 1;
  bool success = 2;
}
```

#### 4. `payment_service.proto`

```proto
syntax = "proto3";

package paymentservice;

service PaymentService {
  rpc ProcessPayment(PaymentRequest) returns (PaymentResponse);
}

message PaymentRequest {
  double amount = 1;
}

message PaymentResponse {
  bool success = 1;
  string message = 2;
}
```

---

### Step 2: Generate C++ Code from `.proto` Files

Use the `protoc` compiler with the gRPC plugin to generate C++ code.

```bash
# Generate code for User Service
protoc -I . --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` user_service.proto
protoc -I . --cpp_out=. user_service.proto

# Generate code for Product Service
protoc -I . --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` product_service.proto
protoc -I . --cpp_out=. product_service.proto

# Generate code for Order Service
protoc -I . --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` order_service.proto
protoc -I . --cpp_out=. order_service.proto

# Generate code for Payment Service
protoc -I . --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` payment_service.proto
protoc -I . --cpp_out=. payment_service.proto
```

This will produce `.pb.h` and `.pb.cc` files for each service.

---

### Step 3: Implement Each Service

#### 1. **User Service**

**Implementation**: Returns static user information.

##### `user_service_impl.h`

```cpp
#ifndef USER_SERVICE_IMPL_H
#define USER_SERVICE_IMPL_H

#include "user_service.grpc.pb.h"

class UserServiceImpl final : public userservice::UserService::Service {
public:
  grpc::Status GetUserInfo(grpc::ServerContext* context, const userservice::UserRequest* request,
                           userservice::UserResponse* response) override {
    // For demonstration, return static user info
    response->set_name("John Doe");
    response->set_email("john@example.com");
    return grpc::Status::OK;
  }
};

#endif // USER_SERVICE_IMPL_H
```

##### `user_service_server.cc`

```cpp
#include "user_service_impl.h"
#include <grpcpp/grpcpp.h>
#include <iostream>

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  UserServiceImpl service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "User Service listening on " << server_address << std::endl;

  server->Wait();
}

int main() {
  RunServer();
  return 0;
}
```

---

#### 2. **Product Service**

**Implementation**: Returns a static product catalog.

##### `product_service_impl.h`

```cpp
#ifndef PRODUCT_SERVICE_IMPL_H
#define PRODUCT_SERVICE_IMPL_H

#include "product_service.grpc.pb.h"

class ProductServiceImpl final : public productservice::ProductService::Service {
public:
  grpc::Status GetProductCatalog(grpc::ServerContext* context, const productservice::ProductRequest* request,
                                 productservice::ProductResponse* response) override {
    // Add first product
    auto* product1 = response->add_products();
    product1->set_name("Laptop");
    product1->set_price(1200.50);

    // Add second product
    auto* product2 = response->add_products();
    product2->set_name("Headphones");
    product2->set_price(200.99);

    return grpc::Status::OK;
  }
};

#endif // PRODUCT_SERVICE_IMPL_H
```

##### `product_service_server.cc`

```cpp
#include "product_service_impl.h"
#include <grpcpp/grpcpp.h>
#include <iostream>

void RunServer() {
  std::string server_address("0.0.0.0:50052");
  ProductServiceImpl service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Product Service listening on " << server_address << std::endl;

  server->Wait();
}

int main() {
  RunServer();
  return 0;
}
```

---

#### 3. **Payment Service**

**Implementation**: Processes payment and returns success.

##### `payment_service_impl.h`

```cpp
#ifndef PAYMENT_SERVICE_IMPL_H
#define PAYMENT_SERVICE_IMPL_H

#include "payment_service.grpc.pb.h"
#include <iostream>

class PaymentServiceImpl final : public paymentservice::PaymentService::Service {
public:
  grpc::Status ProcessPayment(grpc::ServerContext* context, const paymentservice::PaymentRequest* request,
                              paymentservice::PaymentResponse* response) override {
    double amount = request->amount();
    std::cout << "Processing payment of $" << amount << std::endl;

    // Assume payment is successful
    response->set_success(true);
    response->set_message("Payment processed successfully");
    return grpc::Status::OK;
  }
};

#endif // PAYMENT_SERVICE_IMPL_H
```

##### `payment_service_server.cc`

```cpp
#include "payment_service_impl.h"
#include <grpcpp/grpcpp.h>
#include <iostream>

void RunServer() {
  std::string server_address("0.0.0.0:50053");
  PaymentServiceImpl service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Payment Service listening on " << server_address << std::endl;

  server->Wait();
}

int main() {
  RunServer();
  return 0;
}
```

---

#### 4. **Order Service**

**Implementation**: Acts as a client to other services.

##### `order_service_impl.h`

```cpp
#ifndef ORDER_SERVICE_IMPL_H
#define ORDER_SERVICE_IMPL_H

#include "order_service.grpc.pb.h"
#include "user_service.grpc.pb.h"
#include "product_service.grpc.pb.h"
#include "payment_service.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <iostream>

class OrderServiceImpl final : public orderservice::OrderService::Service {
public:
  OrderServiceImpl(std::shared_ptr<grpc::Channel> user_channel,
                   std::shared_ptr<grpc::Channel> product_channel,
                   std::shared_ptr<grpc::Channel> payment_channel)
      : user_stub_(userservice::UserService::NewStub(user_channel)),
        product_stub_(productservice::ProductService::NewStub(product_channel)),
        payment_stub_(paymentservice::PaymentService::NewStub(payment_channel)) {}

  grpc::Status CreateOrder(grpc::ServerContext* context, const orderservice::OrderRequest* request,
                           orderservice::OrderResponse* response) override {
    // Get user info
    userservice::UserRequest user_request;
    user_request.set_user_id(request->user_id());

    userservice::UserResponse user_response;
    grpc::ClientContext user_context;
    grpc::Status user_status = user_stub_->GetUserInfo(&user_context, user_request, &user_response);

    if (!user_status.ok()) {
      std::cerr << "User Service error: " << user_status.error_message() << std::endl;
      response->set_success(false);
      return user_status;
    }

    // Get product catalog
    productservice::ProductRequest product_request;
    productservice::ProductResponse product_response;
    grpc::ClientContext product_context;
    grpc::Status product_status = product_stub_->GetProductCatalog(&product_context, product_request, &product_response);

    if (!product_status.ok()) {
      std::cerr << "Product Service error: " << product_status.error_message() << std::endl;
      response->set_success(false);
      return product_status;
    }

    // Calculate total amount
    double total_amount = 0;
    for (const auto& product_name : request->product_names()) {
      for (const auto& product : product_response.products()) {
        if (product.name() == product_name) {
          total_amount += product.price();
          break;
        }
      }
    }

    // Process payment
    paymentservice::PaymentRequest payment_request;
    payment_request.set_amount(total_amount);

    paymentservice::PaymentResponse payment_response;
    grpc::ClientContext payment_context;
    grpc::Status payment_status = payment_stub_->ProcessPayment(&payment_context, payment_request, &payment_response);

    if (!payment_status.ok() || !payment_response.success()) {
      std::cerr << "Payment Service error: " << payment_status.error_message() << std::endl;
      response->set_success(false);
      return payment_status;
    }

    // Order creation successful
    response->set_order_id("order_12345"); // In real scenarios, generate unique IDs
    response->set_success(true);

    std::cout << "Order created successfully for user: " << user_response.name() << std::endl;
    return grpc::Status::OK;
  }

private:
  std::unique_ptr<userservice::UserService::Stub> user_stub_;
  std::unique_ptr<productservice::ProductService::Stub> product_stub_;
  std::unique_ptr<paymentservice::PaymentService::Stub> payment_stub_;
};

#endif // ORDER_SERVICE_IMPL_H
```

##### `order_service_server.cc`

```cpp
#include "order_service_impl.h"
#include <grpcpp/grpcpp.h>
#include <iostream>

void RunServer() {
  std::string server_address("0.0.0.0:50054");

  // Channels to other services
  auto user_channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
  auto product_channel = grpc::CreateChannel("localhost:50052", grpc::InsecureChannelCredentials());
  auto payment_channel = grpc::CreateChannel("localhost:50053", grpc::InsecureChannelCredentials());

  OrderServiceImpl service(user_channel, product_channel, payment_channel);

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Order Service listening on " << server_address << std::endl;

  server->Wait();
}

int main() {
  RunServer();
  return 0;
}
```

---

### Step 4: Build the Project with CMake

Create a `CMakeLists.txt` file to build all services.

##### `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.5)
project(MicroservicesWithGRPC)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS})

# Function to generate protobuf files
function(generate_proto target_name proto_file)
  get_filename_component(proto_abs ${proto_file} ABSOLUTE)
  get_filename_component(proto_name ${proto_file} NAME_WE)

  # Generate the sources
  protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${proto_abs})
  grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${proto_abs})

  # Create library
  add_library(${target_name} ${PROTO_SRCS} ${PROTO_HDRS} ${GRPC_SRCS} ${GRPC_HDRS})
  target_link_libraries(${target_name} PUBLIC protobuf::libprotobuf gRPC::grpc++)

  set(${target_name}_SRCS ${PROTO_SRCS} PARENT_SCOPE)
  set(${target_name}_HDRS ${PROTO_HDRS} PARENT_SCOPE)
endfunction()

# User Service
generate_proto(user_service_proto user_service.proto)
add_executable(user_service_server user_service_server.cc)
target_link_libraries(user_service_server user_service_proto)

# Product Service
generate_proto(product_service_proto product_service.proto)
add_executable(product_service_server product_service_server.cc)
target_link_libraries(product_service_server product_service_proto)

# Payment Service
generate_proto(payment_service_proto payment_service.proto)
add_executable(payment_service_server payment_service_server.cc)
target_link_libraries(payment_service_server payment_service_proto)

# Order Service
generate_proto(order_service_proto order_service.proto)
add_executable(order_service_server order_service_server.cc)
target_link_libraries(order_service_server
  order_service_proto
  user_service_proto
  product_service_proto
  payment_service_proto)
```

---

### Step 5: Build and Run the Services

#### 1. **Build the Project**

In the project directory:

```bash
mkdir build
cd build
cmake ..
make
```

#### 2. **Run the Services**

Open separate terminals for each service.

- **Terminal 1**: Run User Service

  ```bash
  ./user_service_server
  ```

- **Terminal 2**: Run Product Service

  ```bash
  ./product_service_server
  ```

- **Terminal 3**: Run Payment Service

  ```bash
  ./payment_service_server
  ```

- **Terminal 4**: Run Order Service

  ```bash
  ./order_service_server
  ```

#### 3. **Test the Order Service**

Create a simple client to test the Order Service.

##### `order_service_client.cc`

```cpp
#include "order_service.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <string>

int main() {
  auto channel = grpc::CreateChannel("localhost:50054", grpc::InsecureChannelCredentials());
  auto stub = orderservice::OrderService::NewStub(channel);

  orderservice::OrderRequest request;
  request.set_user_id("user_123");
  request.add_product_names("Laptop");
  request.add_product_names("Headphones");

  orderservice::OrderResponse response;
  grpc::ClientContext context;

  grpc::Status status = stub->CreateOrder(&context, request, &response);

  if (status.ok()) {
    if (response.success()) {
      std::cout << "Order created successfully! Order ID: " << response.order_id() << std::endl;
    } else {
      std::cout << "Order creation failed." << std::endl;
    }
  } else {
    std::cout << "RPC failed: " << status.error_message() << std::endl;
  }

  return 0;
}
```

Update `CMakeLists.txt` to include the client:

```cmake
add_executable(order_service_client order_service_client.cc)
target_link_libraries(order_service_client
  order_service_proto
  user_service_proto
  product_service_proto
  payment_service_proto)
```

Rebuild the project:

```bash
make
```

Run the client:

```bash
./order_service_client
```

---

### Summary

- **Microservices Implemented with gRPC**: We reimplemented the services using gRPC in C++.
- **Service Definitions**: Defined services using Protocol Buffers.
- **Inter-Service Communication**: The Order Service communicates with other services via gRPC calls.
- **Scalable Architecture**: Each service can be independently deployed and scaled.

### Notes

- **Error Handling**: The code includes basic error handling; in a production environment, you would enhance this.
- **Security**: Authentication and encryption (e.g., SSL/TLS) should be added for secure communication.
- **Service Discovery**: For scalable deployment, consider implementing service discovery mechanisms.

---

### Dependencies and Installation

#### **Install Protocol Buffers and gRPC**

Follow the instructions from the [gRPC C++ Quick Start Guide](https://grpc.io/docs/languages/cpp/quickstart/).

#### **CMake Version**

Ensure you have **CMake 3.13** or higher for the `FindProtobuf` and `FindgRPC` modules.

---

### Conclusion

Using gRPC for implementing microservices in C++ allows for efficient, strongly-typed, and scalable inter-service communication. This example demonstrates the foundational steps to build such a system. For a production environment, you would expand upon this with more robust error handling, security features, logging, and configuration management.

---

If you have any questions or need further clarification on any part of the implementation, feel free to ask!

