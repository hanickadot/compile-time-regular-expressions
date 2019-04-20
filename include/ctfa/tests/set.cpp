#include <ctfa/container/set.hpp>

static_assert(0 == []() {
	// empty
	ctfa::set<int, 1> s;
	return s.size();
}());

static_assert(1 == []() {
	// exactly one
	ctfa::set<int, 1> s;
	s.insert(1);
	return s.size();
}());

static_assert(1 == []() {
	// over capacity
	ctfa::set<int, 1> s;
	s.insert(1);
	s.insert(1);
	return s.size();
}());

static_assert(1 == []() {
	// non unique unique insert
	ctfa::set<int, 2> s;
	s.insert(1);
	s.insert(1);
	return s.size();
}());

static_assert(2 == []() {
	// unique insert
	ctfa::set<int, 2> s;
	s.insert(2);
	s.insert(1);
	return s.size();
}());

static_assert(2 == []() {
	// remove none xisting 
	ctfa::set<int, 2> s;
	s.insert(2);
	s.insert(1);
	s.remove(3);
	return s.size();
}());

static_assert(1 == []() {
	// remove first
	ctfa::set<int, 2> s;
	s.insert(2);
	s.insert(1);
	s.remove(2);
	return s.size();
}());

static_assert(1 == []() {
	// multiple remove
	ctfa::set<int, 2> s;
	s.insert(2);
	s.insert(1);
	s.remove(2);
	s.remove(2);
	return s.size();
}());

static_assert(0 == []() {
	// multiple remove => should be empty
	ctfa::set<int, 2> s;
	s.insert(2);
	s.insert(1);
	s.remove(1);
	s.remove(2);
	return s.size();
}());

static_assert(0 == []() {
	// multiple remove of same => should be empty
	ctfa::set<int, 2> s;
	s.insert(2);
	s.insert(1);
	s.remove(1);
	s.remove(2);
	s.remove(2);
	return s.size();
}());

static_assert(6 == []() {
	// insert of find (update)
	ctfa::set<int, 2> s;
	
	auto update = [&](int v) {
		auto it = s.insert(v).first;
		if (it != s.end()) {
			(*it)++;
		}
	};
	
	update(1);
	update(3);
	
	int total = 0;
	for (int i: s) {
		total += i;
	}
	return total;
}());

static_assert(3 == []() {
	ctfa::set<int, 4> s;
	s.insert(1);
	s.insert(2);
	s.insert(3);
	s.insert(4);
	auto it = s.find(3);
	return *it;
}());

static_assert(true == []() {
	ctfa::set<int, 4> s;
	s.insert(1);
	s.insert(2);
	s.insert(3);
	s.insert(4);
	return s.find(5) == s.end();
}());

static_assert(5 == []() {
	ctfa::set<int, 4> s;
	s.insert(1);
	s.insert(2);
	s.insert(3);
	s.remove(1);
	int total = 0;
	for (int i: s) {
		total += i;
	}
	return total;
}());

static_assert(4 == []() {
	ctfa::set<int, 4> s;
	s.insert(1);
	s.insert(2);
	s.insert(3);
	s.remove(2);
	int total = 0;
	for (int i: s) {
		total += i;
	}
	return total;
}());

static_assert(3 == []() {
	ctfa::set<int, 4> s;
	s.insert(1);
	s.insert(2);
	s.insert(3);
	s.remove(3);
	int total = 0;
	for (int i: s) {
		total += i;
	}
	return total;
}());

static_assert(6 == []() {
	ctfa::set<int, 4> s;
	s.insert(1);
	s.insert(2);
	s.insert(3);
	s.remove(4);
	int total = 0;
	for (int i: s) {
		total += i;
	}
	return total;
}());

static_assert(10 == []() {
	ctfa::set<int, 10> s;
	s.insert(1); // first
	s.insert(4); // last
	s.insert(2); // middle
	s.insert(3); // middle+1
	s.insert(2); // same (not insert)
	int total = 0;
	for (int i: s) {
		total += i;
	}
	return total;
}());

static_assert(true == []() {
	ctfa::set<int, 2> a;
	ctfa::set<int, 2> b;
	a.insert(0);
	a.insert(1);
	b.insert(1);
	b.insert(2);
	return a < b;
}());

static_assert(6 == []() {
	ctfa::set<int, 20> a;
	a.insert(0);
	a.insert(1);
	a.insert(2);
	a.insert(3);
	a.insert(4);
	a.insert(5);
	a.insert(6);
	a.insert(7);
	return *(a.upper_bound(5));
}());

static_assert(1 == []() {
	ctfa::set<int, 20> a;
	a.insert(0);
	a.insert(1);
	a.insert(2);
	a.insert(3);
	a.insert(4);
	a.insert(5);
	a.insert(6);
	a.insert(7);
	return *(a.upper_bound(0));
}());

static_assert(true == []() {
	ctfa::set<int, 20> a;
	a.insert(0);
	a.insert(1);
	a.insert(2);
	a.insert(3);
	a.insert(4);
	a.insert(5);
	a.insert(6);
	a.insert(7);
	return a.upper_bound(7) == a.end();
}());