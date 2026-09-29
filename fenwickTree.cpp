#include <bits/stdc++.h>
using namespace std;

// ============================================================================
// Fenwick Tree
// Se asume que solo funciona si problema es:
// manhattan: Solo funciona si los segmentos son o horizontales o verticales
// offline: No sirve para consultas online de ppl(no es persistente)
// conteo: solo cuenta el numero total de insersecciones
// ============================================================================

const int ADD = -1e9 - 1;   // activa y
const int REM = -1e9 - 2;   // desactiva y
const int QUERY = -1e9 - 3; // consutla rango

template<typename T>
struct FenwickTree {
    vector<T> ft;

    FenwickTree(int n) {
        ft.resize(n + 1);
        fill_n(ft.begin(), n + 1, T(0));
    }

    // activa o desactiva la coordenada vertical y
    void update(int pos, T val) {
        ++pos;
        while (pos < ft.size()) {
            ft[pos] += val;
            pos += (-pos) & pos;
        }
    }

    // suma acumulada desde el origen hasta `pos`
    T get_sum(int pos) {
        ++pos;
        T res = T(0);
        while (pos > 0) {
            res += ft[pos];
            pos &= pos - 1;
        }
        return res;
    }

    // devuelve la suma de segmentos activos en [l, r]
    T query(int l, int r) {
        return get_sum(r) - get_sum(l - 1);
    }
};

// convierte valores de y grandes a un rango discreto [0, M-1]
int compress(vector<int> &Y) {
    vector<int> values(Y.begin(), Y.end());
    sort(values.begin(), values.end());
    values.erase(unique(values.begin(), values.end()), values.end());
    for (int &x : Y) {
        x = lower_bound(values.begin(), values.end(), x) - values.begin();
    }
    return values.size(); // Retorna el número de coordenadas Y únicas
}

int main() {
    cin.tie(0) -> sync_with_stdio(false);

    int n;
    cin >> n;
    vector<int> Y;
    vector<int> X;
    vector<tuple<int, int, int>> events;

    for (int i = 0; i < n; ++i) {
        int a, b, c, d;
        cin >> a >> b >> c >> d;

        // identifica que tipo de segmento es
        if (a == c) {
            // segmento Horizontal (y = a, desde x = b hasta x = d)
            if (b > d) swap(b, d);
            Y.emplace_back(a);
            Y.emplace_back(a + 1);
            X.emplace_back(b);
            X.emplace_back(d + 1);
        }
        else {
            // segmento vertical (x = b, desde y = a hasta y = c)
            if (a > c) swap(a, c);
            Y.emplace_back(a);
            Y.emplace_back(c + 1);
            X.emplace_back(QUERY);
            X.emplace_back(b);
        }
    }

    // Comprime todas las coordenadas Y para poder usarlas como índices del Fenwick Tree
    int m = compress(Y);

    // CONSTRUCCIÓN DE LA COLA DE EVENTOS PARA EL SWEEP LINE
    for (int i = 0; i < X.size(); i += 2) {
        if (X[i] == QUERY) {
            // Evento de Consulta: (Posición_X, Y_inferior, Y_superior)
            events.emplace_back(X[i + 1], Y[i], Y[i + 1] - 1);
        }
        else {
            // Eventos de Modificación (Horizontales):
            // 1. Nace el segmento en X[i]: Evento ADD a la altura Y[i]
            events.emplace_back(X[i], ADD, Y[i]);
            // 2. Muere el segmento en X[i+1]: Evento REM a la altura Y[i]
            events.emplace_back(X[i + 1], REM, Y[i]);
        }
    }

    // Ordena los eventos cronológicamente por su coordenada X (Sweep Line barriendo de izquierda a derecha).
    // Si coinciden en X, se ordenan por la constante tipo (ADD < REM < QUERY), evitando conflictos en los bordes.
    sort(events.begin(), events.end());

    int res = 0;
    FenwickTree<int> F(m); // Fenwick Tree que mantiene el número de horizontales activos por altura Y

    // EJECUCIÓN DEL BARRIDO (SWEEP LINE)
    for (auto &e : events) {
        int t, l, r;
        tie(t, l, r) = e; // t = coordenada X, l = tipo de evento / Y_inf, r = Y / Y_sup

        if (l == ADD) {
            F.update(r, 1);  // Activa (+1) la coordenada Y en el Fenwick Tree
        }
        else if (l == REM) {
            F.update(r, -1); // Desactiva (-1) la coordenada Y en el Fenwick Tree
        }
        else {
            // Es una QUERY (segmento vertical):
            // Consulta cuántos horizontales vivos hay en el rango vertical [l, r]
            res += F.query(l, r);
        }
    }

    // Muestra la cantidad total de intersecciones calculadas
    cout << res << '\n';
    return 0;
}