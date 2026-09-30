// Splay Tree siguiendo el pseudocódigo de CS3014 - Dynamic Optimality II (Semana 6)
// Algoritmos: Rotar, Splay, Buscar, Separar, Unir, Insertar, Eliminar
//
// Compilar: g++ -std=c++17 -O2 -o splay_tree splay_tree.cpp

#include <iostream>
#include <string>
#include <utility>
using namespace std;

struct Nodo {
    int valor;
    Nodo *izq = nullptr, *der = nullptr, *padre = nullptr;
    explicit Nodo(int v) : valor(v) {}
};

class SplayTree {
    Nodo* raiz = nullptr;

    // ---------- Rotar(v): sube v un nivel sobre su padre ----------
    // Decide sola la dirección: si v es hijo izquierdo hace rotación a la
    // derecha, si es hijo derecho hace rotación a la izquierda.
    // No toca 'raiz': el "arbol" de v se identifica porque su raíz no tiene padre.
    void rotar(Nodo* v) {
        Nodo* p = v->padre;
        Nodo* g = p->padre;

        if (v == p->izq) {              // rotación a la derecha
            p->izq = v->der;
            if (v->der) v->der->padre = p;
            v->der = p;
        } else {                        // rotación a la izquierda
            p->der = v->izq;
            if (v->izq) v->izq->padre = p;
            v->izq = p;
        }
        p->padre = v;
        v->padre = g;
        if (g) {
            if (g->izq == p) g->izq = v;
            else             g->der = v;
        }
    }

    // ---------- Algoritmo 1: Splay(x) ----------
    // "x != raiz"  <=>  x->padre != nullptr
    void splay(Nodo* x) {
        while (x->padre) {
            Nodo* p = x->padre;
            if (!p->padre) {                            // p es la raíz
                rotar(x);                               // Zig
            } else {
                Nodo* g = p->padre;
                if ((x == p->izq) == (p == g->izq)) {   // misma dirección
                    rotar(p); rotar(x);                 // Zig-Zig
                } else {                                // direcciones opuestas
                    rotar(x); rotar(x);                 // Zig-Zag
                }
            }
        }
    }

    // ---------- Algoritmo 4: Separar(k) ----------
    // Precondición: ya se ejecutó Buscar(k), así que 'raiz' es el nodo
    // encontrado (o su predecesor/sucesor).
    pair<Nodo*, Nodo*> separar(int k) {
        if (!raiz) return {nullptr, nullptr};
        Nodo* r = raiz;
        Nodo *I, *D;
        if (r->valor <= k) {
            I = r; D = r->der; r->der = nullptr;
            if (D) D->padre = nullptr;
        } else {
            D = r; I = r->izq; r->izq = nullptr;
            if (I) I->padre = nullptr;
        }
        return {I, D};
    }

    // ---------- Algoritmo 6: Unir(A, B) ----------
    // Precondición:
    Nodo* unir(Nodo* A, Nodo* B) {
        if (!A) return B;
        if (!B) return A;
        Nodo* v = A;
        while (v->der) v = v->der;      // v = máximo de A
        splay(v);                       // ahora v es raíz de A y no tiene hijo der.
        v->der = B;
        B->padre = v;
        return v;
    }

    void destruir(Nodo* v) {
        if (!v) return;
        destruir(v->izq);
        destruir(v->der);
        delete v;
    }

    void inorden(Nodo* v, string& out) const {
        if (!v) return;
        inorden(v->izq, out);
        out += to_string(v->valor) + " ";
        inorden(v->der, out);
    }

    // Forma del árbol en preorden: valor(izq,der), '.' = vacío
    void forma(Nodo* v, string& out) const {
        if (!v) { out += "."; return; }
        out += to_string(v->valor);
        if (v->izq || v->der) {
            out += "(";
            forma(v->izq, out);
            out += ",";
            forma(v->der, out);
            out += ")";
        }
    }

public:
    SplayTree() = default;
    SplayTree(const SplayTree&) = delete;
    SplayTree& operator=(const SplayTree&) = delete;
    ~SplayTree() { destruir(raiz); }

    // ---------- Algoritmo 3: Buscar(k) ----------
    // Si k no está, splaya el último nodo visitado (predecesor o sucesor de k).
    bool buscar(int k) {
        Nodo* v = raiz;
        Nodo* ultimo = nullptr;
        while (v) {
            ultimo = v;
            if (k == v->valor) {
                splay(v);
                raiz = v;
                return true;
            } else if (k < v->valor) {
                v = v->izq;
            } else {
                v = v->der;
            }
        }
        if (ultimo) {
            splay(ultimo);
            raiz = ultimo;
        }
        return false;
    }

    // ---------- Algoritmo 7: Insertar(k) ----------
    void insertar(int k) {
        if (buscar(k)) return;          // k ya estaba en el árbol
        auto [I, D] = separar(k);
        Nodo* x = new Nodo(k);
        x->izq = I; x->der = D;
        if (I) I->padre = x;
        if (D) D->padre = x;
        raiz = x;
    }

    // ---------- Algoritmo 9: Eliminar(k) ----------
    void eliminar(int k) {
        if (!buscar(k)) return;         // k no está en el árbol
        Nodo* r = raiz;
        Nodo* I = r->izq;
        Nodo* D = r->der;
        if (I) I->padre = nullptr;
        if (D) D->padre = nullptr;
        raiz = unir(I, D);
        delete r;
    }

    // ---------- Utilidades para depurar ----------
    string enOrden() const { string s; inorden(raiz, s); return s; }
    string forma() const   { string s; forma(raiz, s); return s; }
    int valorRaiz() const  { return raiz ? raiz->valor : -1; }
};

int main() {
    SplayTree t;

    // Insertar 1..7 deja una cadena de hijos izquierdos: 7 -> 6 -> ... -> 1
    for (int i = 1; i <= 7; i++) t.insertar(i);
    cout << "Tras insertar 1..7   : " << t.forma() << "\n";
    cout << "  en orden           : " << t.enOrden() << "\n\n";

    // Buscar(1) hace zig-zig, zig-zig, zig-zig, zig: aplana la cadena
    t.buscar(1);
    cout << "Tras buscar(1)       : " << t.forma() << "   (raiz = " << t.valorRaiz() << ")\n";

    // Buscar un valor que no está splaya su predecesor/sucesor
    bool esta = t.buscar(100);
    cout << "buscar(100)          : " << (esta ? "esta" : "no esta")
         << ", nueva raiz = " << t.valorRaiz() << "\n\n";

    // Eliminar
    t.eliminar(4);
    cout << "Tras eliminar(4)     : " << t.forma() << "\n";
    cout << "  en orden           : " << t.enOrden() << "\n";
    t.eliminar(42);                     // no existe: no hace nada
    t.insertar(4);
    cout << "Tras insertar(4)     : " << t.forma() << "   (raiz = " << t.valorRaiz() << ")\n";
    cout << "  en orden           : " << t.enOrden() << "\n";
    return 0;
}