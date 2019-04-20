#include <ctfa/container/interval.hpp>
#include <iostream>

struct range {
	int low;
	int high;
	int feature;
};

int main() {
	ctfa::intervals<3, int, int> i;
	i.insert_range(1,5,1);
	i.insert_range(6,10,1);
	i.merge();
	i.split([](int low, int high, const auto & features) {
		std::cout << low << " ... " << high << "\n";
	});
}