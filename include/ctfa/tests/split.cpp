#include <ctfa/helper/interval.hpp>

static_assert(3 == [](){
	ctfa::intervals<2, int, int> i;
	i.insert_range(0, 10, 1);
	i.insert_range(5, 15, 2);
	return i.merge_and_split();
}());

static_assert(3 == [](){
	ctfa::intervals<3, int, int> i;
	i.insert_range(0, 5, 1);
	i.insert_range(0, 10, 2);
	i.insert_range(0, 15, 3);
	return i.merge_and_split();
}());

static_assert(1 == [](){
	ctfa::intervals<2, int, int> i;
	i.insert_range(0, 5, 1);
	i.insert_range(0, 5, 2);
	return i.merge_and_split();
}());

static_assert(1 == [](){
	ctfa::intervals<1, int, int> i;
	i.insert_range(0, 5, 1);
	return i.merge_and_split();
}());

static_assert(0 == [](){
	ctfa::intervals<1, int, int> i;
	return i.merge_and_split();
}());

static_assert(3 == [](){
	ctfa::intervals<3, int, int> i;
	i.insert_range(0, 5, 1);
	i.insert_range(10, 15, 1);
	i.insert_range(20, 25, 1);
	return i.merge_and_split();
}());

static_assert(1 == [](){
	ctfa::intervals<3, int, int> i;
	i.insert_range(0, 5, 1);
	i.insert_range(0, 10, 1);
	i.insert_range(0, 15, 1);
	return i.merge_and_split();
}());

static_assert(1 == [](){
	ctfa::intervals<3, int, int> i;
	i.insert_range(0, 5, 1);
	i.insert_range(4, 10, 1);
	i.insert_range(9, 15, 1);
	return i.merge_and_split();
}());

static_assert(1 == [](){
	ctfa::intervals<3, int, int> i;
	i.insert_range(0, 5, 1);
	i.insert_range(5, 10, 1);
	i.insert_range(10, 15, 1);
	return i.merge_and_split();
}());

static_assert(1 == [](){
	ctfa::intervals<3, int, int> i;
	i.insert_range(0, 5, 1);
	i.insert_range(5, 10, 1);
	i.insert_range(10, 15, 1);
	return i.merge_and_split();
}());

static_assert(3 == [](){
	ctfa::intervals<3, int, int> i;
	i.insert_range(0, 30, 1);
	i.insert_range(10, 20, 2);
	i.insert_range(25, 30, 1);
	// 0..9
	// 10..20
	// 21..30
	return i.merge_and_split();
}());

static_assert(3 == [](){
	ctfa::intervals<3, int, int> i;
	i.insert_range(0, 30, 1);
	i.insert_range(10, 20, 2);
	i.insert_range(25, 31, 1);
	// 0..9
	// 10..20
	// 21..31
	return i.merge_and_split();
}());

static_assert(2 == [](){
	ctfa::intervals<3, int, int> i;
	i.insert_range(1, 5, 1);
	i.insert_range(10, 15, 1);
	// 1..5
	// 10..15
	return i.merge_and_split();
}());

static_assert(3 == [](){
	ctfa::intervals<3, int, int> i;
	i.insert_range(0, 30, 1);
	i.insert_range(10, 20, 2);
	return i.merge_and_split();
}());

static_assert(2 == [](){
	ctfa::intervals<2, int, int> i;
	i.insert_range(0, 10, 1);
	return i.buffer.size();
}());

static_assert(3 == [](){
	ctfa::intervals<3, int, int> i;
	i.insert_range(0, 1000, 1);
	i.insert_range(10, 100, 2);
	return i.merge_and_split();
}());

static_assert(3 == [](){
	ctfa::intervals<3, unsigned, int> i;
	i.insert_range(0, std::numeric_limits<unsigned>::max(), 1);
	i.insert_range(10, 100, 2);
	return i.merge_and_split();
}());

static_assert(3 == [](){
	ctfa::intervals<3, int, int> i;
	i.insert_range(0, std::numeric_limits<int>::max(), 1);
	i.insert_range(10, 100, 2);
	return i.merge_and_split();
}());

static_assert(1 == [](){
	ctfa::intervals<5, char32_t, int> i;
	i.insert_range(0, 0, 1);
	i.insert_range(4294967295ull, 4294967295ull, 1);
	i.insert_range(0, 'C', 1);
	i.insert_range('E', 4294967295ull, 1);
	i.insert_range('D', 'D', 1);
	return i.merge_and_split();
}());

static_assert(1 == [](){
	ctfa::intervals<5, char32_t, int> i;
	i.insert_range(0, 0, 1);
	i.insert_range(4294967295ull, 4294967295ull, 1);
	i.insert_range(0, 'C', 1);
	i.insert_range('E', 4294967295ull, 1);
	i.insert_range('D', 'D', 1);
	return i.merge_and_split();
}());



struct range {
	int low;
	int high;
	int feature;
};

static_assert(1 == ctfa::split_range<3>(std::initializer_list<range>{range{1,10,1}, range{1,10,1}}));

static_assert(1 == ctfa::split_range<3>(std::initializer_list<range>{range{1,5,1}, range{5,10,1}}));

static_assert(1 == ctfa::split_range<3>(std::initializer_list<range>{range{1,5,1}, range{6,10,1}}));

static_assert(2 == ctfa::split_range<3>(std::initializer_list<range>{range{1,5,1}, range{10,15,1}}));

