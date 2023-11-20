#define COMPRESS_FLAG "-c"
#define DECODE_FLAG "-d"
#define CODES_FLAG "-v"

#include "huffman.h"
#include <stdexcept>
#include <cstring>
#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>
#include <unordered_map>
#include <queue>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace mse {

struct Node {
	unsigned char letter;
	std::uint32_t frequency;
	Node *left;
	Node *right;

	bool operator< (const Node& other) {
		return frequency < other.frequency;
	}
};

struct NodeComp {
	bool operator()(Node* l, Node* r) {
		return l->frequency > r->frequency;
	}
};

class Tree {
private:
	Node* root;
	std::vector<Node> all_nodes;
	std::unordered_map<unsigned char, std::vector<bool>> huffman_codes;

	std::unordered_map<unsigned char, std::uint32_t> count_frequencies(const std::string &input_string) {
		std::unordered_map<unsigned char, std::uint32_t> frequencies;

		for (unsigned char letter : input_string) {
			int a = static_cast<int>(letter);
			++frequencies[a];
		}

		return frequencies;
	}

	std::priority_queue<Node*, std::vector<Node*>, NodeComp> initialize_min_heap(const std::unordered_map<unsigned char, std::uint32_t> &frequencies) {

		std::priority_queue<Node*, std::vector<Node*>, NodeComp> min_heap;

		all_nodes.reserve(frequencies.size() * 2);

		for (auto pair : frequencies) {
			min_heap.push(&all_nodes.emplace_back(
				Node(
					{pair.first,
					 pair.second,
					 nullptr,
					 nullptr})));
		}

		return min_heap;
	}

	void huffman_first_pass(std::priority_queue<Node*, std::vector<Node*>, NodeComp> &min_heap) {
		while (min_heap.size() > 1) {
			Node* left = min_heap.top();
			min_heap.pop();

			Node* right = min_heap.top();
			min_heap.pop();

			std::uint32_t sum_frequency = left->frequency + right->frequency;
			min_heap.push(&all_nodes.emplace_back(
				Node(
					{'0',
					 sum_frequency,
					 left,
					 right})));
		}
	}

	void huffman_second_pass(const Node* node, const std::vector<bool>& code) {
		if (node == nullptr) {
			return;
		}

		if (!node->left && !node->right) {
			if (code.empty())
				huffman_codes[node->letter] = {false};
			else
				huffman_codes[node->letter] = code;
		}

		std::vector<bool> code_left = code;
		code_left.push_back(false);

		std::vector<bool> code_right = code;
		code_right.push_back(true);

		huffman_second_pass(node->left, code_left);
		huffman_second_pass(node->right, code_right);
	}

public:
	void build_huffman_tree(const std::string &input_string) {

		huffman_codes.clear();

		std::unordered_map<unsigned char, std::uint32_t> frequencies = count_frequencies(input_string);
		std::priority_queue<Node*, std::vector<Node*>, NodeComp> min_heap = initialize_min_heap(frequencies);

		huffman_first_pass(min_heap);

		if (!input_string.empty()){
			root = min_heap.top();
			huffman_second_pass(root, {});
		}
	}

	std::unordered_map<unsigned char, std::vector<bool>> get_huffman_codes() {
		return huffman_codes;
	}

	std::vector<bool> compress(const std::string &input_string)
	{
		std::vector<bool> compressed_input_string;
		for (const unsigned char letter : *&input_string) {
			const auto& code = huffman_codes[letter];

			for (const bool bit : *&code) {
				compressed_input_string.push_back(bit);
			}
		}

		return compressed_input_string;
	}

	void put_huffman_codes(std::unordered_map<unsigned char, std::vector<bool>> input_codes) {
		huffman_codes.clear();
		huffman_codes = input_codes;
	}

	std::string decode(std::vector<bool> &input_vector) {

		std::string decoded_string;

		while (input_vector.size() != 0) {
			for (auto pair : huffman_codes) {
				bool found = true;
				auto symbol = pair.first;
				auto code = pair.second;
				auto code_size = code.size();

				for (size_t i = 0; i < code_size; ++i) {
					if (!input_vector[i] == code[i]) {
						found = false;
						break;
					}
				}

				if (found) {
					decoded_string += symbol;
					input_vector.erase(input_vector.begin(), input_vector.begin() + code.size());
				}
			}
		}

		return decoded_string;
	}

	std::uint32_t get_size() {
	std::uint32_t size = 0;
	for (auto pair : huffman_codes) {
		auto code = pair.second;

		size += 1 + code.size() * sizeof(bool);
	}
	return size;
	}
};


void check_compress_decode(char *&argument) {
    if (!(strcmp(argument, COMPRESS_FLAG) == 0 ||
		  strcmp(argument, DECODE_FLAG) == 0)) {
        std::string message = "Incorrect parameter: " + std::string(argument) +
                          ". Available parameters are: -c, -d, -v.";
        throw std::invalid_argument(message);
    }
}

void check_input_format(const int &argc, char **&argv) {
	if (argc < 4) {
		std::string message = "At least 3 parameters should be passed.";
		throw std::invalid_argument(message);
	}

	std::uint8_t index = 1;

	if (strcmp(argv[index], CODES_FLAG) == 0) {
		++index;
	}

	check_compress_decode(argv[index]);
}

std::tuple<std::string, std::string> get_file_paths(char **&argv) {
	std::uint8_t index = 2;

	if (strcmp(argv[1], CODES_FLAG) == 0) {
		++index;
	}

	return std::make_tuple(std::string(argv[index]),
		                   std::string(argv[index+1]));
}

std::tuple<bool, bool, bool> get_modes(char **&argv) {
	bool compress = false;
	bool decode = false;
	bool need_codes = false;

	std::uint8_t index = 1;

	if (strcmp(argv[index], CODES_FLAG) == 0) {
		need_codes = true;
		++ index;
	}

	if (strcmp(argv[index], COMPRESS_FLAG) == 0) {
		compress = true;
	} else {
		decode = true;
	}

	return std::make_tuple(compress, decode, need_codes);
}

std::fstream open_file(const std::string &file_path, const std::_Ios_Openmode mode) {
	std::fstream file;
	file.open(file_path, mode);

	if (!file.is_open()) {
		throw std::invalid_argument("Failed to open file: " + file_path + ".");
	}

	return file;
}

std::string read_string(std::fstream &file) {
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

std::string get_input_string(const std::string &file_path) {
	std::fstream input_file = open_file(file_path, std::ios::in);
	std::string input_string = read_string(input_file);

	return input_string;
}

std::vector<std::string> get_lines(const std::string &input_string) {
	std::vector<std::string> lines;

	std::stringstream ss(input_string);
	std::string line;

	while (std::getline(ss, line, '\n')) {
		lines.push_back(line);
	}

	return lines;
}

std::vector<bool> convert_binary_string_to_vector(const std::string &input_string) {

	std::vector<bool> binary_vector;

	for (const char c : input_string) {
		if (c == '0') {
			binary_vector.push_back(false);
		} else if (c == '1') {
			binary_vector.push_back(true);
		} else {
			throw std::invalid_argument("Incorrect binary string: " + input_string + ".");
		}
	}

	return binary_vector;
}

std::unordered_map<unsigned char, std::vector<bool>> convert_string_to_huffman_codes(const std::string &input_string) {
	std::vector<std::string> input_lines = get_lines(input_string);
	std::unordered_map<unsigned char, std::vector<bool>> huffman_codes;

	for (const std::string &line : input_lines) {
		std::uint32_t space_idx = line.find(" ");
		std::string huffman_code = line.substr(0, space_idx);
		unsigned char symbol = stoi(line.substr(space_idx+1, line.size()));

		huffman_codes.insert({symbol, convert_binary_string_to_vector(huffman_code)});
	}

	return huffman_codes;
}

std::string convert_vector_to_binary_string(const std::vector<bool> &binary_vector) {
	std::string binary_string;
	for (const bool bit : binary_vector) {
		binary_string += std::to_string(int(bit));
	}

	return binary_string;
}

std::string convert_huffman_codes_to_string(const std::unordered_map<unsigned char, std::vector<bool>> &huffman_codes) {
	std::vector<std::string> result_vector;

	for (auto pair : huffman_codes) {
		unsigned char symbol = pair.first;
		std::string huffman_code = convert_vector_to_binary_string(pair.second);
		std::string ASCII_code = std::to_string(int(symbol));
		result_vector.push_back(huffman_code + " " + ASCII_code + "\n");
	}

	std::sort(result_vector.begin(), result_vector.end());

	std::string result_string;
	for (auto str : result_vector) {
		result_string += str;
	}

	return result_string;
}

void write_data(const std::string &result_string, std::fstream file) {
	size_t size = result_string.size();

	file.write(reinterpret_cast<const char*>(result_string.data()), size);
}

std::string get_name_huffman_codes_file_path(const std::string &file_path) {
	return file_path.substr(0, file_path.size() - 3) + "_huffman_codes.txt";
}

void write_compressed_results(const std::string &compressed_string,
	               const std::unordered_map<unsigned char, std::vector<bool>> &huffman_codes,
				   const std::string &file_path,
				   bool need_codes) {
	write_data(compressed_string,
		       open_file(file_path, std::ios::out));

	std::string huffman_codes_path = get_name_huffman_codes_file_path(file_path);
	std::string huffman_codes_string = convert_huffman_codes_to_string(huffman_codes);

	write_data(huffman_codes_string,
			   open_file(huffman_codes_path, std::ios::out));

	if (need_codes) {
		std::cout << huffman_codes_string << std::endl;
	}
}

void write_decoded_results(const std::string &decoded_string,
							const std::string &huffman_codes_string,
							const std::string &file_path,
							bool need_codes) {
	write_data(decoded_string, open_file(file_path, std::ios::out));

	if (need_codes) {
		std::cout << huffman_codes_string << std::endl;
	}
}

void print_string_size(const std::string &input_string, bool binary) {
	if (binary) {
		std::cout << std::ceil(static_cast<double>(input_string.size() * sizeof(std::string::value_type)) / 8) << std::endl;
	} else {
		std::cout << input_string.size() * sizeof(std::string::value_type) << std::endl;
	}
}
} // namespace mse


