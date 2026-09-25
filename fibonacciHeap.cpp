#include <vector>
#include <stdexcept>

using namespace std;

const int INF = 1e9;

// ============================================================
// FIBONACCI HEAP
// Cada nodo vive en una lista circular doblemente ligada (left/right).
// child apunta a UN hijo cualquiera (raiz de la lista circular de hijos).
// ============================================================
struct Node {
    int key;
    int degree;
    bool mark;
    Node* parent;
    Node* child;
    Node* left;
    Node* right;

    Node(int k) : key(k), degree(0), mark(false),
                parent(nullptr), child(nullptr) {
        left = right = this; // lista circular de un solo elemento
    }
};

class FibonacciHeap {
private:
    Node* minNode; // min[H]
    int n;         // n[H]

    // --- Inserta x (solo, como lista circular de 1) en la lista de y ---
    void spliceIntoList(Node* y, Node* x) {
        // x se inserta a la izquierda de y en la lista circular de y
        x->left = y->left;
        x->right = y;
        y->left->right = x;
        y->left = x;
    }

    // --- Quita x de la lista circular en la que vive ---
    void removeFromList(Node* x) {
        x->left->right = x->right;
        x->right->left = x->left;
        x->left = x->right = x; // lo deja como lista de 1
    }

    // --- fibHeapLink(y, x) : hace de y hijo de x. O(1) ---
    // Precondicion: se llama dentro de consolidate, y y x son raices.
    void fibHeapLink(Node* y, Node* x) {
        removeFromList(y);               // sacar y de la lista de raices
        if (x->child == nullptr) {
            x->child = y;
            y->left = y->right = y;
        } else {
            spliceIntoList(x->child, y);
        }
        y->parent = x;
        x->degree++;
        y->mark = false;
    }

    // --- consolidate(H) : O(D(n)) amortizado, O(log n) ---
    void consolidate() {
        vector<Node*> A;

        // Recorrer la lista de raices (hay que copiar los punteros primero,
        // porque la lista se modifica durante el recorrido)
        vector<Node*> roots;
        if (minNode != nullptr) {
            Node* w = minNode;
            do {
                roots.push_back(w);
                w = w->right;
            } while (w != minNode);
        }

        for (Node* w : roots) {
            Node* x = w;
            int d = x->degree;
            if (d>=static_cast<int>(A.size())) {
                A.resize(d+1,nullptr);
            }

            while (A[d] != nullptr) {
                Node* y = A[d];
                if (x->key > y->key)
                    std::swap(x, y);
                fibHeapLink(y, x);
                A[d] = nullptr;
                d++;
                if (d >= static_cast<int>(A.size())) {
                    A.resize(d+1,nullptr);
                }

            }
            A[d] = x;
        }

        minNode = nullptr;
        for (Node* node : A) {
            if (node != nullptr) {
                node->left = node->right = node;
                if (minNode == nullptr) {
                    minNode = node;
                } else {
                    spliceIntoList(minNode, node);
                    if (node->key < minNode->key)
                        minNode = node;
                }
            }
        }
    }

    // --- cut(x, y) : quita x de la lista de hijos de y, lo pone en la raiz. O(1) ---
    void cut(Node* x, Node* y) {
        if (x->right == x)
            y->child = nullptr;
        else {
            if (y->child == x)
                y->child = x->right;
            removeFromList(x);
        }
        y->degree--;

        spliceIntoList(minNode, x);
        x->parent = nullptr;
        x->mark = false;
    }

    // --- cascadingCut(y) : O(1) amortizado ---
    void cascadingCut(Node* y) {
        Node* z = y->parent;
        if (z != nullptr) {
            if (!y->mark) {
                y->mark = true;
            } else {
                cut(y, z);
                cascadingCut(z);
            }
        }
    }

public:
    // --- makeHeap() : O(1) ---
    FibonacciHeap() : minNode(nullptr), n(0) {}

    bool empty() const { return minNode == nullptr; }
    int size() const { return n; }

    // --- insert(key) : O(1). Devuelve el nodo, util para decreaseKey/deleteKey ---
    using NodePtr = Node*;

    NodePtr insert(int key) {
        Node* x = new Node(key);
        if (minNode == nullptr) {
            minNode = x;
        } else {
            spliceIntoList(minNode, x);
            if (x->key < minNode->key)
                minNode = x;
        }
        n++;
        return x;
    }

    // --- minimum() : O(1) ---
    int minimum() const {
        if (minNode == nullptr)
            throw underflow_error("heap vacio");
        return minNode->key;
    }

    // --- unionHeaps(other) : concatena las listas de raices. O(1) ---
    // Deja "other" vacio, igual que en CLRS.
    void unionHeaps(FibonacciHeap& other) {
        if (minNode == nullptr) {
            minNode = other.minNode;
        } else if (other.minNode != nullptr) {
            // concatenar las dos listas circulares de raices
            Node* thisLeft = minNode->left;
            Node* otherLeft = other.minNode->left;

            thisLeft->right = other.minNode;
            other.minNode->left = thisLeft;
            otherLeft->right = minNode;
            minNode->left = otherLeft;

            if (other.minNode->key < minNode->key)
                minNode = other.minNode;
        }
        n += other.n;
        other.minNode = nullptr;
        other.n = 0;
    }

    // --- extractMin() : O(log n) amortizado ---
    int extractMin() {
        Node* z = minNode;
        if (z == nullptr)
            throw underflow_error("heap vacio");

        // 1. Subir todos los hijos de z a la lista de raices
        if (z->child != nullptr) {
            Node* c = z->child;
            vector<Node*> children;
            Node* w = c;
            do {
                children.push_back(w);
                w = w->right;
            } while (w != c);

            for (Node* child : children) {
                removeFromList(child);
                spliceIntoList(minNode, child);
                child->parent = nullptr;
            }
        }

        // 2. Quitar z de la lista de raices (guardando su vecino ANTES)
        Node* zRight = z->right;
        bool wasOnlyRoot = (zRight == z);
        removeFromList(z);

        if (wasOnlyRoot) {
            minNode = nullptr;
        } else {
            minNode = zRight;
            consolidate();
        }

        n--;
        int result = z->key;
        delete z;
        return result;
    }

    // --- decreaseKey(x, k) : O(1) amortizado ---
    void decreaseKey(NodePtr x, int k) {
        if (k > x->key)
            throw invalid_argument("nuevo valor es mayor que el actual");

        x->key = k;
        Node* y = x->parent;
        if (y != nullptr && x->key < y->key) {
            cut(x, y);
            cascadingCut(y);
        }
        if (x->key < minNode->key)
            minNode = x;
    }

    // --- deleteKey(x) : O(log n) amortizado ---
    void deleteKey(NodePtr x) {
        decreaseKey(x, -INF);
        extractMin();
    }
};