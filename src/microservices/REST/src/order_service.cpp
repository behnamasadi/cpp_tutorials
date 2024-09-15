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
