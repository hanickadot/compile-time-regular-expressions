#ifndef CTFA__CONTAINER__SET__HPP
#define CTFA__CONTAINER__SET__HPP

#include <cstddef>
#include <iterator>
#include "../helper/conditional.hpp"

namespace ctfa {

template <bool C, typename T, typename F> using conditional = typename conditional_helper<C>::template type<T,F>;

template <typename T, size_t Capacity> class set {
	// I'm using C array, because it's much quicker in compile-time
	size_t _size{0};
	
	struct storage {
		T _data[Capacity]{};
		constexpr const T * data() const {
			return _data;
		}
		constexpr T * data() {
			return _data;
		}
	};
	
	struct empty_storage {
		constexpr const T * data() const {
			return nullptr;
		}
		constexpr T * data() {
			return nullptr;
		}
	};
	
	
	using data_t = conditional<(Capacity > 0), storage, empty_storage>;
	
	data_t _data;
	
	template <typename, size_t> friend class set;
public:
	static constexpr size_t capacity = Capacity;
	using iterator = T *;
	constexpr set() { }
	
	constexpr set(const std::initializer_list<T> & list) {
		for (const auto & v: list) {
			insert(v);
		}
	}
	constexpr set(const set & rhs) = default;
	template <size_t S2> constexpr set(const set<T, S2> & rhs) {
		for (const auto & v: rhs) {
			push_back(v);
		}
	}
	constexpr const T * data() const {
		return _data.data();
	}
	constexpr T * data() {
		return _data.data();
	}
	constexpr auto begin() {
		return data();
	}
	constexpr auto end() {
		return data() + _size;
	}
	constexpr auto begin() const {
		return data();
	}
	constexpr auto end() const {
		return data() + _size;
	}
	constexpr size_t size() const {
		return _size;
	}
	constexpr const auto & first() const {
		return *_data;
	}
	constexpr const auto & last() const {
		return _data[_size - 1];
	}
	template <size_t C> constexpr bool operator==(const set<T,C> & rhs) const {
		if (_size != rhs._size) return false;
		for (size_t i = 0; i != _size; ++i) {
			if (!(data()[i] == rhs.data()[i])) return false;
		}
		return true;
	}
	template <size_t C> constexpr bool operator<(const set<T,C> & rhs) const {
		for (size_t i = 0; i != _size && i != rhs._size; ++i) {
			if (data()[i] == rhs.data()[i]) continue;
			else if (data()[i] < rhs.data()[i]) return true;
			else return false;
		}
		return _size < rhs._size;
	}
	constexpr auto & operator[](size_t idx) const {
		return data()[idx];
	} 
	constexpr auto & operator[](size_t idx) {
		return data()[idx];
	} 
	template <typename Arg> constexpr auto upper_bound(Arg && obj) {
		auto first = begin();
		auto last = end();
		auto it = first;
		size_t count = std::distance(first, last);
		while (count > 0) {
			it = first;
			size_t step = count / 2;
			std::advance(it, step);
			if (!(obj < *it)) {
				first = ++it;
				count -= step + 1;
			} else {
				count = step;
			}
		}
		return it;
	}
	template <typename Arg> constexpr auto upper_bound(Arg && obj) const {
		auto first = begin();
		auto last = end();
		auto it = first;
		size_t count = std::distance(first, last);
		while (count > 0) {
			it = first;
			size_t step = count / 2;
			std::advance(it, step);
			if (!(obj < *it)) {
				first = ++it;
				count -= step + 1;
			} else {
				count = step;
			}
		}
		return it;
	}
	template <typename Arg> constexpr auto lower_bound(Arg && obj) {
		auto first = begin();
		auto last = end();
		auto it = first;
		size_t count = std::distance(first, last);
		while (count > 0) {
			it = first;
			size_t step = count / 2;
			std::advance(it, step);
			if (*it < obj) {
				first = ++it;
				count -= step + 1;
			} else {
				count = step;
			}
		}
		return it;
	}
	template <typename Arg> constexpr auto lower_bound(Arg && obj) const {
		auto first = begin();
		auto last = end();
		auto it = first;
		size_t count = std::distance(first, last);
		while (count > 0) {
			it = first;
			size_t step = count / 2;
			std::advance(it, step);
			if (*it < obj) {
				first = ++it;
				count -= step + 1;
			} else {
				count = step;
			}
		}
		return it;
	}
	template <typename Arg> constexpr auto find(Arg && obj) {
		auto it = lower_bound(std::forward<Arg>(obj));
		if (it != end() && *it == obj) return it;
		else return end();
	}
	template <typename Arg> constexpr auto find(Arg && obj) const {
		auto it = lower_bound(std::forward<Arg>(obj));
		if (it != end() && *it == obj) return it;
		else return end();
	}
	template <typename Arg> constexpr auto find_linear(Arg && obj) {
		auto it = begin();
		while (it != end()) {
			if (*it == obj) return it;
			it++;
		}
		return it;
	}
	template <typename Arg> constexpr auto find_linear(Arg && obj) const {
		auto it = begin();
		while (it != end()) {
			if (*it == obj) return it;
			it++;
		}
		return it;
	}
	constexpr std::pair<iterator, bool> insert(T obj) {
		auto it = lower_bound(obj);
		if (it == end()) {
			[[maybe_unused]] auto check = data()[_size];
			*it = obj;
			_size++;
			return {it, true};
		} else if (*it == obj) {
			return {it, false};
		} else {
			[[maybe_unused]] auto check = data()[_size];
			auto out = it;
			while (it != end()) {
				// swap
				auto tmp = std::move(*it);
				*it = std::move(obj);
				obj = std::move(tmp);
				
				it++;
			}
			// swap
			auto tmp = std::move(*it);
			*it = std::move(obj);
			obj = std::move(tmp);
			
			_size++;
			return {out, true};
		}
	}
	constexpr void push_back(T obj) {
		auto it = end();
		*it = obj;
		_size++;
	}
	template <typename Arg> constexpr bool remove(Arg && obj) {
		auto it = lower_bound(std::forward<Arg>(obj));
		if (it != end() && *it == obj) {
			return remove(it);
		} else {
			return false;
		}
	}
	constexpr iterator remove(iterator erase_it) {
		iterator out = erase_it;
		if (erase_it == end()) { 
			return out;
		}
		
		while (erase_it != end()) {
			auto next = erase_it;
			next++;
			if (next != end()) {
				*erase_it = std::move(*next);
			}
			erase_it++;
		}
		_size--;
		return out;
	}
};

}

#endif
