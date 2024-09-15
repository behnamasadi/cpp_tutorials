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
