#pragma once

#include <stdexcept>
#include <fstream>
#include <iostream>
#include <algorithm>


void write_data(const std::string &data, const std::string &file_name);

std::string get_256_different_bytes();

std::string get_long_string(int32_t length);

std::string get_long_binary_string(int32_t length);

std::string get_long_random_bytes(int32_t length);