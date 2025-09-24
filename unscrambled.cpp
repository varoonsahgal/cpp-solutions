#include <iostream>
#include <thread>
#include <vector>
#include <mutex>   // Needed for std::mutex

// ----------------------------------------------------------
// Create a global mutex object
// - This will act like a "gatekeeper"
// - Only one thread at a time can lock it
// - Protects the critical section (printing to std::cout)
// ----------------------------------------------------------
std::mutex coutMutex;

// Task function: prints a string character by character
void printString(const std::string& text) {
    // ----------------------------------------------------------
    // Lock the mutex before entering the critical section
    // - This guarantees exclusive access to std::cout
    // - If another thread already holds the lock, this call will
    //   BLOCK until the mutex becomes available
    // ----------------------------------------------------------
    coutMutex.lock();

    // ---------- CRITICAL SECTION START ----------
    for (char c : text) {
        std::cout << c;
        // Small delay increases chance of interleaving if no mutex
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    // ---------- CRITICAL SECTION END ------------

    // ----------------------------------------------------------
    // Unlock the mutex after leaving the critical section
    // - This signals that another thread may now lock the mutex
    // - Very important: forgetting to unlock causes a deadlock
    // ----------------------------------------------------------
    coutMutex.unlock();
}

int main() {
    // Strings that each thread will try to print
    std::vector<std::string> messages = {"ABC", "123", "xyz"};

    // A vector to hold our thread objects
    std::vector<std::thread> threads;

    // ----------------------------------------------------------
    // Create one thread for each string
    // - Each thread will run printString()
    // - All threads share the same global coutMutex
    // ----------------------------------------------------------
    for (const auto& msg : messages) {
        threads.emplace_back(printString, msg);
    }

    // ----------------------------------------------------------
    // Wait for all threads to finish before exiting main()
    // - join() makes sure the main thread does not exit early
    // ----------------------------------------------------------
    for (auto& t : threads) {
        t.join();
    }

    std::cout << std::endl;
    return 0;
}
