#pragma once

#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>

struct ServiceProvider {
    std::string identifier{}; //WOuld this be message ID?
    std::string host{};
    std::int32_t port{};
    // Introduce a new var, last hearbeat
    std::chrono::system_clock::time_point lastHeartbeat{};
};

struct SharedRegistry {
    std::unordered_map<std::string, std::vector<ServiceProvider>> services{};
    // String Represents Service Name and maps to a vector that is the service provider.
    std::mutex mutex{};
};
