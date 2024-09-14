## gRPC
Let's create a simple gRPC-based calculator application in C++ using CMake. We'll have a `CalculatorService` that performs basic operations like addition and subtraction. The server will expose this service, and the client will call the service to request operations.

### Installation
Installation has been done with `vcpk`. Please check the corresponding files.
 run this 
 
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
