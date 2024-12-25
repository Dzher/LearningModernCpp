#include <iostream>
#include <vector>

// 1. Normal Method
int sum(const std::vector<int>& vec) {
    if (vec.empty()) {
        return 0;
    }
    int result = vec[0];
    for (int i = 1; i < vec.size(); ++i) {
        result += vec[i];
    }
    return result;
}

int multi(const std::vector<int>& vec) {
    if (vec.empty()) {
        return 0;
    }
    int result = vec[0];
    for (int i = 1; i < vec.size(); ++i) {
        result *= vec[i];
    }
    return result;
}

// 2. Enum Method
enum class Algorithm
{
    Sum,
    Multi,
    // Max
};

int generate(const std::vector<int>& vec, Algorithm algorithm) {
    if (vec.empty()) {
        return 0;
    }
    int result = vec[0];
    if (algorithm == Algorithm::Sum) {
        for (int i = 1; i < vec.size(); ++i) {
            result += vec[i];
        }
    }
    else if (algorithm == Algorithm::Multi) {
        for (int i = 1; i < vec.size(); ++i) {
            result *= vec[i];
        }
    }
    return result;
}

// 3. Functional Programming
int sum_op(int a, int b) {
    return a + b;
}

int mul_op(int a, int b) {
    return a * b;
}

int calculator(const std::vector<int>& vec, auto op) {
    if (vec.empty()) {
        return 0;
    }
    int result = vec[0];
    for (int i = 1; i < vec.size(); ++i) {
        result = op(result, vec[i]);
    }

    return result;
}

// 3.1 lambda
// ...

int main(int argc, char* argv[]) {
    std::vector<int> int_vec{1, 2, 3, 4, 5};
    if (argc != 2) {
        std::cout << "Error: Please input your method!\n";
        std::cout << "Normal\n"
                  << "Enum\n"
                  << "Func\n"
                  << "lambda" << std::endl;
        return -1;
    }
    int sum_res = 0;
    int mul_res = 0;

    if (std::string_view(argv[1]) == "Normal") {
        sum_res = sum(int_vec);
        mul_res = multi(int_vec);
    }
    else if (std::string_view(argv[1]) == "Enum") {
        sum_res = generate(int_vec, Algorithm::Sum);
        mul_res = generate(int_vec, Algorithm::Multi);
    }
    else if (std::string_view(argv[1]) == "Func") {
        sum_res = calculator(int_vec, sum_op);
        mul_res = calculator(int_vec, mul_op);
    }
    else if (std::string_view(argv[1]) == "lambda") {
        sum_res = calculator(int_vec, [](int a, int b) { return a + b; });
        mul_res = calculator(int_vec, [](int a, int b) { return a * b; });
    }
    else {
        std::cout << "Error: Please input valid method!\n";
        return -1;
    }
    std::cout << "Sum result is " << sum_res << std::endl;
    std::cout << "Mul result is " << mul_res << std::endl;
    return 0;
}