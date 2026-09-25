#include <stdexcept>
#include <limits>

// ============================================================
// BINOMIAL HEAP
// Estructura enlazada, cada nodo tiene key, degree, parent,
// child, sibling.
// ============================================================

const int INF = 1e9;

using namespace std;

struct Node {
    int key;
    int degree;
    Node* parent;
    Node* child;
    Node* sibling;

    Node(int k) : key(k), degree(0), parent(nullptr), child(nullptr), sibling(nullptr) {}
};

class BinomialHeap {
private:
    Node* head; // head[H]: primer nodo de la lista de raices
                // osea el arbol con menor degree (Ej: B0)

    // --- binomialLink(y, z) : hace de y hijo de z en O(1) ---
    // Solo si: degree[y] == degree[z]
    void binomialLink(Node* y, Node* z) {
        y->parent = z;
        y->sibling = z->child;
        z->child = y;
        z->degree++;
    }

    // --- binomialHeapMerge(h1, h2) : mezcla dos listas de raices
    // ordenadas por grado en una sola lista ordenada por grado. O(log n) ---
    Node* binomialHeapMerge(Node* h1, Node* h2) {
        if (!h1) return h2;
        if (!h2) return h1;

        Node* head_ = nullptr;
        Node** tail = &head_;

        while (h1 && h2) {
            if (h1->degree <= h2->degree) {
                *tail = h1;
                h1 = h1->sibling;
            } else {
                *tail = h2;
                h2 = h2->sibling;
            }
            tail = &((*tail)->sibling);
        }
        *tail = h1 ? h1 : h2;
        return head_;
    }

public:
    // --- makeHeap() : O(1) ---
    BinomialHeap() : head(nullptr) {}

    bool empty() const { return head == nullptr; }

    // --- unionHeaps(other) : une this con other, deja other vacio. O(log n) ---
    void unionHeaps(BinomialHeap& other) {
        Node* newHead = binomialHeapMerge(head, other.head);
        head = nullptr;
        other.head = nullptr;

        if (!newHead) {
            head = newHead;
            return;
        }

        Node* prevX = nullptr;
        Node* x = newHead;
        Node* nextX = x->sibling;

        while (nextX != nullptr) {
            if ((x->degree != nextX->degree) ||
                (nextX->sibling != nullptr && nextX->sibling->degree == x->degree)) {
                // Caso 1 y 2: no se combinan todavia
                prevX = x;
                x = nextX;
            } else if (x->key <= nextX->key) {
                // Caso 3: x pasa a ser raiz, nextX se cuelga de x
                x->sibling = nextX->sibling;
                binomialLink(nextX, x);
            } else {
                // Caso 4: nextX pasa a ser raiz, x se cuelga de nextX
                if (prevX == nullptr)
                    newHead = nextX;
                else
                    prevX->sibling = nextX;
                binomialLink(x, nextX);
                x = nextX;
            }
            nextX = x->sibling;
        }
        head = newHead;
    }

    // --- insert(key) : O(log n) ---
    void insert(int key) {
        BinomialHeap hPrime;
        hPrime.head = new Node(key);
        unionHeaps(hPrime);
    }

    // --- minimum() : O(log n) ---
    int minimum() const {
        if (!head)
            throw std::underflow_error("heap vacio");

        Node* y = nullptr;
        Node* x = head;
        int min = INF;

        while (x != nullptr) {
            if (x->key < min) {
                min = x->key;
                y = x;
            }
            x = x->sibling;
        }
        return y->key;
    }

    // --- extractMin() : O(log n) ---
    int extractMin() {
        if (!head)
            throw underflow_error("heap vacio");

        // 1. Encontrar la raiz x con la clave minima en la lista de raices
        Node* prevMin = nullptr;
        Node* prevX = nullptr;
        Node* x = head;
        Node* minNode = head;
        int min = head->key;

        while (x != nullptr) {
            if (x->key < min) {
                min = x->key;
                minNode = x;
                prevMin = prevX;
            }
            prevX = x;
            x = x->sibling;
        }

        // 2. Quitar minNode de la lista de raices de H
        if (prevMin == nullptr)
            head = minNode->sibling;
        else
            prevMin->sibling = minNode->sibling;

        // 3. Invertir la lista de hijos de minNode para formar hPrime
        Node* child = minNode->child;
        Node* newHead = nullptr;
        while (child != nullptr) {
            Node* next = child->sibling;
            child->sibling = newHead;
            child->parent = nullptr;
            newHead = child;
            child = next;
        }

        BinomialHeap hPrime;
        hPrime.head = newHead;

        // 4. H = unionHeaps(H, hPrime)
        unionHeaps(hPrime);

        int result = minNode->key;
        delete minNode;
        return result;
    }

    // --- decreaseKey(x, k) : O(log n) ---
    // x es un puntero a un nodo interno (por ejemplo, guardado al insertar)
    void decreaseKey(Node* x, int k) {
        if (k > x->key)
            throw std::invalid_argument("nuevo valor es mayor que el actual");

        x->key = k;
        Node* y = x;
        Node* z = y->parent;

        while (z != nullptr && y->key < z->key) {
            std::swap(y->key, z->key);
            y = z;
            z = y->parent;
        }
    }

    // --- deleteKey(x) : O(log n) ---
    void deleteKey(Node* x) {
        decreaseKey(x, -INF);
        extractMin();
    }
};