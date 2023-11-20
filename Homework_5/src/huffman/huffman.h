#pragma once

#include <string>
#include "huffman.h"
#include <stdexcept>
#include <cstring>
#include <fstream>
#include <iostream>
#include <tuple>
#include <map>
#include <vector>
#include <unordered_map>
#include <queue>
#include <sstream>
#include <algorithm>

namespace mse {

	void check_compress_decode(char *&argument) ;

	void check_input_format(const int &argc, char **&argv);

	std::tuple<std::string, std::string> get_file_paths(char **&argv);

	std::tuple<bool, bool, bool> get_modes(char **&argv);

	std::fstream open_file(const std::string &file_path, const std::_Ios_Openmode mode);

	std::string read_string(std::fstream &file);

	std::string get_input_string(const std::string &file_path);

	std::vector<std::string> get_lines(const std::string &input_string);

	std::vector<bool> convert_binary_string_to_vector(const std::string &input_string);

	std::unordered_map<unsigned char, std::vector<bool>> convert_string_to_huffman_codes(const std::string &input_string);

	std::string convert_vector_to_binary_string(const std::vector<bool> &binary_vector);

	std::string convert_huffman_codes_to_string(const std::unordered_map<unsigned char, std::vector<bool>> &huffman_codes);

	void write_data(const std::string &result_string, std::fstream file);

	std::string get_name_huffman_codes_file_path(const std::string &file_path);

	void write_compressed_results(const std::string &compressed_string,
		const std::unordered_map<unsigned char, std::vector<bool>> &huffman_codes,
		const std::string &file_path,
		bool need_codes);

	void write_decoded_results(const std::string &decoded_string,
		const std::string &huffman_codes_string,
		const std::string &file_path,
		bool need_codes);

	void print_string_size(const std::string &input_string, bool binary);

} // namespace mse
