#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <iostream>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

void processPayment(double amount) {
    std::cout << "Processing payment of $" << amount << std::endl;
}

void handle_post(http_request request) {
    std::cout << "Payment Service: POST request received" << std::endl;

    request.extract_json().then([&](json::value body) {
        double amount = body[U("amount")].as_double();
        processPayment(amount);
        request.reply(status_codes::OK, U("Payment processed successfully"));
    }).wait();
}

int main() {
    http_listener listener(U("http://localhost:8083/payment"));

    listener.support(methods::POST, handle_post);

    try {
        listener
            .open()
            .then([]() { std::cout << "Payment Service listening on http://localhost:8083/payment" << std::endl; })
            .wait();

        std::string line;
        std::getline(std::cin, line); // Keep service alive
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

