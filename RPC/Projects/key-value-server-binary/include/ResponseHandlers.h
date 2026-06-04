#pragma once
#include <vector>
#include <string>
#include <limits>
#include <cstdint>

enum class ResponseOpcode : std::uint8_t {
    Ok = 0x40,
    Value = 0x41,
    Count = 0x42,
    Bye = 0x43,
    Keys = 0x44,
    Exists = 0x45,
    Error = 0x7F,
    Not_Found = 0x7E
};

std::vector<std::uint8_t> buildStatusResponse(ResponseOpcode opcode);
std::vector<std::uint8_t> buildErrorResponse(const std::string& error);
std::vector<std::uint8_t> buildValueResponse(const std::string& value);
std::vector<std::uint8_t> buildCountResponse(std::size_t count);

