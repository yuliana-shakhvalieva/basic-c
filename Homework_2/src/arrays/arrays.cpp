#include "arrays.h"

namespace mse {
int32_t partition(std::vector<std::int32_t> &elements, int32_t start, int32_t end) {
  int32_t pivot = elements[start];

  int32_t count = 0;
  for (int32_t i = start + 1; i <= end; i++) {
    if (elements[i] <= pivot)
      count++;
  }

  int32_t pivotIndex = start + count;
  std::swap(elements[pivotIndex], elements[start]);

  int32_t i = start, j = end;
  while (i < pivotIndex && j > pivotIndex) {
    while (elements[i] <= pivot)
      i++;

    while (elements[j] > pivot)
      j--;

    if (i < pivotIndex && j > pivotIndex)
      std::swap(elements[i++], elements[j--]);
  }
  return pivotIndex;
}


void quick_sort(std::vector<std::int32_t> &arr, int32_t start, int32_t end) {
  if (start >= end)
    return;

  int32_t p = partition(arr, start, end);

  quick_sort(arr, start, p - 1);
  quick_sort(arr, p + 1, end);
}


void sort(std::vector<std::int32_t> &elements) {
  quick_sort(elements, 0, elements.size() - 1);
}


std::int32_t trace(const std::vector<std::vector<std::int32_t>> &matrix) {
  int32_t n = matrix.size();
  int32_t trace = 0;

  for (int32_t i = 0; i < n; i++)
    trace += matrix[i][i];

  return trace;
}


std::int32_t find_unique_element(const std::vector<std::int32_t> &elements) {
  int32_t n = elements.size();

  for (int32_t i = 0; i < n; i++){
    int32_t pivot = elements[i];
    int32_t count_pivot = 0;
    for (int32_t j = 0; j < n; j++){
      if (elements[j] == pivot)
        count_pivot++;
    }
    if (count_pivot == 1)
      return pivot;
  }
  return -1;
}


std::size_t find_max_square(const std::vector<std::vector<std::int8_t>> &matrix){
  std::size_t rows = matrix.size();

  if (rows == 0)
    return 0;

  std::size_t cols = matrix[0].size();

  std::vector<std::vector<std::uint32_t>> help_matrix(rows, std::vector<std::uint32_t>(cols, 0));

  for (uint i = 0; i < rows; i++) {
    for (uint j = 0; j < cols; j++) {
      if (matrix[i][j] == 1) {
        if (j >= 1)
          help_matrix[i][j] = help_matrix[i][j - 1] + 1;
        else
          help_matrix[i][j] = 1;
      }
    }
  }

  uint max_square = 0;

  for (uint i = 0; i < rows; i++) {
    for (uint j = 0; j < cols; j++) {
      if (help_matrix[i][j] >= 1) {
        uint count = help_matrix[i][j];
        bool enough = true;

        if (count - 1 > rows - i - 1)
          break;

        uint max_side = count;
        for (uint k = 1; k < count; k++){
          uint pivot = help_matrix[i+k][j];
          if (pivot < count && pivot == k + 1) {
            max_side = pivot;
            break;
          }
          else if (pivot < count) {
            enough = false;
            break;
          }
        }

        uint square = max_side * max_side;
        if (enough && (square > max_square))
          max_square = square;

      }
    }
  }
  return max_square;
}

} // namespace mse
