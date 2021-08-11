#ifndef H_COR3NTIN_UNICODE_SYNOPSYS
#define H_COR3NTIN_UNICODE_SYNOPSYS

#include <string_view>

namespace uni
{
    enum class category;
    enum class property;
    enum class version : unsigned char;
    enum class script ;
    enum class block;

    struct script_extensions_view {
        constexpr script_extensions_view(char32_t);

        struct sentinel {};
        struct iterator {

            constexpr iterator(char32_t c);
            constexpr script operator*() const;

            constexpr iterator& operator++(int);

            constexpr iterator operator++();

            constexpr bool operator==(sentinel) const;
            constexpr bool operator!=(sentinel) const;

        private:
            char32_t m_c;
            script m_script;
            int idx = 1;
        };

        constexpr iterator begin() const;
        constexpr sentinel end() const;

        private:
            char32_t c;
    };

    struct numeric_value {

        constexpr double value() const;
        constexpr long long numerator() const;
        constexpr int denominator() const;
        constexpr bool is_valid() const;

    protected:
        constexpr numeric_value() = default;
        constexpr numeric_value(long long n, int16_t d);

        long long _n = 0;
        int16_t _d = 0;
        friend constexpr numeric_value cp_numeric_value(char32_t cp);
    };

    constexpr category cp_category(char32_t cp);
    constexpr script cp_script(char32_t cp);
    constexpr script_extensions_view cp_script_extensions(char32_t cp);
    constexpr version cp_age(char32_t cp);
    constexpr block cp_block(char32_t cp);
    constexpr bool cp_is_valid(char32_t cp);
    constexpr bool cp_is_assigned(char32_t cp);
    constexpr bool cp_is_ascii(char32_t cp);
    constexpr numeric_value cp_numeric_value(char32_t cp);

    template<script>
    constexpr bool cp_is(char32_t);
    template<property>
    constexpr bool cp_is(char32_t);
    template<category>
    constexpr bool cp_is(char32_t);

    namespace detail
    {
        enum class binary_prop;
        constexpr int propnamecomp(std::string_view sa, std::string_view sb);
        constexpr binary_prop binary_prop_from_string(std::string_view s);

        template<binary_prop p>
        constexpr bool get_binary_prop(char32_t) = delete;

        constexpr script   script_from_string(std::string_view s);
        constexpr block    block_from_string(std::string_view s);
        constexpr version  age_from_string(std::string_view a);
        constexpr category category_from_string(std::string_view a);

        constexpr bool is_unassigned(category cat);
        constexpr bool is_unknown(script s);
        constexpr bool is_unknown(block b);
        constexpr bool is_unassigned(version v);
        constexpr bool is_unknown(binary_prop s);
    }
}

#endif
