#include "test_framework.hpp"

// Les TEST() de chaque fichier s'enregistrent automatiquement via leurs variables statiques.
// Ce main se contente de les exécuter tous.

int main() {
    return run_all_tests();
}
