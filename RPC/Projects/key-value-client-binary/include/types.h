#ifndef TYPES_H
#define TYPES_H

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

enum class RequestOpcode : std::uint8_t {
    Put = 1,
    Get = 2,
    Delete = 3,
    Count = 4,
    Exists = 5, 
    Clear = 6,
    Keys = 7,
    Quit = 10
};

enum class ResponseOpcode : std::uint8_t {
    Ok = 64,
    Value = 65,
    Count = 66,
    Bye = 67,
    Keys = 68,
    Exists = 69,
    Error = 127,
    Not_Found = 143
};

struct BinaryResponse {
    ResponseOpcode opcode;
    std::vector<std::uint8_t> payload;
};


bool appendInt32(std::vector<std::uint8_t>& payload, std::int32_t value);
bool appendString(std::vector<std::uint8_t>& payload, const std::string& value);
std::vector<std::uint8_t> frameRequest(RequestOpcode opcode, const std::vector<std::uint8_t>& arguments);
std::optional<BinaryResponse> parseResponseMessage(const std::vector<std::uint8_t>& message);

bool parseStatusResponse(const BinaryResponse& response);
std::optional<std::string> parseValueResponse(const BinaryResponse& response);
std::optional<std::size_t> parseCountResponse(const BinaryResponse& response);
bool isErrorResponse(const BinaryResponse& response);
std::string getErrorMessage(const BinaryResponse& response);

#endif // TYPES_H
