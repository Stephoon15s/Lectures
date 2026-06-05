#include "RequestHandlers.h"
#include "ResponseHandlers.h"
#include <utility>

std::vector<std::uint8_t> handlePutRequest(MessageReader& reader, SharedStore& store){
    // Set up vars
    std::optional<std::string> message{reader.readString()};
    std::string seperator = " ";

    std::string key;
    std::string value;

    // Check if we have the appropriate vals
    if (!message.has_value() || !reader.isAtEnd()) {
        return buildErrorResponse("PUT requires Key and Value");
    }
    
    // Split the String into key and val. 
    std::size_t pos = message.find(seperator);
    if (pos != std::string::npos){
        key = message.substr(0, pos);
        value = message.substr(pos + seperator.length());
    }else{
        return buildErrorResponse("PUT requires Key and Value");
    }
    // Put the key and value pair into the store
    {
        std::lock_guard<std::mutex> lock{store.mutex}; 
        store.values[key] = value;
    }

    return buildStatusResponse(ResponseOpcode::Ok);
}

std::vector<std::uint8_t> handleGetRequest(MessageReader& reader, SharedStore& store){
    // Check if there is a key
    std::optional<std::string> key{reader.readString()};
    if (!message.has_value() || !reader.isAtEnd()) {
        return buildErrorResponse("GET requires a key");
    }

    std::string value{};
    {
        std::lock_guard<std::mutex> lock{store.mutex};
        // Check if it even exists.
        auto it{store.values.find(key)};
        if (it == store.values.end()) {
            return buildErrorResponse("Invalid Key")
        }
        // Gets the value
        value = store.values[getIndex];
    }

    return buildValueResponse(value);

}
std::vector<std::uint8_t> handleDeleteRequest(MessageReader& reader, SharedStore& store){
    // Check if there is a key
    std::optional<std::string> key{reader.readString()};
    if (!message.has_value() || !reader.isAtEnd()) {
        return buildErrorResponse("Delete requires a key");
    }

    std::size_t removed;
    {
        // Handle Deletion
        std::lock_guard<std::mutex> lock{store.mutex};
        removed{store.erase(key)};
        if (removed == 0) {
            return buildErrorResponse("Invalid Key");
        }
    }
    return buildValueResponse(removed);
}
std::vector<std::uint8_t> handleCountRequest(MessageReader& reader, SharedStore& store){
    // Implement

}
std::vector<std::uint8_t> handleExistsRequest(MessageReader& reader, SharedStore& store){
    // Implement

}
std::vector<std::uint8_t> handleClearRequest(MessageReader& reader, SharedStore& store){
    // Implement

}
std::vector<std::uint8_t> handleKeysRequest(MessageReader& reader, SharedStore& store){
    // Implement

}
std::vector<std::uint8_t> handleQuitRequest(MessageReader& reader, SharedStore& store){
    // Implement

}
std::vector<std::uint8_t> handleNotFoundRequest(MessageReader& reader, SharedStore& store){
    // Implement

}


// OLD STUFF
/*
std::vector<std::uint8_t> handlePushRequest(MessageReader& reader, SharedStore& store) {
    // Push: 1 string argument.
    std::optional<std::string> value{reader.readString()};
    if (!value.has_value() || !reader.isAtEnd()) {
        return buildErrorResponse("PUSH requires value");
    }

    {
        std::lock_guard<std::mutex> lock{store.mutex};
        store.values.push_back(value.value());
    }

    return buildStatusResponse(ResponseOpcode::Ok);
}

std::vector<std::uint8_t> handlePopRequest(MessageReader& reader, SharedStore& store) {
    if (!reader.isAtEnd()) {
        return buildErrorResponse("POP takes no arguments");
    }

    std::string value{};
    {
        std::lock_guard<std::mutex> lock{store.mutex};
        if (store.values.empty()) {
            return buildErrorResponse("list is empty");
        }

        value = store.values.back();
        store.values.pop_back();
    }

    return buildValueResponse(value);
}

std::vector<std::uint8_t> handleInsertRequest(MessageReader& reader, SharedStore& store) {
    std::optional<std::int32_t> index{reader.readInt32()};
    std::optional<std::string> value{reader.readString()};
    if (!index.has_value() || !value.has_value() || index.value() < 0 || !reader.isAtEnd()) {
        return buildErrorResponse("INSERT requires index and value");
    }

    {
        std::lock_guard<std::mutex> lock{store.mutex};
        const std::size_t insertIndex{static_cast<std::size_t>(index.value())};
        if (insertIndex > store.values.size()) {
            return buildErrorResponse("invalid index");
        }

        store.values.insert(store.values.begin() + static_cast<std::ptrdiff_t>(insertIndex), value.value());
    }

    return buildStatusResponse(ResponseOpcode::Ok);
}

std::vector<std::uint8_t> handleRemoveRequest(MessageReader& reader, SharedStore& store) {
    std::optional<std::int32_t> index{reader.readInt32()};
    if (!index.has_value() || index.value() < 0 || !reader.isAtEnd()) {
        return buildErrorResponse("REMOVE requires index");
    }

    std::string value{};
    {
        std::lock_guard<std::mutex> lock{store.mutex};
        const std::size_t removeIndex{static_cast<std::size_t>(index.value())};
        if (removeIndex >= store.values.size()) {
            return buildErrorResponse("invalid index");
        }

        value = store.values[removeIndex];
        store.values.erase(store.values.begin() + static_cast<std::ptrdiff_t>(removeIndex));
    }

    return buildValueResponse(value);
}

std::vector<std::uint8_t> handleCountRequest(MessageReader& reader, SharedStore& store) {
    if (!reader.isAtEnd()) {
        return buildErrorResponse("COUNT takes no arguments");
    }

    std::size_t count{};
    {
        std::lock_guard<std::mutex> lock{store.mutex};
        count = store.values.size();
    }

    return buildCountResponse(count);
}

std::vector<std::uint8_t> handleGetRequest(MessageReader& reader, SharedStore& store) {
    std::optional<std::int32_t> index{reader.readInt32()};
    if (!index.has_value() || index.value() < 0 || !reader.isAtEnd()) {
        return buildErrorResponse("GET requires index");
    }

    std::string value{};
    {
        std::lock_guard<std::mutex> lock{store.mutex};
        const std::size_t getIndex{static_cast<std::size_t>(index.value())};
        if (getIndex >= store.values.size()) {
            return buildErrorResponse("invalid index");
        }

        value = store.values[getIndex];
    }

    return buildValueResponse(value);
}

std::vector<std::uint8_t> handleSetRequest(MessageReader& reader, SharedStore& store) {
    std::optional<std::int32_t> index{reader.readInt32()};
    std::optional<std::string> value{reader.readString()};
    if (!index.has_value() || !value.has_value() || index.value() < 0 || !reader.isAtEnd()) {
        return buildErrorResponse("SET requires index and value");
    }

    {
        std::lock_guard<std::mutex> lock{store.mutex};
        const std::size_t setIndex{static_cast<std::size_t>(index.value())};
        if (setIndex >= store.values.size()) {
            return buildErrorResponse("invalid index");
        }

        store.values[setIndex] = value.value();
    }

    return buildStatusResponse(ResponseOpcode::Ok);
}

std::vector<std::uint8_t> handleSwapRequest(MessageReader& reader, SharedStore& store) {
    std::optional<std::int32_t> firstIndex{reader.readInt32()};
    std::optional<std::int32_t> secondIndex{reader.readInt32()};
    if (!firstIndex.has_value() || !secondIndex.has_value()
        || firstIndex.value() < 0 || secondIndex.value() < 0 || !reader.isAtEnd()) {
        return buildErrorResponse("SWAP requires two indices");
    }

    {
        std::lock_guard<std::mutex> lock{store.mutex};
        const std::size_t left{static_cast<std::size_t>(firstIndex.value())};
        const std::size_t right{static_cast<std::size_t>(secondIndex.value())};
        if (left >= store.values.size() || right >= store.values.size()) {
            return buildErrorResponse("invalid index");
        }

        std::swap(store.values[left], store.values[right]);
    }

    return buildStatusResponse(ResponseOpcode::Ok);
}

std::vector<std::uint8_t> handleClearRequest(MessageReader& reader, SharedStore& store) {
    if (!reader.isAtEnd()) {
        return buildErrorResponse("CLEAR takes no arguments");
    }

    {
        std::lock_guard<std::mutex> lock{store.mutex};
        store.values.clear();
    }

    return buildStatusResponse(ResponseOpcode::Ok);
}

std::vector<std::uint8_t> handleQuitRequest(MessageReader& reader, SharedStore&) {
    if (!reader.isAtEnd()) {
        return buildErrorResponse("QUIT takes no arguments");
    }

    return buildStatusResponse(ResponseOpcode::Bye);
}
*/