//
// Created by Usuario on 25/9/2026.
//

#include <vector>
#include <stdexcept>

using namespace std;

const int INF = 1e9;
// ============================================================
// MIN-HEAP
// Indices internos empiezan en 0
// ============================================================
class BinaryHeap {
private:
    vector<int> A;
    int heap_size = 0;

    // --- Indices ---
    static int parent(const int i) { return (i - 1) / 2; }
    static int left(const int i)   { return 2 * i + 1; }
    static int right(const int i)  { return 2 * i + 2; }

    // --- MIN-HEAPIFY(A, i) : O(log n) ---
    // Se asume que los subarboles izq. y der. de i ya son min-heaps,
    // pero A[i] puede violar la propiedad de heap.
    void minHeapify(int i) {
        int l = left(i);
        int r = right(i);
        int smallest = i;

        if (l < heap_size && A[l] < A[smallest])
            smallest = l;
        if (r < heap_size && A[r] < A[smallest])
            smallest = r;

        if (smallest != i) {
            swap(A[i], A[smallest]);
            minHeapify(smallest);
        }
    }

public:
    BinaryHeap() = default;
    explicit BinaryHeap(const vector<int> &A_): A(A_),heap_size(A_.size()) {
        builMinHeap();
    }

    // --- BUILD-MIN-HEAP(A) : O(n) ---
    void builMinHeap() {
        for (int i = heap_size / 2 - 1; i >= 0; --i)
            minHeapify(i);
    }

    // --- HEAP-MINIMUM(A) : O(1) ---
    int heapMinimum() const {
        if (heap_size < 1)
            throw underflow_error("heap underflow");
        return A[0];
    }

    // --- HEAP-EXTRACT-MIN(A) : O(log n) ---
    int heapExtractMin() {
        if (heap_size < 1)
            throw std::underflow_error("heap underflow");

        int min = A[0];
        A[0] = A[heap_size - 1];
        A.pop_back();
        heap_size--;
        minHeapify(0);
        return min;
    }

    // --- HEAP-DECREASE-KEY(A, i, key) : O(log n) ---
    // Disminuye el valor de la clave en la posicion i al nuevo valor 'key'
    void heapDecreseKey(int i, int key) {
        if (key > A[i])
            throw invalid_argument("nuevo valor es mayor que el actual");

        A[i] = key;
        while (i > 0 && A[parent(i)] > A[i]) {
            swap(A[i], A[parent(i)]);
            i = parent(i);
        }
    }

    // --- MIN-HEAP-INSERT(A, key) : O(log n) ---
    void minHeapInsert(int key) {
        A.push_back(INF);
        heap_size++;
        heapDecreseKey(heap_size - 1, key);
    }

    // --- Utilidades  ---
    int size() const { return heap_size; }
    bool empty() const { return heap_size == 0; }
};