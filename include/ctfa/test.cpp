#include <stack>
#include <set>
#include <iostream>
#include <vector>
#include <functional>
#include <limits>
#include <cassert>
#include <algorithm>
#include <map>
#include <optional>

class RangeSplitter {
	struct Point {
		enum Type {
			positive = 0,
			negative = 1
		};
		int position;
		Type type;
		
		int feature; // doesn't participate on ordering
	
		bool operator<(const Point & rhs) const {
			return std::tie(position, type, feature) < std::tie(rhs.position, rhs.type, rhs.feature);
		}
		bool operator==(const Point & rhs) const {
			return std::tie(position, type, feature) == std::tie(rhs.position, rhs.type, rhs.feature);
		}
		
		bool similar(const Point & rhs) const {
			return std::tie(position, type) == std::tie(rhs.position, rhs.type);
		}
		size_t distance(const Point & rhs) const {
			if (position < rhs.position) {
				return rhs.position - position;
			} else {
				return position - rhs.position;
			}
		}
		int get_starting_position() const {
			if (type == positive) return position;
			else return position+1;
		}
		int get_ending_position() const {
			if (type == positive) return position-1;
			else return position;
		}
		bool fusable(const Point & rhs) const {
			return ((feature == rhs.feature) && (type == negative && rhs.type == positive));
		}
	};
	
	std::multiset<Point> points;
	
public:
	std::multiset<Point> & getPoints() {
		return points;
	}
	void addRange(int low, int high, int feature) {
		// positive start range (with the value)
		points.emplace(Point{low, Point::positive, feature});
		// negative ends range (with the value)
		points.emplace(Point{high, Point::negative, feature});
		
		// range is |<---->| <low,high> 
	}
	
	using feature = int;
	
	void removeUnneededPoints() {
		std::map<feature, int> counter;
		
		auto add_feature = [&](feature f) {
			auto pair = counter.emplace(f, 1);
			if (!pair.second) {
				// point is not needed, feature was already there
				pair.first->second++;
				return false;
			} else {
				// point is NEEDED, it adds new feature
				return true;
			}
		};
		
		auto remove_feature = [&](feature f) {
			auto it = counter.find(f);
			if (it->second > 1) {
				it->second--;
				// point is not needed, it doesn't remove anything
				return false;
			} else {
				counter.erase(it);
				// point is NEEDED, it removes feature
				return true;
			}
		};
		
		auto try_fuse_points = [&](auto & it) {
			auto next = it;
			// search for neighbor point which is positive (I'm negatiev)
			while (next != points.end()) {
				// I can only fuse with neighbor
				if (it->distance(*next) == 0) {
					// skip these
				} else if (it->distance(*next) == 1) {
					// fusable ranges are next to each other
					if (it->fusable(*next)) {
						// and they are negative->positive with same feature
						points.erase(next);
						it = points.erase(it);
						return true;
					}
				} else {
					break;
				}
				
				// try next one
				next++;
			}
			return false;
		};
		
		auto it = points.begin();

		while (it != points.end()) {
			// fuse current point if possible
			// |---a---|
			//          |---a---|
			// should be:
			// |--------a-------|
			
			if (it->type == Point::negative) {
				if (try_fuse_points(it)) continue;
			}
			
			// remove unneeded points:
			// |---a---|
			//      |---a---|
			// should be:
			// |------a-----|
			//
			// or
			// |---a---|
			// |---a---|
			// or
			// |---a---|
			//         |---a---|
			if (it->type == Point::positive) {
				if (!add_feature(it->feature)) {
					std::cout << "remove + " << it->feature << " pos = " << it->position << "\n";
					it = points.erase(it);
					continue;
				}
			} else {
				if (!remove_feature(it->feature)) {
					std::cout << "remove - " << it->feature << " pos = " << it->position << "\n";
					it = points.erase(it);
					continue;
				}
			}
			it++;
		}
	}

	size_t iterateOver(std::function<void(int, int, const std::set<int> &)> cb) {
		removeUnneededPoints();
		
		size_t count = 0;
		
		std::set<feature> features;
		
		auto it = points.begin();
		std::optional<Point> last_point;
		
		while (it != points.end()) {
			if (last_point.has_value()) {
				cb(last_point->get_starting_position(), it->get_ending_position(), features);
				count++;
			}
			
			last_point = *it;
			
			auto first = it;
			if (it->type == Point::positive) {
				while (it != points.end() && it->similar(*first)) {
					features.emplace(it->feature);
					it++;
				}
			} else {
				while (it != points.end() && it->similar(*first)) {
					features.erase(it->feature);
					it++;
				}
			}
			if (features.size() == 0) {
				last_point = std::nullopt;
			}
		}
		return count;
		// application should return 
	}
};

// just test here

struct Range {
	int low;
	int high;
	std::set<int> features;
	bool operator==(const Range & rhs) const noexcept {
		return std::tie(low, high, features) == std::tie(rhs.low, rhs.high, rhs.features);
	}
};

void test0() {
	std::vector<int> points;
	
	RangeSplitter splitter;
	splitter.addRange(0,10,1);
	splitter.addRange(5,15,1);
	
	splitter.iterateOver([](int, int, const auto &){});
	
	for (auto p: splitter.getPoints()) {
		points.push_back(p.position);
	}
	
	assert(points.size() == 2);
	assert(points[0] == 0);
	assert(points[1] == 15);
}

void test0b() {
	std::vector<int> points;
	
	RangeSplitter splitter;
	splitter.addRange(0,0,1);
	splitter.addRange(0,0,1);
	
	splitter.iterateOver([](int, int, const auto &){});
	
	for (auto p: splitter.getPoints()) {
		points.push_back(p.position);
	}
	
	assert(points.size() == 2);
	assert(points[0] == 0);
	assert(points[1] == 0);
}

void test0c() {
	std::vector<int> points;
	
	RangeSplitter splitter;
	splitter.addRange(0,10,1);
	splitter.addRange(10,20,1);
	
	splitter.iterateOver([](int, int, const auto &){});
	
	for (auto p: splitter.getPoints()) {
		points.push_back(p.position);
	}
	
	assert(points.size() == 2);
	assert(points[0] == 0);
	assert(points[1] == 20);
}

void test1() {
	std::vector<Range> ranges;
	
	RangeSplitter splitter;
	splitter.addRange(0,10,1);
	
	size_t count = splitter.iterateOver([&](int low, int high, const std::set<int> & s){
		ranges.emplace_back(Range{low, high, s});
	});
	
	assert(count == 1);
	assert(ranges.size() == 1);
	assert((ranges[0] == Range{0,10,{1}}));
}


void test2() {
	std::vector<Range> ranges;
	
	RangeSplitter splitter;
	splitter.addRange(0,10,1);
	splitter.addRange(5,10,2);
	splitter.addRange(10,15,3);
	
	size_t count = splitter.iterateOver([&](int low, int high, const std::set<int> & s){
		ranges.emplace_back(Range{low, high, s});
	});
	
	assert(count == 4);
	assert(ranges.size() == 4);
	assert((ranges[0] == Range{0,4,{1}}));
	assert((ranges[1] == Range{5,9,{1,2}}));
	assert((ranges[2] == Range{10,10,{1,2,3}}));
	assert((ranges[3] == Range{11,15,{3}}));
}

void test3() {
	std::vector<Range> ranges;
	
	RangeSplitter splitter;
	splitter.addRange(0,10,1);
	splitter.addRange(0,10,2);
	
	size_t count = splitter.iterateOver([&](int low, int high, const std::set<int> & s){
		ranges.emplace_back(Range{low, high, s});
	});
	
	assert(count == 1);
	assert(ranges.size() == 1);
	assert((ranges[0] == Range{0,10,{1,2}}));
}

void test4() {
	std::vector<Range> ranges;
	
	RangeSplitter splitter;
	splitter.addRange(0,10,1);
	splitter.addRange(10,20,2);
	
	size_t count = splitter.iterateOver([&](int low, int high, const std::set<int> & s){
		ranges.emplace_back(Range{low, high, s});
	});
	
	assert(count == 3);
	assert(ranges.size() == 3);
	assert((ranges[0] == Range{0,9,{1}}));
	assert((ranges[1] == Range{10,10,{1,2}}));
	assert((ranges[2] == Range{11,20,{2}}));
}

void test5() {
	std::vector<Range> ranges;
	
	RangeSplitter splitter;
	splitter.addRange(std::numeric_limits<int>::min(),std::numeric_limits<int>::max(),1);
	splitter.addRange(10,20,2);
	
	size_t count = splitter.iterateOver([&](int low, int high, const std::set<int> & s){
		ranges.emplace_back(Range{low, high, s});
	});
	
	assert(count == 3);
	assert(ranges.size() == 3);
	assert((ranges[0] == Range{std::numeric_limits<int>::min(),9,{1}}));
	assert((ranges[1] == Range{10,20,{1,2}}));
	assert((ranges[2] == Range{21,std::numeric_limits<int>::max(),{1}}));
}

void test6() {
	std::vector<Range> ranges;
	
	RangeSplitter splitter;
	splitter.addRange(std::numeric_limits<int>::min(),std::numeric_limits<int>::max(),1);
	splitter.addRange(10,10,2);
	
	size_t count = splitter.iterateOver([&](int low, int high, const std::set<int> & s){
		ranges.emplace_back(Range{low, high, s});
	});
	
	assert(count == 3);
	assert(ranges.size() == 3);
	assert((ranges[0] == Range{std::numeric_limits<int>::min(),9,{1}}));
	assert((ranges[1] == Range{10,10,{1,2}}));
	assert((ranges[2] == Range{11,std::numeric_limits<int>::max(),{1}}));
}

void test7() {
	std::vector<Range> ranges;
	
	RangeSplitter splitter;
	splitter.addRange(0,11,1);
	splitter.addRange(10,20,1);
	
	size_t count = splitter.iterateOver([&](int low, int high, const std::set<int> & s){
		ranges.emplace_back(Range{low, high, s});
	});
	
	assert(count == 1);
	assert(ranges.size() == 1);
	assert((ranges[0] == Range{0,20,{1}}));
}

void test8() {
	std::vector<Range> ranges;
	
	RangeSplitter splitter;
	splitter.addRange(0,10,1);
	splitter.addRange(10,20,1);
	
	size_t count = splitter.iterateOver([&](int low, int high, const std::set<int> & s){
		ranges.emplace_back(Range{low, high, s});
	});
	
	assert(count == 1);
	assert(ranges.size() == 1);
	assert((ranges[0] == Range{0,20,{1}}));
}

void test9() {
	std::vector<Range> ranges;
	
	RangeSplitter splitter;
	splitter.addRange(0,9,1);
	splitter.addRange(10,20,1);
	
	size_t count = splitter.iterateOver([&](int low, int high, const std::set<int> & s){
		ranges.emplace_back(Range{low, high, s});
	});
	
	//assert(count == 2);
	//assert(ranges.size() == 2);
	//assert((ranges[0] == Range{0,9,{1}}));
	//assert((ranges[1] == Range{10,20,{1}}));
	
	assert(count == 1);
	assert(ranges.size() == 1);
	assert((ranges[0] == Range{0,20,{1}}));
}

void test10() {
	std::vector<Range> ranges;
	
	RangeSplitter splitter;
	splitter.addRange(0,20,1);
	splitter.addRange(10,30,1);
	
	size_t count = splitter.iterateOver([&](int low, int high, const std::set<int> & s){
		ranges.emplace_back(Range{low, high, s});
	});
	
	assert(count == 1);
	assert(ranges.size() == 1);
	assert((ranges[0] == Range{0,30,{1}}));
}

void test11() {
	std::vector<Range> ranges;
	
	RangeSplitter splitter;
	splitter.addRange(0,10,1);
	splitter.addRange(20,30,1);
	
	size_t count = splitter.iterateOver([&](int low, int high, const std::set<int> & s){
		ranges.emplace_back(Range{low, high, s});
	});
	
	assert(count == 2);
	assert(ranges.size() == 2);
	assert((ranges[0] == Range{0,10,{1}}));
	assert((ranges[1] == Range{20,30,{1}}));
}

void test12() {
	std::vector<Range> ranges;
	
	RangeSplitter splitter;
	splitter.addRange(0,30,1);
	splitter.addRange(10,20,2);
	splitter.addRange(25,30,1);
	
	size_t count = splitter.iterateOver([&](int low, int high, const std::set<int> & s){
		std::cout << low << " .. " << high << " = ";
		for (auto v: s) {
			std::cout << v << " ";
		}
		std::cout << "\n";
		ranges.emplace_back(Range{low, high, s});
	});
	
	assert(count == 3);
	assert(ranges.size() == 3);
	assert((ranges[0] == Range{0,9,{1}}));
	assert((ranges[1] == Range{10,20,{1,2}}));
	assert((ranges[2] == Range{21,30,{1}}));
}

void test13() {
	std::vector<Range> ranges;
	
	RangeSplitter splitter;
	splitter.addRange(1,5,1);
	splitter.addRange(10,15,1);
	
	size_t count = splitter.iterateOver([&](int low, int high, const std::set<int> & s){
		std::cout << low << " .. " << high << " = ";
		for (auto v: s) {
			std::cout << v << " ";
		}
		std::cout << "\n";
		ranges.emplace_back(Range{low, high, s});
	});
	
	assert(count == 2);
	assert(ranges.size() == 2);
	assert((ranges[0] == Range{1,5,{1}}));
	assert((ranges[1] == Range{10,15,{1}}));
}


int main() {
	test0();
	test0b();
	test0c();
	test1();
	test2();
	test3();
	test4();
	test5();
	test6();
	test7();
	test8();
	test9();
	test10();
	test11();
	
	std::cout << "------------------------\n";
	test12();
	test13();
}