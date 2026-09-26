#include <stdexcept>
#include <vector>
#include <iostream>

using namespace std;

// ============================================================
// PERSISTENT STACK
// Es parcial y total
// Implementada como una lista enlazada
// donde head[H] = top de un Stack
// cada actualizacion es O(1) memoria y O(1) tiempo
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
    persistentStack Pila;
    Node* version_0 = Pila.queue_version(0);
    Node* version_1 = Pila.push_persistent(version_0,5);
    Node* version_2 = Pila.push_persistent(version_1,10);
    Node* version_3 = Pila.push_persistent(version_2,20);
    Node* version_4 = Pila.pop_persistent(version_2);

    for (int i = 0; i <= 4; i++) {
        Pila.print_version(i);
    }

    return 0;
}