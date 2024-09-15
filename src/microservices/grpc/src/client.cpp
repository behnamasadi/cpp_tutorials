#include "calculator.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>
#include <string>

using calculator::CalcRequest;
using calculator::CalcResponse;
using calculator::CalculatorService;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

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
  CalculatorClient client(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));

  double num1 = 10.0;
  double num2 = 5.0;

  while (true) {

    std::cout << "Enter first number: " << std::endl;
    std::cin >> num1;

    if (std::cin.fail()) {
      std::cin.clear(); // clear the error state
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                      '\n'); // ignore invalid input
      std::cout << "Invalid input. Please enter a valid number." << std::endl;
      continue;
    }

    std::cout << "Enter second number: " << std::endl;
    std::cin >> num2;
    if (std::cin.fail()) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cout << "Invalid input. Please enter a valid number." << std::endl;
      continue;
    }

    std::cout << "Add: " << client.Add(num1, num2) << std::endl;
    std::cout << "Subtract: " << client.Subtract(num1, num2) << std::endl;

    std::cout << "-----------------------------------" << std::endl;
  }

  return 0;
}
