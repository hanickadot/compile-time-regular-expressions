#ifndef STATIC_REGEXP_HPP
#define STATIC_REGEXP_HPP

#include "string.hpp"
#include <vector>

namespace sre {

template <typename... T> struct StateHelper;
	
template <> struct StateHelper<> {
	void save() { }
	void load() const { }
};	

template <typename First, typename... Rest> struct StateHelper<First, Rest...> {
	First first;
	StateHelper<Rest...> rest;
	StateHelper(const First & right, const Rest & ... lrest): first{right}, rest{lrest...} { }
	void save(const First & right, const Rest & ... lrest) {
		//printf("  obj: %p\n",&right);
		first = right;
		rest.save(lrest...);
	}
	void load(First & right, Rest & ... lrest) const {
		right = first;
		rest.load(lrest...);
	}
};

template <typename... T> struct State {
	StateHelper<T...> helper;
	State(T & ... rest): helper{rest...} { }
	void save(const T & ... rest) {
		helper.save(rest...);
	}
	void load(T & ... rest) const {
		helper.load(rest...);
	}
};

struct PositionPair {
	mutable unsigned int begin;
	mutable unsigned int end;
	unsigned int len() const {
		return end - begin;
	}
	template <typename string_t> StringRange<string_t> operator()(StringRef<string_t> && view) const {
		return StringRange<string_t>{view.getRef()+begin, view.getRef()+end};
	}
	template <typename string_t> StringRange<string_t> operator()(StringRef<string_t> & view) const {
		return StringRange<string_t>{view.getRef()+begin, view.getRef()+end};
	}
	template <typename string_t> StringRange<string_t> operator()(const string_t && str) const {
		return StringRange<string_t>{str.begin()+begin, str.begin()+end};
	}
	template <typename string_t> StringRange<string_t> operator()(const string_t & str) const {
		return StringRange<string_t>{str.begin()+begin, str.begin()+end};
	}
};

class CatchRange {
protected:
	mutable const PositionPair * ptrBegin{nullptr};
	mutable const PositionPair * ptrEnd{nullptr};
public:
	CatchRange() { }
	CatchRange(const PositionPair * lptrBegin, const PositionPair * lptrEnd): ptrBegin{lptrBegin}, ptrEnd{lptrEnd} { }
	const PositionPair * begin() const {
		return ptrBegin;
	}
	const PositionPair * end() const {
		return ptrEnd;
	}
	const PositionPair & operator[](size_t id) const {
		return *(ptrBegin+id);
	}
	size_t size() const {
		return ptrEnd - ptrBegin;
	}
	operator bool() const {
		return ptrEnd != ptrBegin;
	}
};

template <size_t maxCount> class StaticMemory {
protected:
	size_t count{0};
	PositionPair list[maxCount];
public:
	void store(PositionPair && pair) {
		if (count < maxCount) {
			list[count++] = pair;
		}
	}
	const PositionPair * begin() const {
		return &list[0];
	}
	const PositionPair * end() const {
		return &list[count];
	}
	CatchRange getRange() const {
		return {begin(),end()};
	}
	void reset() {
		count = 0;
	}
	StaticMemory & operator=(const StaticMemory & right) {
		count = right.count;
		//printf("moving with memory: %p\n",this);
		for (unsigned int i{0}; i != maxCount; ++i) {
			list[i] = right.list[i];
		}
		return *this;
	}
};	

class DynamicMemory {
protected:
	std::vector<PositionPair> data;
public:
	void store(PositionPair && pair) {
		data.emplace_back(std::forward<PositionPair>(pair));
	}
	const PositionPair * begin() const {
		return data.empty() ? nullptr : &data.front();
	}
	const PositionPair * end() const {
		return data.empty() ? nullptr : &data.back() + 1;
	}
	CatchRange getRange() const {
		return {begin(),end()};
	}
	void reset() {
		data = {};
	}
	DynamicMemory & operator=(const DynamicMemory & right) = default;
};

using OneMemory = StaticMemory<1>;

class PositiveClosure {
public:
	template <typename string_t> bool operator()(sre::StringRef<string_t> && view) {
		// there is no FarRight or Right, because Closure should be last thing on the right side :)
		return true;
	}
};

class Begin {
public:
	template <typename Right, typename... FarRight, typename string_t> bool operator()(sre::StringRef<string_t> && view, Right & right, FarRight & ... fright) {
		return view.isBegin() && right(std::forward<sre::StringRef<string_t>>(view), fright...);
	}
	template <unsigned int> size_t get(CatchRange &) const {
		return 0;
	}
	void resetMemory() { }
	template <unsigned int> unsigned int getId() const {
		return 0;
	}
};

class End {
public:
	template <typename Right, typename... FarRight, typename string_t> bool operator()(sre::StringRef<string_t> && view, Right & right, FarRight & ... fright) {
		return view.isEnd() && right(std::forward<sre::StringRef<string_t>>(view), fright...);
	}
	template <unsigned int> size_t get(CatchRange &) const {
		return 0;
	}
	void resetMemory() { }
	template <unsigned int> unsigned int getId() const {
		return 0;
	}
};

class Empty {
public:
	template <typename Right, typename... FarRight, typename string_t> bool operator()(sre::StringRef<string_t> && view, Right & right, FarRight & ... fright) {
		return right(std::forward<sre::StringRef<string_t>>(view), fright...);
	}
	template <unsigned int> size_t get(CatchRange &) const {
		return 0;
	}
	void resetMemory() { }
	template <unsigned int> unsigned int getId() const {
		return 0;
	}
};

template <unsigned int key, unsigned int value> class Identifier {
protected:
	bool matched{false};
public:
	template <typename Right, typename... FarRight, typename string_t> bool operator()(sre::StringRef<string_t> && view, Right & right, FarRight & ... fright) {
		if (right(std::forward<sre::StringRef<string_t>>(view), fright...)) {
			return matched = true;
		}
		return matched = false;
	}
	template <unsigned int> size_t get(CatchRange &) const {
		return 0;
	}
	void resetMemory() { matched = false; }
	template <unsigned int reqkey> unsigned int getId() const {
		if (key == reqkey && matched) return value;
		else return 0;
	}
};

template <unsigned int key, unsigned int value> using Id = Identifier<key,value>;

template <unsigned int... c> struct CharHelper;

template <> struct CharHelper<> {
	template <typename char_t> static bool anyEqual(char_t) {
		return false;
	}
	template <typename char_t> static bool noEqual(char_t) {
		return true;
	}
};

template <unsigned int c, unsigned int... rest> struct CharHelper<c,rest...> {
	template <typename char_t> static bool anyEqual(char_t value) {
		return value == c || CharHelper<rest...>::anyEqual(value);
	}
	template <typename char_t> static bool noEqual(char_t value) {
		return value != c && CharHelper<rest...>::noEqual(value);
	}
};

template <unsigned int... c> class Char {
public:
	template <typename Right, typename... FarRight, typename string_t> bool operator()(sre::StringRef<string_t> && view, Right & right, FarRight & ... fright) {
		// i can use folded expression in c++17
		// ((*view == c) || ...) 
		return !view.isEnd() && CharHelper<c...>::anyEqual(*view) && right(std::forward<sre::StringRef<string_t>>(view).next(), fright...);
	}
	template <unsigned int> size_t get(CatchRange &) const {
		return 0;
	}
	void resetMemory() { }
	template <unsigned int> unsigned int getId() const {
		return 0;
	}
};

using White = Char<'\n','\r','\t',' '>;

template <unsigned int... c> class NegChar {
public:
	template <typename Right, typename... FarRight, typename string_t> bool operator()(sre::StringRef<string_t> && view, Right & right, FarRight & ... fright) {
		// i can use folded expression in c++17
		// ((*view != c) && ...)
		return !view.isEnd() && CharHelper<c...>::noEqual(*view) && right(std::forward<sre::StringRef<string_t>>(view).next(), fright...);
	}
	template <unsigned int> size_t get(CatchRange &) const {
		return false;
	}
	void resetMemory() { }
	template <unsigned int> unsigned int getId() const {
		return 0;
	}
};

class Anything {
public:
	template <typename Right, typename... FarRight, typename string_t> bool operator()(sre::StringRef<string_t> && view, Right & right, FarRight & ... fright) {
		return !view.isEnd() && right(std::forward<sre::StringRef<string_t>>(view).next(), fright...);
	}
	template <unsigned int> size_t get(CatchRange &) const {
		return false;
	}
	void resetMemory() { }
	template <unsigned int> unsigned int getId() const {
		return 0;
	}
};

template <unsigned int...> struct RangeDefinition;

template <unsigned int min, unsigned int max, unsigned int... rest> struct RangeDefinition<min, max, rest...> {
	template <typename value_type> static bool isContains(value_type c) {
		return ((min <= c) && (c <= max)) || RangeDefinition<rest...>::isContains(c);
	}
	template <typename value_type> static bool doesnotContains(value_type c) {
		return !((min <= c) && (c <= max)) && RangeDefinition<rest...>::doesnotContains(c);
	}
};

template <> struct RangeDefinition<> {
	template <typename value_type> static bool isContains(value_type) {
		return false;
	}
	template <typename value_type> static bool doesnotContains(value_type) {
		return true;
	}
};

template <unsigned int... def> class Range {
public:
	template <typename Right, typename... FarRight, typename string_t> bool operator()(sre::StringRef<string_t> && view, Right & right, FarRight & ... fright) {
		return !view.isEnd() && (RangeDefinition<def...>::isContains(*view)) && right(std::forward<sre::StringRef<string_t>>(view).next(), fright...);
	}
	template <unsigned int> size_t get(CatchRange &) const {
		return 0;
	}
	void resetMemory() { }
	template <unsigned int> unsigned int getId() const {
		return 0;
	}
};

template <unsigned int... def> class NegativeRange {
public:
	template <typename Right, typename... FarRight, typename string_t> bool operator()(sre::StringRef<string_t> && view, Right & right, FarRight & ... fright) {
		return !view.isEnd() && (RangeDefinition<def...>::doesnotContains(*view)) && right(std::forward<sre::StringRef<string_t>>(view).next(), fright...);
	}
	template <unsigned int> size_t get(CatchRange &) const {
		return 0;
	}
	void resetMemory() { }
	template <unsigned int> unsigned int getId() const {
		return 0;
	}
};

using Number = Range<'0','9'>;
using Alpha = Range<'a','z','A','Z'>;
using AlphaNumeric = Range<'a','z','A','Z','0','9'>;


template <typename... Parts> class Sequence;

template <> class Sequence<> {
public:
	template <typename Right, typename... FarRight, typename string_t> bool operator()(sre::StringRef<string_t> && view, Right & right, FarRight & ... fright) {
		return right(std::forward<sre::StringRef<string_t>>(view), fright...);
	}
	template <unsigned int> size_t get(CatchRange &) const {
		return 0;
	}
	void resetMemory() { }
	template <unsigned int> unsigned int getId() const {
		return 0;
	}
};

template <typename First> class Sequence<First>: public First {
public:
	template <typename Right, typename... FarRight, typename string_t> bool operator()(sre::StringRef<string_t> && view, Right & right, FarRight & ... fright) {
		return First::operator()(std::forward<sre::StringRef<string_t>>(view), right, fright...);
	}
	template <unsigned int reqid> size_t get(CatchRange & cr) const {
		return First::template get<reqid>(cr);
	}
	void resetMemory() {
		First::resetMemory();
	}
	template <unsigned int reqkey> unsigned int getId() const {
		return First::template getId<reqkey>();
	}
};

template <typename First, typename... Rest> class Sequence<First, Rest...>: public First {
protected:
	Sequence<Rest...> rest;
public:
	template <typename Right, typename... FarRight, typename string_t> bool operator()(sre::StringRef<string_t> && view, Right & right, FarRight & ... fright) {
		return First::operator()(std::forward<sre::StringRef<string_t>>(view), rest, right, fright...);
	}
	template <unsigned int reqid> size_t get(CatchRange & cr) const {
		if (size_t sz = First::template get<reqid>(cr)) {
			return sz;
		} else {
			return rest.template get<reqid>(cr);
		}
	}
	void resetMemory() {
		First::resetMemory();
		rest.resetMemory();
	}
	template <unsigned int reqkey> unsigned int getId() const {
		unsigned int tmp{First::template getId<reqkey>()};
		return tmp ? tmp : rest.template getId<reqkey>();
	}
};

template <unsigned int... c> using String = Sequence<Char<c>...>;

template <typename... Options> class Select;

template <> class Select<> {
public:
	template <typename Right, typename... FarRight, typename string_t> bool operator()(sre::StringRef<string_t> &&, Right &, FarRight & ...) const {
		return false;
	}
	template <unsigned int reqid> size_t get(CatchRange & cr) const {
		return false;
	}
	void resetMemory() { }
	template <unsigned int> unsigned int getId() const {
		return 0;
	}
};

template <typename First> class Select<First>: public First {
public:
	template <typename Right, typename... FarRight, typename string_t> bool operator()(sre::StringRef<string_t> && view, Right & right, FarRight & ... fright) {
		return First::operator()(std::forward<sre::StringRef<string_t>>(view), right, fright...);
	}
	template <unsigned int reqid> size_t get(CatchRange & cr) const {
		return First::template get<reqid>(cr);
	}
	void resetMemory() {
		First::resetMemory();
	}
	template <unsigned int reqkey> unsigned int getId() const {
		return First::template getId<reqkey>();
	}
};

template <typename First, typename... Rest> class Select<First, Rest...>: public First {
protected:
	Select<Rest...> rest;
public:
	template <typename Right, typename... FarRight, typename string_t> bool operator()(sre::StringRef<string_t> && view, Right & right, FarRight & ... fright) {
		State<Right, FarRight...> state{right, fright...};
		if (First::operator()(std::forward<sre::StringRef<string_t>>(view), right, fright...)) {
			return true;
		} else {
			state.load(right, fright...);
			return rest(std::forward<sre::StringRef<string_t>>(view), right, fright...);
		}
	}
	template <unsigned int reqid> size_t get(CatchRange & cr) const {
		if (size_t sz = First::template get<reqid>(cr)) {
			return sz;
		} else {
			return rest.template get<reqid>(cr);
		}
	}
	void resetMemory() {
		First::resetMemory();
		rest.resetMemory();
	}
	template <unsigned int reqkey> unsigned int getId() const {
		unsigned int tmp{First::template getId<reqkey>()};
		return tmp ? tmp : rest.template getId<reqkey>();
	}
};

template <typename... Inner> class Optional: Sequence<Inner...> {
public:
	template <typename Right, typename... FarRight, typename string_t> bool operator()(sre::StringRef<string_t> && view, Right & right, FarRight & ... fright) {
		State<Right, FarRight...> state{right, fright...};
		if (Sequence<Inner...>::operator()(std::forward<sre::StringRef<string_t>>(view), right, fright...)) {
			return true;
		} else {
			state.load(right, fright...);
			return right(std::forward<sre::StringRef<string_t>>(view), fright...);
		}
	}
	template <unsigned int reqid> size_t get(CatchRange & cr) const {
		return Sequence<Inner...>::template get<reqid>(cr);
	}
	void resetMemory() {
		Sequence<Inner...>::resetMemory();
	}
	template <unsigned int reqkey> unsigned int getId() const {
		return Sequence<Inner...>::template getId<reqkey>();
	}
};

template <unsigned int count, typename... Inner> class ExactRepeat: public Sequence<Inner...> {
protected:
	template <typename string_t> struct Helper {
		mutable sre::StringRef<string_t> storedView;
		bool operator()(sre::StringRef<string_t> && view) const {
			storedView = std::move(view);
			return true;
		};
	};
public:
	template <typename Right, typename... FarRight, typename string_t> bool operator()(sre::StringRef<string_t> && view, Right & right, FarRight & ... fright) {
		Helper<string_t> helper{std::forward<sre::StringRef<string_t>>(view)};
		for (unsigned int i{0}; i != count; ++i) {
			if (!Sequence<Inner...>::operator()(std::move(helper.storedView), helper)) return false;
		}
		return right(std::move(helper.storedView), fright...);
	}
	template <unsigned int reqid> size_t get(CatchRange & cr) const {
		return Sequence<Inner...>::template get<reqid>(cr);
	}
	void resetMemory() {
		Sequence<Inner...>::resetMemory();
	}
	template <unsigned int reqkey> unsigned int getId() const {
		return Sequence<Inner...>::template getId<reqkey>();
	}
};

template <typename... Inner> class ExactRepeat<0,Inner...> {
public:
	template <typename Right, typename... FarRight, typename string_t> bool operator()(sre::StringRef<string_t> && view, Right & right, FarRight & ... fright) {
		return right(std::forward<sre::StringRef<string_t>>(view), fright...);
	}
	template <unsigned int> size_t get(CatchRange &) const {
		return 0;
	}
	void resetMemory() { }
	template <unsigned int> unsigned int getId() const {
		return 0;
	}
};


template <unsigned int min, unsigned int max, bool gready, typename... Inner> class RepeatHelper: public Sequence<Inner...> {
protected:
	template <typename string_t> struct Helper {
		mutable sre::StringRef<string_t> storedView;
		bool operator()(sre::StringRef<string_t> && view) const {
			storedView = std::move(view);
			return true;
		};
		void operator=(const Helper &) const {
			// this is here to avoid copying
		}
	};
public:
	template <typename Right, typename... FarRight, typename string_t> bool operator()(sre::StringRef<string_t> && view, Right & right, FarRight & ... fright) {
		Helper<string_t> helper{std::forward<sre::StringRef<string_t>>(view)};
		bool havePositive{false};
	
		//printf("cycle: %p (helper = %p)\n",this,&helper);
		State<decltype(*this), Right, FarRight...> success{*this, right, fright...};
		for (unsigned int i{0};; ++i) {
			
			if (max && max < i) break;
			else if (!min || (min && min <= i)) {
				State<decltype(*this), Right, FarRight...> state{*this,right, fright...};
		
				if (right(helper.storedView.copy(),fright...)) {
					if (gready) {
						havePositive = true;
						success.save(*this, right, fright...);
						state.load(*this, right, fright...);
					} else {
						// MAYBE this need fix
						return true;
					}
				}
			}
			
			if (!Sequence<Inner...>::operator()(helper.storedView.copy(), helper)) {
				break;
			}
		}
		if (havePositive) {
			success.load(*this, right, fright...);
			return true;
		}
		return false;
	}
	template <unsigned int reqid> size_t get(CatchRange & cr) const {
		return Sequence<Inner...>::template get<reqid>(cr);
	}
	void resetMemory() {
		Sequence<Inner...>::resetMemory();
	}
	template <unsigned int reqkey> unsigned int getId() const {
		return Sequence<Inner...>::template getId<reqkey>();
	}
};

//template <unsigned int count, typename... Inner> class Repeat<count,count,Inner...>: public ExactRepeat<count,Inner...> { };

template <unsigned int min, unsigned int max, typename... Inner> using Repeat = RepeatHelper<min,max,true,Inner...>;
template <unsigned int min, unsigned int max, typename... Inner> using NGRepeat = RepeatHelper<min,max,false,Inner...>;

template <typename... Inner> using Plus = RepeatHelper<1,0,true,Inner...>;
template <typename... Inner> using Star = RepeatHelper<0,0,true,Inner...>;

template <typename... Inner> using NGPlus = RepeatHelper<1,0,false,Inner...>;
template <typename... Inner> using NGStar = RepeatHelper<0,0,false,Inner...>;


template <unsigned int id, typename Storage, typename... Inner> class Catch: public Sequence<Inner...> {
protected:
	struct Helper {
		Storage storage;
		unsigned int firstPos;
		template <typename Right, typename... FarRight, typename string_t> bool operator()(sre::StringRef<string_t> && view, Right & right, FarRight & ... fright) {
			unsigned int secondPos{view.getPosition()};
			if (right(std::forward<sre::StringRef<string_t>>(view), fright...)) {
				storage.store({firstPos,secondPos});
				return true;
			}
			else return false;
		};
	};
	Helper helper;
public:
	template <typename Right, typename... FarRight, typename string_t> bool operator()(sre::StringRef<string_t> && view, Right & right, FarRight & ... fright) {
		helper.firstPos = view.getPosition();
		return Sequence<Inner...>::operator()(std::forward<sre::StringRef<string_t>>(view), helper, right, fright...);
	}
	template <unsigned int reqid> size_t get(CatchRange & cr) const {
		if (id == reqid) {
			return (cr = helper.storage.getRange()).size();
		} else {
			return Sequence<Inner...>::template get<reqid>(cr);
		}
	}
	void resetMemory() {
		helper.storage.reset();
		Sequence<Inner...>::resetMemory();
	}
	template <unsigned int reqkey> unsigned int getId() const {
		return Sequence<Inner...>::template getId<reqkey>();
	}
};

template <unsigned int id, typename... Inner> using OneCatch = Catch<id, OneMemory, Inner...>;
template <unsigned int id, size_t count, typename... Inner> using StaticCatch = Catch<id, StaticMemory<count>, Inner...>;
template <unsigned int id, typename... Inner> using DynamicCatch = Catch<id, DynamicMemory, Inner...>;

template <typename... Inner> class Floating: public Sequence<Inner...> {
public:
	template <typename Right, typename... FarRight, typename string_t> bool operator()(sre::StringRef<string_t> && view, Right & right, FarRight & ... fright) {
		if (view.isEnd() && Sequence<Inner...>::operator()(std::forward<sre::StringRef<string_t>>(view), right, fright...)) {
			return true;
		} else {
			while (!view.isEnd()) {
				if (Sequence<Inner...>::operator()(std::forward<sre::StringRef<string_t>>(view), right, fright...)) {
					return true;
				} else {
					view.move();
				}
			}
			return false;
		}
	}
	template <unsigned int reqid> size_t get(CatchRange & cr) const {
		return Sequence<Inner...>::template get<reqid>(cr);
	}
	void resetMemory() {
		Sequence<Inner...>::resetMemory();
	}
	template <unsigned int reqkey> unsigned int getId() const {
		return Sequence<Inner...>::template getId<reqkey>();
	}
};

template <typename... Inner> class RegExp: public Sequence<Inner...> {
public:
	template <typename string_t> bool match(sre::StringRef<string_t> && view) {
		PositiveClosure closure;
		Sequence<Inner...>::resetMemory();
		return Sequence<Inner...>::operator()(std::forward<sre::StringRef<string_t>>(view), closure);
	}
	template <typename string_t> bool match(sre::StringRef<string_t> & view) {
		PositiveClosure closure;
		Sequence<Inner...>::resetMemory();
		return Sequence<Inner...>::operator()(std::forward<sre::StringRef<string_t>>(view), closure);
	}
	template <typename string_t> bool match(const string_t && str) {
		return match(sre::make_sref(str));
	}
	template <typename string_t> bool match(const string_t & str) {
		return match(sre::make_sref(str));
	}
	template <unsigned int reqid> size_t getCatchRef(CatchRange & cr) const {
		return Sequence<Inner...>::template get<reqid>(cr);
	}
	template <unsigned int reqid> CatchRange getCatch() const {
		CatchRange cr;
		Sequence<Inner...>::template get<reqid>(cr);
		return cr;
	}
	template <unsigned int reqkey> unsigned int getId() const {
		return Sequence<Inner...>::template getId<reqkey>();
	}
};

template <typename... Inner> class FloatingRegExp: public Floating<Inner...> {
public:
	template <typename string_t> bool match(sre::StringRef<string_t> && view) {
		PositiveClosure closure;
		Floating<Inner...>::resetMemory();
		return Floating<Inner...>::operator()(std::forward<sre::StringRef<string_t>>(view), closure);
	}
	template <typename string_t> bool match(sre::StringRef<string_t> & view) {
		PositiveClosure closure;
		Floating<Inner...>::resetMemory();
		return Floating<Inner...>::operator()(std::forward<sre::StringRef<string_t>>(view), closure);
	}
	template <typename string_t> bool match(const string_t && str) {
		return match(sre::make_sref(str));
	}
	template <typename string_t> bool match(const string_t & str) {
		return match(sre::make_sref(str));
	}
	template <unsigned int reqid> size_t getCatchRef(CatchRange & cr) const {
		return Floating<Inner...>::template get<reqid>(cr);
	}
	template <unsigned int reqid> CatchRange getCatch() const {
		CatchRange cr;
		Floating<Inner...>::template get<reqid>(cr);
		return cr;
	}
	template <unsigned int reqkey> unsigned int getId() const {
		return Sequence<Inner...>::template getId<reqkey>();
	}
};

}

#endif
