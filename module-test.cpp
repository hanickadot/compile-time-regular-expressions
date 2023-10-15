import ctre;

#include <string_view>
#include <iostream>

int main() {
    for (auto item: "123456 2313 23132" | ctre::search_all<"[0-9]++">) {
        std::cout << item << "\n";
    }
    
    return 0;
}