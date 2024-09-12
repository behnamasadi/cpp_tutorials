#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <iostream>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

json::value getUserInfo() {
  json::value user;
  user[U("name")] = json::value::string(U("John Doe"));
  user[U("email")] = json::value::string(U("john@example.com"));
  return user;
}

void handle_get(http_request request) {
  std::cout << "User Service: GET request received" << std::endl;
  request.reply(status_codes::OK, getUserInfo());
}

int main() {
  http_listener listener(U("http://localhost:8080/user"));

  listener.support(methods::GET, handle_get);

  try {
    listener.open()
        .then([]() {
          std::cout << "User Service listening on http://localhost:8080/user"
                    << std::endl;
        })
        .wait();

    std::string line;
    std::getline(std::cin, line); // Keep service alive
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }

  return 0;
}
