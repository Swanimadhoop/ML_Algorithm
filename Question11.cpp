#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <pthread.h>

using namespace std;
using namespace chrono;

int n, key; // Array size and key element
vector<int> arr; // Global array

// Structure to pass data to threads
struct ThreadData {
    int start, end;
    long long sum;
    int found;
};

// Function to generate random array
void generateArray(int size) {
    arr.resize(size);
    for (int i = 0; i < size; i++)
        arr[i] = rand() % 1000; // Random numbers from 0-999
}

// Sequential function to find sum
long long findSumSequential() {
    long long sum = 0;
    for (int num : arr)
        sum += num;
    return sum;
}

// Sequential function to search key
bool searchKeySequential(int key) {
    for (int num : arr)
        if (num == key)
            return true;
    return false;
}

// Thread function for sum computation
void* findSumThread(void* arg) {
    ThreadData* data = (ThreadData*) arg;
    data->sum = 0;
    for (int i = data->start; i < data->end; i++)
        data->sum += arr[i];
    return NULL;
}

// Thread function for searching key
void* searchKeyThread(void* arg) {
    ThreadData* data = (ThreadData*) arg;
    data->found = 0;
    for (int i = data->start; i < data->end; i++) {
        if (arr[i] == key) {
            data->found = 1;
            break;
        }
    }
    return NULL;
}

// Multithreaded sum computation
long long findSumParallel(int numThreads) {
    pthread_t threads[numThreads];
    ThreadData threadData[numThreads];

    int chunkSize = n / numThreads;
    long long totalSum = 0;

    // Creating threads
    for (int i = 0; i < numThreads; i++) {
        threadData[i].start = i * chunkSize;
        threadData[i].end = (i == numThreads - 1) ? n : (i + 1) * chunkSize;
        pthread_create(&threads[i], NULL, findSumThread, &threadData[i]);
    }

    // Joining threads and summing results
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
        totalSum += threadData[i].sum;
    }

    return totalSum;
}

// Multithreaded search for key
bool searchKeyParallel(int numThreads) {
    pthread_t threads[numThreads];
    ThreadData threadData[numThreads];

    int chunkSize = n / numThreads;
    bool found = false;

    // Creating threads
    for (int i = 0; i < numThreads; i++) {
        threadData[i].start = i * chunkSize;
        threadData[i].end = (i == numThreads - 1) ? n : (i + 1) * chunkSize;
        pthread_create(&threads[i], NULL, searchKeyThread, &threadData[i]);
    }

    // Joining threads
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
        if (threadData[i].found)
            found = true;
    }

    return found;
}

int main() {
    srand(time(0)); // Seed random generator

    // Input array size and key to search
    cout << "Enter the size of the array: ";
    cin >> n;
    cout << "Enter the key to search: ";
    cin >> key;

    // Generate array
    generateArray(n);

    // Sequential execution
    auto startSeq = high_resolution_clock::now();
    long long sumSeq = findSumSequential();
    bool foundSeq = searchKeySequential(key);
    auto endSeq = high_resolution_clock::now();
    auto durationSeq = duration_cast<microseconds>(endSeq - startSeq);

    // Multithreading execution
    int numThreads = 4; // Number of threads
    auto startPar = high_resolution_clock::now();
    long long sumPar = findSumParallel(numThreads);
    bool foundPar = searchKeyParallel(numThreads);
    auto endPar = high_resolution_clock::now();
    auto durationPar = duration_cast<microseconds>(endPar - startPar);

    // Results
    cout << "\nSequential Execution:\n";
    cout << "Sum: " << sumSeq << "\n";
    cout << "Key Found: " << (foundSeq ? "Yes" : "No") << "\n";
    cout << "Execution Time: " << durationSeq.count() << " microseconds\n";

    cout << "\nMultithreaded Execution (" << numThreads << " threads):\n";
    cout << "Sum: " << sumPar << "\n";
    cout << "Key Found: " << (foundPar ? "Yes" : "No") << "\n";
    cout << "Execution Time: " << durationPar.count() << " microseconds\n";

    return 0;
}
