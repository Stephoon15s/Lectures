#include "RemoteKeyValueStore.h"
#include "types.h"

#include <limits>

RemoteKeyValueStore::RemoteKeyValueStore(const std::string& host, int port)
    : m_stub{host, port} {
}

bool RemoteKeyValueStore::sendStatusCommand(RequestOpcode opcode, const std::vector<std::uint8_t>& arguments) {
    auto response = m_stub.sendRequest(opcode, arguments);
    if (!response.has_value()) {
        return false;
    }

    return parseStatusResponse(response.value());
}

std::optional<std::string> RemoteKeyValueStore::sendValueCommand(RequestOpcode opcode,
                                                        const std::vector<std::uint8_t>& arguments) {
    auto response = m_stub.sendRequest(opcode, arguments);
    if (!response.has_value()) {
        return std::nullopt;
    }

    return parseValueResponse(response.value());
}

// New Stuff

bool RemoteKeyValueStore::put(const std::string& key, const std::string& value){
    // Encode the bytes of the string argument, including its length.
    std::vector<std::uint8_t> arguments{};
    if (!appendString(arguments, value)) {
        return false;
    }

    // Send and parse a PUT command, expecting to receive back an OK ("Status") response.
    return sendStatusCommand(RequestOpcode::Put, arguments); 
}

std::optional<std::string> RemoteKeyValueStore::get(const std::string& key){
    // Encode the bytes of the string argument, including its length.
    std::vector<std::uint8_t> arguments{};
    if (!appendString(arguments, key)) {
        return std::nullopt;
    }

    // Send and parse a Get command, expecting to receive back a Value response.
    return sendValueCommand(RequestOpcode::Get, arguments); 
}

bool RemoteKeyValueStore::exists(const std::string& key){
    // Encode the bytes of the string argument, including its length.
    std::vector<std::uint8_t> arguments{};
    if (!appendString(arguments, key)) {
        return false;
    }

    // Send and parse a PUT command, expecting to receive back an OK ("Status") response.
    return sendStatusCommand(RequestOpcode::Exists, arguments);
}

// std::optional<std::vector<std::string>> RemoteKeyValueStore::keys();
// FIXME:: IMPLEMENT KEYS

// New Stuff


/*
// START HERE: to push a string into the list, we send a message with a Push opcode
// and the string argument. Since PUSH responds with OK if the push succeeds,
// we parse that response as a bool value and return it.
bool RemoteKeyValueStore::push(const std::string& value) {
    // Encode the bytes of the string argument, including its length.
    std::vector<std::uint8_t> arguments{};
    if (!appendString(arguments, value)) {
        return false;
    }

    // Send and parse a PUSH command, expecting to receive back an OK ("Status") response.
    return sendStatusCommand(RequestOpcode::Push, arguments);
}

std::optional<std::string> RemoteKeyValueStore::pop() {
    // Send and parse a POP command, expecting to receive back a string VALUE response.
    return sendValueCommand(RequestOpcode::Pop);
}

bool RemoteKeyValueStore::insert(std::size_t index, const std::string& value) {
    if (index > static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max())) {
        return false;
    }

    std::vector<std::uint8_t> arguments{};
    appendInt32(arguments, static_cast<std::int32_t>(index));
    if (!appendString(arguments, value)) {
        return false;
    }

    return sendStatusCommand(RequestOpcode::Insert, arguments);
}

std::optional<std::string> RemoteKeyValueStore::remove(std::size_t index) {
    if (index > static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max())) {
        return std::nullopt;
    }

    std::vector<std::uint8_t> arguments{};
    appendInt32(arguments, static_cast<std::int32_t>(index));
    return sendValueCommand(RequestOpcode::Remove, arguments);
}

*/ 
std::optional<std::size_t> RemoteKeyValueStore::count() {
    auto response = m_stub.sendRequest(RequestOpcode::Count, {});
    if (!response.has_value()) {
        return std::nullopt;
    }

    return parseCountResponse(response.value());
}

/*
std::optional<std::string> RemoteKeyValueStore::get(std::size_t index) {
    if (index > static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max())) {
        return std::nullopt;
    }

    std::vector<std::uint8_t> arguments{};
    appendInt32(arguments, static_cast<std::int32_t>(index));
    return sendValueCommand(RequestOpcode::Get, arguments);
}

bool RemoteKeyValueStore::set(std::size_t index, const std::string& value) {
    if (index > static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max())) {
        return false;
    }

    std::vector<std::uint8_t> arguments{};
    appendInt32(arguments, static_cast<std::int32_t>(index));
    if (!appendString(arguments, value)) {
        return false;
    }

    return sendStatusCommand(RequestOpcode::Set, arguments);
}

bool RemoteKeyValueStore::swap(std::size_t firstIndex, std::size_t secondIndex) {
    if (firstIndex > static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max())
        || secondIndex > static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max())) {
        return false;
    }

    std::vector<std::uint8_t> arguments{};
    appendInt32(arguments, static_cast<std::int32_t>(firstIndex));
    appendInt32(arguments, static_cast<std::int32_t>(secondIndex));
    return sendStatusCommand(RequestOpcode::Swap, arguments);
}
*/ 
bool RemoteKeyValueStore::clear() {
    return sendStatusCommand(RequestOpcode::Clear);
}

bool RemoteKeyValueStore::isConnected() const {
    return m_stub.isConnected();
}
