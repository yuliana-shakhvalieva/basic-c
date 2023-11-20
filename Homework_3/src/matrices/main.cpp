#define ADD "--add"
#define MULT "--mult"

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "main.h"

namespace mse {

void check_input_format(const int &argc, char** &argv) {
    if (argc < 2) {
        throw std::invalid_argument("At least one parameter should be passed.");
    }

    for (auto i = 2; i < argc; i+=2) {
        if (strcmp(argv[i], ADD) != 0 && strcmp(argv[i], MULT) != 0) {
            throw std::invalid_argument(
                "Incorrect parameter: " + std::string(argv[i]) +
                ". Available parameters are: --add, --mult.");
        }
    }
}

std::fstream open_file(const std::string &file_name) {
    std::fstream file;
    file.open(file_name,std::ios::in);

    if (!file.is_open()) {
        throw std::invalid_argument("Failed to open file: " + file_name + ".");
    }

    return file;
}

void close_file(std::fstream &file) {
  file.close();
}

std::vector<std::vector<double>> read_matrix(std::fstream &file) {
    int n_rows = 0;
    int n_cols = 0;
    file >> n_rows >> n_cols;

    std::vector<std::vector<double>> matrix(n_rows, std::vector<double>(n_cols));

    for (auto i = 0; i < n_rows; i++) {
        for (auto j = 0; j < n_cols; j++) {
            file >> matrix[i][j];
        }
    }

    return matrix;
}

std::vector<std::vector<double>> get_matrix(const std::string &file_name) {
    std::fstream file = mse::open_file(file_name);
    const std::vector<std::vector<double>> matrix = mse::read_matrix(file);
    mse::close_file(file);

    return matrix;
}

void check_add_shapes(const std::vector<std::vector<double>> &matrix_1,
                      const std::vector<std::vector<double>> &matrix_2) {
    if (matrix_1.size() != matrix_2.size() || matrix_1[0].size() != matrix_2[0].size()) {
        throw std::invalid_argument("Matrix sizes do not match each other.");
    }
}

std::vector<std::vector<double>> add(const std::vector<std::vector<double>> &matrix_1,
                                     const std::vector<std::vector<double>> &matrix_2) {
    check_add_shapes(matrix_1, matrix_2);

    const int n_rows = matrix_1.size();
    const int n_cols = matrix_1[0].size();

    std::vector<std::vector<double>> result_matrix(n_rows, std::vector<double>(n_cols));

    for (auto i = 0; i < n_rows; ++i) {
        for (auto j = 0; j < n_cols; ++j) {
            result_matrix[i][j] = matrix_1[i][j] + matrix_2[i][j];
        }
    }

    return result_matrix;
}

void check_mult_shapes(const std::vector<std::vector<double>> &matrix_1,
                       const std::vector<std::vector<double>> &matrix_2) {
    if (matrix_1[0].size() != matrix_2.size()) {
        throw std::invalid_argument("Matrix sizes do not match each other.");
    }
}

std::vector<std::vector<double>> mult(const std::vector<std::vector<double>> &matrix_1,
                                      const std::vector<std::vector<double>> &matrix_2) {
    check_mult_shapes(matrix_1, matrix_2);

    const int n_rows = matrix_1.size();
    const int n_cols = matrix_2[0].size();
    const int inner_side = matrix_1[0].size();

    std::vector<std::vector<double>> result_matrix(n_rows, std::vector<double>(n_cols));

    for (auto i = 0; i < n_rows; i++) {
        for (auto j = 0; j < n_cols; j++) {
            result_matrix[i][j] = 0;

            for (auto k = 0; k < inner_side; k++) {
                result_matrix[i][j] += matrix_1[i][k] * matrix_2[k][j];
            }
        }
    }

    return result_matrix;
}

void print_result(const std::vector<std::vector<double>> &result_matrix) {
    const int n_rows = result_matrix.size();
    const int n_cols = result_matrix[0].size();

    std::cout << n_rows << " " << n_cols << std::endl;

    for (auto i = 0; i < n_rows; ++i) {
        for (auto j = 0; j < n_cols; ++j) {
            std::cout << result_matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}
} // namespace mse


int main([[maybe_unused]] int argc, [[maybe_unused]] char ** argv) {
    mse::check_input_format(argc, argv);

    std::vector<std::vector<double>> result_matrix = mse::get_matrix(argv[1]);

    for (auto i = 2; i < argc; i += 2) {
        const std::vector<std::vector<double>> current_matrix =
            mse::get_matrix(argv[i + 1]);

        if (strcmp(argv[i], ADD) == 0) {
            result_matrix = mse::add(result_matrix, current_matrix);
        } else if (strcmp(argv[i], MULT) == 0) {
            result_matrix = mse::mult(result_matrix, current_matrix);
        }
    }

    mse::print_result(result_matrix);
    return 0;
}
