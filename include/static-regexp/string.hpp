#ifndef STATIC_REGEXP__STRING_HPP
#define STATIC_REGEXP__STRING_HPP

#include <string>

namespace sre {
	
template <typename T> class StringRange {
public:
	using const_iterator = typename T::const_iterator;
protected:
	const_iterator beginValue;
	const_iterator endValue;
public:
	StringRange(const_iterator begin, const_iterator end): beginValue{begin}, endValue{end} { }
	const_iterator begin() const {
		return beginValue;
	}
	const_iterator end() const {
		return endValue;
	}
	T toString() const {
		return T(beginValue,endValue);
	}
};

template <typename T> class StringRef;

template <class CharT, class Traits, class Allocator> class StringRef<const std::basic_string<CharT, Traits, Allocator>> {
public:
	using T = const std::basic_string<CharT, Traits, Allocator>;
	using const_iterator = typename T::const_iterator;
	using value_type = typename T::value_type;
protected:
	const T & ref;
	const_iterator current;
public:
	StringRef(const T & source): ref{source}, current{ref.cbegin()} { }
	StringRef(const StringRef & orig): ref{orig.ref}, current{orig.current} { }
	StringRef(const StringRef & orig, size_t move): ref{orig.ref}, current{orig.current+move} { }
	StringRef(StringRef && orig): ref{orig.ref}, current{orig.current} { }
	~StringRef() = default;
	unsigned int getPosition() const {
		return current - ref.cbegin();
	}
	bool isBegin() const {
		return current == ref.cbegin();
	}
	bool isEnd() const {
		return current == ref.cend();
	}
	value_type get() const {
		return *current;
	}
	StringRef copy() const {
		return StringRef(*this);
	}
	StringRef next() const {
		return StringRef(*this,1);
	}
	void move() {
		current++;
	}
	const T & getRef() const {
		return ref;
	}
	const value_type & operator*() const {
		return *current;
	}
	StringRef & operator=(StringRef && right) {
		current = right.current;
		return *this;
	}
	StringRef & operator=(const StringRef & right) {
		current = right.current;
		return *this;
	}
};

template <typename CharT> class StringRef<const CharT * const> {
public:
	using T = const CharT *;
	using const_iterator = T;
	using value_type = decltype(*T{});
protected:
	const CharT * ref;
	const CharT * current;
public:
	StringRef(const CharT * source): ref{source}, current{ref} { }
	StringRef(const StringRef & orig): ref{orig.ref}, current{orig.current} { }
	StringRef(const StringRef & orig, size_t move): ref{orig.ref}, current{orig.current+move} { }
	StringRef(StringRef && orig): ref{orig.ref}, current{orig.current} { }
	~StringRef() = default;
	unsigned int getPosition() const {
		return current - ref;
	}
	bool isBegin() const {
		return current == ref;
	}
	bool isEnd() const {
		return *current == '\0';
	}
	value_type get() const {
		return *current;
	}
	StringRef copy() const {
		return StringRef(*this);
	}
	StringRef next() const {
		return StringRef(*this,1);
	}
	void move() {
		current++;
	}
	const CharT * getRef() const {
		return ref;
	}
	const CharT & operator*() const {
		return *current;
	}
	StringRef & operator=(StringRef && right) {
		current = right.current;
		return *this;
	}
	StringRef & operator=(const StringRef & right) {
		current = right.current;
		return *this;
	}	
};

template <typename T> StringRef<T> make_sref(T & ref);
template <typename T> StringRef<T> make_sref(T && ref);

template <class CharT, class Traits, class Allocator> StringRef<const std::basic_string<CharT, Traits, Allocator>> make_sref(const std::basic_string<CharT, Traits, Allocator> & ref) {
	return StringRef<const std::basic_string<CharT, Traits, Allocator>>(ref);
}

template <class CharT, class Traits, class Allocator> StringRef<const std::basic_string<CharT, Traits, Allocator>> make_sref(const std::basic_string<CharT, Traits, Allocator> && ref) {
	return StringRef<const std::basic_string<CharT, Traits, Allocator>>(ref);
}

template <class CharT> StringRef<const CharT * const> make_sref(CharT * const ref) {
	return StringRef<const CharT * const>(ref);
}

template <class CharT> StringRef<const CharT * const> make_sref(const CharT * const ref) {
	return StringRef<const CharT * const>(ref);
}


}

#endif