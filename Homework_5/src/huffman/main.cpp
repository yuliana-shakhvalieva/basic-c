#include "huffman.h"
#include "huffman.cpp"
#include <iostream>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
	try {
		mse::check_input_format(argc, argv);
		auto [input_file_path, output_file_path] = mse::get_file_paths(argv);

		std::string input_string = mse::get_input_string(input_file_path);
		mse::print_string_size(input_string, false);

		auto [compress, decode, need_codes] = mse::get_modes(argv);

		mse::Tree tree;
		if (compress) {
			tree.build_huffman_tree(input_string);
			std::vector<bool> compressed_vector = tree.compress(input_string);
			std::unordered_map<unsigned char, std::vector<bool>> huffman_codes = tree.get_huffman_codes();
			std::string compressed_string = mse::convert_vector_to_binary_string(compressed_vector);

			mse::print_string_size(compressed_string, true);
			std::cout << tree.get_size() << std::endl;

			mse::write_compressed_results(compressed_string, huffman_codes, output_file_path, need_codes);
		} else if (decode) {
			std::string huffman_codes_file_path = mse::get_name_huffman_codes_file_path(input_file_path);
			std::string huffman_codes_string = mse::get_input_string(huffman_codes_file_path);

			std::unordered_map<unsigned char, std::vector<bool>> huffman_codes = mse::convert_string_to_huffman_codes(huffman_codes_string);
			tree.put_huffman_codes(huffman_codes);

			std::vector<bool> input_vector = mse::convert_binary_string_to_vector(input_string);
			std::string decoded_string = tree.decode(input_vector);

			mse::print_string_size(decoded_string, false);
			std::cout << tree.get_size() << std::endl;

			mse::write_decoded_results(decoded_string, huffman_codes_string, output_file_path, need_codes);
		}
	}

	catch (std::invalid_argument const &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
