#pragma once

#include <numeric>
#include <string>
#include <vector>

namespace mse {

void check_input_format(const int &argc, char** &argv);

std::fstream open_file(const std::string &file_name);

void close_file(std::fstream &file);

std::vector<std::vector<double>> read_matrix(std::fstream &file);

std::vector<std::vector<double>> get_matrix(const std::string &file_name);

void check_add_shapes(const std::vector<std::vector<double>> &matrix_1,
                      const std::vector<std::vector<double>> &matrix_2);

std::vector<std::vector<double>> add(const std::vector<std::vector<double>> &matrix_1,
                                     const std::vector<std::vector<double>> &matrix_2);

void check_mult_shapes(const std::vector<std::vector<double>> &matrix_1,
                       const std::vector<std::vector<double>> &matrix_2);

std::vector<std::vector<double>> mult(const std::vector<std::vector<double>> &matrix_1,
                                      const std::vector<std::vector<double>> &matrix_2);

void print_result(const std::vector<std::vector<double>> &result_matrix);

} // namespace mse
