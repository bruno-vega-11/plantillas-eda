#include <bits/stdc++.h>
using namespace std;

// ordenamiento estilo Merge Sort para combinar las hojas en la siguiente dimensión
template<typename build_type>
void merge(vector<build_type> &l, vector<build_type> &r, vector<build_type> &res, int dim) {
    int at = 0;
    for (auto &e : l) {
        // Inserta elementos de 'r' ordenados por la dimensión 'dim' antes de 'e'
        while (at < (int)r.size() && r[at][dim] < e[dim]) {
            res.emplace_back(r[at++]);
        }
        res.emplace_back(e);
    }
    // Inserta los elementos restantes de 'r'
    while (at < (int)r.size()) {
        res.emplace_back(r[at++]);
    }
}

// Extrae las hojas ordenadas por la dimensión 'dim' de los árboles asociados de ambos hijos y las combina
template<typename build_type, typename node_type>
vector<build_type> compute_sorted_values(int dim, node_type *L, node_type *R) {
    vector<build_type> l = L->next->get_leaves();
    vector<build_type> r = R->next->get_leaves();
    vector<build_type> res;
    merge(l, r, res, dim);
    return res;
}

// ============================================================================
// ÁRBOL ASOCIADO DEL ÚLTIMO NIVEL (Filtro en Y, K = 1)
// ============================================================================
template<typename data_type, typename build_type, typename index_type, int K>
struct LastLevelRangeTree {
    struct LastLevelRangeTreeNode {
        data_type sum;               // Peso acumulado de los puntos en este subárbol
        build_type minimum, maximum; // Límites inferior y superior (punto min y max) en este nodo
        LastLevelRangeTreeNode *left;
        LastLevelRangeTreeNode *right;

        // Constructor para nodos hoja
        LastLevelRangeTreeNode(build_type single_value)
            : sum(single_value[K + 1]), minimum(single_value), maximum(single_value) {
            left = right = nullptr;
        }

        // Constructor para nodos internos
        LastLevelRangeTreeNode(data_type sum) : sum(sum), minimum(), maximum() {
            left = right = nullptr;
        }
    };

    LastLevelRangeTreeNode *root;

    LastLevelRangeTree(vector<build_type> &a) {
        root = build_range_tree_from_indices(index_type(0), (index_type)a.size() - 1, a);
    }

    // Construcción del árbol 1D sobre la dimensión Y
    LastLevelRangeTreeNode* build_range_tree_from_indices(index_type l, index_type r, vector<build_type> &a) {
        if (l == r) {
            return new LastLevelRangeTreeNode(a[l]);
        }
        index_type mi = (l + r) / 2;
        LastLevelRangeTreeNode *left = build_range_tree_from_indices(l, mi, a);
        LastLevelRangeTreeNode *right = build_range_tree_from_indices(mi + 1, r, a);

        LastLevelRangeTreeNode *node = new LastLevelRangeTreeNode(left->sum + right->sum);
        node->left = left;
        node->right = right;
        node->minimum = a[l]; // Al estar ordenado por Y, a[l] contiene la menor Y del rango
        node->maximum = a[r]; // a[r] contiene la mayor Y del rango
        return node;
    }

    // Consulta de rango 1D en la dimensión Y [d, u]
    data_type query(LastLevelRangeTreeNode *node, index_type l, index_type r) {
        // Descarte: Fuera del rango en Y
        if (r < node->minimum[K] || node->maximum[K] < l) return data_type(0);

        // Inclusión Total: El nodo está completamente dentro del rango [d, u] en Y
        if (l <= node->minimum[K] && node->maximum[K] <= r) {
            return node->sum;
        }

        // Solapamiento Parcial: Recurrir en ambos subárboles
        return query(node->left, l, r) + query(node->right, l, r);
    }

    data_type query(index_type l, index_type r) {
        return query(root, l, r);
    }

    // Obtiene todos los puntos de las hojas ordenados por la dimensión actual (Y)
    vector<build_type> get_leaves() {
        vector<build_type> res;
        stack<LastLevelRangeTreeNode*> S;
        S.emplace(root);
        while (!S.empty()) {
            LastLevelRangeTreeNode *node = S.top();
            S.pop();
            if (node->left || node->right) {
                if (node->right) S.emplace(node->right);
                if (node->left) S.emplace(node->left);
            } else {
                res.emplace_back(node->minimum);
            }
        }
        return res;
    }
};

// ============================================================================
// ÁRBOL PRIMARIO (Filtro en X, K = 0)
// ============================================================================
template<typename data_type, typename build_type, typename index_type, int K>
struct TrivialRangeTree {
    struct TrivialRangeTreeNode {
        data_type sum;               // Suma total en el subárbol
        build_type minimum, maximum; // Punto con menor y mayor coordenada X

        // Puntero al Árbol Asociado ordenado por Y para los puntos de este subárbol
        LastLevelRangeTree<data_type, build_type, index_type, K + 1> *next;

        TrivialRangeTreeNode *left;
        TrivialRangeTreeNode *right;

        TrivialRangeTreeNode(build_type single_value)
            : sum(single_value[K]), minimum(single_value), maximum(single_value) {
            left = right = nullptr;
        }

        TrivialRangeTreeNode(data_type sum) : sum(sum), minimum(), maximum() {
            left = right = nullptr;
        }
    };

    TrivialRangeTreeNode *root;

    TrivialRangeTree(vector<build_type> &a) {
        root = build_range_tree_from_indices(index_type(0), index_type((int)a.size() - 1), a);
    }

    // Construcción del árbol 2D (Árbol principal en X + Árboles asociados en Y)
    TrivialRangeTreeNode* build_range_tree_from_indices(index_type l, index_type r, vector<build_type> &a) {
        if (l == r) {
            TrivialRangeTreeNode *node = new TrivialRangeTreeNode(a[l][K]);
            vector<build_type> single_value = {a[l]};

            // La hoja crea su propio árbol asociado en Y con un solo elemento
            node->next = new LastLevelRangeTree<data_type, build_type, index_type, K + 1>(single_value);
            node->minimum = a[l];
            node->maximum = a[l];
            return node;
        }

        index_type mi = (l + r) / 2;
        TrivialRangeTreeNode *left = build_range_tree_from_indices(l, mi, a);
        TrivialRangeTreeNode *right = build_range_tree_from_indices(mi + 1, r, a);

        TrivialRangeTreeNode *node = new TrivialRangeTreeNode(left->sum + right->sum);
        node->left = left;
        node->right = right;
        node->minimum = a[l];
        node->maximum = a[r];

        // Combina las hojas de 'left' y 'right' ya ordenadas por Y en tiempo O(N)
        vector<build_type> sorted_by_next_dimension =
            compute_sorted_values<build_type, TrivialRangeTreeNode>(K + 1, node->left, node->right);

        // Construye el árbol asociado en Y para este nodo interno
        node->next = new LastLevelRangeTree<data_type, build_type, index_type, K + 1>(sorted_by_next_dimension);
        return node;
    }

    // Consulta de rectángulo 2D: X en [l, r], Y en [d, u]
    data_type query(TrivialRangeTreeNode *node, index_type l, index_type r, index_type d, index_type u) {
        // Descarte: Fuera del rango en X
        if (r < node->minimum[K] || node->maximum[K] < l) return data_type(0);

        // Inclusión Total en X: Delegar la consulta al árbol secundario en Y
        if (l <= node->minimum[K] && node->maximum[K] <= r) {
            return node->next->query(d, u);
        }

        // Solapamiento Parcial en X: Continuar la búsqueda por el árbol principal
        return query(node->left, l, r, d, u) + query(node->right, l, r, d, u);
    }

    data_type query(index_type l, index_type r, index_type d, index_type u) {
        return query(root, l, r, d, u);
    }
};

int main() {
    // Optimización de I/O
    cin.tie(0)->sync_with_stdio(false);

    int n, q;
    if (!(cin >> n >> q)) return 0;

    // a[i][0] = X, a[i][1] = Y, a[i][2] = Peso
    vector<array<int, 3>> a(n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < 3; ++j) {
            cin >> a[i][j];
        }
    }

    // Ordenar inicialmente los puntos por la coordenada X (dimensión 0)
    sort(a.begin(), a.end());

    // Construir el Range Tree 2D
    TrivialRangeTree<long long, array<int, 3>, int, 0> Solver(a);

    while (q--) {
        int l, r, d, u;
        cin >> l >> d >> r >> u;

        // Ajuste de rangos semiabiertos [l, r) x [d, u) a rangos cerrados [l, r-1] x [d, u-1]
        --r;
        --u;

        // Consulta la suma de pesos en el rectángulo [l, r] x [d, u]
        cout << Solver.query(l, r, d, u) << '\n';
    }

    return 0;
}