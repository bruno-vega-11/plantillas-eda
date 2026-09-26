#include <stdexcept>
#include <vector>
#include <iostream>
#include <windows.h>
using namespace std;

// ============================================================
// PERSISTENT SEGMENT TREE
// Es parcial y total
// Implementada con punteros,
// donde root[H] = raiz de la version v del  SEGMENT TREE
// ============================================================
struct Node {
    int k;
    Node* left;
    Node* right;
};

struct persistentSegmentTree {
    vector<Node*> versiones;
    int n;

    Node* aux(vector<int>&A,int l,int r) {
        // Caso base: es una hoja == A[i] normal
        if (l==r) {
            return new Node(A[l],nullptr,nullptr);
        }
        int mid = (l+r)/2;
        // mitad izquierda: cubre [l,mid]
        Node* left = aux(A,l,mid);
        // mitad derecha: cubre [mid+1,r]
        Node* right = aux(A,mid+1,r);
        // Es un segmentree de suma
        Node* parent = new Node(left->k+right->k   ,left,right);
        return parent;
    }

    // node_old = puntero al nodo actual de la version base que recorremos
    // l,r  = rango de indices del arreglo que cubre el node actual
    // pos  = posicion del arreglo original que queremos modificar
    // val  = el nuevo valor que queremos colocar en la posicion
    Node* update(Node* node_old, int l, int r,int pos,int _k) {
        // se copia el nodo old
        Node* node_new = new Node(node_old->k,node_old->left,node_old->right);
        // si estamos en una hoja
        if (l == r) {
            // solo se modifica el valor
            node_new->k = _k;
        }
        // si no es hoja, hay que bajar
        // ojo que por el camino que no baja lo deja igual
        // (por eso hicimos una copia linea 44)
        else {
            int mid = (l+r)/2;
            // decide si bajar a la izq
            if (pos <= mid) {
                node_new->left = update(node_old->left,l,mid,pos,_k);
            }
            // o a la derecha
            else {
                node_new->right = update(node_old->right,mid+1,r,pos,_k);
            }
            // para nodos intermedio, se recalcula funcion merge (en este caso suma)
            node_new->k = node_new->left->k + node_new->right->k;
        }
        return node_new;
    }

    void new_version(int version, int pos, int k_) {
        Node* new_root = update(versiones[version],0,n-1,pos,k_);
        versiones.push_back(new_root);
    }

    // node = raiz de una version S
    // l,r = rango que cubre el nodo actual
    // ql,qr = rango de la consulta


    int query(Node* node, int l, int r, int ql, int qr) {
        if (qr <= l || r <= ql || node == nullptr) {
            return 0; // elemento fuera de rango
        }

        if (ql <= l && r <= qr ) {
            return node->k;
        }

        int mid = (l+r)/2;
        int suma_izq = query(node->left,l,mid,ql,qr);
        int suma_der = query(node->right,mid+1,r,ql,qr);

        return suma_izq+suma_der;
    }

    int query_version(int version,int ql,int qr) {
        return query(versiones[version],0,n-1,ql,qr);
    }

    persistentSegmentTree(vector<int>& A) {
        if (A.empty()) return;
        n = A.size();
        Node* v0 = aux(A,0,n - 1);
        versiones.push_back(v0);
    }

    void print_ascii(Node* node, int l, int r, const string& prefix, bool isLeft) {
        if (node == nullptr) return;
        cout << prefix;
        cout << (isLeft ? "├── " : "└── ");
        cout << "[" << l << "," << r << "] -> val: " << node->k << " (ptr: " << node << ")\n";

        if (l != r) {
            int mid = (l + r) / 2;
            string newPrefix = prefix + (isLeft ? "│   " : "    ");
            print_ascii(node->left, l, mid, newPrefix, true);
            print_ascii(node->right, mid + 1, r, newPrefix, false);
        }
    }

    void print_version(int version) {
        cout << "\n=============================================\n";
        cout << "   ESTRUCTURA DEL ÁRBOL - VERSIÓN " << version << "\n";
        cout << "=============================================\n";
        print_ascii(versiones[version], 0, n - 1, "", false);
        cout << endl;
    }
};

int main() {
    SetConsoleOutputCP(CP_UTF8);
    vector A = {1, 2, 3, 4};
    // VERSION 0;
    persistentSegmentTree tree(A);
    tree.print_version(0);
    cout << "Creando v1 (Update pos 2 = 10 sobre v0)...\n";
    tree.new_version(0, 2, 10);
    tree.print_version(1);
    return 0;
}