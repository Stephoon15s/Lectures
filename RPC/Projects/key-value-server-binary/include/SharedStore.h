#pragma once
// HW 2 Includes
#include <unordered_map>

struct SharedStore {
    std::vector<std::string> values{};
    std::unordered_map<std::string, std::string> store{}; // Replaces the vector?? 
    std::mutex mutex{};
};