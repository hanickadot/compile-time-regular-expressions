#include <cstdlib>
#include <cassert>
#include <string_view>
#include <optional>

#include <ctre.hpp>


struct date { std::string_view year; std::string_view month; std::string_view day; };

constexpr std::optional<date> extract_date(std::string_view s) noexcept {
    using namespace ctre::literals;
    if (auto [whole, year, month, day] = "^([0-9]{4})/([0-9]{1,2}+)/([0-9]{1,2}+)$"_pcre.match(s); whole
    ) {
        return date{year.to_view(), month.to_view(), day.to_view()};
    } else {
        return std::nullopt;
    }
}

int main() {
 
    assert(extract_date("2018/08/27").has_value());
    assert(extract_date("2018/08/27")->year == "2018");
    assert(extract_date("2018/08/27")->month == "08");
    assert(extract_date("2018/08/27")->day == "27");

    return EXIT_SUCCESS;
}
