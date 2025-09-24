// -----------------------------------------------------------------------------
// PURPOSE OF THIS PROGRAM
// -----------------------------------------------------------------------------
// This example demonstrates an "internally synchronized class" in C++.
//
// - The class (Vector) contains both a data member (std::vector<int>) 
//   and a mutex (std::mutex).
// - Every time a member function accesses the vector, it first locks the mutex.
// - This prevents multiple threads from reading/writing the vector at the same time.
// - By doing this, we avoid data races and undefined behavior.
//
// The main() function then launches three threads that:
//   1. Push integers into the vector.
//   2. Print out the vector contents.
// Each function call is synchronized with the mutex, so the output is safe
// (though still possibly interleaved in time).
// -----------------------------------------------------------------------------

#include <thread>     // std::thread
#include <mutex>      // std::mutex
#include <vector>     // std::vector
#include <iostream>   // std::cout
#include <chrono>     // std::this_thread::sleep_for

using namespace std::literals; // Enables use of "50ms" style literals

// -----------------------------------------------------------------------------
// A class which is internally synchronized
// -----------------------------------------------------------------------------
// - The class holds both a vector<int> and a mutex.
// - push_back() and print() lock the mutex before accessing the vector.
// - This ensures safe concurrent access across multiple threads.
// -----------------------------------------------------------------------------
class Vector {
    std::mutex mut;          // Private mutex to protect access to vec
    std::vector<int> vec;    // The underlying data container
public:
    // -------------------------------------------------------------------------
    // Adds a new integer to the vector.
    // Mutex is locked before modifying vec, and unlocked afterwards.
    // -------------------------------------------------------------------------
    void push_back(const int& i)
    {
        mut.lock();              // Acquire the lock → only one thread can enter
        // ----- Start of critical section -----
        vec.push_back(i);        // Safe modification of the vector
        // ----- End of critical section -------
        mut.unlock();            // Release the lock → another thread may enter
    }

    // -------------------------------------------------------------------------
    // Prints all elements in the vector.
    // Mutex is locked while iterating through vec to prevent conflicts
    // if another thread modifies vec at the same time.
    // -------------------------------------------------------------------------
    void print() {
        mut.lock();              // Acquire the lock
        // ----- Start of critical section -----
        for (auto i : vec) {
            std::cout << i << ", ";  // Safe read of vector elements
        }
        std::cout << std::endl;
        // ----- End of critical section -------
        mut.unlock();            // Release the lock
    }
};

// -----------------------------------------------------------------------------
// Worker function for each thread.
// - Repeatedly pushes integers into the shared Vector.
// - Sleeps for 50ms to simulate work and encourage interleaving.
// - Prints the vector contents after each insertion.
// -----------------------------------------------------------------------------
void func(Vector& vec)
{
    for (int i = 0; i < 5; ++i) {
        vec.push_back(i);                 // Add i to the vector (thread-safe)
        std::this_thread::sleep_for(50ms);// Pause → increases chance of overlap
        vec.print();                      // Print current vector contents
    }
}

// -----------------------------------------------------------------------------
// Program entry point.
// - Creates a single Vector object (shared by all threads).
// - Launches 3 threads, each running func() on the same Vector.
// - Waits for all threads to finish using join().
// -----------------------------------------------------------------------------
int main()
{
    Vector vec;  // Shared object, internally synchronized

    // Launch three threads that all operate on the same Vector
    std::thread thr1(func, std::ref(vec));
    std::thread thr2(func, std::ref(vec));
    std::thread thr3(func, std::ref(vec));

    // Wait for all threads to complete before exiting main()
    thr1.join(); 
    thr2.join(); 
    thr3.join();
}
