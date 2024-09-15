#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "calculator.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using calculator::CalcRequest;
using calculator::CalcResponse;
using calculator::CalculatorService;

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

