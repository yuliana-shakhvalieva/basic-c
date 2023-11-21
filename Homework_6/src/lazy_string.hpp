#pragma once
#include <memory>
#include <iostream>

namespace std_utils {

namespace details {
} // namespace details

	template <class T, class S>
	class lazy_basic_string;

	template <class T, class S>
	bool operator== (const T * left, const lazy_basic_string<T, S> & right) {
		return (right == left);
	}

	template <class T, class S>
	bool operator< (const T * left, const lazy_basic_string<T, S> & right) {
		return !(right < left);
	}

	template <class T, class S>
	bool operator!= (const T * left, const lazy_basic_string<T, S> & right) {
		return right != left;
	}

	template <class T, class S>
	lazy_basic_string<T, S> operator+ (const T * left, const lazy_basic_string<T, S> & right) {
		lazy_basic_string<T, S> result;
		result += left;
		result += right;
		return result;
	}

	template <class T, class S>
	lazy_basic_string<T, S> operator+(const T left, const lazy_basic_string<T, S> & right) {
		lazy_basic_string<T, S> result;
		result += left;
		result += right;
		return result;
	}

	template <class T, class S>
	void swap(lazy_basic_string<T, S> & left, lazy_basic_string<T, S> & right) {
		left.swap(right);
	}

	template <class CharT, class Traits = std::char_traits<CharT>>
	class Proxy {
	public:
		Proxy(lazy_basic_string <CharT, Traits> * input_string, size_t idx)
			:lazy_string_ptr_(input_string) , position_(idx)
		{ }

		operator CharT() {
			return *(lazy_string_ptr_->c_str() + position_);
		}

		Proxy& operator= (const CharT & value) {
			size_t size = lazy_string_ptr_->size();

			std::shared_ptr<CharT[]> new_data = std::make_unique<CharT[]>(size + 1);

			Traits::copy(new_data.get(), lazy_string_ptr_->c_str(), size);

			*(new_data.get() + position_) = value;
			lazy_string_ptr_->set_data(new_data);

			return *this;
		}

	private:
		lazy_basic_string<CharT, Traits> * lazy_string_ptr_;
		size_t position_;
	};

	template <class CharT, class Traits = std::char_traits<CharT>>
	class lazy_basic_string {
 	public:

		lazy_basic_string (std::shared_ptr<CharT[]> data, size_t size)
			: data_(data), size_(size)
		{ }

		// Конструктор от другой ленивой строки
		lazy_basic_string (lazy_basic_string const & c)
			: data_(c.data_), size_(c.size_)
		{ }

		// Конструктор без параметров
		lazy_basic_string ()
			: data_(std::make_unique<CharT[]>(0)), size_(0)
		{ }

		// Конструктор неявный от C-строки
		lazy_basic_string (CharT const * c)	{
			size_ = Traits::length(c);
			data_ = std::make_unique<CharT[]>(size_ + 1);

			Traits::copy(data_.get(), c, size_);
		}

		// Конструктор для заполнения заданным количеством указанных символов
		lazy_basic_string (size_t n, const CharT & a)
			: data_(std::make_unique<CharT[]>(n + 1)), size_(n) {

			Traits::assign(data_.get(), n, a);
			Traits::assign(data_.get() + n, 1, CharT());
		}

		//Деструктор
		~lazy_basic_string() {
			clear();
		}

		// Оператор присваивания
		lazy_basic_string& operator=(lazy_basic_string const & c) {
			data_ = c.data_;
			size_ = c.size();
			return *this;
		}

		// Оператор сложения += для ленивой строки
		lazy_basic_string & operator+= (const lazy_basic_string & input_string) {
			size_t current_size = size();
			size_t add_size = input_string.size();
			size_t new_size = current_size + add_size;

			std::shared_ptr<CharT[]> new_data = std::make_unique<CharT[]>(new_size + 1);

			Traits::copy(new_data.get(), data_.get(), current_size);
			Traits::copy(new_data.get() + current_size, input_string.data_.get(), add_size);

			data_ = new_data;
			size_ = new_size;

			return *this;
		}

		// Оператор сложения += для C-строки
		lazy_basic_string & operator+= (const CharT * input_string) {
			size_t current_size = size();
			size_t add_size = Traits::length(input_string);
			size_t new_size = current_size + add_size;

			std::shared_ptr<CharT[]> new_data = std::make_unique<CharT[]>(new_size + 1);

			Traits::copy(new_data.get(), data_.get(), current_size);
			Traits::copy(new_data.get() + current_size, input_string, add_size);

			data_ = new_data;
			size_ = new_size;

			return *this;
		}

		// Оператор сложения += для символа
		lazy_basic_string & operator+= (CharT const input_symbol) {
			size_t current_size = size();
			size_t add_size = 1;
			size_t new_size = current_size + add_size;

			std::shared_ptr<CharT[]> new_data = std::make_unique<CharT[]>(new_size + 1);

			Traits::copy(new_data.get(), data_.get(), current_size);
			Traits::copy(new_data.get() + current_size, &input_symbol, add_size);

			data_ = new_data;
			size_ = new_size;

			return *this;
		}

		// Оператор сложения + для ленивой строки
		lazy_basic_string operator+ (const lazy_basic_string & input_string) const {
			size_t current_size = size();
			size_t add_size = input_string.size();
			size_t new_size = current_size + add_size;

			std::shared_ptr<CharT[]> new_data = std::make_unique<CharT[]>(new_size + 1);

			Traits::copy(new_data.get(), data_.get(), current_size);
			Traits::copy(new_data.get() + current_size, input_string.data_.get(), add_size);

			return lazy_basic_string({new_data, new_size});
		}

		// Оператор сложения + для C-строки
		lazy_basic_string operator+ (const CharT * input_string) const {
			size_t current_size = size();
			size_t add_size = Traits::length(input_string);
			size_t new_size = current_size + add_size;

			std::shared_ptr<CharT[]> new_data = std::make_unique<CharT[]>(new_size + 1);

			Traits::copy(new_data.get(), data_.get(), current_size);
			Traits::copy(new_data.get() + current_size, input_string, add_size);

			return lazy_basic_string({new_data, new_size});
		}

		// Оператор сложения + для символа
		lazy_basic_string operator+ (const CharT input_symbol) const {
			size_t current_size = size();
			size_t add_size = 1;
			size_t new_size = current_size + add_size;

			std::shared_ptr<CharT[]> new_data = std::make_unique<CharT[]>(new_size + 1);

			Traits::copy(new_data.get(), data_.get(), current_size);
			Traits::copy(new_data.get() + current_size, &input_symbol, add_size);

			return lazy_basic_string({new_data, new_size});
		}

		// Константное обращение по индексу []
		CharT operator[](size_t i) const {
			return *(data_.get() + i);
		}

		// Не константное обращение по индексу []
		Proxy<CharT, Traits> operator[](size_t i) {
			return Proxy(this, i);
		}

		// Сравнение ленивых строк между собой ==
		bool operator== (const lazy_basic_string & input_string) const {
			return Traits::compare(data_.get(), input_string.data_.get(), size()) == 0;
		}

		// Сравнение ленивых строк с C-строками ==
		bool operator== (const CharT * input_string) const {
			return Traits::compare(data_.get(), input_string, size()) == 0;
		}

		// Сравнение ленивых строк между собой !=
		bool operator!= (const lazy_basic_string & input_string) const {
			return !operator == (input_string);
		}

		// Сравнение ленивых строк с C-строками !=
		bool operator!= (const CharT * input_string) const {
			return !operator == (input_string);
		}

		// Сравнение ленивых строк между собой <
		bool operator< (const lazy_basic_string & input_string) const {
			return Traits::compare(data_.get(), input_string.data_.get(), size()) < 0;
		}

		// Сравнение ленивых строк с C-строками <
		bool operator< (const CharT * input_string) const {
			return Traits::compare(data_.get(), input_string, size()) < 0;
		}

		// Методы, аналогичные std::basic_string

		void swap(lazy_basic_string & input_string) {
			std::swap(data_, input_string.data_);
			std::swap(size_, input_string.size_);
		}

		void clear() {
			data_ = std::make_unique<CharT[]>(0);
			size_ = 0;
		}

		size_t size() const {
			return size_;
		}

		bool empty() const {
			return !size();
		}

		CharT const * c_str() const {
			return data_.get();
		}

		void set_data(std::shared_ptr<CharT[]> data) {
			data_ = data;
		}

	private:
		std::shared_ptr<CharT[]> data_;
		size_t size_;
	};

	// Структура, обеспечивающая нечувствительность к регистру при сравнении
	template <class CharT>
	struct char_itraits : public std::char_traits<CharT> {
		static int compare(CharT const * string_1, CharT const * string_2, size_t size) {
			for (size_t i = 0; i != size; ++i) {

				if (std::toupper(string_1[i]) < std::toupper(string_2[i])) {
					return -1;
				}

				if (std::toupper(string_1[i]) > std::toupper(string_2[i])) {
					return 1;
				}
			}
			return 0;
		}
	};

	// Определение синонимов
	typedef lazy_basic_string<char> lazy_string;
	typedef lazy_basic_string<wchar_t> lazy_wstring;
	typedef lazy_basic_string<char, char_itraits<char>> lazy_istring;

} // namespace std_utils