#include "crow.h"

std::string getUserInfo() {
  return "{\"name\": \"John Doe\", \"email\": \"john@example.com\"}";
}

int main() {

  crow::SimpleApp app;

  CROW_ROUTE(app, "/user")
  ([]() { return crow::response(getUserInfo()); });

  app.port(18080).multithreaded().run();
}
