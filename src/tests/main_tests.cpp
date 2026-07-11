#include "test_framework.hpp"

std::vector<TestResult> _results;
std::string _current_test;

int main() {
    return run_all_tests();
}