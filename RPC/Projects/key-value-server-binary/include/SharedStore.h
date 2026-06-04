#pragma once
// HW 2 Includes
#include <unordered_map>

struct SharedStore {
    std::unordered_map<std::string, std::string> values{}; // Replaces the vector
    std::mutex mutex{};
};