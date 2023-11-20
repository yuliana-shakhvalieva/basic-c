#define INPUT_FILE_FLAG "-i"
#define OUTPUT_FILE_FLAG "-o"
#define FILTER_SIZE_FLAG "-r"

#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>

#pragma pack(push, 1) // disable alignment

struct BMPHeader
{
  std::uint16_t magic = 0x4D42;
  std::uint32_t file_size = 0;
  std::uint16_t reserved = 0; // Always 0.
  std::uint16_t reserved_other = 0; // Always 0.
  std::uint32_t data_offset = 0;
};

struct BMPInfoHeader
{
  std::uint32_t header_size = 0;
  std::int32_t width = 0;
  std::int32_t height = 0;
  std::uint16_t planes = 1;
  std::uint16_t bit_count = 24;
  std::uint32_t compression = 0;
  std::uint32_t image_size = 0;
  std::int32_t x_pixels_per_meter = 2834;
  std::int32_t y_pixels_per_meter = 2834;
  std::uint32_t colors_used = 0;
  std::uint32_t important_colors = 0;
};
#pragma pack(pop)

struct Pixel
{
  std::uint8_t blue = 0;
  std::uint8_t green = 0;
  std::uint8_t red = 0;

};


struct Image
{
    std::int32_t width = 0;
    std::int32_t height = 0;
    std::vector<std::vector<Pixel>> pixels;
};


namespace mse {

void check_input_format(const int &argc, char **&argv) {
    if (argc < 4) {
      std::string message = "At least 4 parameters should be passed.";
      throw std::invalid_argument(message);
    }

    std::uint16_t count_INPUT_FILE_FLAG = 0;
    std::uint16_t count_OUTPUT_FILE_FLAG = 0;
    std::uint16_t count_FILTER_SIZE_FLAG = 0;

    for (auto i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], INPUT_FILE_FLAG) == 0) {
            ++count_INPUT_FILE_FLAG;
        } else if (strcmp(argv[i], OUTPUT_FILE_FLAG) == 0) {
            ++count_OUTPUT_FILE_FLAG;
        } else if (strcmp(argv[i], FILTER_SIZE_FLAG) == 0) {
            ++count_FILTER_SIZE_FLAG;
        } else {
            std::string message = "Incorrect parameter: " + std::string(argv[i]) +
                       ". Available parameters are: -i, -o, -r.";
            throw std::invalid_argument(message);
        }

        if (i + 1 == argc ||
            (strcmp(argv[i + 1], INPUT_FILE_FLAG) == 0) ||
            (strcmp(argv[i + 1], OUTPUT_FILE_FLAG) == 0) ||
            (strcmp(argv[i + 1], FILTER_SIZE_FLAG) == 0)) {

            std::string message = "Incorrect parameter after flag: " + std::string(argv[i]) + ".";
            throw std::invalid_argument(message);
        }
    }

    if (!(count_OUTPUT_FILE_FLAG == count_INPUT_FILE_FLAG && count_INPUT_FILE_FLAG == count_FILTER_SIZE_FLAG)) {
        std::string message = "Not enough parameters.";
        throw std::invalid_argument(message);
    }
}

std::fstream open_file(const std::string &image_path) {
    std::fstream file(image_path, std::ios::binary);
    file.open(image_path,std::ios::in);

    if (!file.is_open()) {
        throw std::invalid_argument("Failed to open file: " + image_path + ".");
    }

    return file;
}

BMPInfoHeader get_info_header(std::fstream &file) {
    BMPHeader file_header;
    file.read(reinterpret_cast<char*>(&file_header), sizeof(file_header));

    BMPInfoHeader info_header;
    file.read(reinterpret_cast<char*>(&info_header), sizeof(info_header));

    file.seekg(file_header.data_offset, file.beg);

    return info_header;
}

std::vector<std::vector<Pixel>> get_pixels(std::fstream &file, const BMPInfoHeader &info_header) {
    std::vector<std::vector<Pixel>> pixels(info_header.height, std::vector<Pixel>(info_header.width));

    for (auto i = 0; i < info_header.height; i++)
    {
        for (auto j = 0; j < info_header.width; j++)
        {
            std::uint8_t blue = file.get();
            std::uint8_t green = file.get();
            std::uint8_t red = file.get();

            pixels[i][j] = Pixel({blue, green, red});
        }

        file.seekg((4 - (info_header.width * 3) % 4) % 4, std::ios::cur);
    }

    return pixels;
}

Image map_image(const BMPInfoHeader &info_header, const std::vector<std::vector<Pixel>> &pixels) {
    return Image({info_header.width, info_header.height, pixels});
}

Image read_image(const std::string &image_path) {
    std::fstream file = open_file(image_path);
    const BMPInfoHeader info_header = get_info_header(file);
    std::vector<std::vector<Pixel>> pixels = get_pixels(file, info_header);
    Image image = map_image(info_header, pixels);
    return image;
}

void check_filter_size(const Image &image, const std::int32_t filter_size) {
    std::int32_t width = image.width;
    std::int32_t height = image.height;

    if (filter_size <= 0 || filter_size > std::min(width, height) - 2 || filter_size % 2 == 0) {
        std::string message = "Invalid filter size.";
        throw std::invalid_argument(message);
    }
}

std::vector<double> get_gaussian_kernel(const std::int32_t &filter_size) {
    double sigma = 0.3 * ((filter_size - 1) * 0.5 - 1) + 0.8;
    std::vector<double> filter(filter_size);

    double sum = 0;

    for (auto i = 0; i < filter_size; ++i) {
        double numerator = i - (filter_size - 1) * 0.5;
        double denominator = 2 * sigma * sigma;
        double value = std::exp(- numerator * numerator / denominator);
        filter[i] = value;
        sum += value;
    }

    for (auto i = 0; i < filter_size; ++i) {
        filter[i] /= sum;
    }

    return filter;
}

Image apply_gaussian_blur(const Image &input_image, const std::vector<double> &filter, const std::int32_t &filter_size) {

    std::int32_t padding = filter_size / 2;
    std::int32_t output_image_width = input_image.width - padding * 2;

    std::vector<std::vector<Pixel>> output_pixels_after_horizontal(input_image.height,
                                                                   std::vector<Pixel>(output_image_width));

    for (auto i = 0; i < input_image.height; ++i) {
        for (auto j = padding; j < input_image.width - padding; ++j) {
            double sum_blue = 0;
            double sum_green = 0;
            double sum_red = 0;
            for (auto k = 0, s = -padding; s <= padding; ++k, ++s) {
               sum_blue += filter[k] * input_image.pixels[i][j+s].blue;
               sum_green += filter[k] * input_image.pixels[i][j+s].green;
               sum_red += filter[k] * input_image.pixels[i][j+s].red;
            }
            output_pixels_after_horizontal[i][j-padding].blue = int8_t(sum_blue);
            output_pixels_after_horizontal[i][j-padding].green = int8_t(sum_green);
            output_pixels_after_horizontal[i][j-padding].red = int8_t(sum_red);
        }
    }

    std::vector<std::vector<Pixel>> output_pixels_final(input_image.height,
                                                        std::vector<Pixel>(input_image.width));

    for (auto i = padding; i < input_image.height - padding; ++i) {
        for (auto j = 0; j < output_image_width; ++j) {
            double sum_blue = 0;
            double sum_green = 0;
            double sum_red = 0;
            for (auto k = 0, s = -padding; s <=padding; ++k, ++s) {
               sum_blue += filter[k] * output_pixels_after_horizontal[i+s][j].blue;
               sum_green += filter[k] * output_pixels_after_horizontal[i+s][j].green;
               sum_red += filter[k] * output_pixels_after_horizontal[i+s][j].red;
            }
            output_pixels_final[i][j+padding].blue = int8_t(sum_blue);
            output_pixels_final[i][j+padding].green = int8_t(sum_green);
            output_pixels_final[i][j+padding].red = int8_t(sum_red);
        }
    }


    for (auto i = 0; i < input_image.height; ++i) {
        for (auto j = 0; j < input_image.width; ++j) {
            if (i < padding || j < padding || i >= input_image.height - padding || j >= input_image.width - padding) {
               output_pixels_final[i][j].blue = input_image.pixels[i][j].blue;
               output_pixels_final[i][j].green = input_image.pixels[i][j].green;
               output_pixels_final[i][j].red = input_image.pixels[i][j].red;
            }
        }
    }



    return Image({input_image.width, input_image.height, output_pixels_final});
}


void write_output_file(const Image &image, const std::string &image_path) {

    std::fstream file(image_path, std::ios::binary);
    file.open(image_path,std::ios::out);

    BMPHeader file_header;
    BMPInfoHeader info_header;
    file_header.data_offset = sizeof(file_header) + sizeof(info_header);

    file_header.file_size = file_header.data_offset
                       + (image.height * 3 + image.width % 4) * image.height;

    file.write((char*)(&file_header), sizeof(file_header));

    info_header.header_size = sizeof(info_header);
    info_header.width = image.width;
    info_header.height = image.height;
    info_header.image_size = image.width * image.height * 3;
    file.write((char*)(&info_header), sizeof(info_header));

    for (int i = 0; i < image.height ; i++) {
        for (int j = 0; j < image.width; j++) {
            const Pixel pix = image.pixels[i][j];
            file.put((unsigned char)(pix.blue));
            file.put((unsigned char)(pix.green));
            file.put((unsigned char)(pix.red));
        }

        for (int c = 0; c < image.height % 4; c++) {
            file.put(0);
        }
    }

    file.close();
}

void process_input_data(const int &argc, char **&argv) {

    Image input_image;
    std::int32_t filter_size = 0;
    std::string output_file_path;

    bool get_input_image = false;
    bool get_output_image = false;
    bool get_filter = false;


    for (auto i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], INPUT_FILE_FLAG) == 0) {
            input_image = read_image(argv[i + 1]);
            get_input_image = true;
        } else if (strcmp(argv[i], OUTPUT_FILE_FLAG) == 0) {
            output_file_path = std::string(argv[i+1]);
            get_output_image = true;
        } else if (strcmp(argv[i], FILTER_SIZE_FLAG) == 0) {
            filter_size = atoi(argv[i+1]);
            get_filter = true;
        }

        if (get_input_image && get_output_image && get_filter) {
            check_filter_size(input_image, filter_size);
            std::vector<double> filter = get_gaussian_kernel(filter_size);
            Image output_image = apply_gaussian_blur(input_image, filter, filter_size);
            write_output_file(output_image, output_file_path);
        }
    }
}
} // namespace mse


int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
    try {
       mse::check_input_format(argc, argv);
       mse::process_input_data(argc, argv);
    }
    catch (std::invalid_argument const &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
