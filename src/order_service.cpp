#include <cpprest/http_client.h>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <iostream>

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace web::http::experimental::listener;

json::value createOrder() {
    http_client userClient(U("http://localhost:8080/user"));
    http_client productClient(U("http://localhost:8081/products"));

    // Get User Info
    pplx::task<json::value> userTask = userClient.request(methods::GET).then([](http_response response) {
        return response.extract_json();
    });

    // Get Product Catalog
    pplx::task<json::value> productTask = productClient.request(methods::GET).then([](http_response response) {
        return response.extract_json();
    });

    // Wait for both user info and product catalog to be fetched
    json::value userInfo = userTask.get();
    json::value products = productTask.get();

    // Create the order
    json::value order;
    order[U("user")] = userInfo;
    order[U("products")] = products;
    return order;
}

void handle_post(http_request request) {
    std::cout << "Order Service: POST request received" << std::endl;
    json::value order = createOrder();
    request.reply(status_codes::OK, order);
}

int main() {
    http_listener listener(U("http://localhost:8082/order"));

    listener.support(methods::POST, handle_post);

    try {
        listener
            .open()
            .then([]() { std::cout << "Order Service listening on http://localhost:8082/order" << std::endl; })
            .wait();

        std::string line;
        std::getline(std::cin, line); // Keep service alive
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

