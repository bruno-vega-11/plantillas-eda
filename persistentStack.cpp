#include <stdexcept>
#include <vector>
#include <iostream>

using namespace std;

// ============================================================
// PERSISTENT STACK
// Es parcial y total
// Implementada como una lista enlazada
// donde head[H] = top de un Stack
// ============================================================


// Se puede implementar un stack como una lista enlazada
// Donde el head de la lista es el top del heap
// El unico punto donde ocurren modificaciones en el top
// El camino desde la raiz hasta el punto de cambio es de tamaño 1.
struct Node {
    int k;
    Node* siguiente;
};

struct persistentStack {
    vector<Node*> versiones;

    Node* push_persistent(Node* S,int x) {
        Node* new_version = new Node(x,S);
        versiones.push_back(new_version);
        return new_version;
    }

    Node* pop_persistent(Node* S) {
        if (S == nullptr) return nullptr;
        Node* new_version = S->siguiente;
        versiones.push_back(new_version);
        return  new_version;
    }

    Node* queue_version(int i) {
        if (i < 0 || i >= versiones.size())
            throw std::invalid_argument("indice no válido");
        return versiones[i];
    }

    int top(int version) {
        Node* head = queue_version(version);
        if (head == nullptr)
            throw std::runtime_error("stack vacio");
        return head->k;
    }

    void print_version(int version) {
        Node* head = queue_version(version);
        cout << "Version: " << version << endl;
        while (head!=nullptr) {
            cout << "[" << head->k << "] -> ";
            head = head->siguiente;
        }
        cout << "nulltpr" << endl;
    }

    persistentStack() {
        versiones.push_back(nullptr);
    }
};

int main() {

}