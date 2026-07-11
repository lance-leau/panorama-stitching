#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <cmath>

struct TestResult {
    std::string name;
    bool passed;
    std::string message;
};

extern std::vector<TestResult> _results;
extern std::string _current_test;

#define TEST(name) \
    static void test_##name(); \
    static bool _reg_##name = ([]{ \
        _results.push_back({#name, false, "not run"}); \
        try { \
            _current_test = #name; \
            test_##name(); \
            _results.back().passed  = true; \
            _results.back().message = "OK"; \
        } catch (const std::exception& e) { \
            _results.back().message = e.what(); \
        } catch (...) { \
            _results.back().message = "exception inconnue"; \
        } \
        return true; \
    }(), true); \
    static void test_##name()

#define ASSERT_TRUE(expr) \
    do { if (!(expr)) throw std::runtime_error("ASSERT_TRUE échoué: " #expr " (ligne " + std::to_string(__LINE__) + ")"); } while(0)

#define ASSERT_FALSE(expr) \
    do { if ((expr))  throw std::runtime_error("ASSERT_FALSE échoué: " #expr " (ligne " + std::to_string(__LINE__) + ")"); } while(0)

#define ASSERT_EQ(a, b) \
    do { if ((a) != (b)) throw std::runtime_error(std::string("ASSERT_EQ: ") + std::to_string(a) + " != " + std::to_string(b) + " (ligne " + std::to_string(__LINE__) + ")"); } while(0)

#define ASSERT_NEAR(a, b, tol) \
    do { if (std::abs((double)(a) - (double)(b)) > (tol)) \
        throw std::runtime_error(std::string("ASSERT_NEAR: |") + std::to_string(a) + " - " + std::to_string(b) + "| > " + std::to_string(tol) + " (ligne " + std::to_string(__LINE__) + ")"); } while(0)

#define ASSERT_GE(a, b) \
    do { if ((a) < (b)) throw std::runtime_error(std::string("ASSERT_GE: ") + std::to_string(a) + " < " + std::to_string(b) + " (ligne " + std::to_string(__LINE__) + ")"); } while(0)

#define ASSERT_THROWS(expr) \
    do { bool _threw = false; try { expr; } catch (...) { _threw = true; } \
    if (!_threw) throw std::runtime_error("ASSERT_THROWS: aucune exception levée pour: " #expr); } while(0)

inline int run_all_tests() {
    int passed = 0, failed = 0;
    std::cout << "\n=== Tests unitaires C++ ===\n\n";
    for (const auto& r : _results) {
        if (r.passed) {
            std::cout << "  [PASS] " << r.name << "\n";
            ++passed;
        } else {
            std::cout << "  [FAIL] " << r.name << "\n"
                      << "         " << r.message << "\n";
            ++failed;
        }
    }
    std::cout << "\n" << passed << "/" << (passed + failed) << " tests passés";
    if (failed > 0)
        std::cout << " — " << failed << " ÉCHEC(S)";
    std::cout << "\n";
    return failed == 0 ? 0 : 1;
}
