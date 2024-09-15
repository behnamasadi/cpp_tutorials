#define CROW_MAIN
#include "crow.h"
#include <unordered_map>
#include <mutex>

struct Item {
    int id;
    std::string name;
};

std::unordered_map<int, Item> items;  // In-memory data store
std::mutex items_mutex;  // To handle concurrent access

int main() {
    crow::SimpleApp app;

    // Retrieve an item by ID
    CROW_ROUTE(app, "/item/<int>").methods(crow::HTTPMethod::GET)
    ([](int id) {
        std::lock_guard<std::mutex> guard(items_mutex);
        if (items.count(id)) {
            auto& item = items[id];
            crow::json::wvalue x;
            x["id"] = item.id;
            x["name"] = item.name;
            return crow::response(x);
        } else {
            return crow::response(404, "Item not found");
        }
    });

    // Create a new item
    CROW_ROUTE(app, "/item").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) {
            return crow::response(400, "Invalid JSON");
        }
        int id = body["id"].i();
        std::string name = body["name"].s();

        std::lock_guard<std::mutex> guard(items_mutex);
        if (items.count(id)) {
            return crow::response(400, "Item already exists");
        }

        items[id] = {id, name};
        return crow::response(201, "Item created");
    });

    // Update an item
    CROW_ROUTE(app, "/item/<int>").methods(crow::HTTPMethod::PUT)
    ([](int id, const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) {
            return crow::response(400, "Invalid JSON");
        }
        std::string name = body["name"].s();

        std::lock_guard<std::mutex> guard(items_mutex);
        if (items.count(id)) {
            items[id].name = name;
            return crow::response(200, "Item updated");
        } else {
            return crow::response(404, "Item not found");
        }
    });

    // Delete an item
    CROW_ROUTE(app, "/item/<int>").methods(crow::HTTPMethod::DELETE)
    ([](int id) {
        std::lock_guard<std::mutex> guard(items_mutex);
        if (items.count(id)) {
            items.erase(id);
            return crow::response(200, "Item deleted");
        } else {
            return crow::response(404, "Item not found");
        }
    });

    app.port(18080).multithreaded().run();
}

