#include <bits/stdc++.h>
using namespace std;

// data_type: tipo de dato del resultado
// build_type: tipo de dato de los elementos iniciales del arreglo
// index_type: tipo de dato para los índices y posiciones
template<typename data_type, typename build_type, typename index_type>
struct RangeTree {

    // Nodos del arbol
    struct RangeTreeNode {
        data_type sum;         // guarda la suma acumulada de las hojas en este subarol
        index_type l, r;       // rango [l,r] que cubre el nodo
        RangeTreeNode *left;   // hijo izquierdo
        RangeTreeNode *right;  // hijo derecho

        // constructor para nodos hoj
        RangeTreeNode(build_type single_value, index_type l, index_type r)
            : sum(single_value), l(l), r(r) {
            left = right = nullptr;
        }

        // constructor para nodos internos /suma de sus subarbole/
        RangeTreeNode(data_type sum, index_type l, index_type r)
            : sum(sum), l(l), r(r) {
            left = right = nullptr;
        }
    };

    RangeTreeNode *root;

    //  Range Tree a partir un arreglo
    RangeTree(vector<build_type> &a) {
        root = build_range_tree_from_indices(index_type(0), index_type((int)a.size() - 1), a);
    }

    // construccion recursiva del árbol O(N)
    RangeTreeNode* build_range_tree_from_indices(index_type l, index_type r, vector<build_type> &a) {
        // es hoja
        if (l == r) {
            RangeTreeNode *node = new RangeTreeNode(a[l], l, r);
            return node;
        }

        // Dos mitades
        index_type mi = l + (r - l) / 2;
        RangeTreeNode *left = build_range_tree_from_indices(l, mi, a);
        RangeTreeNode *right = build_range_tree_from_indices(mi + 1, r, a);

        // nodo interno guarda suma de sus 2 hijos
        RangeTreeNode *node = new RangeTreeNode(left->sum + right->sum, l, r);
        node->left = left;
        node->right = right;
        return node;
    }

    // query en el rango [l,r] O(log N)
    data_type query(RangeTreeNode *node, index_type l, index_type r) {
        // rango de nodo fuera de rango [l,r]
        if (r < node->l || node->r < l) {
            return data_type(0);
        }

        // rango de nodo completamente dentro rango [l,r]
        if (l <= node->l && node->r <= r) {
            return node->sum;
        }

        // solapamiento parcial, se consulta ambos lados y se combina respuesta
        return query(node->left, l, r) + query(node->right, l, r);
    }

    data_type query(index_type l, index_type r) {
        return query(root, l, r);
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, q;
    if (!(cin >> n >> q)) return 0;

    vector<int> a(n);
    for (int i = 0; i < n; ++i) {
        cin >> a[i];
    }

    RangeTree<long long, int, int> Solver(a);

    //q consultas
    while (q--) {
        int l, r;
        cin >> l >> r;

        cout << Solver.query(l, r) << '\n';
    }

    return 0;
}