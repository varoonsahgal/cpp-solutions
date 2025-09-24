#include <iostream>
#include <thread>
#include <vector>

// Task function: prints a string character by character
void printString(const std::string& text) {
    for (char c : text) {
        std::cout << c;
        // Small delay makes interleaving more likely
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int main() {
    // Strings to print
    std::vector<std::string> messages = {"ABC", "123", "xyz"};

    // Create threads
    std::vector<std::thread> threads;
    for (const auto& msg : messages) {
        threads.emplace_back(printString, msg);
    }

    // Join threads
    for (auto& t : threads) {
        t.join();
    }

    std::cout << std::endl;
    return 0;
}
