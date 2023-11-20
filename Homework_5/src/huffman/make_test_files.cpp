#include <stdexcept>
#include <fstream>
#include <iostream>
#include <algorithm>

void write_data(const std::string &data, const std::string &file_name) {
	std::fstream file;

	std::string file_path = "test/" + file_name;
	file.open(file_path, std::ios::out);
	size_t size = data.size();

	file.write(reinterpret_cast<const char*>(data.data()), size);
}

std::string get_256_different_bytes() {
	std::string result;

	for (auto i = 0; i < 256; ++i) {
		result.push_back(char(i));
	}

	return result;
}

std::string get_long_string(int32_t length) {
	std::string result;

	for (auto i = 0; i < length; ++i) {
		int random_letter = 'a' + rand() % 26;
		result.push_back(char(random_letter));
	}

	return result;
}

std::string get_long_binary_string(const int32_t length) {
	std::string result;

	for (auto i = 0; i < length; ++i) {
		result += std::to_string(rand() % 2);
	}

	return result;
}

std::string get_long_random_bytes(const int32_t length) {
	std::string result;

	for (auto i = 0; i < length; ++i) {
		result.push_back(char(rand()));
	}

	return result;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
	std::string data;

	// abacaba file
	data = "abacaba";
	write_data(data, "abacaba.txt");

	// from_readme file
	data = "aaaaaabbbccdde\xff";
	write_data(data, "verbose_example.in");

	// empty file
	data = "";
	write_data(data, "empty.txt");

	// one_byte file
	data = "a";
	write_data(data, "one_byte.txt");

	// code_0 file
	data = char(0);
	write_data(data, "code_0.txt");

	// code_255 file
	data = char(255);
	write_data(data, "code_255.txt");

	// different_bytes_256 file
	data = get_256_different_bytes();
	write_data(data, "different_bytes_256.txt");

	// two_same_bytes file
	data = "aa";
	write_data(data, "two_same_bytes.txt");

	// long_string file
	data = get_long_string(10000);
	write_data(data, "long_string.txt");

	// long_binary_string file
	data = get_long_binary_string(10000);
	write_data(data, "long_binary_string.txt");

	// long_random_bytes file
	data = get_long_random_bytes(10000);
	write_data(data, "long_random_bytes.txt");

	return 0;
}