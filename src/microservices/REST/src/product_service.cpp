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
