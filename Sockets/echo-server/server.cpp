#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <chrono>
#include <iomanip>
#include <mutex>
#include <thread>

namespace {
    constexpr int PORT{9090};
    constexpr int BACKLOG{5};
    constexpr int BUFFER_SIZE{1024};

    void close_socket(int fd) {
        if (fd >= 0) {
            close(fd);
        }
    }

    void fail(const std::string& message) {
        throw std::runtime_error{message + ": " + std::strerror(errno)};
    }
}

void handleClient(int client_fd, int& counter, std::mutex& mtx){
        bool run = true;
        // Handle Mutex Increments
        mtx.lock();
        counter += 1;
        if (counter > 1){
            run = false;
        }
        mtx.unlock();

        char buffer[BUFFER_SIZE]{};

        while (true) {
            // Receive up to BUFFER_SIZE-1 bytes from the client.
            ssize_t bytes_received{recv(client_fd, buffer, BUFFER_SIZE - 1, 0)};

            if (bytes_received < 0) {
                std::cerr << "recv failed: " << std::strerror(errno) << "\n";
                break;
            }

            if (bytes_received == 0) {
                std::cout << "Client disconnected.\n";
                break;
            }

            // We know we got a successful message from the client.

            buffer[bytes_received] = '\0';

            std::string message{buffer};

            if (run == false){
                message == "ERROR: Too Many Clients\n";
                break;
            }
            // We will now check the message to see if it fits out requirments
            if (message.rfind("ECHO", 0) == 0){
                // Checks to see if the message begins with ECHO. If so, it will respond with
                // the message after ECHO
                message.erase(0, 4);
            }
            else if (message.rfind("TIME", 0) == 0){
                // Checks to see if the message begins with TIME. Post Server time
                // Converted into the Proper Format
                // Get the Time
                auto now = std::chrono::system_clock::now();

                std::time_t time_now = std::chrono::system_clock::to_time_t(now);

                // This makes the code match the local time
                std::tm tm_now;
                #if defined(_MSC_VER)
                    localtime_s(&tm_now, &time_now); // Windows
                #else
                    localtime_r(&time_now, &tm_now); // POSIX/Linux
                #endif

                // Format
                std::stringstream ss;
                ss << std::put_time(&tm_now, "%H:%M:%S");
                message = ss.str();
                // Makes it so that the Time Messages match the formating that the others would have
                message += '\n';
            }
            else if (message.rfind("HELP", 0) == 0){
                // The Help Command
                message = "ECHO: Prints the message that is after the word\n";
                message += "TIME: Prints the current server time in the ISO 8601 time format of \"hh:mm:ss\"\n";
                message += "HELP: Describes the operations supported by the server\n";
                message += "QUIT: CLoses the client connection\n";

            }else if (message.rfind("CLIENTS", 0) == 0){
                // Implement
                // Lock Mutex
                std::lock_guard<std::mutex> lock(mtx);
                message = "Number of Clients: ";
                message += std::to_string(counter);
                message += "\n";
                // Message is the bumber of clients
                // Unlock Mutex
            }else if (message.rfind("QUIT", 0) == 0){
                message = "QUIT\n";
                break;
            }
            else{
                message = "ERROR: unknown command\n";
            }

            // Echo the message to stdout.
            std::cout << "Received: " << message;

            // Respond to the client with the same message.
            ssize_t bytes_sent{send(client_fd, message.c_str(), message.size(), 0)};

            if (bytes_sent < 0) {
                std::cerr << "send failed: " << std::strerror(errno) << "\n";
                break;
            }
        }

        close_socket(client_fd);
        std::lock_guard<std::mutex> lock(mtx);
        counter -= 1;
        std::cout << "Client disconnected.\n";
}

int main() {
    int counter = 0; // Keeps track of number of clients in
    std::mutex mtx;
    try {
        // Initialize an Internet (AF_INET) socket using a reliable TCP connection (SOCK_STREAM).
        int server_fd{socket(AF_INET, SOCK_STREAM, 0)};
        if (server_fd < 0) {
            fail("socket failed");
        }

        int reuse{1};
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
            close_socket(server_fd);
            fail("setsockopt failed");
        }

        // Define the address at which the socket will accept connection requests.
        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET; // Internet connections only.
        server_addr.sin_addr.s_addr = INADDR_ANY; // Any networking interface on the computer (Wi-Fi, ethernet, etc.)
        server_addr.sin_port = htons(PORT); // Convert the port number from local-machine endianness to Network (big) endianness.

        // Bind our socket to the desired address.
        if (bind(server_fd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
            close_socket(server_fd);
            fail("bind failed");
        }

        // Wait for a connection request.
        if (listen(server_fd, BACKLOG) < 0) {
            close_socket(server_fd);
            fail("listen failed");
        }

        std::cout << "Server listening on port " << PORT << "...\n";


        // ACCEPTING CLIENTS HERE
        while (true){
        // Initialize a second socket to use when responding to a client.
        sockaddr_in client_addr{};
        socklen_t client_len{sizeof(client_addr)};

        // Accept the incoming request, and initialize the response socket.
        // This will block the process until a client connects.
        int client_fd{accept(
            server_fd,
            reinterpret_cast<sockaddr*>(&client_addr),
            &client_len
        )};

        if (client_fd < 0) {
            std::cerr << "accept failed: " << std::strerror(errno) << '\n';
            continue;
        }
        
        char client_ip[INET_ADDRSTRLEN]{};
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        
        std::cout << "Client connected from "
                  << client_ip << ":"
                  << ntohs(client_addr.sin_port)
                  << "\n";

        // Handle the Client in it's own thread
        std::thread clientThread(handleClient, client_fd, std::ref(counter), std::ref(mtx));
        clientThread.detach();
        }

        // Around this point, make it into an iterative server, similar to the Key-Value Server from Day 2
        // Weap the code that accepts a client and calls handle client in a while loop

        close_socket(server_fd);
    }
    catch (const std::exception& ex) {
        std::cerr << "Server error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}