#!/bin/bash -eu

if [ "$#" -lt 1 ]; then
	echo "Usage:"
echo "    $0 params"
exit -1
fi

EXECUTABLE="$1"
REAL_EXEC="$EXECUTABLE"
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

run()
{
	echo "***** Running: $EXECUTABLE $@"
	$REAL_EXEC "$@" 1>out.txt 2>err.txt
	CODE=$?
		 cat out.txt
	cat err.txt
	return $CODE
}

compare_result()
{
  FILE_1="$1"
  FILE_2="$2"
  cmp -s "$FILE_1" "$FILE_2"
}

check_encode_decode()
{
  INPUT_TXT="$1"
  OUTPUT_TXT="${INPUT_TXT::-3}in"
  TEST_TXT="${INPUT_TXT::-4}_test.txt"

  run -c $INPUT_TXT $OUTPUT_TXT || expected_ok
  check_non_empty_out
  check_empty_err
  run -d $OUTPUT_TXT $TEST_TXT || expected_ok
  check_non_empty_out
  check_empty_err
  compare_result $INPUT_TXT $TEST_TXT || expected_ok
}

check_empty_err()
{
	test ! -s err.txt || (echo "Error: expected empty stderr, got:"; cat err.txt; exit 1)
}

check_empty_out()
{
	test ! -s out.txt || (echo "Error: expected empty stdout, got:"; cat out.txt; exit 1)
}

check_non_empty_err()
{
	test -s err.txt || (echo "Error: expected non-empty stderr"; exit 1)
}

check_non_empty_out()
{
	test -s out.txt || (echo "Error: expected non-empty stdout"; exit 1)
}

expected_error()
{
	echo "Error: expected non-zero exit code"
	exit 1
}

expected_ok()
{
	echo "Error: expected zero exit code"
	echo "stdout:"
	cat out.txt
	echo "stderr:"
	cat err.txt
	exit 1
}

h()
{
	echo $@
}


# no args - should be an error
run && expected_error
check_empty_out
check_non_empty_err

# abacaba file
check_encode_decode abacaba.txt

# empty file
check_encode_decode empty.txt

# one_byte file
check_encode_decode one_byte.txt

# code_0 file
check_encode_decode code_0.txt

# code_255 file
check_encode_decode code_255.txt

# different_bytes_256 file
check_encode_decode different_bytes_256.txt

# two_same_bytes file
check_encode_decode two_same_bytes.txt

# long_string file
check_encode_decode long_string.txt

# long_binary_string file
check_encode_decode long_binary_string.txt

# long_random_bytes file
check_encode_decode long_random_bytes.txt

# wrong operation name - should be an error
run -a abacaba.txt abacaba.in && expected_error
check_empty_out
check_non_empty_err

# wrong operation name - should be an error
run -b abacaba.in abacaba.txt && expected_error
check_empty_out
check_non_empty_err

# not enough args - should be an error
run -c abacaba.txt && expected_error
check_empty_out
check_non_empty_err

# not enough args - should be an error
run -d abacaba.in && expected_error
check_empty_out
check_non_empty_err

# not enough args - should be an error
run -v abacaba.txt abacaba.in && expected_error
check_empty_out
check_non_empty_err

echo "Huffman test passed! You are doing great :))"
