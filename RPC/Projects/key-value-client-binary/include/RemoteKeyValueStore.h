#pragma once
#include "RemoteListStub.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

// Abstracts a list of strings, which secretly communicates with a list-server-binary instance
// where the real list is stored.
class RemoteKeyValueStore {
public:
    explicit RemoteKeyValueStore(const std::string& host = "127.0.0.1", int port = 9090);
    ~RemoteKeyValueStore() = default;

    RemoteKeyValueStore(const RemoteKeyValueStore&) = delete;
    RemoteKeyValueStore& operator=(const RemoteKeyValueStore&) = delete;
    RemoteKeyValueStore(RemoteKeyValueStore&&) = default;
    RemoteKeyValueStore& operator=(RemoteKeyValueStore&&) = default;

    // New Stuff
    bool put(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key);
    bool exists(const std::string& key);
    std::optional<std::vector<std::string>> keys();
    // New Stuff

    // Old Stuff
    // bool push(const std::string& value);
    // std::optional<std::string> pop();
    // bool insert(std::size_t index, const std::string& value);
    // std::optional<std::string> remove(std::size_t index);
    std::optional<std::size_t> count();
    // std::optional<std::string> get(std::size_t index);
    // bool set(std::size_t index, const std::string& value);
    // bool swap(std::size_t firstIndex, std::size_t secondIndex);
    bool clear();
    bool isConnected() const;
    // Old Stuff

private:
    RemoteListStub m_stub;

    bool sendStatusCommand(RequestOpcode opcode, const std::vector<std::uint8_t>& arguments = {});
    std::optional<std::string> sendValueCommand(RequestOpcode opcode,
                                                const std::vector<std::uint8_t>& arguments = {});
    std::optional<std::vector<std::string>> sendKeysCommand(RequestOpcode opcode,
                                                        const std::vector<std::uint8_t>& arguments = {});
};

