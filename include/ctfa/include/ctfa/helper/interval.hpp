#ifndef CTFA__HELPER__INTERVAL__HPP
#define CTFA__HELPER__INTERVAL__HPP

#include "../container/set.hpp"
#include <numeric>

namespace ctfa {

enum class point_type {
	start = 0,
	end = 1
};

constexpr bool operator<(point_type lhs, point_type rhs) {
	return static_cast<int>(lhs) < static_cast<int>(rhs);
}

template <typename Point, typename Feature> struct ipoint {
	Point position;
	point_type type;
	Feature feature;
	unsigned counter;
	constexpr bool operator<(const ipoint & rhs) const {
		if (position == rhs.position) {
			if (type == rhs.type) {
				if (feature == rhs.feature) {
					return counter < rhs.counter;
				} else {
					return feature < rhs.feature;
				}
			} else {
				return type < rhs.type;
			}
		} else {
			return position < rhs.position;
		}
	}
	constexpr bool operator==(const ipoint & rhs) const {
		return position == rhs.position && type == rhs.type && feature == rhs.feature && counter == rhs.counter;
	}
	constexpr bool similar(const ipoint & rhs) const noexcept {
		return position == rhs.position && type == rhs.type;
	}
	constexpr bool fusable(const ipoint & rhs) const noexcept {
		return type == point_type::end && rhs.type == point_type::start && feature == rhs.feature;
	}
	constexpr size_t distance(const ipoint & rhs) const noexcept {
		if (position < rhs.position) {
			return rhs.position - position;
		} else {
			return position - rhs.position;
		}
	}
	constexpr Point get_starting_position() const noexcept {
		if (type == point_type::start) {
			return position;
		} else {
			return position+1;
		}
	}
	constexpr Point get_ending_position() const noexcept {
		if (type == point_type::start) {
			return position-1;
		} else {
			return position;
		}
	}
};

template <size_t NumberOfRanges, typename Point, typename Feature> struct intervals {
	using point = ipoint<Point, Feature>;
	struct feature_with_count {
		Feature feature{};
		size_t count{0};
		constexpr bool operator==(const Feature & rhs) const noexcept {
			return feature == rhs;
		}
		constexpr bool operator<(const Feature & rhs) const noexcept {
			return feature < rhs;
		}
		constexpr bool operator==(const feature_with_count & rhs) const noexcept {
			return feature == rhs.feature;
		}
		constexpr bool operator<(const feature_with_count & rhs) const noexcept {
			return feature < rhs.feature;
		}
		constexpr operator Feature() const noexcept {
			return feature;
		}
	};
	
	ctfa::set<point, NumberOfRanges * 2> buffer;
	unsigned counter{0};
	
	constexpr void insert_range(Point low, Point high, Feature f) {
		buffer.insert(point{low, point_type::start, f, counter++});
		buffer.insert(point{high, point_type::end, f, counter++});
	}
	
	constexpr auto begin() const {
		return buffer.begin();
	}
	
	constexpr auto end() const {
		return buffer.end();
	}
	
	constexpr size_t number_of_ranges() const noexcept {
		return buffer.size() / 2;
	}
	
	constexpr bool merge() {
		bool changed = false;
		
		ctfa::set<feature_with_count, NumberOfRanges> features;
		
		auto add_feature = [&](Feature f) {
			auto pair = features.insert(feature_with_count{f, 1});
			if (!pair.second) {
				// point is not needed, feature was already there
				pair.first->count++;
				changed = true;
				return false;
			} else {
				// point is NEEDED, it adds new feature
				return true;
			}
		};
		
		auto remove_feature = [&](Feature f) {
			auto it = features.find(f);
			if (it->count > 1) {
				it->count--;
				// point is not needed, it doesn't remove anything
				changed = true;
				return false;
			} else {
				features.remove(it);
				// point is NEEDED, it removes feature
				return true;
			}
		};
		
		auto try_fuse_points = [&](auto & it) -> bool {
			auto next = it;
			// search for neighbor point which is positive (I'm negative)
			while (next != buffer.end()) {
				// I can only fuse with neighbor
				if (it->distance(*next) == 0) {
					// skip these
				} else if (it->distance(*next) == 1) {
					// fusable ranges are next to each other
					if (it->fusable(*next)) {
						// and they are negative->positive with same feature
						auto tmp = *it;
						buffer.remove(next);
						changed = true;
						it = buffer.remove(buffer.find(tmp));
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
		
		auto it = buffer.begin();
		
		while (it != buffer.end()) {
			// fuse current point if possible
			// |---a---|
			//          |---a---|
			// should be:
			// |--------a-------|
			
			if (it->type == point_type::end) {
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
			if (it->type == point_type::start) {
				if (!add_feature(it->feature)) {
					it = buffer.remove(it);
					continue;
				}
			} else {
				if (!remove_feature(it->feature)) {
					it = buffer.remove(it);
					continue;
				}
			}
			
			it++;
		}
		
		return changed;
	}
	
	template <typename CB> constexpr size_t split(CB && output) const {
		size_t count = 0;
		
		ctfa::set<Feature, NumberOfRanges> features;
		
		auto it = buffer.begin();
		bool has_last_point = false;
		point last_point{};
		
		while (it != buffer.end()) {
			if (has_last_point) {
				if (last_point.get_starting_position() <= it->get_ending_position()) {
					output(last_point.get_starting_position(), it->get_ending_position(), features);
					count++;
				}
			}
			
			has_last_point = true;
			last_point = *it;
			
			auto first = *it;
			if (it->type == point_type::start) {
				while (it != buffer.end() && it->similar(first)) {
					features.insert(it->feature);
					it++;
				}
			} else {
				while (it != buffer.end() && it->similar(first)) {
					features.remove(it->feature);
					it++;
				}
			}
			if (features.size() == 0) {
				has_last_point = false;
			}
		}
		
		return count;
	}
	template <typename CB> constexpr size_t merge_and_split(CB && output) {
		merge();
		return split(std::forward<CB>(output));
	}
	constexpr size_t merge_and_split() {
		merge();
		return split([](const auto & ...){});
	}
};

template <size_t NumberOfRanges, typename RangeIt, typename CB> constexpr size_t split(const RangeIt & first, const RangeIt & last, CB && cb) {
	intervals<NumberOfRanges, decltype(first->low), decltype(first->feature)> i;
	RangeIt current = first;
	while (current != last) {
		i.insert_range(current->low, current->high, current->feature);
		current++;
	}
	return i.merge_and_split(std::forward<CB>(cb));
}

template <size_t NumberOfRanges, typename RangeIt> constexpr size_t split(const RangeIt & first, const RangeIt & last) {
	intervals<NumberOfRanges, decltype(first->low), decltype(first->feature)> i;
	RangeIt current = first;
	while (current != last) {
		i.insert_range(current->low, current->high, current->feature);
		current++;
	}
	return i.merge_and_split();
}

template <size_t NumberOfRanges, typename Range, typename CB> constexpr size_t split_range(const Range & range, CB && cb) {
	return split<NumberOfRanges>(range.begin(), range.end(), std::forward<CB>(cb));
}

template <size_t NumberOfRanges, typename Range> constexpr size_t split_range(Range && range) {
	return split<NumberOfRanges>(range.begin(), range.end());
}

}

#endif
