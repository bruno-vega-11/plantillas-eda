#include <stdexcept>
#include <vector>
#include <iostream>

using namespace std;

// ============================================================
// PERSISTENT TRIE
// Es parcial y total
// cada nodo tiene 26 hijos (26 caracteres en ingles)
// ============================================================

struct TrieNode {
    bool isleaf;
    vector<TrieNode*> children;

    TrieNode(bool leaf = false) : isleaf(leaf), children(26, nullptr) {}
    TrieNode(bool leaf, vector<TrieNode*>& _c): isleaf(leaf),children(_c){}
};

struct Trie {
    vector<TrieNode*> versiones;

    // version = un root
     TrieNode* insertPersistent (TrieNode* nodo_old, const string& key, int i=0) {
         TrieNode* nodo_new;
         if (nodo_old != nullptr) {
             nodo_new = new TrieNode(nodo_old->isleaf);
             nodo_new->children = nodo_old->children;
         } else {
             nodo_new = new TrieNode();
         }

         if (i == key.length()) {
             nodo_new->isleaf = true;
             return nodo_new;
         }

         // Convertimos el caracter a indice [0-25]
         int c = key[i] - 'a';
         TrieNode* hijoviejo = (nodo_old != nullptr)? nodo_old->children[c]: nullptr;
         nodo_new->children[c] = insertPersistent(hijoviejo,key,i+1);
         return nodo_new;
    }

    bool search(TrieNode* root, const string& key) {
         TrieNode* curr = root;
         for (char c : key) {
             int idx = c - 'a';
             if (curr->children[idx] == nullptr) {
                 return false; // No existe el camino para esta letra
             }
             curr = curr->children[idx];
         }
         return curr->isleaf; // Retorna true solo si es final de palabra
     }

    bool search_version(int version, const string& key) {
         if (version < 0 || version >= versiones.size()) return false;
         return search(versiones[version], key);
     }

    void new_version(int version_base,const string&key) {
         TrieNode* old_v = versiones[version_base];
         TrieNode* new_v = insertPersistent(old_v,key);
         versiones.push_back(new_v);
     }

    // Auxiliar recursivo para imprimir el Trie en formato ASCII
    void print_ascii(TrieNode* node, const string& prefix, char charFromParent, bool isLast) {
         if (node == nullptr) return;

         cout << prefix;
         if (charFromParent != '\0') {
             cout << (isLast ? "\\-- " : "|-- ") << "'" << charFromParent << "'";
         } else {
             cout << "ROOT";
         }

         cout << (node->isleaf ? " [LEAF]" : "") << " (ptr: " << node << ")\n";

         // Contamos cuántos hijos válidos existen para dar el formato de ramas
         vector<int> valid_children;
         for (int i = 0; i < 26; i++) {
             if (node->children[i] != nullptr) {
                 valid_children.push_back(i);
             }
         }

         for (size_t idx = 0; idx < valid_children.size(); idx++) {
             int child_idx = valid_children[idx];
             bool lastChild = (idx == valid_children.size() - 1);
             char child_char = 'a' + child_idx;

             string newPrefix = prefix + (charFromParent == '\0' ? "    " : (isLast ? "    " : "|   "));
             print_ascii(node->children[child_idx], newPrefix, child_char, lastChild);
         }
     }

    void print_version(int version) {
         if (version < 0 || version >= versiones.size()) {
             cout << "Versión no válida\n";
             return;
         }
         cout << "\n=============================================\n";
         cout << "   ESTRUCTURA DEL TRIE - VERSIÓN " << version << "\n";
         cout << "=============================================\n";
         print_ascii(versiones[version], "", '\0', true);
         cout << endl;
     }

    Trie() {
        TrieNode* root = new TrieNode();
        versiones.push_back(root); // v0 = trie vacio
    }
};

int main() {
    Trie t;
    cout << "v0 creada (Trie vacío)";
    t.print_version(0);

    // v1: Insertamos "cat" sobre v0
    t.new_version(0, "cat");
    t.print_version(1);

    // v2: Insertamos "car" sobre v1 (se debe compartir la 'c' y la 'a')
    t.new_version(1, "car");
    t.print_version(2);
    return 0;
}