#pragma once

#include <string_view>

#ifndef CTRE_UNICODE_SYNOPSYS_WAS_INCLUDED
namespace uni {
enum class category;
enum class property;
enum class version : unsigned char;
enum class script;
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
constexpr bool cp_script_is(char32_t);
template<property>
constexpr bool cp_property_is(char32_t);
template<category>
constexpr bool cp_category_is(char32_t);

namespace detail {
    enum class binary_prop;
    constexpr int propnamecomp(std::string_view sa, std::string_view sb);
    constexpr binary_prop binary_prop_from_string(std::string_view s);

    template<binary_prop p>
    constexpr bool get_binary_prop(char32_t) = delete;

    constexpr script script_from_string(std::string_view s);
    constexpr block block_from_string(std::string_view s);
    constexpr version age_from_string(std::string_view a);
    constexpr category category_from_string(std::string_view a);

    constexpr bool is_unassigned(category cat);
    constexpr bool is_unknown(script s);
    constexpr bool is_unknown(block b);
    constexpr bool is_unassigned(version v);
    constexpr bool is_unknown(binary_prop s);
}    // namespace detail
}    // namespace uni
#endif

#include <cstdint>
#include <algorithm>
#include <string_view>

namespace uni::detail {

template<class ForwardIt, class T, class Compare>
constexpr ForwardIt upper_bound(ForwardIt first, ForwardIt last, const T& value, Compare comp) {
    ForwardIt it = first;
    typename std::iterator_traits<ForwardIt>::difference_type count = std::distance(first, last);
    typename std::iterator_traits<ForwardIt>::difference_type step = count / 2;

    while(count > 0) {
        it = first;
        step = count / 2;
        std::advance(it, step);
        if(!comp(value, *it)) {
            first = ++it;
            count -= step + 1;
        } else
            count = step;
    }
    return first;
}

template<class ForwardIt, class T, class Compare>
constexpr ForwardIt lower_bound(ForwardIt first, ForwardIt last, const T& value, Compare comp) {
    ForwardIt it = first;
    typename std::iterator_traits<ForwardIt>::difference_type count = std::distance(first, last);
    typename std::iterator_traits<ForwardIt>::difference_type step = count / 2;

    while(count > 0) {
        it = first;
        step = count / 2;
        std::advance(it, step);
        if(comp(*it, value)) {
            first = ++it;
            count -= step + 1;
        } else
            count = step;
    }
    return first;
}

template<class ForwardIt, class T>
constexpr ForwardIt lower_bound(ForwardIt first, ForwardIt last, const T& value) {
    ForwardIt it = first;
    typename std::iterator_traits<ForwardIt>::difference_type count = std::distance(first, last);
    typename std::iterator_traits<ForwardIt>::difference_type step = count / 2;

    while(count > 0) {
        it = first;
        step = count / 2;
        std::advance(it, step);
        if(*it < value) {
            first = ++it;
            count -= step + 1;
        } else
            count = step;
    }
    return first;
}

template<class ForwardIt, class T>
constexpr bool binary_search(ForwardIt first, ForwardIt last, const T& value) {
    first = detail::lower_bound(first, last, value);
    return (!(first == last) && !(value < *first));
}
template<typename T, auto N>
struct compact_range {
    std::uint32_t _data[N];
    constexpr T value(char32_t cp, T default_value) const {
        const auto end = std::end(_data);
        auto it =
            detail::upper_bound(std::begin(_data), end, cp, [](char32_t local_cp, uint32_t v) {
                char32_t c = (v >> 8);
                return local_cp < c;
            });
        if(it == end)
            return default_value;
        it--;
        return *(it)&0xFF;
    }
};
template<class T, class... U>
compact_range(T, U...) -> compact_range<T, sizeof...(U) + 1>;

template<typename T, auto N>
struct compact_list {
    std::uint32_t _data[N];
    constexpr T value(char32_t cp, T default_value) const {
        const auto end = std::end(_data);
        auto it =
            detail::lower_bound(std::begin(_data), end, cp, [](uint32_t v, char32_t local_cp) {
                char32_t c = (v >> 8);
                return c < local_cp;
            });
        if(it == end || ((*it) >> 8) != cp)
            return default_value;
        return *(it)&0xFF;
    }
};
template<class T, class... U>
compact_list(T, U...) -> compact_list<T, sizeof...(U) + 1>;

template<typename T, std::size_t N>
struct array {
    using type = T[N];
};

template<typename T>
struct array<T, 0> {
    using type = T*;
};

template<typename T, std::size_t N>
using array_t = typename array<T, N>::type;

template<std::size_t r1_s, std::size_t r2_s, int16_t r2_t_f, int16_t r2_t_b, std::size_t r3_s,
         std::size_t r4_s, int16_t r4_t_f, int16_t r4_t_b, std::size_t r5_s, int16_t r5_t_f,
         int16_t r5_t_b, std::size_t r6_s>
struct bool_trie {

    // not tries, just bitmaps for all code points 0..0x7FF (UTF-8 1- and 2-byte sequences)
    std::uint64_t r1[32];

    // trie for code points 0x800..0xFFFF (UTF-8 3-byte sequences, aka rest of BMP)

    uint8_t r2[r2_s];
    array_t<std::uint64_t, r3_s> r3;    // leaves can be shared, so size isn't fixed

    // trie for 0x10000..0x10FFFF (UTF-8 4-byte sequences, aka non-BMP code points)
    array_t<std::uint8_t, r4_s> r4;
    array_t<std::uint8_t, r5_s> r5;     // two level to exploit sparseness of non-BMP
    array_t<std::uint64_t, r6_s> r6;    // again, leaves are shared

    constexpr bool lookup(char32_t u) const {
        std::uint32_t c = u;
        if(c < 0x800) {
            if constexpr(r1_s == 0) {
                return false;
            } else {
                return trie_range_leaf(c, r1[std::size_t(c >> 6)]);
            }
        } else if(c < 0x10000) {
            if constexpr(r3_s == 0) {
                return false;
            } else {
                std::size_t i = (std::size_t(c >> 6) - 0x20);
                auto child = 0;
                if(i >= r2_t_f && i < r2_t_f + r2_s)
                    child = r2[i - r2_t_f];
                return trie_range_leaf(c, r3[child]);
            }
        } else {
            if constexpr(r6_s == 0)
                return false;
            std::size_t i4 = (c >> 12) - 0x10;
            auto child = 0;
            if constexpr(r4_s > 0) {
                if(i4 >= r4_t_f && i4 < r4_t_f + r4_s)
                    child = r4[i4 - r4_t_f];
            }

            std::size_t i5 = static_cast<std::size_t>(std::size_t(child << 6) +
                                                      (std::size_t(c >> 6) & std::size_t(0x3f)));
            auto leaf = 0;
            if constexpr(r5_s != 0) {
                if(i5 >= std::size_t(r5_t_f) && i5 < std::size_t(r5_t_f) + r5_s)
                    leaf = r5[i5 - std::size_t(r5_t_f)];
            }
            return trie_range_leaf(c, r6[leaf]);
        }
    }

    constexpr bool trie_range_leaf(std::uint32_t c, std::uint64_t chunk) const {
        return (chunk >> (c & 0b111111)) & 0b1;
    }
};

template<std::size_t size>
struct flat_array {
    char32_t data[size];
    constexpr bool lookup(char32_t u) const {
        if constexpr(size < 20) {
            for(auto it = std::begin(data); it != std::end(data); ++it) {
                if(*it == u)
                    return true;
                if(it == std::end(data))
                    return false;
            }
            return false;
        } else {
            return detail::binary_search(std::begin(data), std::end(data), u);
        }
    }
};

template<auto N>
struct range_array {
    std::uint32_t _data[N];
    constexpr bool lookup(char32_t cp) const {
        const auto end = std::end(_data);
        auto it =
            detail::upper_bound(std::begin(_data), end, cp, [](char32_t local_cp, uint32_t v) {
                char32_t c = (v >> 8);
                return local_cp < c;
            });
        if(it == end)
            return false;
        it--;
        return (*it) & 0xFF;
    }
};

template<class... U>
range_array(U...) -> range_array<sizeof...(U)>;

constexpr char propcharnorm(char a) {
    if(a >= 'A' && a <= 'Z')
        return static_cast<char>(a + char(32));
    if(a == ' ' || a == '-')
        return '_';
    return a;
}

constexpr int propcharcomp(char a, char b) {
    a = propcharnorm(a);
    b = propcharnorm(b);
    if(a == b)
        return 0;
    if(a < b)
        return -1;
    return 1;
}

constexpr int propnamecomp(std::string_view sa, std::string_view sb) {
    // workaround, iterators in std::string_view are not constexpr in libc++ (for now)
    const char* a = sa.data();
    const char* b = sb.data();

    const char* ae = sa.data() + sa.size();
    const char* be = sb.data() + sb.size();

    for(; a != ae && b != be; a++, b++) {
        auto res = propcharcomp(*a, *b);
        if(res != 0)
            return res;
    }
    if(sa.size() < sb.size())
        return -1;
    else if(sb.size() < sa.size())
        return 1;
    return 0;
}

template<typename A, typename B>
struct pair {
    A first;
    B second;
};

template<typename A, typename B>
pair(A, B) -> pair<A, B>;

struct string_with_idx {
    const char* name;
    uint32_t value;
};

}    // namespace uni::detail

namespace uni {

constexpr double numeric_value::value() const {
    return static_cast<double>(numerator()) / static_cast<double>(_d);
}

constexpr long long numeric_value::numerator() const {
    return _n;
}

constexpr int numeric_value::denominator() const {
    return _d;
}

constexpr bool numeric_value::is_valid() const {
    return _d != 0;
}

constexpr numeric_value::numeric_value(long long n, int16_t d) : _n(n), _d(d) {}

}    // namespace uni

namespace uni {
enum class version : uint8_t {
    unassigned,
    v1_1,
    v2_0,
    v2_1,
    v3_0,
    v3_1,
    v3_2,
    v4_0,
    v4_1,
    v5_0,
    v5_1,
    v5_2,
    v6_0,
    v6_1,
    v6_2,
    v6_3,
    v7_0,
    v8_0,
    v9_0,
    v10_0,
    v11_0,
    v12_0,
    v12_1,
    v13_0,
    v14_0,
    latest_version = v14_0
};
enum class category {
    c,
    other = c,
    cc,
    control = cc,
    cf,
    format = cf,
    cn,
    unassigned = cn,
    co,
    private_use = co,
    cs,
    surrogate = cs,
    l,
    letter = l,
    lc,
    cased_letter = lc,
    ll,
    lowercase_letter = ll,
    lm,
    modifier_letter = lm,
    lo,
    other_letter = lo,
    lt,
    titlecase_letter = lt,
    lu,
    uppercase_letter = lu,
    m,
    mark = m,
    mc,
    spacing_mark = mc,
    me,
    enclosing_mark = me,
    mn,
    nonspacing_mark = mn,
    n,
    number = n,
    nd,
    decimal_number = nd,
    nl,
    letter_number = nl,
    no,
    other_number = no,
    p,
    punctuation = p,
    pc,
    connector_punctuation = pc,
    pd,
    dash_punctuation = pd,
    pe,
    close_punctuation = pe,
    pf,
    final_punctuation = pf,
    pi,
    initial_punctuation = pi,
    po,
    other_punctuation = po,
    ps,
    open_punctuation = ps,
    s,
    symbol = s,
    sc,
    currency_symbol = sc,
    sk,
    modifier_symbol = sk,
    sm,
    math_symbol = sm,
    so,
    other_symbol = so,
    z,
    separator = z,
    zl,
    line_separator = zl,
    zp,
    paragraph_separator = zp,
    zs,
    space_separator = zs,
    max
};
enum class block {
    no_block,    // 0
    nb = no_block,
    basic_latin,    // 1
    ascii = basic_latin,
    latin_1_supplement,    // 2
    latin_1_sup = latin_1_supplement,
    latin_extended_a,    // 3
    latin_ext_a = latin_extended_a,
    latin_extended_b,    // 4
    latin_ext_b = latin_extended_b,
    ipa_extensions,    // 5
    ipa_ext = ipa_extensions,
    spacing_modifier_letters,    // 6
    modifier_letters = spacing_modifier_letters,
    combining_diacritical_marks,    // 7
    diacriticals = combining_diacritical_marks,
    greek_and_coptic,    // 8
    greek = greek_and_coptic,
    cyrillic,               // 9
    cyrillic_supplement,    // 10
    cyrillic_sup = cyrillic_supplement,
    armenian,             // 11
    hebrew,               // 12
    arabic,               // 13
    syriac,               // 14
    arabic_supplement,    // 15
    arabic_sup = arabic_supplement,
    thaana,               // 16
    nko,                  // 17
    samaritan,            // 18
    mandaic,              // 19
    syriac_supplement,    // 20
    syriac_sup = syriac_supplement,
    arabic_extended_b,    // 21
    arabic_ext_b = arabic_extended_b,
    arabic_extended_a,    // 22
    arabic_ext_a = arabic_extended_a,
    devanagari,     // 23
    bengali,        // 24
    gurmukhi,       // 25
    gujarati,       // 26
    oriya,          // 27
    tamil,          // 28
    telugu,         // 29
    kannada,        // 30
    malayalam,      // 31
    sinhala,        // 32
    thai,           // 33
    lao,            // 34
    tibetan,        // 35
    myanmar,        // 36
    georgian,       // 37
    hangul_jamo,    // 38
    jamo = hangul_jamo,
    ethiopic,               // 39
    ethiopic_supplement,    // 40
    ethiopic_sup = ethiopic_supplement,
    cherokee,                                 // 41
    unified_canadian_aboriginal_syllabics,    // 42
    ucas = unified_canadian_aboriginal_syllabics,
    ogham,                                             // 43
    runic,                                             // 44
    tagalog,                                           // 45
    hanunoo,                                           // 46
    buhid,                                             // 47
    tagbanwa,                                          // 48
    khmer,                                             // 49
    mongolian,                                         // 50
    unified_canadian_aboriginal_syllabics_extended,    // 51
    ucas_ext = unified_canadian_aboriginal_syllabics_extended,
    limbu,                                   // 52
    tai_le,                                  // 53
    new_tai_lue,                             // 54
    khmer_symbols,                           // 55
    buginese,                                // 56
    tai_tham,                                // 57
    combining_diacritical_marks_extended,    // 58
    diacriticals_ext = combining_diacritical_marks_extended,
    balinese,               // 59
    sundanese,              // 60
    batak,                  // 61
    lepcha,                 // 62
    ol_chiki,               // 63
    cyrillic_extended_c,    // 64
    cyrillic_ext_c = cyrillic_extended_c,
    georgian_extended,    // 65
    georgian_ext = georgian_extended,
    sundanese_supplement,    // 66
    sundanese_sup = sundanese_supplement,
    vedic_extensions,    // 67
    vedic_ext = vedic_extensions,
    phonetic_extensions,    // 68
    phonetic_ext = phonetic_extensions,
    phonetic_extensions_supplement,    // 69
    phonetic_ext_sup = phonetic_extensions_supplement,
    combining_diacritical_marks_supplement,    // 70
    diacriticals_sup = combining_diacritical_marks_supplement,
    latin_extended_additional,    // 71
    latin_ext_additional = latin_extended_additional,
    greek_extended,    // 72
    greek_ext = greek_extended,
    general_punctuation,    // 73
    punctuation = general_punctuation,
    superscripts_and_subscripts,    // 74
    super_and_sub = superscripts_and_subscripts,
    currency_symbols,                           // 75
    combining_diacritical_marks_for_symbols,    // 76
    diacriticals_for_symbols = combining_diacritical_marks_for_symbols,
    letterlike_symbols,        // 77
    number_forms,              // 78
    arrows,                    // 79
    mathematical_operators,    // 80
    math_operators = mathematical_operators,
    miscellaneous_technical,    // 81
    misc_technical = miscellaneous_technical,
    control_pictures,                 // 82
    optical_character_recognition,    // 83
    ocr = optical_character_recognition,
    enclosed_alphanumerics,    // 84
    enclosed_alphanum = enclosed_alphanumerics,
    box_drawing,              // 85
    block_elements,           // 86
    geometric_shapes,         // 87
    miscellaneous_symbols,    // 88
    misc_symbols = miscellaneous_symbols,
    dingbats,                                // 89
    miscellaneous_mathematical_symbols_a,    // 90
    misc_math_symbols_a = miscellaneous_mathematical_symbols_a,
    supplemental_arrows_a,    // 91
    sup_arrows_a = supplemental_arrows_a,
    braille_patterns,    // 92
    braille = braille_patterns,
    supplemental_arrows_b,    // 93
    sup_arrows_b = supplemental_arrows_b,
    miscellaneous_mathematical_symbols_b,    // 94
    misc_math_symbols_b = miscellaneous_mathematical_symbols_b,
    supplemental_mathematical_operators,    // 95
    sup_math_operators = supplemental_mathematical_operators,
    miscellaneous_symbols_and_arrows,    // 96
    misc_arrows = miscellaneous_symbols_and_arrows,
    glagolitic,          // 97
    latin_extended_c,    // 98
    latin_ext_c = latin_extended_c,
    coptic,                 // 99
    georgian_supplement,    // 100
    georgian_sup = georgian_supplement,
    tifinagh,             // 101
    ethiopic_extended,    // 102
    ethiopic_ext = ethiopic_extended,
    cyrillic_extended_a,    // 103
    cyrillic_ext_a = cyrillic_extended_a,
    supplemental_punctuation,    // 104
    sup_punctuation = supplemental_punctuation,
    cjk_radicals_supplement,    // 105
    cjk_radicals_sup = cjk_radicals_supplement,
    kangxi_radicals,    // 106
    kangxi = kangxi_radicals,
    ideographic_description_characters,    // 107
    idc = ideographic_description_characters,
    cjk_symbols_and_punctuation,    // 108
    cjk_symbols = cjk_symbols_and_punctuation,
    hiragana,                     // 109
    katakana,                     // 110
    bopomofo,                     // 111
    hangul_compatibility_jamo,    // 112
    compat_jamo = hangul_compatibility_jamo,
    kanbun,               // 113
    bopomofo_extended,    // 114
    bopomofo_ext = bopomofo_extended,
    cjk_strokes,                     // 115
    katakana_phonetic_extensions,    // 116
    katakana_ext = katakana_phonetic_extensions,
    enclosed_cjk_letters_and_months,    // 117
    enclosed_cjk = enclosed_cjk_letters_and_months,
    cjk_compatibility,    // 118
    cjk_compat = cjk_compatibility,
    cjk_unified_ideographs_extension_a,    // 119
    cjk_ext_a = cjk_unified_ideographs_extension_a,
    yijing_hexagram_symbols,    // 120
    yijing = yijing_hexagram_symbols,
    cjk_unified_ideographs,    // 121
    cjk = cjk_unified_ideographs,
    yi_syllables,           // 122
    yi_radicals,            // 123
    lisu,                   // 124
    vai,                    // 125
    cyrillic_extended_b,    // 126
    cyrillic_ext_b = cyrillic_extended_b,
    bamum,                    // 127
    modifier_tone_letters,    // 128
    latin_extended_d,         // 129
    latin_ext_d = latin_extended_d,
    syloti_nagri,                 // 130
    common_indic_number_forms,    // 131
    indic_number_forms = common_indic_number_forms,
    phags_pa,               // 132
    saurashtra,             // 133
    devanagari_extended,    // 134
    devanagari_ext = devanagari_extended,
    kayah_li,                  // 135
    rejang,                    // 136
    hangul_jamo_extended_a,    // 137
    jamo_ext_a = hangul_jamo_extended_a,
    javanese,              // 138
    myanmar_extended_b,    // 139
    myanmar_ext_b = myanmar_extended_b,
    cham,                  // 140
    myanmar_extended_a,    // 141
    myanmar_ext_a = myanmar_extended_a,
    tai_viet,                   // 142
    meetei_mayek_extensions,    // 143
    meetei_mayek_ext = meetei_mayek_extensions,
    ethiopic_extended_a,    // 144
    ethiopic_ext_a = ethiopic_extended_a,
    latin_extended_e,    // 145
    latin_ext_e = latin_extended_e,
    cherokee_supplement,    // 146
    cherokee_sup = cherokee_supplement,
    meetei_mayek,        // 147
    hangul_syllables,    // 148
    hangul = hangul_syllables,
    hangul_jamo_extended_b,    // 149
    jamo_ext_b = hangul_jamo_extended_b,
    high_surrogates,                // 150
    high_private_use_surrogates,    // 151
    high_pu_surrogates = high_private_use_surrogates,
    low_surrogates,      // 152
    private_use_area,    // 153
    pua = private_use_area,
    cjk_compatibility_ideographs,    // 154
    cjk_compat_ideographs = cjk_compatibility_ideographs,
    alphabetic_presentation_forms,    // 155
    alphabetic_pf = alphabetic_presentation_forms,
    arabic_presentation_forms_a,    // 156
    arabic_pf_a = arabic_presentation_forms_a,
    variation_selectors,    // 157
    vs = variation_selectors,
    vertical_forms,          // 158
    combining_half_marks,    // 159
    half_marks = combining_half_marks,
    cjk_compatibility_forms,    // 160
    cjk_compat_forms = cjk_compatibility_forms,
    small_form_variants,    // 161
    small_forms = small_form_variants,
    arabic_presentation_forms_b,    // 162
    arabic_pf_b = arabic_presentation_forms_b,
    halfwidth_and_fullwidth_forms,    // 163
    half_and_full_forms = halfwidth_and_fullwidth_forms,
    specials,                 // 164
    linear_b_syllabary,       // 165
    linear_b_ideograms,       // 166
    aegean_numbers,           // 167
    ancient_greek_numbers,    // 168
    ancient_symbols,          // 169
    phaistos_disc,            // 170
    phaistos = phaistos_disc,
    lycian,                  // 171
    carian,                  // 172
    coptic_epact_numbers,    // 173
    old_italic,              // 174
    gothic,                  // 175
    old_permic,              // 176
    ugaritic,                // 177
    old_persian,             // 178
    deseret,                 // 179
    shavian,                 // 180
    osmanya,                 // 181
    osage,                   // 182
    elbasan,                 // 183
    caucasian_albanian,      // 184
    vithkuqi,                // 185
    linear_a,                // 186
    latin_extended_f,        // 187
    latin_ext_f = latin_extended_f,
    cypriot_syllabary,         // 188
    imperial_aramaic,          // 189
    palmyrene,                 // 190
    nabataean,                 // 191
    hatran,                    // 192
    phoenician,                // 193
    lydian,                    // 194
    meroitic_hieroglyphs,      // 195
    meroitic_cursive,          // 196
    kharoshthi,                // 197
    old_south_arabian,         // 198
    old_north_arabian,         // 199
    manichaean,                // 200
    avestan,                   // 201
    inscriptional_parthian,    // 202
    inscriptional_pahlavi,     // 203
    psalter_pahlavi,           // 204
    old_turkic,                // 205
    old_hungarian,             // 206
    hanifi_rohingya,           // 207
    rumi_numeral_symbols,      // 208
    rumi = rumi_numeral_symbols,
    yezidi,                     // 209
    old_sogdian,                // 210
    sogdian,                    // 211
    old_uyghur,                 // 212
    chorasmian,                 // 213
    elymaic,                    // 214
    brahmi,                     // 215
    kaithi,                     // 216
    sora_sompeng,               // 217
    chakma,                     // 218
    mahajani,                   // 219
    sharada,                    // 220
    sinhala_archaic_numbers,    // 221
    khojki,                     // 222
    multani,                    // 223
    khudawadi,                  // 224
    grantha,                    // 225
    newa,                       // 226
    tirhuta,                    // 227
    siddham,                    // 228
    modi,                       // 229
    mongolian_supplement,       // 230
    mongolian_sup = mongolian_supplement,
    takri,                                               // 231
    ahom,                                                // 232
    dogra,                                               // 233
    warang_citi,                                         // 234
    dives_akuru,                                         // 235
    nandinagari,                                         // 236
    zanabazar_square,                                    // 237
    soyombo,                                             // 238
    unified_canadian_aboriginal_syllabics_extended_a,    // 239
    ucas_ext_a = unified_canadian_aboriginal_syllabics_extended_a,
    pau_cin_hau,        // 240
    bhaiksuki,          // 241
    marchen,            // 242
    masaram_gondi,      // 243
    gunjala_gondi,      // 244
    makasar,            // 245
    lisu_supplement,    // 246
    lisu_sup = lisu_supplement,
    tamil_supplement,    // 247
    tamil_sup = tamil_supplement,
    cuneiform,                            // 248
    cuneiform_numbers_and_punctuation,    // 249
    cuneiform_numbers = cuneiform_numbers_and_punctuation,
    early_dynastic_cuneiform,               // 250
    cypro_minoan,                           // 251
    egyptian_hieroglyphs,                   // 252
    egyptian_hieroglyph_format_controls,    // 253
    anatolian_hieroglyphs,                  // 254
    bamum_supplement,                       // 255
    bamum_sup = bamum_supplement,
    mro,                                    // 256
    tangsa,                                 // 257
    bassa_vah,                              // 258
    pahawh_hmong,                           // 259
    medefaidrin,                            // 260
    miao,                                   // 261
    ideographic_symbols_and_punctuation,    // 262
    ideographic_symbols = ideographic_symbols_and_punctuation,
    tangut,                 // 263
    tangut_components,      // 264
    khitan_small_script,    // 265
    tangut_supplement,      // 266
    tangut_sup = tangut_supplement,
    kana_extended_b,    // 267
    kana_ext_b = kana_extended_b,
    kana_supplement,    // 268
    kana_sup = kana_supplement,
    kana_extended_a,    // 269
    kana_ext_a = kana_extended_a,
    small_kana_extension,    // 270
    small_kana_ext = small_kana_extension,
    nushu,                        // 271
    duployan,                     // 272
    shorthand_format_controls,    // 273
    znamenny_musical_notation,    // 274
    znamenny_music = znamenny_musical_notation,
    byzantine_musical_symbols,    // 275
    byzantine_music = byzantine_musical_symbols,
    musical_symbols,    // 276
    music = musical_symbols,
    ancient_greek_musical_notation,    // 277
    ancient_greek_music = ancient_greek_musical_notation,
    mayan_numerals,           // 278
    tai_xuan_jing_symbols,    // 279
    tai_xuan_jing = tai_xuan_jing_symbols,
    counting_rod_numerals,    // 280
    counting_rod = counting_rod_numerals,
    mathematical_alphanumeric_symbols,    // 281
    math_alphanum = mathematical_alphanumeric_symbols,
    sutton_signwriting,    // 282
    latin_extended_g,      // 283
    latin_ext_g = latin_extended_g,
    glagolitic_supplement,    // 284
    glagolitic_sup = glagolitic_supplement,
    nyiakeng_puachue_hmong,    // 285
    toto,                      // 286
    wancho,                    // 287
    ethiopic_extended_b,       // 288
    ethiopic_ext_b = ethiopic_extended_b,
    mende_kikakui,                             // 289
    adlam,                                     // 290
    indic_siyaq_numbers,                       // 291
    ottoman_siyaq_numbers,                     // 292
    arabic_mathematical_alphabetic_symbols,    // 293
    arabic_math = arabic_mathematical_alphabetic_symbols,
    mahjong_tiles,    // 294
    mahjong = mahjong_tiles,
    domino_tiles,    // 295
    domino = domino_tiles,
    playing_cards,                       // 296
    enclosed_alphanumeric_supplement,    // 297
    enclosed_alphanum_sup = enclosed_alphanumeric_supplement,
    enclosed_ideographic_supplement,    // 298
    enclosed_ideographic_sup = enclosed_ideographic_supplement,
    miscellaneous_symbols_and_pictographs,    // 299
    misc_pictographs = miscellaneous_symbols_and_pictographs,
    emoticons,                    // 300
    ornamental_dingbats,          // 301
    transport_and_map_symbols,    // 302
    transport_and_map = transport_and_map_symbols,
    alchemical_symbols,    // 303
    alchemical = alchemical_symbols,
    geometric_shapes_extended,    // 304
    geometric_shapes_ext = geometric_shapes_extended,
    supplemental_arrows_c,    // 305
    sup_arrows_c = supplemental_arrows_c,
    supplemental_symbols_and_pictographs,    // 306
    sup_symbols_and_pictographs = supplemental_symbols_and_pictographs,
    chess_symbols,                         // 307
    symbols_and_pictographs_extended_a,    // 308
    symbols_and_pictographs_ext_a = symbols_and_pictographs_extended_a,
    symbols_for_legacy_computing,          // 309
    cjk_unified_ideographs_extension_b,    // 310
    cjk_ext_b = cjk_unified_ideographs_extension_b,
    cjk_unified_ideographs_extension_c,    // 311
    cjk_ext_c = cjk_unified_ideographs_extension_c,
    cjk_unified_ideographs_extension_d,    // 312
    cjk_ext_d = cjk_unified_ideographs_extension_d,
    cjk_unified_ideographs_extension_e,    // 313
    cjk_ext_e = cjk_unified_ideographs_extension_e,
    cjk_unified_ideographs_extension_f,    // 314
    cjk_ext_f = cjk_unified_ideographs_extension_f,
    cjk_compatibility_ideographs_supplement,    // 315
    cjk_compat_ideographs_sup = cjk_compatibility_ideographs_supplement,
    cjk_unified_ideographs_extension_g,    // 316
    cjk_ext_g = cjk_unified_ideographs_extension_g,
    tags,                              // 317
    variation_selectors_supplement,    // 318
    vs_sup = variation_selectors_supplement,
    supplementary_private_use_area_a,    // 319
    sup_pua_a = supplementary_private_use_area_a,
    supplementary_private_use_area_b,    // 320
    sup_pua_b = supplementary_private_use_area_b,
    __max
};
enum class script {
    adlm,    // 0
    adlam = adlm,
    aghb,    // 1
    caucasian_albanian = aghb,
    ahom,    // 2
    arab,    // 3
    arabic = arab,
    armi,    // 4
    imperial_aramaic = armi,
    armn,    // 5
    armenian = armn,
    avst,    // 6
    avestan = avst,
    bali,    // 7
    balinese = bali,
    bamu,    // 8
    bamum = bamu,
    bass,    // 9
    bassa_vah = bass,
    batk,    // 10
    batak = batk,
    beng,    // 11
    bengali = beng,
    bhks,    // 12
    bhaiksuki = bhks,
    bopo,    // 13
    bopomofo = bopo,
    brah,    // 14
    brahmi = brah,
    brai,    // 15
    braille = brai,
    bugi,    // 16
    buginese = bugi,
    buhd,    // 17
    buhid = buhd,
    cakm,    // 18
    chakma = cakm,
    cans,    // 19
    canadian_aboriginal = cans,
    cari,    // 20
    carian = cari,
    cham,    // 21
    cher,    // 22
    cherokee = cher,
    chrs,    // 23
    chorasmian = chrs,
    copt,    // 24
    coptic = copt,
    cpmn,    // 25
    cypro_minoan = cpmn,
    cprt,    // 26
    cypriot = cprt,
    cyrl,    // 27
    cyrillic = cyrl,
    deva,    // 28
    devanagari = deva,
    diak,    // 29
    dives_akuru = diak,
    dogr,    // 30
    dogra = dogr,
    dsrt,    // 31
    deseret = dsrt,
    dupl,    // 32
    duployan = dupl,
    egyp,    // 33
    egyptian_hieroglyphs = egyp,
    elba,    // 34
    elbasan = elba,
    elym,    // 35
    elymaic = elym,
    ethi,    // 36
    ethiopic = ethi,
    geor,    // 37
    georgian = geor,
    glag,    // 38
    glagolitic = glag,
    gong,    // 39
    gunjala_gondi = gong,
    gonm,    // 40
    masaram_gondi = gonm,
    goth,    // 41
    gothic = goth,
    gran,    // 42
    grantha = gran,
    grek,    // 43
    greek = grek,
    gujr,    // 44
    gujarati = gujr,
    guru,    // 45
    gurmukhi = guru,
    hang,    // 46
    hangul = hang,
    hani,    // 47
    han = hani,
    hano,    // 48
    hanunoo = hano,
    hatr,    // 49
    hatran = hatr,
    hebr,    // 50
    hebrew = hebr,
    hira,    // 51
    hiragana = hira,
    hluw,    // 52
    anatolian_hieroglyphs = hluw,
    hmng,    // 53
    pahawh_hmong = hmng,
    hmnp,    // 54
    nyiakeng_puachue_hmong = hmnp,
    hrkt,    // 55
    katakana_or_hiragana = hrkt,
    hung,    // 56
    old_hungarian = hung,
    ital,    // 57
    old_italic = ital,
    java,    // 58
    javanese = java,
    kali,    // 59
    kayah_li = kali,
    kana,    // 60
    katakana = kana,
    khar,    // 61
    kharoshthi = khar,
    khmr,    // 62
    khmer = khmr,
    khoj,    // 63
    khojki = khoj,
    kits,    // 64
    khitan_small_script = kits,
    knda,    // 65
    kannada = knda,
    kthi,    // 66
    kaithi = kthi,
    lana,    // 67
    tai_tham = lana,
    laoo,    // 68
    lao = laoo,
    latn,    // 69
    latin = latn,
    lepc,    // 70
    lepcha = lepc,
    limb,    // 71
    limbu = limb,
    lina,    // 72
    linear_a = lina,
    linb,    // 73
    linear_b = linb,
    lisu,    // 74
    lyci,    // 75
    lycian = lyci,
    lydi,    // 76
    lydian = lydi,
    mahj,    // 77
    mahajani = mahj,
    maka,    // 78
    makasar = maka,
    mand,    // 79
    mandaic = mand,
    mani,    // 80
    manichaean = mani,
    marc,    // 81
    marchen = marc,
    medf,    // 82
    medefaidrin = medf,
    mend,    // 83
    mende_kikakui = mend,
    merc,    // 84
    meroitic_cursive = merc,
    mero,    // 85
    meroitic_hieroglyphs = mero,
    mlym,    // 86
    malayalam = mlym,
    modi,    // 87
    mong,    // 88
    mongolian = mong,
    mroo,    // 89
    mro = mroo,
    mtei,    // 90
    meetei_mayek = mtei,
    mult,    // 91
    multani = mult,
    mymr,    // 92
    myanmar = mymr,
    nand,    // 93
    nandinagari = nand,
    narb,    // 94
    old_north_arabian = narb,
    nbat,    // 95
    nabataean = nbat,
    newa,    // 96
    nkoo,    // 97
    nko = nkoo,
    nshu,    // 98
    nushu = nshu,
    ogam,    // 99
    ogham = ogam,
    olck,    // 100
    ol_chiki = olck,
    orkh,    // 101
    old_turkic = orkh,
    orya,    // 102
    oriya = orya,
    osge,    // 103
    osage = osge,
    osma,    // 104
    osmanya = osma,
    ougr,    // 105
    old_uyghur = ougr,
    palm,    // 106
    palmyrene = palm,
    pauc,    // 107
    pau_cin_hau = pauc,
    perm,    // 108
    old_permic = perm,
    phag,    // 109
    phags_pa = phag,
    phli,    // 110
    inscriptional_pahlavi = phli,
    phlp,    // 111
    psalter_pahlavi = phlp,
    phnx,    // 112
    phoenician = phnx,
    plrd,    // 113
    miao = plrd,
    prti,    // 114
    inscriptional_parthian = prti,
    rjng,    // 115
    rejang = rjng,
    rohg,    // 116
    hanifi_rohingya = rohg,
    runr,    // 117
    runic = runr,
    samr,    // 118
    samaritan = samr,
    sarb,    // 119
    old_south_arabian = sarb,
    saur,    // 120
    saurashtra = saur,
    sgnw,    // 121
    signwriting = sgnw,
    shaw,    // 122
    shavian = shaw,
    shrd,    // 123
    sharada = shrd,
    sidd,    // 124
    siddham = sidd,
    sind,    // 125
    khudawadi = sind,
    sinh,    // 126
    sinhala = sinh,
    sogd,    // 127
    sogdian = sogd,
    sogo,    // 128
    old_sogdian = sogo,
    sora,    // 129
    sora_sompeng = sora,
    soyo,    // 130
    soyombo = soyo,
    sund,    // 131
    sundanese = sund,
    sylo,    // 132
    syloti_nagri = sylo,
    syrc,    // 133
    syriac = syrc,
    tagb,    // 134
    tagbanwa = tagb,
    takr,    // 135
    takri = takr,
    tale,    // 136
    tai_le = tale,
    talu,    // 137
    new_tai_lue = talu,
    taml,    // 138
    tamil = taml,
    tang,    // 139
    tangut = tang,
    tavt,    // 140
    tai_viet = tavt,
    telu,    // 141
    telugu = telu,
    tfng,    // 142
    tifinagh = tfng,
    tglg,    // 143
    tagalog = tglg,
    thaa,    // 144
    thaana = thaa,
    thai,    // 145
    tibt,    // 146
    tibetan = tibt,
    tirh,    // 147
    tirhuta = tirh,
    tnsa,    // 148
    tangsa = tnsa,
    toto,    // 149
    ugar,    // 150
    ugaritic = ugar,
    vaii,    // 151
    vai = vaii,
    vith,    // 152
    vithkuqi = vith,
    wara,    // 153
    warang_citi = wara,
    wcho,    // 154
    wancho = wcho,
    xpeo,    // 155
    old_persian = xpeo,
    xsux,    // 156
    cuneiform = xsux,
    yezi,    // 157
    yezidi = yezi,
    yiii,    // 158
    yi = yiii,
    zanb,    // 159
    zanabazar_square = zanb,
    zinh,    // 160
    inherited = zinh,
    zyyy,    // 161
    common = zyyy,
    zzzz,    // 162
    unknown = zzzz,
    max
};
namespace detail::tables {
    static constexpr const char* age_strings[] = {
        "unassigned", "1.1",  "2.0",  "2.1",  "3.0",  "3.1",  "3.2", "4.0", "4.1",
        "5.0",        "5.1",  "5.2",  "6.0",  "6.1",  "6.2",  "6.3", "7.0", "8.0",
        "9.0",        "10.0", "11.0", "12.0", "12.1", "13.0", "14.0"};
    static constexpr compact_range age_data = {
        0x00000001, 0x0001F604, 0x0001FA01, 0x00021804, 0x00022006, 0x00022107, 0x00022204,
        0x00023407, 0x00023708, 0x00024209, 0x00025001, 0x0002A904, 0x0002AE07, 0x0002B001,
        0x0002DF04, 0x0002E001, 0x0002EA04, 0x0002EF07, 0x00030001, 0x00034604, 0x00034F06,
        0x00035007, 0x00035808, 0x00035D07, 0x00036001, 0x00036204, 0x00036306, 0x0003700A,
        0x00037401, 0x0003760A, 0x00037800, 0x00037A01, 0x00037B09, 0x00037E01, 0x00037F10,
        0x00038000, 0x00038401, 0x00038B00, 0x00038C01, 0x00038D00, 0x00038E01, 0x0003A200,
        0x0003A301, 0x0003CF0A, 0x0003D001, 0x0003D704, 0x0003D806, 0x0003DA01, 0x0003DB04,
        0x0003DC01, 0x0003DD04, 0x0003DE01, 0x0003DF04, 0x0003E001, 0x0003E104, 0x0003E201,
        0x0003F405, 0x0003F606, 0x0003F707, 0x0003FC08, 0x00040004, 0x00040101, 0x00040D04,
        0x00040E01, 0x00045004, 0x00045101, 0x00045D04, 0x00045E01, 0x0004870A, 0x00048804,
        0x00048A06, 0x00048C04, 0x00049001, 0x0004C506, 0x0004C701, 0x0004C906, 0x0004CB01,
        0x0004CD06, 0x0004CF09, 0x0004D001, 0x0004EC04, 0x0004EE01, 0x0004F608, 0x0004F801,
        0x0004FA09, 0x00050006, 0x00051009, 0x0005140A, 0x0005240B, 0x0005260C, 0x00052810,
        0x00053000, 0x00053101, 0x00055700, 0x00055901, 0x00056014, 0x00056101, 0x00058814,
        0x00058901, 0x00058A04, 0x00058B00, 0x00058D10, 0x00058F0D, 0x00059000, 0x00059102,
        0x0005A208, 0x0005A302, 0x0005B001, 0x0005BA09, 0x0005BB01, 0x0005C402, 0x0005C508,
        0x0005C800, 0x0005D001, 0x0005EB00, 0x0005EF14, 0x0005F001, 0x0005F500, 0x00060007,
        0x0006040D, 0x00060510, 0x0006060A, 0x00060B08, 0x00060C01, 0x00060D07, 0x0006160A,
        0x00061B01, 0x00061C0F, 0x00061D18, 0x00061E08, 0x00061F01, 0x0006200C, 0x00062101,
        0x00063B0A, 0x00064001, 0x00065304, 0x00065607, 0x00065908, 0x00065F0C, 0x00066001,
        0x00066E06, 0x00067001, 0x0006B804, 0x0006BA01, 0x0006BF04, 0x0006C001, 0x0006CF04,
        0x0006D001, 0x0006EE07, 0x0006F001, 0x0006FA04, 0x0006FF07, 0x00070004, 0x00070E00,
        0x00070F04, 0x00072D07, 0x00073004, 0x00074B00, 0x00074D07, 0x00075008, 0x00076E0A,
        0x00078004, 0x0007B106, 0x0007B200, 0x0007C009, 0x0007FB00, 0x0007FD14, 0x0008000B,
        0x00082E00, 0x0008300B, 0x00083F00, 0x0008400C, 0x00085C00, 0x00085E0C, 0x00085F00,
        0x00086013, 0x00086B00, 0x00087018, 0x00088F00, 0x00089018, 0x00089200, 0x00089818,
        0x0008A00D, 0x0008A110, 0x0008A20D, 0x0008AD10, 0x0008B311, 0x0008B518, 0x0008B612,
        0x0008BE17, 0x0008C818, 0x0008D314, 0x0008D412, 0x0008E311, 0x0008E40D, 0x0008FF10,
        0x0009000B, 0x00090101, 0x00090407, 0x00090501, 0x00093A0C, 0x00093C01, 0x00094E0B,
        0x00094F0C, 0x00095001, 0x0009550B, 0x0009560C, 0x00095801, 0x0009710A, 0x0009730C,
        0x00097810, 0x0009790B, 0x00097B09, 0x00097D08, 0x00097E09, 0x00098010, 0x00098101,
        0x00098400, 0x00098501, 0x00098D00, 0x00098F01, 0x00099100, 0x00099301, 0x0009A900,
        0x0009AA01, 0x0009B100, 0x0009B201, 0x0009B300, 0x0009B601, 0x0009BA00, 0x0009BC01,
        0x0009BD07, 0x0009BE01, 0x0009C500, 0x0009C701, 0x0009C900, 0x0009CB01, 0x0009CE08,
        0x0009CF00, 0x0009D701, 0x0009D800, 0x0009DC01, 0x0009DE00, 0x0009DF01, 0x0009E400,
        0x0009E601, 0x0009FB0B, 0x0009FC13, 0x0009FE14, 0x0009FF00, 0x000A0107, 0x000A0201,
        0x000A0307, 0x000A0400, 0x000A0501, 0x000A0B00, 0x000A0F01, 0x000A1100, 0x000A1301,
        0x000A2900, 0x000A2A01, 0x000A3100, 0x000A3201, 0x000A3400, 0x000A3501, 0x000A3700,
        0x000A3801, 0x000A3A00, 0x000A3C01, 0x000A3D00, 0x000A3E01, 0x000A4300, 0x000A4701,
        0x000A4900, 0x000A4B01, 0x000A4E00, 0x000A510A, 0x000A5200, 0x000A5901, 0x000A5D00,
        0x000A5E01, 0x000A5F00, 0x000A6601, 0x000A750A, 0x000A7614, 0x000A7700, 0x000A8101,
        0x000A8400, 0x000A8501, 0x000A8C07, 0x000A8D01, 0x000A8E00, 0x000A8F01, 0x000A9200,
        0x000A9301, 0x000AA900, 0x000AAA01, 0x000AB100, 0x000AB201, 0x000AB400, 0x000AB501,
        0x000ABA00, 0x000ABC01, 0x000AC600, 0x000AC701, 0x000ACA00, 0x000ACB01, 0x000ACE00,
        0x000AD001, 0x000AD100, 0x000AE001, 0x000AE107, 0x000AE400, 0x000AE601, 0x000AF00D,
        0x000AF107, 0x000AF200, 0x000AF911, 0x000AFA13, 0x000B0000, 0x000B0101, 0x000B0400,
        0x000B0501, 0x000B0D00, 0x000B0F01, 0x000B1100, 0x000B1301, 0x000B2900, 0x000B2A01,
        0x000B3100, 0x000B3201, 0x000B3400, 0x000B3507, 0x000B3601, 0x000B3A00, 0x000B3C01,
        0x000B440A, 0x000B4500, 0x000B4701, 0x000B4900, 0x000B4B01, 0x000B4E00, 0x000B5517,
        0x000B5601, 0x000B5800, 0x000B5C01, 0x000B5E00, 0x000B5F01, 0x000B620A, 0x000B6400,
        0x000B6601, 0x000B7107, 0x000B720C, 0x000B7800, 0x000B8201, 0x000B8400, 0x000B8501,
        0x000B8B00, 0x000B8E01, 0x000B9100, 0x000B9201, 0x000B9600, 0x000B9901, 0x000B9B00,
        0x000B9C01, 0x000B9D00, 0x000B9E01, 0x000BA000, 0x000BA301, 0x000BA500, 0x000BA801,
        0x000BAB00, 0x000BAE01, 0x000BB608, 0x000BB701, 0x000BBA00, 0x000BBE01, 0x000BC300,
        0x000BC601, 0x000BC900, 0x000BCA01, 0x000BCE00, 0x000BD00A, 0x000BD100, 0x000BD701,
        0x000BD800, 0x000BE608, 0x000BE701, 0x000BF307, 0x000BFB00, 0x000C0010, 0x000C0101,
        0x000C0414, 0x000C0501, 0x000C0D00, 0x000C0E01, 0x000C1100, 0x000C1201, 0x000C2900,
        0x000C2A01, 0x000C3410, 0x000C3501, 0x000C3A00, 0x000C3C18, 0x000C3D0A, 0x000C3E01,
        0x000C4500, 0x000C4601, 0x000C4900, 0x000C4A01, 0x000C4E00, 0x000C5501, 0x000C5700,
        0x000C580A, 0x000C5A11, 0x000C5B00, 0x000C5D18, 0x000C5E00, 0x000C6001, 0x000C620A,
        0x000C6400, 0x000C6601, 0x000C7000, 0x000C7715, 0x000C780A, 0x000C8012, 0x000C8110,
        0x000C8201, 0x000C8414, 0x000C8501, 0x000C8D00, 0x000C8E01, 0x000C9100, 0x000C9201,
        0x000CA900, 0x000CAA01, 0x000CB400, 0x000CB501, 0x000CBA00, 0x000CBC07, 0x000CBE01,
        0x000CC500, 0x000CC601, 0x000CC900, 0x000CCA01, 0x000CCE00, 0x000CD501, 0x000CD700,
        0x000CDD18, 0x000CDE01, 0x000CDF00, 0x000CE001, 0x000CE209, 0x000CE400, 0x000CE601,
        0x000CF000, 0x000CF109, 0x000CF300, 0x000D0013, 0x000D0110, 0x000D0201, 0x000D0417,
        0x000D0501, 0x000D0D00, 0x000D0E01, 0x000D1100, 0x000D1201, 0x000D290C, 0x000D2A01,
        0x000D3A0C, 0x000D3B13, 0x000D3D0A, 0x000D3E01, 0x000D440A, 0x000D4500, 0x000D4601,
        0x000D4900, 0x000D4A01, 0x000D4E0C, 0x000D4F12, 0x000D5000, 0x000D5412, 0x000D5701,
        0x000D5812, 0x000D5F11, 0x000D6001, 0x000D620A, 0x000D6400, 0x000D6601, 0x000D700A,
        0x000D7612, 0x000D790A, 0x000D8000, 0x000D8117, 0x000D8204, 0x000D8400, 0x000D8504,
        0x000D9700, 0x000D9A04, 0x000DB200, 0x000DB304, 0x000DBC00, 0x000DBD04, 0x000DBE00,
        0x000DC004, 0x000DC700, 0x000DCA04, 0x000DCB00, 0x000DCF04, 0x000DD500, 0x000DD604,
        0x000DD700, 0x000DD804, 0x000DE000, 0x000DE610, 0x000DF000, 0x000DF204, 0x000DF500,
        0x000E0101, 0x000E3B00, 0x000E3F01, 0x000E5C00, 0x000E8101, 0x000E8300, 0x000E8401,
        0x000E8500, 0x000E8615, 0x000E8701, 0x000E8915, 0x000E8A01, 0x000E8B00, 0x000E8C15,
        0x000E8D01, 0x000E8E15, 0x000E9401, 0x000E9815, 0x000E9901, 0x000EA015, 0x000EA101,
        0x000EA400, 0x000EA501, 0x000EA600, 0x000EA701, 0x000EA815, 0x000EAA01, 0x000EAC15,
        0x000EAD01, 0x000EBA15, 0x000EBB01, 0x000EBE00, 0x000EC001, 0x000EC500, 0x000EC601,
        0x000EC700, 0x000EC801, 0x000ECE00, 0x000ED001, 0x000EDA00, 0x000EDC01, 0x000EDE0D,
        0x000EE000, 0x000F0002, 0x000F4800, 0x000F4902, 0x000F6A04, 0x000F6B0A, 0x000F6D00,
        0x000F7102, 0x000F8C0C, 0x000F9002, 0x000F9604, 0x000F9702, 0x000F9800, 0x000F9902,
        0x000FAE04, 0x000FB102, 0x000FB804, 0x000FB902, 0x000FBA04, 0x000FBD00, 0x000FBE04,
        0x000FCD00, 0x000FCE0A, 0x000FCF04, 0x000FD008, 0x000FD20A, 0x000FD50B, 0x000FD90C,
        0x000FDB00, 0x00100004, 0x0010220A, 0x00102304, 0x0010280A, 0x00102904, 0x00102B0A,
        0x00102C04, 0x0010330A, 0x00103604, 0x00103A0A, 0x00104004, 0x00105A0A, 0x00109A0B,
        0x00109E0A, 0x0010A001, 0x0010C600, 0x0010C70D, 0x0010C800, 0x0010CD0D, 0x0010CE00,
        0x0010D001, 0x0010F706, 0x0010F908, 0x0010FB01, 0x0010FC08, 0x0010FD0D, 0x00110001,
        0x00115A0B, 0x00115F01, 0x0011A30B, 0x0011A801, 0x0011FA0B, 0x00120004, 0x00120708,
        0x00120804, 0x00124708, 0x00124804, 0x00124900, 0x00124A04, 0x00124E00, 0x00125004,
        0x00125700, 0x00125804, 0x00125900, 0x00125A04, 0x00125E00, 0x00126004, 0x00128708,
        0x00128804, 0x00128900, 0x00128A04, 0x00128E00, 0x00129004, 0x0012AF08, 0x0012B004,
        0x0012B100, 0x0012B204, 0x0012B600, 0x0012B804, 0x0012BF00, 0x0012C004, 0x0012C100,
        0x0012C204, 0x0012C600, 0x0012C804, 0x0012CF08, 0x0012D004, 0x0012D700, 0x0012D804,
        0x0012EF08, 0x0012F004, 0x00130F08, 0x00131004, 0x00131100, 0x00131204, 0x00131600,
        0x00131804, 0x00131F08, 0x00132004, 0x00134708, 0x00134804, 0x00135B00, 0x00135D0C,
        0x00135F08, 0x00136104, 0x00137D00, 0x00138008, 0x00139A00, 0x0013A004, 0x0013F511,
        0x0013F600, 0x0013F811, 0x0013FE00, 0x0014000B, 0x00140104, 0x0016770B, 0x00168004,
        0x00169D00, 0x0016A004, 0x0016F110, 0x0016F900, 0x00170006, 0x00170D18, 0x00170E06,
        0x00171518, 0x00171600, 0x00171F18, 0x00172006, 0x00173700, 0x00174006, 0x00175400,
        0x00176006, 0x00176D00, 0x00176E06, 0x00177100, 0x00177206, 0x00177400, 0x00178004,
        0x0017DD07, 0x0017DE00, 0x0017E004, 0x0017EA00, 0x0017F007, 0x0017FA00, 0x00180004,
        0x00180F18, 0x00181004, 0x00181A00, 0x00182004, 0x00187814, 0x00187900, 0x00188004,
        0x0018AA0A, 0x0018AB00, 0x0018B00B, 0x0018F600, 0x00190007, 0x00191D10, 0x00191F00,
        0x00192007, 0x00192C00, 0x00193007, 0x00193C00, 0x00194007, 0x00194100, 0x00194407,
        0x00196E00, 0x00197007, 0x00197500, 0x00198008, 0x0019AA0B, 0x0019AC00, 0x0019B008,
        0x0019CA00, 0x0019D008, 0x0019DA0B, 0x0019DB00, 0x0019DE08, 0x0019E007, 0x001A0008,
        0x001A1C00, 0x001A1E08, 0x001A200B, 0x001A5F00, 0x001A600B, 0x001A7D00, 0x001A7F0B,
        0x001A8A00, 0x001A900B, 0x001A9A00, 0x001AA00B, 0x001AAE00, 0x001AB010, 0x001ABF17,
        0x001AC118, 0x001ACF00, 0x001B0009, 0x001B4C18, 0x001B4D00, 0x001B5009, 0x001B7D18,
        0x001B7F00, 0x001B800A, 0x001BAB0D, 0x001BAE0A, 0x001BBA0D, 0x001BC00C, 0x001BF400,
        0x001BFC0C, 0x001C000A, 0x001C3800, 0x001C3B0A, 0x001C4A00, 0x001C4D0A, 0x001C8012,
        0x001C8900, 0x001C9014, 0x001CBB00, 0x001CBD14, 0x001CC00D, 0x001CC800, 0x001CD00B,
        0x001CF30D, 0x001CF713, 0x001CF810, 0x001CFA15, 0x001CFB00, 0x001D0007, 0x001D6C08,
        0x001DC409, 0x001DCB0A, 0x001DE710, 0x001DF613, 0x001DFA18, 0x001DFB12, 0x001DFC0C,
        0x001DFD0B, 0x001DFE09, 0x001E0001, 0x001E9B02, 0x001E9C0A, 0x001EA001, 0x001EFA0A,
        0x001F0001, 0x001F1600, 0x001F1801, 0x001F1E00, 0x001F2001, 0x001F4600, 0x001F4801,
        0x001F4E00, 0x001F5001, 0x001F5800, 0x001F5901, 0x001F5A00, 0x001F5B01, 0x001F5C00,
        0x001F5D01, 0x001F5E00, 0x001F5F01, 0x001F7E00, 0x001F8001, 0x001FB500, 0x001FB601,
        0x001FC500, 0x001FC601, 0x001FD400, 0x001FD601, 0x001FDC00, 0x001FDD01, 0x001FF000,
        0x001FF201, 0x001FF500, 0x001FF601, 0x001FFF00, 0x00200001, 0x00202F04, 0x00203001,
        0x00204706, 0x00204804, 0x00204E06, 0x00205307, 0x00205508, 0x00205706, 0x00205808,
        0x00205F06, 0x0020640A, 0x00206500, 0x0020660F, 0x00206A01, 0x00207106, 0x00207200,
        0x00207401, 0x00208F00, 0x00209008, 0x0020950C, 0x00209D00, 0x0020A001, 0x0020AB02,
        0x0020AC03, 0x0020AD04, 0x0020B006, 0x0020B208, 0x0020B60B, 0x0020B90C, 0x0020BA0E,
        0x0020BB10, 0x0020BE11, 0x0020BF13, 0x0020C018, 0x0020C100, 0x0020D001, 0x0020E204,
        0x0020E406, 0x0020EB08, 0x0020EC09, 0x0020F00A, 0x0020F100, 0x00210001, 0x00213904,
        0x00213B07, 0x00213C08, 0x00213D06, 0x00214C08, 0x00214D09, 0x00214F0A, 0x0021500B,
        0x00215301, 0x00218304, 0x00218409, 0x0021850A, 0x0021890B, 0x00218A11, 0x00218C00,
        0x00219001, 0x0021EB04, 0x0021F406, 0x00220001, 0x0022F206, 0x00230001, 0x00230104,
        0x00230201, 0x00237B04, 0x00237C06, 0x00237D04, 0x00239B06, 0x0023CF07, 0x0023D108,
        0x0023DC09, 0x0023E80B, 0x0023E90C, 0x0023F410, 0x0023FB12, 0x0023FF13, 0x00240001,
        0x00242504, 0x00242700, 0x00244001, 0x00244B00, 0x00246001, 0x0024EB06, 0x0024FF07,
        0x00250001, 0x00259606, 0x0025A001, 0x0025F004, 0x0025F806, 0x00260001, 0x00261407,
        0x00261606, 0x00261808, 0x00261904, 0x00261A01, 0x00267004, 0x00267206, 0x00267E08,
        0x00268006, 0x00268A07, 0x00269208, 0x00269D0A, 0x00269E0B, 0x0026A007, 0x0026A208,
        0x0026B209, 0x0026B30A, 0x0026BD0B, 0x0026C00A, 0x0026C40B, 0x0026CE0C, 0x0026CF0B,
        0x0026E20C, 0x0026E30B, 0x0026E40C, 0x0026E80B, 0x00270010, 0x00270101, 0x0027050C,
        0x00270601, 0x00270A0C, 0x00270C01, 0x0027280C, 0x00272901, 0x00274C0C, 0x00274D01,
        0x00274E0C, 0x00274F01, 0x0027530C, 0x00275601, 0x0027570B, 0x00275801, 0x00275F0C,
        0x00276101, 0x00276806, 0x00277601, 0x0027950C, 0x00279801, 0x0027B00C, 0x0027B101,
        0x0027BF0C, 0x0027C008, 0x0027C709, 0x0027CB0D, 0x0027CC0A, 0x0027CD0D, 0x0027CE0C,
        0x0027D006, 0x0027EC0A, 0x0027F006, 0x00280004, 0x00290006, 0x002B0007, 0x002B0E08,
        0x002B1409, 0x002B1B0A, 0x002B2009, 0x002B240A, 0x002B4D10, 0x002B500A, 0x002B550B,
        0x002B5A10, 0x002B7400, 0x002B7610, 0x002B9600, 0x002B9717, 0x002B9810, 0x002BBA14,
        0x002BBD10, 0x002BC915, 0x002BCA10, 0x002BD213, 0x002BD314, 0x002BEC11, 0x002BF014,
        0x002BFF15, 0x002C0008, 0x002C2F18, 0x002C3008, 0x002C5F18, 0x002C6009, 0x002C6D0A,
        0x002C700B, 0x002C710A, 0x002C7409, 0x002C780A, 0x002C7E0B, 0x002C8008, 0x002CEB0B,
        0x002CF20D, 0x002CF400, 0x002CF908, 0x002D2600, 0x002D270D, 0x002D2800, 0x002D2D0D,
        0x002D2E00, 0x002D3008, 0x002D660D, 0x002D6800, 0x002D6F08, 0x002D700C, 0x002D7100,
        0x002D7F0C, 0x002D8008, 0x002D9700, 0x002DA008, 0x002DA700, 0x002DA808, 0x002DAF00,
        0x002DB008, 0x002DB700, 0x002DB808, 0x002DBF00, 0x002DC008, 0x002DC700, 0x002DC808,
        0x002DCF00, 0x002DD008, 0x002DD700, 0x002DD808, 0x002DDF00, 0x002DE00A, 0x002E0008,
        0x002E180A, 0x002E1C08, 0x002E1E0A, 0x002E310B, 0x002E320D, 0x002E3C10, 0x002E4312,
        0x002E4513, 0x002E4A14, 0x002E4F15, 0x002E5017, 0x002E5318, 0x002E5E00, 0x002E8004,
        0x002E9A00, 0x002E9B04, 0x002EF400, 0x002F0004, 0x002FD600, 0x002FF004, 0x002FFC00,
        0x00300001, 0x00303804, 0x00303B06, 0x00303E04, 0x00303F01, 0x00304000, 0x00304101,
        0x00309506, 0x00309700, 0x00309901, 0x00309F06, 0x0030A101, 0x0030FF06, 0x00310000,
        0x00310501, 0x00312D0A, 0x00312E13, 0x00312F14, 0x00313000, 0x00313101, 0x00318F00,
        0x00319001, 0x0031A004, 0x0031B80C, 0x0031BB17, 0x0031C008, 0x0031D00A, 0x0031E400,
        0x0031F006, 0x00320001, 0x00321D07, 0x00321F00, 0x00322001, 0x0032440B, 0x00325007,
        0x00325106, 0x00326001, 0x00327C07, 0x00327E08, 0x00327F01, 0x0032B106, 0x0032C001,
        0x0032CC07, 0x0032D001, 0x0032FF16, 0x00330001, 0x00337707, 0x00337B01, 0x0033DE07,
        0x0033E001, 0x0033FF07, 0x00340004, 0x004DB617, 0x004DC007, 0x004E0001, 0x009FA608,
        0x009FBC0A, 0x009FC40B, 0x009FCC0D, 0x009FCD11, 0x009FD613, 0x009FEB14, 0x009FF017,
        0x009FFD18, 0x00A00004, 0x00A48D00, 0x00A49004, 0x00A4A206, 0x00A4A404, 0x00A4B406,
        0x00A4B504, 0x00A4C106, 0x00A4C204, 0x00A4C506, 0x00A4C604, 0x00A4C700, 0x00A4D00B,
        0x00A5000A, 0x00A62C00, 0x00A6400A, 0x00A6600C, 0x00A6620A, 0x00A6740D, 0x00A67C0A,
        0x00A69810, 0x00A69E11, 0x00A69F0D, 0x00A6A00B, 0x00A6F800, 0x00A70008, 0x00A71709,
        0x00A71B0A, 0x00A72009, 0x00A7220A, 0x00A78D0C, 0x00A78F11, 0x00A7900C, 0x00A7920D,
        0x00A79410, 0x00A7A00C, 0x00A7AA0D, 0x00A7AB10, 0x00A7AE12, 0x00A7AF14, 0x00A7B010,
        0x00A7B211, 0x00A7B814, 0x00A7BA15, 0x00A7C018, 0x00A7C215, 0x00A7C717, 0x00A7CB00,
        0x00A7D018, 0x00A7D200, 0x00A7D318, 0x00A7D400, 0x00A7D518, 0x00A7DA00, 0x00A7F218,
        0x00A7F517, 0x00A7F710, 0x00A7F80D, 0x00A7FA0C, 0x00A7FB0A, 0x00A80008, 0x00A82C17,
        0x00A82D00, 0x00A8300B, 0x00A83A00, 0x00A84009, 0x00A87800, 0x00A8800A, 0x00A8C512,
        0x00A8C600, 0x00A8CE0A, 0x00A8DA00, 0x00A8E00B, 0x00A8FC11, 0x00A8FE14, 0x00A9000A,
        0x00A95400, 0x00A95F0A, 0x00A9600B, 0x00A97D00, 0x00A9800B, 0x00A9CE00, 0x00A9CF0B,
        0x00A9DA00, 0x00A9DE0B, 0x00A9E010, 0x00A9FF00, 0x00AA000A, 0x00AA3700, 0x00AA400A,
        0x00AA4E00, 0x00AA500A, 0x00AA5A00, 0x00AA5C0A, 0x00AA600B, 0x00AA7C10, 0x00AA800B,
        0x00AAC300, 0x00AADB0B, 0x00AAE00D, 0x00AAF700, 0x00AB010C, 0x00AB0700, 0x00AB090C,
        0x00AB0F00, 0x00AB110C, 0x00AB1700, 0x00AB200C, 0x00AB2700, 0x00AB280C, 0x00AB2F00,
        0x00AB3010, 0x00AB6011, 0x00AB6410, 0x00AB6615, 0x00AB6817, 0x00AB6C00, 0x00AB7011,
        0x00ABC00B, 0x00ABEE00, 0x00ABF00B, 0x00ABFA00, 0x00AC0002, 0x00D7A400, 0x00D7B00B,
        0x00D7C700, 0x00D7CB0B, 0x00D7FC00, 0x00D80002, 0x00E00001, 0x00FA2E0D, 0x00FA3006,
        0x00FA6B0B, 0x00FA6E00, 0x00FA7008, 0x00FADA00, 0x00FB0001, 0x00FB0700, 0x00FB1301,
        0x00FB1800, 0x00FB1D04, 0x00FB1E01, 0x00FB3700, 0x00FB3801, 0x00FB3D00, 0x00FB3E01,
        0x00FB3F00, 0x00FB4001, 0x00FB4200, 0x00FB4301, 0x00FB4500, 0x00FB4601, 0x00FBB20C,
        0x00FBC218, 0x00FBC300, 0x00FBD301, 0x00FD4018, 0x00FD5001, 0x00FD9000, 0x00FD9201,
        0x00FDC800, 0x00FDCF18, 0x00FDD000, 0x00FDF001, 0x00FDFC06, 0x00FDFD07, 0x00FDFE18,
        0x00FE0006, 0x00FE1008, 0x00FE1A00, 0x00FE2001, 0x00FE240A, 0x00FE2710, 0x00FE2E11,
        0x00FE3001, 0x00FE4506, 0x00FE4707, 0x00FE4901, 0x00FE5300, 0x00FE5401, 0x00FE6700,
        0x00FE6801, 0x00FE6C00, 0x00FE7001, 0x00FE7306, 0x00FE7401, 0x00FE7500, 0x00FE7601,
        0x00FEFD00, 0x00FEFF01, 0x00FF0000, 0x00FF0101, 0x00FF5F06, 0x00FF6101, 0x00FFBF00,
        0x00FFC201, 0x00FFC800, 0x00FFCA01, 0x00FFD000, 0x00FFD201, 0x00FFD800, 0x00FFDA01,
        0x00FFDD00, 0x00FFE001, 0x00FFE700, 0x00FFE801, 0x00FFEF00, 0x00FFF904, 0x00FFFC03,
        0x00FFFD01, 0x00FFFE00, 0x01000007, 0x01000C00, 0x01000D07, 0x01002700, 0x01002807,
        0x01003B00, 0x01003C07, 0x01003E00, 0x01003F07, 0x01004E00, 0x01005007, 0x01005E00,
        0x01008007, 0x0100FB00, 0x01010007, 0x01010300, 0x01010707, 0x01013400, 0x01013707,
        0x01014008, 0x01018B10, 0x01018D12, 0x01018F00, 0x0101900A, 0x01019C17, 0x01019D00,
        0x0101A010, 0x0101A100, 0x0101D00A, 0x0101FE00, 0x0102800A, 0x01029D00, 0x0102A00A,
        0x0102D100, 0x0102E010, 0x0102FC00, 0x01030005, 0x01031F10, 0x01032005, 0x01032400,
        0x01032D13, 0x01033005, 0x01034B00, 0x01035010, 0x01037B00, 0x01038007, 0x01039E00,
        0x01039F07, 0x0103A008, 0x0103C400, 0x0103C808, 0x0103D600, 0x01040005, 0x01042607,
        0x01042805, 0x01044E07, 0x01049E00, 0x0104A007, 0x0104AA00, 0x0104B012, 0x0104D400,
        0x0104D812, 0x0104FC00, 0x01050010, 0x01052800, 0x01053010, 0x01056400, 0x01056F10,
        0x01057018, 0x01057B00, 0x01057C18, 0x01058B00, 0x01058C18, 0x01059300, 0x01059418,
        0x01059600, 0x01059718, 0x0105A200, 0x0105A318, 0x0105B200, 0x0105B318, 0x0105BA00,
        0x0105BB18, 0x0105BD00, 0x01060010, 0x01073700, 0x01074010, 0x01075600, 0x01076010,
        0x01076800, 0x01078018, 0x01078600, 0x01078718, 0x0107B100, 0x0107B218, 0x0107BB00,
        0x01080007, 0x01080600, 0x01080807, 0x01080900, 0x01080A07, 0x01083600, 0x01083707,
        0x01083900, 0x01083C07, 0x01083D00, 0x01083F07, 0x0108400B, 0x01085600, 0x0108570B,
        0x01086010, 0x01089F00, 0x0108A710, 0x0108B000, 0x0108E011, 0x0108F300, 0x0108F411,
        0x0108F600, 0x0108FB11, 0x01090009, 0x01091A0B, 0x01091C00, 0x01091F09, 0x0109200A,
        0x01093A00, 0x01093F0A, 0x01094000, 0x0109800D, 0x0109B800, 0x0109BC11, 0x0109BE0D,
        0x0109C011, 0x0109D000, 0x0109D211, 0x010A0008, 0x010A0400, 0x010A0508, 0x010A0700,
        0x010A0C08, 0x010A1400, 0x010A1508, 0x010A1800, 0x010A1908, 0x010A3414, 0x010A3600,
        0x010A3808, 0x010A3B00, 0x010A3F08, 0x010A4814, 0x010A4900, 0x010A5008, 0x010A5900,
        0x010A600B, 0x010A8010, 0x010AA000, 0x010AC010, 0x010AE700, 0x010AEB10, 0x010AF700,
        0x010B000B, 0x010B3600, 0x010B390B, 0x010B5600, 0x010B580B, 0x010B7300, 0x010B780B,
        0x010B8010, 0x010B9200, 0x010B9910, 0x010B9D00, 0x010BA910, 0x010BB000, 0x010C000B,
        0x010C4900, 0x010C8011, 0x010CB300, 0x010CC011, 0x010CF300, 0x010CFA11, 0x010D0014,
        0x010D2800, 0x010D3014, 0x010D3A00, 0x010E600B, 0x010E7F00, 0x010E8017, 0x010EAA00,
        0x010EAB17, 0x010EAE00, 0x010EB017, 0x010EB200, 0x010F0014, 0x010F2800, 0x010F3014,
        0x010F5A00, 0x010F7018, 0x010F8A00, 0x010FB017, 0x010FCC00, 0x010FE015, 0x010FF700,
        0x0110000C, 0x01104E00, 0x0110520C, 0x01107018, 0x01107600, 0x01107F10, 0x0110800B,
        0x0110C218, 0x0110C300, 0x0110CD14, 0x0110CE00, 0x0110D00D, 0x0110E900, 0x0110F00D,
        0x0110FA00, 0x0111000D, 0x01113500, 0x0111360D, 0x01114414, 0x01114717, 0x01114800,
        0x01115010, 0x01117700, 0x0111800D, 0x0111C911, 0x0111CD10, 0x0111CE17, 0x0111D00D,
        0x0111DA10, 0x0111DB11, 0x0111E000, 0x0111E110, 0x0111F500, 0x01120010, 0x01121200,
        0x01121310, 0x01123E12, 0x01123F00, 0x01128011, 0x01128700, 0x01128811, 0x01128900,
        0x01128A11, 0x01128E00, 0x01128F11, 0x01129E00, 0x01129F11, 0x0112AA00, 0x0112B010,
        0x0112EB00, 0x0112F010, 0x0112FA00, 0x01130011, 0x01130110, 0x01130400, 0x01130510,
        0x01130D00, 0x01130F10, 0x01131100, 0x01131310, 0x01132900, 0x01132A10, 0x01133100,
        0x01133210, 0x01133400, 0x01133510, 0x01133A00, 0x01133B14, 0x01133C10, 0x01134500,
        0x01134710, 0x01134900, 0x01134B10, 0x01134E00, 0x01135011, 0x01135100, 0x01135710,
        0x01135800, 0x01135D10, 0x01136400, 0x01136610, 0x01136D00, 0x01137010, 0x01137500,
        0x01140012, 0x01145A17, 0x01145B12, 0x01145C00, 0x01145D12, 0x01145E14, 0x01145F15,
        0x01146017, 0x01146200, 0x01148010, 0x0114C800, 0x0114D010, 0x0114DA00, 0x01158010,
        0x0115B600, 0x0115B810, 0x0115CA11, 0x0115DE00, 0x01160010, 0x01164500, 0x01165010,
        0x01165A00, 0x01166012, 0x01166D00, 0x0116800D, 0x0116B815, 0x0116B918, 0x0116BA00,
        0x0116C00D, 0x0116CA00, 0x01170011, 0x01171A14, 0x01171B00, 0x01171D11, 0x01172C00,
        0x01173011, 0x01174018, 0x01174700, 0x01180014, 0x01183C00, 0x0118A010, 0x0118F300,
        0x0118FF10, 0x01190017, 0x01190700, 0x01190917, 0x01190A00, 0x01190C17, 0x01191400,
        0x01191517, 0x01191700, 0x01191817, 0x01193600, 0x01193717, 0x01193900, 0x01193B17,
        0x01194700, 0x01195017, 0x01195A00, 0x0119A015, 0x0119A800, 0x0119AA15, 0x0119D800,
        0x0119DA15, 0x0119E500, 0x011A0013, 0x011A4800, 0x011A5013, 0x011A8415, 0x011A8613,
        0x011A9D14, 0x011A9E13, 0x011AA300, 0x011AB018, 0x011AC010, 0x011AF900, 0x011C0012,
        0x011C0900, 0x011C0A12, 0x011C3700, 0x011C3812, 0x011C4600, 0x011C5012, 0x011C6D00,
        0x011C7012, 0x011C9000, 0x011C9212, 0x011CA800, 0x011CA912, 0x011CB700, 0x011D0013,
        0x011D0700, 0x011D0813, 0x011D0A00, 0x011D0B13, 0x011D3700, 0x011D3A13, 0x011D3B00,
        0x011D3C13, 0x011D3E00, 0x011D3F13, 0x011D4800, 0x011D5013, 0x011D5A00, 0x011D6014,
        0x011D6600, 0x011D6714, 0x011D6900, 0x011D6A14, 0x011D8F00, 0x011D9014, 0x011D9200,
        0x011D9314, 0x011D9900, 0x011DA014, 0x011DAA00, 0x011EE014, 0x011EF900, 0x011FB017,
        0x011FB100, 0x011FC015, 0x011FF200, 0x011FFF15, 0x01200009, 0x01236F10, 0x01239911,
        0x01239A00, 0x01240009, 0x01246310, 0x01246F00, 0x01247009, 0x01247410, 0x01247500,
        0x01248011, 0x01254400, 0x012F9018, 0x012FF300, 0x0130000B, 0x01342F00, 0x01343015,
        0x01343900, 0x01440011, 0x01464700, 0x0168000C, 0x016A3900, 0x016A4010, 0x016A5F00,
        0x016A6010, 0x016A6A00, 0x016A6E10, 0x016A7018, 0x016ABF00, 0x016AC018, 0x016ACA00,
        0x016AD010, 0x016AEE00, 0x016AF010, 0x016AF600, 0x016B0010, 0x016B4600, 0x016B5010,
        0x016B5A00, 0x016B5B10, 0x016B6200, 0x016B6310, 0x016B7800, 0x016B7D10, 0x016B9000,
        0x016E4014, 0x016E9B00, 0x016F000D, 0x016F4515, 0x016F4B00, 0x016F4F15, 0x016F500D,
        0x016F7F15, 0x016F8800, 0x016F8F0D, 0x016FA000, 0x016FE012, 0x016FE113, 0x016FE215,
        0x016FE417, 0x016FE500, 0x016FF017, 0x016FF200, 0x01700012, 0x0187ED14, 0x0187F215,
        0x0187F800, 0x01880012, 0x018AF317, 0x018CD600, 0x018D0017, 0x018D0900, 0x01AFF018,
        0x01AFF400, 0x01AFF518, 0x01AFFC00, 0x01AFFD18, 0x01AFFF00, 0x01B0000C, 0x01B00213,
        0x01B11F18, 0x01B12300, 0x01B15015, 0x01B15300, 0x01B16415, 0x01B16800, 0x01B17013,
        0x01B2FC00, 0x01BC0010, 0x01BC6B00, 0x01BC7010, 0x01BC7D00, 0x01BC8010, 0x01BC8900,
        0x01BC9010, 0x01BC9A00, 0x01BC9C10, 0x01BCA400, 0x01CF0018, 0x01CF2E00, 0x01CF3018,
        0x01CF4700, 0x01CF5018, 0x01CFC400, 0x01D00005, 0x01D0F600, 0x01D10005, 0x01D12700,
        0x01D1290A, 0x01D12A05, 0x01D1DE11, 0x01D1E918, 0x01D1EB00, 0x01D20008, 0x01D24600,
        0x01D2E014, 0x01D2F400, 0x01D30007, 0x01D35700, 0x01D36009, 0x01D37214, 0x01D37900,
        0x01D40005, 0x01D45500, 0x01D45605, 0x01D49D00, 0x01D49E05, 0x01D4A000, 0x01D4A205,
        0x01D4A300, 0x01D4A505, 0x01D4A700, 0x01D4A905, 0x01D4AD00, 0x01D4AE05, 0x01D4BA00,
        0x01D4BB05, 0x01D4BC00, 0x01D4BD05, 0x01D4C107, 0x01D4C205, 0x01D4C400, 0x01D4C505,
        0x01D50600, 0x01D50705, 0x01D50B00, 0x01D50D05, 0x01D51500, 0x01D51605, 0x01D51D00,
        0x01D51E05, 0x01D53A00, 0x01D53B05, 0x01D53F00, 0x01D54005, 0x01D54500, 0x01D54605,
        0x01D54700, 0x01D54A05, 0x01D55100, 0x01D55205, 0x01D6A408, 0x01D6A600, 0x01D6A805,
        0x01D7CA09, 0x01D7CC00, 0x01D7CE05, 0x01D80011, 0x01DA8C00, 0x01DA9B11, 0x01DAA000,
        0x01DAA111, 0x01DAB000, 0x01DF0018, 0x01DF1F00, 0x01E00012, 0x01E00700, 0x01E00812,
        0x01E01900, 0x01E01B12, 0x01E02200, 0x01E02312, 0x01E02500, 0x01E02612, 0x01E02B00,
        0x01E10015, 0x01E12D00, 0x01E13015, 0x01E13E00, 0x01E14015, 0x01E14A00, 0x01E14E15,
        0x01E15000, 0x01E29018, 0x01E2AF00, 0x01E2C015, 0x01E2FA00, 0x01E2FF15, 0x01E30000,
        0x01E7E018, 0x01E7E700, 0x01E7E818, 0x01E7EC00, 0x01E7ED18, 0x01E7EF00, 0x01E7F018,
        0x01E7FF00, 0x01E80010, 0x01E8C500, 0x01E8C710, 0x01E8D700, 0x01E90012, 0x01E94B15,
        0x01E94C00, 0x01E95012, 0x01E95A00, 0x01E95E12, 0x01E96000, 0x01EC7114, 0x01ECB500,
        0x01ED0115, 0x01ED3E00, 0x01EE000D, 0x01EE0400, 0x01EE050D, 0x01EE2000, 0x01EE210D,
        0x01EE2300, 0x01EE240D, 0x01EE2500, 0x01EE270D, 0x01EE2800, 0x01EE290D, 0x01EE3300,
        0x01EE340D, 0x01EE3800, 0x01EE390D, 0x01EE3A00, 0x01EE3B0D, 0x01EE3C00, 0x01EE420D,
        0x01EE4300, 0x01EE470D, 0x01EE4800, 0x01EE490D, 0x01EE4A00, 0x01EE4B0D, 0x01EE4C00,
        0x01EE4D0D, 0x01EE5000, 0x01EE510D, 0x01EE5300, 0x01EE540D, 0x01EE5500, 0x01EE570D,
        0x01EE5800, 0x01EE590D, 0x01EE5A00, 0x01EE5B0D, 0x01EE5C00, 0x01EE5D0D, 0x01EE5E00,
        0x01EE5F0D, 0x01EE6000, 0x01EE610D, 0x01EE6300, 0x01EE640D, 0x01EE6500, 0x01EE670D,
        0x01EE6B00, 0x01EE6C0D, 0x01EE7300, 0x01EE740D, 0x01EE7800, 0x01EE790D, 0x01EE7D00,
        0x01EE7E0D, 0x01EE7F00, 0x01EE800D, 0x01EE8A00, 0x01EE8B0D, 0x01EE9C00, 0x01EEA10D,
        0x01EEA400, 0x01EEA50D, 0x01EEAA00, 0x01EEAB0D, 0x01EEBC00, 0x01EEF00D, 0x01EEF200,
        0x01F0000A, 0x01F02C00, 0x01F0300A, 0x01F09400, 0x01F0A00C, 0x01F0AF00, 0x01F0B10C,
        0x01F0BF10, 0x01F0C000, 0x01F0C10C, 0x01F0D000, 0x01F0D10C, 0x01F0E010, 0x01F0F600,
        0x01F1000B, 0x01F10B10, 0x01F10D17, 0x01F1100B, 0x01F12F14, 0x01F1300C, 0x01F1310B,
        0x01F1320C, 0x01F13D0B, 0x01F13E0C, 0x01F13F0B, 0x01F1400C, 0x01F1420B, 0x01F1430C,
        0x01F1460B, 0x01F1470C, 0x01F14A0B, 0x01F14F0C, 0x01F1570B, 0x01F1580C, 0x01F15F0B,
        0x01F1600C, 0x01F16A0D, 0x01F16C15, 0x01F16D17, 0x01F1700C, 0x01F1790B, 0x01F17A0C,
        0x01F17B0B, 0x01F17D0C, 0x01F17F0B, 0x01F1800C, 0x01F18A0B, 0x01F18E0C, 0x01F1900B,
        0x01F1910C, 0x01F19B12, 0x01F1AD17, 0x01F1AE00, 0x01F1E60C, 0x01F2000B, 0x01F2010C,
        0x01F20300, 0x01F2100B, 0x01F2320C, 0x01F23B12, 0x01F23C00, 0x01F2400B, 0x01F24900,
        0x01F2500C, 0x01F25200, 0x01F26013, 0x01F26600, 0x01F3000C, 0x01F32110, 0x01F32D11,
        0x01F3300C, 0x01F33610, 0x01F3370C, 0x01F37D10, 0x01F37E11, 0x01F3800C, 0x01F39410,
        0x01F3A00C, 0x01F3C510, 0x01F3C60C, 0x01F3CB10, 0x01F3CF11, 0x01F3D410, 0x01F3E00C,
        0x01F3F110, 0x01F3F811, 0x01F4000C, 0x01F43F10, 0x01F4400C, 0x01F44110, 0x01F4420C,
        0x01F4F810, 0x01F4F90C, 0x01F4FD10, 0x01F4FF11, 0x01F5000C, 0x01F53E10, 0x01F5400D,
        0x01F54410, 0x01F54B11, 0x01F5500C, 0x01F56810, 0x01F57A12, 0x01F57B10, 0x01F5A412,
        0x01F5A510, 0x01F5FB0C, 0x01F6000D, 0x01F6010C, 0x01F6110D, 0x01F6120C, 0x01F6150D,
        0x01F6160C, 0x01F6170D, 0x01F6180C, 0x01F6190D, 0x01F61A0C, 0x01F61B0D, 0x01F61C0C,
        0x01F61F0D, 0x01F6200C, 0x01F6260D, 0x01F6280C, 0x01F62C0D, 0x01F62D0C, 0x01F62E0D,
        0x01F6300C, 0x01F6340D, 0x01F6350C, 0x01F64110, 0x01F64311, 0x01F6450C, 0x01F65010,
        0x01F6800C, 0x01F6C610, 0x01F6D011, 0x01F6D112, 0x01F6D313, 0x01F6D515, 0x01F6D617,
        0x01F6D800, 0x01F6DD18, 0x01F6E010, 0x01F6ED00, 0x01F6F010, 0x01F6F412, 0x01F6F713,
        0x01F6F914, 0x01F6FA15, 0x01F6FB17, 0x01F6FD00, 0x01F7000C, 0x01F77400, 0x01F78010,
        0x01F7D514, 0x01F7D900, 0x01F7E015, 0x01F7EC00, 0x01F7F018, 0x01F7F100, 0x01F80010,
        0x01F80C00, 0x01F81010, 0x01F84800, 0x01F85010, 0x01F85A00, 0x01F86010, 0x01F88800,
        0x01F89010, 0x01F8AE00, 0x01F8B017, 0x01F8B200, 0x01F90013, 0x01F90C17, 0x01F90D15,
        0x01F91011, 0x01F91912, 0x01F91F13, 0x01F92012, 0x01F92813, 0x01F93012, 0x01F93113,
        0x01F93312, 0x01F93F15, 0x01F94012, 0x01F94C13, 0x01F94D14, 0x01F95012, 0x01F95F13,
        0x01F96C14, 0x01F97115, 0x01F97217, 0x01F97314, 0x01F97717, 0x01F97918, 0x01F97A14,
        0x01F97B15, 0x01F97C14, 0x01F98011, 0x01F98512, 0x01F99213, 0x01F99814, 0x01F9A317,
        0x01F9A515, 0x01F9AB17, 0x01F9AE15, 0x01F9B014, 0x01F9BA15, 0x01F9C011, 0x01F9C114,
        0x01F9C315, 0x01F9CB17, 0x01F9CC18, 0x01F9CD15, 0x01F9D013, 0x01F9E714, 0x01FA0015,
        0x01FA5400, 0x01FA6014, 0x01FA6E00, 0x01FA7015, 0x01FA7417, 0x01FA7500, 0x01FA7815,
        0x01FA7B18, 0x01FA7D00, 0x01FA8015, 0x01FA8317, 0x01FA8700, 0x01FA9015, 0x01FA9617,
        0x01FAA918, 0x01FAAD00, 0x01FAB017, 0x01FAB718, 0x01FABB00, 0x01FAC017, 0x01FAC318,
        0x01FAC600, 0x01FAD017, 0x01FAD718, 0x01FADA00, 0x01FAE018, 0x01FAE800, 0x01FAF018,
        0x01FAF700, 0x01FB0017, 0x01FB9300, 0x01FB9417, 0x01FBCB00, 0x01FBF017, 0x01FBFA00,
        0x02000005, 0x02A6D717, 0x02A6DE18, 0x02A6E000, 0x02A7000B, 0x02B73518, 0x02B73900,
        0x02B7400C, 0x02B81E00, 0x02B82011, 0x02CEA200, 0x02CEB013, 0x02EBE100, 0x02F80005,
        0x02FA1E00, 0x03000017, 0x03134B00, 0x0E000105, 0x0E000200, 0x0E002005, 0x0E008000,
        0x0E010007, 0x0E01F000, 0x0F000002, 0x0FFFFE00, 0x10000002, 0x10FFFE00, 0xFFFFFFFF};
    static constexpr string_with_idx categories_names[] = {
        string_with_idx{"c", 0},
        string_with_idx{"cased_letter", 7},
        string_with_idx{"cc", 1},
        string_with_idx{"cf", 2},
        string_with_idx{"close_punctuation", 24},
        string_with_idx{"cn", 3},
        string_with_idx{"co", 4},
        string_with_idx{"connector_punctuation", 22},
        string_with_idx{"control", 1},
        string_with_idx{"cs", 5},
        string_with_idx{"currency_symbol", 30},
        string_with_idx{"dash_punctuation", 23},
        string_with_idx{"decimal_number", 18},
        string_with_idx{"enclosing_mark", 15},
        string_with_idx{"final_punctuation", 25},
        string_with_idx{"format", 2},
        string_with_idx{"initial_punctuation", 26},
        string_with_idx{"l", 6},
        string_with_idx{"lc", 7},
        string_with_idx{"letter", 6},
        string_with_idx{"letter_number", 19},
        string_with_idx{"line_separator", 35},
        string_with_idx{"ll", 8},
        string_with_idx{"lm", 9},
        string_with_idx{"lo", 10},
        string_with_idx{"lowercase_letter", 8},
        string_with_idx{"lt", 11},
        string_with_idx{"lu", 12},
        string_with_idx{"m", 13},
        string_with_idx{"mark", 13},
        string_with_idx{"math_symbol", 32},
        string_with_idx{"mc", 14},
        string_with_idx{"me", 15},
        string_with_idx{"mn", 16},
        string_with_idx{"modifier_letter", 9},
        string_with_idx{"modifier_symbol", 31},
        string_with_idx{"n", 17},
        string_with_idx{"nd", 18},
        string_with_idx{"nl", 19},
        string_with_idx{"no", 20},
        string_with_idx{"nonspacing_mark", 16},
        string_with_idx{"number", 17},
        string_with_idx{"open_punctuation", 28},
        string_with_idx{"other", 0},
        string_with_idx{"other_letter", 10},
        string_with_idx{"other_number", 20},
        string_with_idx{"other_punctuation", 27},
        string_with_idx{"other_symbol", 33},
        string_with_idx{"p", 21},
        string_with_idx{"paragraph_separator", 36},
        string_with_idx{"pc", 22},
        string_with_idx{"pd", 23},
        string_with_idx{"pe", 24},
        string_with_idx{"pf", 25},
        string_with_idx{"pi", 26},
        string_with_idx{"po", 27},
        string_with_idx{"private_use", 4},
        string_with_idx{"ps", 28},
        string_with_idx{"punctuation", 21},
        string_with_idx{"s", 29},
        string_with_idx{"sc", 30},
        string_with_idx{"separator", 34},
        string_with_idx{"sk", 31},
        string_with_idx{"sm", 32},
        string_with_idx{"so", 33},
        string_with_idx{"space_separator", 37},
        string_with_idx{"spacing_mark", 14},
        string_with_idx{"surrogate", 5},
        string_with_idx{"symbol", 29},
        string_with_idx{"titlecase_letter", 11},
        string_with_idx{"unassigned", 3},
        string_with_idx{"uppercase_letter", 12},
        string_with_idx{"z", 34},
        string_with_idx{"zl", 35},
        string_with_idx{"zp", 36},
        string_with_idx{"zs", 37}};
    static constexpr range_array cat_cc = {0x00000001, 0x00002000, 0x00007F01, 0x0000A000};
    static constexpr range_array cat_zs = {0x00000000, 0x00002001, 0x00002100, 0x0000A001,
                                           0x0000A100, 0x00168001, 0x00168100, 0x00200001,
                                           0x00200B00, 0x00202F01, 0x00203000, 0x00205F01,
                                           0x00206000, 0x00300001, 0x00300100};
    static constexpr bool_trie<32, 991, 1, 0, 51, 255, 1, 0, 482, 4, 26, 43> cat_po{
        {0x8c00d4ee00000000, 0x0000000010000001, 0x80c0008200000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x4000000000000000, 0x0000000000000080, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x00000000fc000000, 0x0000000000000200, 0x0018000000000049,
         0x00000000e8003600, 0x00003c0000000000, 0x0000000000000000, 0x0000000000100000,
         0x0000000000003fff, 0x0000000000000000, 0x0000000000000000, 0x0380000000000000},
        {1,  2,  2,  2,  3,  2,  4,  2,  5,  2, 6,  2,  2, 2, 2,  2,  7,  8, 2,  2,  2,  2, 9,  2,
         10, 2,  2,  11, 2,  12, 13, 2,  14, 2, 15, 2,  2, 2, 2,  2,  2,  2, 2,  2,  16, 2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  17, 2, 18, 19, 2, 2, 20, 21, 2,  2, 2,  2,  22, 2, 2,  23,
         2,  24, 2,  2,  25, 2,  26, 27, 28, 2, 29, 2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  30,
         31, 2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  32, 2,  6,  2,  2,  33, 34, 2, 2,  2,  2, 2, 2,  35, 2,  2, 15, 2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  28, 2,  2,  2,  2,  36, 37, 2, 38, 2,  2, 2, 2,  2,  39, 2, 40, 41, 42, 2, 43, 2,
         44, 2,  45, 2,  2,  2,  46, 2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 2,  2,  2, 2, 2,  2,  2,  2, 2,  2,  2,  2, 2,  47,
         48, 2,  2,  49, 50, 2,  2},
        {0x7fff000000000000, 0x0000000040000000, 0x0000000000000000, 0x0001003000000000,
         0x2000000000000000, 0x0040000000000000, 0x0001000000000000, 0x0080000000000000,
         0x0000000000000010, 0x0010000000000000, 0x000000000c008000, 0x000000000017fff0,
         0x0000000000000020, 0x00000000061f0000, 0x000000000000fc00, 0x0800000000000000,
         0x000001ff00000000, 0x0000400000000000, 0x0000380000000000, 0x0060000000000000,
         0x0000000007700000, 0x00000000000007bf, 0x0000000000000030, 0x00000000c0000000,
         0x00003f7f00000000, 0x60000001fc000000, 0xf000000000000000, 0xf800000000000000,
         0xc000000000000000, 0x00000000000800ff, 0x79ff00ff00c00000, 0x000000007febff8e,
         0xde00000000000000, 0xf3ff7c00cb7fc9c3, 0x00000000001cfffa, 0x200000000000000e,
         0x000000000000e000, 0x4008000000000000, 0x00fc000000000000, 0x00f0000000000000,
         0x170000000000c000, 0x0000c00000000000, 0x0000000080000000, 0x00000000c0003ffe,
         0x00000000f0000000, 0x00030000c0000000, 0x0000080000000000, 0x00010000027f0000,
         0x00000d0380f71e60, 0x100000018c00d4ee, 0x0000003200000000},
        {1, 2, 3, 3, 3, 4, 3, 3, 3, 3, 5, 3, 6, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
         3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
         3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
         3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
         3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
         3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
         3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
         3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
         3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
        {1,  0,  0, 0,  0,  0,  0, 0,  0,  0,  2,  3,  0,  0, 0,  0,  0,  4,  0, 0,  0,  0,  0,
         0,  0,  0, 0,  0,  0,  5, 0,  0,  6,  0,  0,  0,  0, 7,  0,  8,  9,  0, 10, 0,  0,  0,
         0,  0,  0, 0,  0,  0,  0, 0,  0,  0,  0,  11, 12, 0, 0,  13, 14, 15, 0, 16, 0,  17, 18,
         0,  19, 0, 0,  0,  0,  0, 0,  20, 0,  21, 0,  0,  0, 22, 0,  23, 24, 0, 25, 0,  0,  0,
         26, 0,  0, 0,  0,  27, 0, 28, 29, 30, 31, 0,  0,  0, 0,  0,  0,  32, 0, 0,  0,  0,  0,
         0,  0,  0, 0,  33, 0,  0, 0,  29, 0,  0,  0,  0,  0, 0,  0,  0,  0,  0, 0,  0,  0,  0,
         0,  0,  0, 34, 0,  0,  0, 0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0, 0,  0,  0,  0,
         0,  0,  0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0, 0,  0,  0,  0,
         0,  0,  0, 35, 0,  0,  0, 0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0, 0,  0,  0,  0,
         0,  0,  0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0, 0,  0,  0,  0,
         0,  0,  0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0, 0,  0,  0,  0,
         0,  0,  0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0, 0,  0,  0,  0,
         0,  0,  0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  36, 0, 37, 38, 39, 0,
         0,  0,  0, 0,  0,  0,  0, 0,  0,  0,  0,  40, 0,  0, 0,  0,  28, 0,  0, 0,  0,  0,  0,
         0,  0,  0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0, 0,  0,  0,  0,
         0,  0,  0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0, 0,  0,  2,  0,
         0,  0,  0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0, 0,  0,  0,  0,
         0,  0,  0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0, 0,  0,  0,  0,
         0,  0,  0, 0,  0,  0,  0, 0,  41, 0,  0,  0,  0,  0, 0,  0,  0,  0,  0, 0,  0,  0,  0,
         0,  0,  0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0, 0,  0,  0,  0,
         0,  0,  0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0, 0,  0,  42},
        {0x0000000000000000, 0x0000000000000007, 0x0000000080000000, 0x0000000000010000,
         0x0000800000000000, 0x0000000000800000, 0x8000000080000000, 0x8000000001ff0000,
         0x007f000000000000, 0xfe00000000000000, 0x000000001e000000, 0x0000000003e00000,
         0x00000000000003c0, 0x0000000000003f80, 0xd800000000000000, 0x0000000000000003,
         0x003000000000000f, 0x00000000e80021e0, 0x3f00000000000000, 0x0000020000000000,
         0x000000002c00f800, 0x0000000000000040, 0x0000000000fffffe, 0x00001fff0000000e,
         0x0200000000000000, 0x7000000000000000, 0x0800000000000000, 0x0000000000000070,
         0x0000000400000000, 0x8000000000000000, 0x000000000000007f, 0x00000007dc000000,
         0x000300000000003e, 0x0180000000000000, 0x001f000000000000, 0x0006000000000000,
         0x0000c00000000000, 0x0020000000000000, 0x0f80000000000000, 0x0000000000000010,
         0x0000000007800000, 0x0000000000000f80, 0x00000000c0000000}};
    static constexpr range_array cat_sc = {
        0x00000000, 0x00002401, 0x00002500, 0x0000A201, 0x0000A600, 0x00058F01, 0x00059000,
        0x00060B01, 0x00060C00, 0x0007FE01, 0x00080000, 0x0009F201, 0x0009F400, 0x0009FB01,
        0x0009FC00, 0x000AF101, 0x000AF200, 0x000BF901, 0x000BFA00, 0x000E3F01, 0x000E4000,
        0x0017DB01, 0x0017DC00, 0x0020A001, 0x0020C100, 0x00A83801, 0x00A83900, 0x00FDFC01,
        0x00FDFD00, 0x00FE6901, 0x00FE6A00, 0x00FF0401, 0x00FF0500, 0x00FFE001, 0x00FFE200,
        0x00FFE501, 0x00FFE700, 0x011FDD01, 0x011FE100, 0x01E2FF01, 0x01E30000, 0x01ECB001,
        0x01ECB100};
    static constexpr bool_trie<32, 962, 28, 2, 19, 0, 0, 0, 0, 0, 0, 0> cat_ps{
        {0x0000010000000000, 0x0800000008000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000},
        {1,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 2, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 3,  4,  5, 0, 0, 0, 0, 0, 0,  0, 0, 0,  6, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  7,  0, 8, 0, 0, 0, 0, 0,  0, 9, 10, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 11, 12, 0, 0, 0, 0, 0, 0, 13, 0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0,
         14, 0, 0, 0, 15, 16, 0, 0, 17, 18},
        {0x0000000000000000, 0x1400000000000000, 0x0000000008000000, 0x0000000044000000,
         0x2000000000000020, 0x0000000000002000, 0x0000020000000500, 0x0015550000000000,
         0x0000554000000020, 0x0000000000aaaaa8, 0x1000000005000000, 0x0000015400000000,
         0x000000000aa00004, 0x0000000025515500, 0x8000000000000000, 0xaaa0000000800000,
         0x000000002a00008a, 0x0800000000000100, 0x0000000488000000},
        {},
        {},
        {}};
    static constexpr bool_trie<32, 962, 28, 2, 18, 0, 0, 0, 0, 0, 0, 0> cat_pe{
        {0x0000020000000000, 0x2000000020000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000},
        {1,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 2, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  3,  4, 0, 0, 0, 0, 0, 0,  0, 0, 0, 5, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  6,  0, 7, 0, 0, 0, 0, 0,  0, 8, 9, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 10, 11, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         0,  0, 0, 0, 0,  0,  0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,
         13, 0, 0, 0, 14, 15, 0, 0, 16, 17},
        {0x0000000000000000, 0x2800000000000000, 0x0000000010000000, 0x4000000000000040,
         0x0000000000004000, 0x0000040000000a00, 0x002aaa0000000000, 0x0000aa8000000040,
         0x0000000001555550, 0x200000000a000000, 0x000002a800000000, 0x0000000015400000,
         0x00000000caa2aa00, 0x4000000000000000, 0x5540000001000000, 0x0000000054000115,
         0x2000000000000200, 0x0000000920000000},
        {},
        {},
        {}};
    static constexpr bool_trie<32, 895, 97, 0, 25, 2, 13, 241, 97, 91, 4, 7> cat_sm{
        {0x7000080000000000, 0x5000000000000000, 0x0002100000000000, 0x0080000000800000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0040000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x00000000000001c0, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000},
        {1, 2, 0, 3, 4,  5, 6, 7,  7,  7, 7, 8,  9,  10, 11, 0, 0, 0,  0,  0, 0, 12, 13, 0, 14, 0,
         0, 0, 0, 0, 15, 0, 0, 0,  0,  7, 7, 16, 17, 7,  7,  7, 7, 18, 19, 0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,  0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 0,  0, 0, 0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 20, 0,  0, 0, 0,  0,  0, 0,  0,
         0, 0, 0, 0, 21, 0, 0, 22, 23, 0, 24},
        {0x0000000000000000, 0x1c00000000040010, 0x0000000000001c00, 0x0000000001000000,
         0x000000000000081f, 0x000040490c1f0000, 0xfff000000014c000, 0xffffffffffffffff,
         0x0000000300000000, 0x1000000000000000, 0x000ffffff8000000, 0x00000003f0000000,
         0x0080000000000000, 0xff00000000000002, 0x0000800000000000, 0xffff003fffffff9f,
         0xfffffffffe000007, 0xcffffffff0ffffff, 0xffff000000000000, 0x0000000000001f9f,
         0x0000020000000000, 0x0000007400000000, 0x0000000070000800, 0x0000000050000000,
         0x00001e0400000000},
        {1, 2},
        {1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6},
        {0x0000000000000000, 0x0800000008000002, 0x0020000000200000, 0x0000800000008000,
         0x0000020000000200, 0x0000000000000008, 0x0003000000000000}};
    static constexpr flat_array<26> cat_pd{{0x1400, 0x1806,  0x058A, 0xFF0D, 0x2010, 0x2011, 0x2012,
                                            0x2013, 0x2014,  0x2015, 0x2E17, 0x2E1A, 0x301C, 0x30A0,
                                            0x002D, 0x10EAD, 0x3030, 0xFE31, 0xFE32, 0x2E3A, 0x2E3B,
                                            0x05BE, 0x2E40,  0xFE58, 0x2E5D, 0xFE63}};
    static constexpr bool_trie<32, 984, 5, 3, 9, 255, 1, 0, 414, 18, 16, 8> cat_nd{
        {0x03ff000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x000003ff00000000, 0x0000000000000000, 0x03ff000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x00000000000003ff},
        {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 0, 2, 3, 0, 0, 0, 0, 4, 2,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2,
         0, 0, 0, 0, 5, 0, 2, 0, 0, 6, 0, 0, 2, 7, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 2, 4, 0, 0, 8, 0, 2, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
        {0x0000000000000000, 0x0000ffc000000000, 0x0000000003ff0000, 0x000003ff00000000,
         0x00000000000003ff, 0x000000000000ffc0, 0x0000000003ff03ff, 0x03ff000000000000,
         0x03ff000003ff0000},
        {1, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 4, 5, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 4, 0, 0, 5, 0, 0, 0, 2, 0, 0,
         0, 0, 0, 5, 0, 5, 0, 0, 0, 0, 0, 5, 0, 6, 2, 0, 0, 0, 0, 0, 0, 1, 0, 5, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 5, 0, 0, 0, 5, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 1, 0, 6, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
        {0x0000000000000000, 0x000003ff00000000, 0x03ff000000000000, 0x0000ffc000000000,
         0xffc0000000000000, 0x0000000003ff0000, 0x00000000000003ff, 0xffffffffffffc000}};
    static constexpr bool_trie<32, 955, 34, 3, 24, 255, 1, 0, 341, 16, 27, 26> cat_lu{
        {0x0000000000000000, 0x0000000007fffffe, 0x0000000000000000, 0x000000007f7fffff,
         0xaa55555555555555, 0x2b555555555554aa, 0x11aed2d5b1dbced6, 0x55d255554aaaa490,
         0x6c05555555555555, 0x000000000000557a, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x8045000000000000, 0x00000ffbfffed740, 0xe6905555551c8000,
         0x0000ffffffffffff, 0x5555555500000000, 0x5555555555555401, 0x5555555555552aab,
         0xfffe555555555555, 0x00000000007fffff, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000},
        {1, 2,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 1, 3, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 4, 0, 0, 0,
         0, 0,  5,  5, 6,  5,  7,  8,  9, 10, 0, 0, 0, 0, 11, 12, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  14, 15, 5, 16, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 17, 18, 0, 19, 20, 21, 22, 0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0,  0,  0, 0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,  0,  0,  0, 23},
        {0x0000000000000000, 0xffffffff00000000, 0x00000000000020bf, 0x003fffffffffffff,
         0xe7ffffffffff0000, 0x5555555555555555, 0x5555555540155555, 0xff00ff003f00ff00,
         0x0000ff00aa003f00, 0x0f00000000000000, 0x0f001f000f000f00, 0xc00f3d503e273884,
         0x0000000000000020, 0x0000000000000008, 0x0000ffffffffffff, 0xc025ea9d00000000,
         0x0004280555555555, 0x0000155555555555, 0x0000000005555555, 0x5554555400000000,
         0x6a00555555555555, 0x555f7d5555452855, 0x00200000014102f5, 0x07fffffe00000000},
        {1, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 4, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
        {1, 0, 2, 3, 0, 4, 5, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0,  0,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         0, 0, 0, 0, 0, 0, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  25},
        {0x0000000000000000, 0x000000ffffffffff, 0xffff000000000000, 0x00000000000fffff,
         0xf7ff000000000000, 0x000000000037f7ff, 0x0007ffffffffffff, 0xffffffff00000000,
         0x00000000ffffffff, 0xfff0000003ffffff, 0xffffff0000003fff, 0x003fde64d0000003,
         0x000003ffffff0000, 0x7b0000001fdfe7b0, 0xfffff0000001fc5f, 0x03ffffff0000003f,
         0x00003ffffff00000, 0xf0000003ffffff00, 0xffff0000003fffff, 0xffffff00000003ff,
         0x07fffffc00000001, 0x001ffffff0000000, 0x00007fffffc00000, 0x000001ffffff0000,
         0x0000000000000400, 0x00000003ffffffff}};
    static constexpr bool_trie<32, 990, 2, 0, 13, 1, 15, 240, 1, 79, 48, 2> cat_sk{
        {0x0000000000000000, 0x0000000140000000, 0x0110810000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0xffffafe0fffc003c,
         0x0000000000000000, 0x0020000000000000, 0x0000000000000030, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000},
        {1,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 6, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         10, 11, 0, 12},
        {0x0000000000000000, 0x0000000000000100, 0xa000000000000000, 0x6000e000e000e003,
         0x0000000018000000, 0x00000003007fffff, 0x0000000000000600, 0x00000c0008000000,
         0xfffc000000000000, 0x0000000000000007, 0x4000000000000000, 0x0000000000000001,
         0x0000000800000000},
        {1},
        {1},
        {0x0000000000000000, 0xf800000000000000}};
    static constexpr flat_array<10> cat_pc{
        {0x2040, 0xFE4D, 0xFE4E, 0xFE4F, 0xFE33, 0x2054, 0xFE34, 0xFF3F, 0x005F, 0x203F}};
    static constexpr bool_trie<32, 955, 35, 2, 30, 255, 1, 0, 342, 16, 26, 27> cat_ll{
        {0x0000000000000000, 0x07fffffe00000000, 0x0020000000000000, 0xff7fffff80000000,
         0x55aaaaaaaaaaaaaa, 0xd4aaaaaaaaaaab55, 0xe6512d2a4e243129, 0xaa29aaaab5555240,
         0x93faaaaaaaaaaaaa, 0xffffffffffffaa85, 0x0000ffffffefffff, 0x0000000000000000,
         0x0000000000000000, 0x388a000000000000, 0xfffff00000010000, 0x192faaaaaae37fff,
         0xffff000000000000, 0xaaaaaaaaffffffff, 0xaaaaaaaaaaaaa802, 0xaaaaaaaaaaaad554,
         0x0000aaaaaaaaaaaa, 0xffffffff00000000, 0x00000000000001ff, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000},
        {1, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 2,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 3, 0,  4,
         5, 6,  0,  7,  7,  8, 7, 9, 10, 11, 12, 0, 0,  0,  0, 13, 14, 15, 0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  16, 17, 7, 18, 19, 0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 20, 21,
         0, 22, 23, 24, 25, 0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  16, 26, 27, 0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0,  0, 28, 0,  0, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0,
         0, 0,  0,  0,  29},
        {0x0000000000000000, 0xe7ffffffffff0000, 0x3f00000000000000, 0x00000000000001ff,
         0x00000fffffffffff, 0xfefff80000000000, 0x0000000007ffffff, 0xaaaaaaaaaaaaaaaa,
         0xaaaaaaaabfeaaaaa, 0x00ff00ff003f00ff, 0x3fff00ff00ff003f, 0x40df00ff00ff00ff,
         0x00dc00ff00cf00dc, 0x321080000008c400, 0x00000000000043c0, 0x0000000000000010,
         0xffff000000000000, 0x0fda1562ffffffff, 0x0008501aaaaaaaaa, 0x000020bfffffffff,
         0x00002aaaaaaaaaaa, 0x000000000aaaaaaa, 0xaaabaaa800000000, 0x95feaaaaaaaaaaaa,
         0xaaa082aaaaba50aa, 0x0440000002aa050a, 0xffff01ff07ffffff, 0xffffffffffffffff,
         0x0000000000f8007f, 0x0000000007fffffe},
        {1, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 4, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
        {1,  2, 0, 3, 0, 0, 4, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         0,  0, 0, 0, 0, 0, 0, 0, 6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         0,  0, 0, 0, 0, 0, 0, 0, 7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         0,  0, 0, 0, 0, 0, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 0, 0, 0,
         0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         24, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,
         0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  25, 26},
        {0x0000000000000000, 0xffffff0000000000, 0x000000000000ffff, 0x0fffffffff000000,
         0x1bfbfffbff800000, 0x0007ffffffffffff, 0x00000000ffffffff, 0xffffffff00000000,
         0x000ffffffc000000, 0x000000ffffdfc000, 0xebc000000ffffffc, 0xfffffc000000ffef,
         0x00ffffffc000000f, 0x00000ffffffc0000, 0xfc000000ffffffc0, 0xffffc000000fffff,
         0x0ffffffc000000ff, 0x0000ffffffc00000, 0x0000003ffffffc00, 0xf0000003f7fffffc,
         0xffc000000fdfffff, 0xffff0000003f7fff, 0xfffffc000000fdff, 0x0000000000000bf7,
         0x000000007ffffbff, 0xfffffffc00000000, 0x000000000000000f}};
    static constexpr bool_trie<32, 985, 7, 0, 54, 255, 1, 0, 556, 4, 16, 48> cat_so{
        {0x0000000000000000, 0x0000000000000000, 0x0001424000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000004, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000006000, 0x0000000000000000,
         0x000000000000c000, 0x0000000000000000, 0x0000000000000000, 0x6000020040000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0040000000000000},
        {1,  0,  0,  0,  0,  0,  2,  0,  3,  0,  4,  0,  0,  0,  5,  0,  0,  0,  0,  0,  0,  6,  0,
         7,  8,  0,  0,  9,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  10, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  11, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  12, 0,  13, 0,  0,  0,  0,
         0,  14, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  15, 16, 17, 18, 0,  0,  0,  0,  19, 20, 21, 22, 23, 24, 25, 26, 27, 27, 28, 29, 27, 30,
         27, 27, 27, 31, 32, 0,  27, 27, 27, 27, 0,  0,  0,  0,  0,  0,  0,  0,  33, 34, 35, 27, 0,
         0,  0,  36, 0,  0,  0,  0,  0,  37, 38, 39, 27, 27, 27, 40, 41, 0,  0,  0,  0,  0,  42, 43,
         44, 45, 46, 27, 27, 27, 27, 27, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  27, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  47, 48,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  49, 0,  0,  0,  0,  0,  0,  0,  0,  50, 0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  51, 0,  52, 0,  0,  0,  0,  0,  0,  0,  53},
        {0x0000000000000000, 0x0400000000000000, 0x0001000000000000, 0x05f8000000000000,
         0x8000000000000000, 0x0200000000008000, 0x01500000fce8000e, 0xc000000000000000,
         0x0000000001e0dfbf, 0x00000000c0000000, 0x0000000003ff0000, 0x0000200000000000,
         0x0000000000000001, 0xffffffffc0000000, 0x1ff007fe00000000, 0x0c0042afc0d0037b,
         0x000000000000b400, 0xffffbfb6f3e00c00, 0x000fffffffeb3fff, 0xfffff9fcfffff0ff,
         0xefffffffffffffff, 0xfff0000007ffffff, 0xfffffffc0fffffff, 0x0000007fffffffff,
         0x00000000000007ff, 0xfffffffff0000000, 0x000003ffffffffff, 0xffffffffffffffff,
         0xff7fffffffffffff, 0x00fffffffffffffd, 0xffff7fffffffffff, 0x000000ffffffffff,
         0xfffffffffff00000, 0x0000ffffffffffff, 0xffcfffffffffe060, 0xffffffffffbfffff,
         0x000007e000000000, 0x0000000000030000, 0xfffffffffbffffff, 0x000fffffffffffff,
         0x0fff0000003fffff, 0xc0c00001000c0010, 0x00000000ffc30000, 0x0000000fffffffff,
         0xfffffc007fffffff, 0xffffffff000100ff, 0x0001fffffffffc00, 0xffffffffffff0000,
         0x000000000000007f, 0x02c00f0000000000, 0x0380000000000000, 0x000000000000ffff,
         0xe000000000008000, 0x3000611000000000},
        {1, 2, 2, 2, 2, 3, 2, 2, 2, 2, 4, 5, 6, 7, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
        {1,  2,  3,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  6,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  9,  10, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  11, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  12, 13, 14, 13, 13, 13, 15, 16, 17, 18, 19, 13, 20, 0,
         0,  13, 21, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  13, 13,
         13, 13, 13, 13, 13, 13, 22, 23, 24, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  25, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  26, 0,  27, 0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  28, 13, 29, 30, 31, 13, 32, 33, 34, 35, 0,  0,  13, 13, 13, 36, 13, 13, 13, 13, 13,
         13, 13, 13, 13, 13, 13, 37, 13, 38, 13, 39, 40, 41, 42, 0,  13, 13, 13, 13, 13, 43, 44, 45,
         13, 13, 46, 47},
        {0x0000000000000000, 0xff80000000000000, 0xfe00000000000000, 0x000000011fff73ff,
         0x1fffffffffff0000, 0x0180000000000000, 0x0000000000000100, 0x8000000000000000,
         0x0003fffe1fe00000, 0xf000000000000000, 0x0000000000000020, 0x0000000010000000,
         0xffffffffffff0000, 0xffffffffffffffff, 0x000000000000000f, 0x003fffffffffffff,
         0xfffffe7fffffffff, 0x00001c1fffffffff, 0xffffc3fffffff018, 0x000007ffffffffff,
         0x0000000000000023, 0x00000000007fffff, 0x0780000000000000, 0xffdfe00000000000,
         0x000000000000006f, 0x0000000000008000, 0x0000100000000000, 0x0000400000000000,
         0xffff0fffffffffff, 0xfffe7fff000fffff, 0x003ffffffffefffe, 0xffffffffffffe000,
         0x00003fffffffffff, 0xffffffc000000000, 0x0fffffffffff0007, 0x0000003f000301ff,
         0x07ffffffffffffff, 0x1fff1fffe0ffffff, 0x000fffffffffffff, 0x00010fff01ffffff,
         0xffffffffffff0fff, 0xffffffff03ff00ff, 0x00033fffffff00ff, 0x1f1f3fff000fffff,
         0x07ff1fffffff007f, 0x007f00ff03ff003f, 0xfffffffffff7ffff, 0x00000000000007ff}};
    static constexpr bool_trie<32, 991, 1, 0, 114, 255, 1, 0, 1151, 1, 0, 103> cat_lo{
        {0x0000000000000000, 0x0000000000000000, 0x0400040000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0800000000000000, 0x000000000000000f,
         0x0000000000000000, 0x0000000000000000, 0x0000000000100000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x000787ffffff0000,
         0xffffffff00000000, 0xfffec000000007fe, 0xffffffffffffffff, 0x9c00c000002fffff,
         0x0000fffffffd0000, 0xffffffffffffe000, 0x0002003fffffffff, 0x000007fffffffc00},
        {1,   2,   3,   4,   5,   6,   7,  8,  9,  10, 11,  12,  13,  14,  15, 16,  17, 18,  19,
         20,  21,  22,  23,  24,  25,  26, 27, 28, 29, 30,  31,  32,  33,  34, 31,  35, 35,  35,
         35,  35,  36,  37,  38,  39,  40, 41, 31, 42, 35,  35,  35,  35,  35, 35,  35, 35,  43,
         44,  45,  46,  47,  48,  49,  50, 51, 52, 53, 54,  55,  56,  57,  58, 59,  31, 31,  60,
         61,  62,  63,  64,  65,  31,  66, 31, 31, 31, 31,  31,  31,  31,  31, 31,  31, 31,  31,
         31,  31,  31,  31,  67,  31,  31, 31, 31, 31, 31,  31,  31,  31,  31, 31,  31, 31,  31,
         31,  31,  31,  31,  31,  31,  31, 31, 31, 31, 31,  31,  31,  31,  31, 31,  31, 31,  31,
         31,  31,  31,  31,  31,  31,  31, 31, 31, 31, 31,  31,  31,  31,  68, 69,  70, 71,  31,
         31,  31,  31,  31,  31,  31,  31, 72, 42, 73, 74,  75,  35,  76,  68, 31,  31, 31,  31,
         31,  31,  31,  31,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  31,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  77,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 78,  79,
         35,  35,  35,  35,  80,  81,  50, 63, 31, 31, 82,  83,  84,  48,  85, 86,  87, 88,  89,
         90,  91,  92,  93,  94,  95,  31, 31, 96, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  35,  35,  35, 35,  35, 35,  35,
         35,  35,  35,  35,  35,  35,  35, 35, 35, 35, 35,  35,  97,  98,  31, 31,  31, 31,  31,
         31,  31,  31,  31,  31,  31,  31, 31, 31, 31, 31,  31,  31,  31,  31, 31,  31, 31,  31,
         31,  31,  31,  31,  31,  31,  31, 31, 31, 31, 31,  31,  31,  31,  31, 31,  31, 31,  31,
         31,  31,  31,  31,  31,  31,  31, 31, 31, 31, 31,  31,  31,  31,  31, 31,  31, 31,  31,
         31,  31,  31,  31,  31,  31,  31, 31, 31, 31, 31,  31,  31,  31,  31, 31,  31, 31,  31,
         31,  31,  31,  31,  31,  31,  31, 31, 31, 31, 31,  31,  31,  31,  31, 31,  31, 31,  31,
         31,  31,  31,  31,  31,  31,  31, 31, 31, 31, 31,  31,  31,  31,  31, 31,  31, 31,  31,
         31,  31,  31,  31,  31,  31,  31, 31, 31, 31, 31,  31,  31,  35,  35, 35,  35, 35,  99,
         35,  100, 101, 102, 103, 104, 35, 35, 35, 35, 105, 106, 107, 108, 31, 109, 35, 110, 31,
         111, 112, 113},
        {0x00000000003fffff, 0xffff07ff01ffffff, 0xffffffff00007eff, 0x00000000000001ff,
         0x23fffffffffffff0, 0xfffc0003ff010000, 0x23c5fdfffff99fe1, 0x10030003b0004000,
         0x036dfdfffff987e0, 0x001c00005e000000, 0x23edfdfffffbbfe0, 0x0200000300010000,
         0x23edfdfffff99fe0, 0x00020003b0000000, 0x03ffc718d63dc7e8, 0x0000000000010000,
         0x23fffdfffffddfe0, 0x0000000327000000, 0x23effdfffffddfe1, 0x0006000360000000,
         0x27fffffffffddff0, 0xfc00000380704000, 0x2ffbfffffc7fffe0, 0x000000000000007f,
         0x000dfffffffffffe, 0x000000000000003f, 0x200dffaffffff7d6, 0x00000000f000001f,
         0x0000000000000001, 0x00001ffffffffeff, 0x0000000000001f00, 0x0000000000000000,
         0x800007ffffffffff, 0xffe1c0623c3f0000, 0x0000000000004003, 0xffffffffffffffff,
         0xffffffff3d7f3dff, 0x7f3dffffffff3dff, 0xffffffffff7fff3d, 0xffffffffff3dffff,
         0x0000000007ffffff, 0x000000000000ffff, 0xfffffffffffffffe, 0xffff9fffffffffff,
         0xffffffff07fffffe, 0x01fe07ffffffffff, 0x0003ffff8003ffff, 0x0001dfff0003ffff,
         0x000fffffffffffff, 0x0000000010000000, 0xffffffff00000000, 0x01fffffffffffff7,
         0xffff05ffffffff9f, 0x003fffffffffffff, 0x000000007fffffff, 0x001f3fffffff0000,
         0xffff0fffffffffff, 0x00000000000003ff, 0xffffffff007fffff, 0x00000000001fffff,
         0x000fffffffffffe0, 0x0000000000001fe0, 0xfc00c001fffffff8, 0x0000003fffffffff,
         0x0000000fffffffff, 0x00fffffffc00e000, 0x046fde0000000000, 0x01e0000000000000,
         0xffff000000000000, 0x000000ffffffffff, 0x7f7f7f7f007fffff, 0x000000007f7f7f7f,
         0x1000000000000040, 0xfffffffe807fffff, 0x87ffffffffffffff, 0xfffeffffffffffe0,
         0xffffffff00007fff, 0xffffffffffdfffff, 0x0000000000001fff, 0x00ffffffffff0000,
         0x00000c00ffff0fff, 0x0000400000000000, 0x0000000000008000, 0xf880000000000000,
         0x00000007fffff7bb, 0x000ffffffffffffc, 0x68fc000000000000, 0xffff003ffffffc00,
         0x1fffffff0000007f, 0x0007fffffffffff0, 0x7c00ff9f00000000, 0x000001ffffffffff,
         0xc47effff00000ff7, 0x3e62ffffffffffff, 0x000407ff18000005, 0x00007f7f007e7e7e,
         0x00000007ffffffff, 0xffff000fffffffff, 0x0ffffffffffff87f, 0xffff3fffffffffff,
         0x0000000003ffffff, 0x5f7ffdffa0000000, 0xffffffffffffffdb, 0x0003ffffffffffff,
         0xfffffffffff80000, 0x3fffffffffffffff, 0xffffffffffff0000, 0xfffffffffffcffff,
         0x0fff0000000000ff, 0xffdf000000000000, 0x1fffffffffffffff, 0xfffeffc000000000,
         0x7fffffff3fffffff, 0x000000001cfcfcfc},
        {1,  2,  3, 4,  5,  6, 7,  8, 5, 5, 9, 5, 10, 11, 5, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 12,
         13, 14, 7, 15, 16, 7, 17, 5, 5, 5, 5, 5, 5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5, 5, 5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5, 5, 5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5, 5, 5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5, 5, 5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5, 5, 5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5, 5, 5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5, 5, 5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5, 5, 5,  5,  5, 5, 5, 5, 5, 5, 5},
        {1,   2,  3,  4,  4,  4,  4,  4,   4,  5,  6,  7,  8,  9,  10, 4,  11,  12, 4,  13, 14, 4,
         4,   2,  2,  2,  2,  15, 16, 4,   4,  17, 18, 19, 20, 21, 4,  22, 4,   23, 24, 25, 26, 27,
         28,  29, 4,  2,  30, 4,  4,  14,  4,  4,  4,  4,  4,  31, 4,  32, 33,  34, 35, 36, 37, 38,
         39,  40, 41, 42, 43, 44, 4,  45,  19, 46, 47, 4,  4,  48, 49, 50, 51,  4,  4,  52, 53, 50,
         54,  55, 4,  56, 57, 4,  4,  58,  4,  4,  59, 60, 61, 62, 63, 64, 65,  66, 67, 4,  4,  4,
         4,   68, 69, 70, 4,  71, 72, 73,  4,  4,  4,  4,  74, 4,  4,  75, 4,   2,  2,  2,  2,  2,
         2,   2,  2,  2,  2,  2,  2,  2,   2,  76, 4,  4,  4,  2,  2,  2,  77,  4,  4,  4,  4,  4,
         4,   4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,
         4,   4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,  11, 78, 2,  2,   2,  2,  2,  2,  2,
         2,   2,  2,  2,  2,  2,  2,  2,   2,  52, 4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,
         4,   4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,
         4,   4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,
         4,   4,  4,  4,  4,  4,  4,  2,   2,  2,  2,  2,  2,  2,  2,  2,  57,  4,  4,  4,  4,  4,
         4,   4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,
         4,   4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,
         4,   4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,
         4,   4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,
         4,   4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,
         4,   4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,  4,  4,  4,  4,   4,  4,  2,  2,  2,
         2,   2,  2,  2,  2,  67, 79, 80,  81, 50, 82, 70, 4,  4,  4,  4,  4,   4,  4,  4,  4,  4,
         4,   4,  4,  2,  83, 4,  4,  2,   2,  2,  2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,
         2,   2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,
         2,   2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,
         2,   2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,
         2,   2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,  2,  84, 2,  2,   2,  2,  2,  2,  2,
         2,   2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  85, 30, 4,  4,  4,   4,  4,  4,  4,  4,
         4,   4,  4,  2,  2,  2,  2,  86,  87, 2,  2,  2,  2,  2,  88, 4,  4,   4,  4,  4,  4,  4,
         4,   4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,
         4,   4,  4,  4,  4,  4,  4,  2,   89, 90, 4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,
         4,   4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,
         4,   4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,
         4,   4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,  4,  4,  4,  4,   91, 4,  4,  4,  4,
         4,   4,  4,  92, 93, 4,  4,  4,   4,  81, 58, 4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,
         4,   4,  4,  4,  4,  4,  4,  4,   94, 2,  2,  2,  95, 4,  4,  4,  4,   4,  4,  4,  4,  4,
         4,   4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  96, 97, 98, 4,  4,  4,   4,  4,  2,  2,  2,
         2,   2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,
         2,   2,  99, 2,  2,  2,  2,  2,   2,  2,  2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,
         2,   2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,
         2,   2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,
         2,   67, 2,  2,  2,  9,  2,  2,   2,  2,  2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,
         2,   2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,
         2,   2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,
         2,   2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,
         2,   2,  2,  2,  2,  2,  2,  100, 2,  2,  2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,
         2,   2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,
         2,   2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,  2,  2,  2,  101, 4,  4,  4,  4,  4,
         4,   4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,
         4,   4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  2,
         2,   2,  2,  2,  2,  2,  2,  12,  4,  4,  4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,
         4,   4,  4,  4,  4,  4,  4,  4,   4,  2,  2,  2,  2,  2,  2,  2,  2,   2,  2,  2,  2,  2,
         102, 4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,
         4,   4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,  4,  4,  4,  4,   4,  4,  4,  4,  4,
         4,   4,  4,  4,  4,  4,  4},
        {0xb7ffff7fffffefff, 0x000000003fff3fff, 0xffffffffffffffff, 0x07ffffffffffffff,
         0x0000000000000000, 0xffffffff1fffffff, 0x000000000001ffff, 0xffffe000ffffffff,
         0x003fffffffff03fd, 0xffffffff3fffffff, 0x000000000000ff0f, 0xffffffffffff0000,
         0x000000003fffffff, 0xffff00ffffffffff, 0x0000000fffffffff, 0x007fffffffffffff,
         0x000000ff003fffff, 0x91bffffffffffd3f, 0x007fffff003fffff, 0x000000007fffffff,
         0x0037ffff00000000, 0x03ffffff003fffff, 0xc0ffffffffffffff, 0x003ffffffeef0001,
         0x1fffffff00000000, 0x000000001fffffff, 0x0000001ffffffeff, 0x003fffffffffffff,
         0x0007ffff003fffff, 0x000000000003ffff, 0x00000000000001ff, 0x000303ffffffffff,
         0xffff00801fffffff, 0xffff00000000003f, 0xffff000000000003, 0x007fffff0000001f,
         0x00fffffffffffff8, 0x0026000000000000, 0x0000fffffffffff8, 0x000001ffffff0000,
         0x0000007ffffffff8, 0x0047ffffffff0090, 0x0007fffffffffff8, 0x000000001400001e,
         0x00000ffffffbffff, 0xffff01ffbfffbd7f, 0x23edfdfffff99fe0, 0x00000003e0010000,
         0x001fffffffffffff, 0x0000000380000780, 0x0000ffffffffffff, 0x00000000000000b0,
         0x00007fffffffffff, 0x000000000f000000, 0x0000000000000010, 0x010007ffffffffff,
         0x0000000007ffffff, 0x000000000000007f, 0x00000fffffffffff, 0x8000000000000000,
         0x8000ffffff6ff27f, 0x0000000000000002, 0xfffffcff00000000, 0x0000000a0001ffff,
         0x0407fffffffff801, 0xfffffffff0010000, 0xffff0000200003ff, 0x01ffffffffffffff,
         0x00007ffffffffdff, 0xfffc000000000001, 0x000000000000ffff, 0x0001fffffffffb7f,
         0xfffffdbf00000040, 0x00000000010003ff, 0x0007ffff00000000, 0x0001000000000000,
         0x0000000003ffffff, 0x000000000000000f, 0x0001ffffffffffff, 0xffff00007fffffff,
         0x7fffffffffffffff, 0x00003fffffff0000, 0xe0fffff800000000, 0x00000000000107ff,
         0x00ffffffffffffff, 0x00000000003fffff, 0x00000007ffffffff, 0xffff00f000070000,
         0x0fffffffffffffff, 0x1fff07ffffffffff, 0x0000000003ff01ff, 0x0000000000000400,
         0x00001fffffffffff, 0x0000000000004000, 0x7fff6f7f00000000, 0x000000000000001f,
         0x0af7fe96ffffffef, 0x5ef7f796aa96ea84, 0x0ffffbee0ffffbff, 0x00000000ffffffff,
         0xffff0003ffffffff, 0x00000001ffffffff, 0x00000000000007ff}};
    static constexpr flat_array<12> cat_pi{{0x2E20, 0x2E02, 0x2E04, 0x2E09, 0x00AB, 0x2E0C, 0x2018,
                                            0x2039, 0x201B, 0x201C, 0x2E1C, 0x201F}};
    static constexpr range_array cat_cf = {
        0x00000000, 0x0000AD01, 0x0000AE00, 0x00060001, 0x00060600, 0x00061C01, 0x00061D00,
        0x0006DD01, 0x0006DE00, 0x00070F01, 0x00071000, 0x00089001, 0x00089200, 0x0008E201,
        0x0008E300, 0x00180E01, 0x00180F00, 0x00200B01, 0x00201000, 0x00202A01, 0x00202F00,
        0x00206001, 0x00206500, 0x00206601, 0x00207000, 0x00FEFF01, 0x00FF0000, 0x00FFF901,
        0x00FFFC00, 0x0110BD01, 0x0110BE00, 0x0110CD01, 0x0110CE00, 0x01343001, 0x01343900,
        0x01BCA001, 0x01BCA400, 0x01D17301, 0x01D17B00, 0x0E000101, 0x0E000200, 0x0E002001,
        0x0E008000};
    static constexpr bool_trie<32, 634, 7, 351, 25, 255, 1, 0, 385, 4, 59, 37> cat_no{
        {0x0000000000000000, 0x0000000000000000, 0x720c000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000},
        {1,  0,  0, 0, 0, 0,  2, 0, 3, 0,  4,  0,  0, 0,  5,  0, 0, 0, 0, 0, 0, 6, 0, 0, 0,  0,  0,
         0,  0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  7,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  8, 0, 0, 0,  0, 0, 0, 0,  9,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0, 0, 0, 0,  0, 0, 0, 10, 11, 0,  0, 12, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  14, 15,
         16, 0,  0, 0, 0, 0,  0, 0, 0, 0,  17, 18, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0, 0, 0, 19, 0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 0,  21,
         22, 23, 0, 0, 0, 0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0,  24},
        {0x0000000000000000, 0x03f0000000000000, 0x00fc000000000000, 0x0007000000000000,
         0x7f00000000000000, 0x01ff00007f000000, 0x000ffc0000000000, 0x1ffffe0000000000,
         0x03ff000000000000, 0x0000000004000000, 0x03f1000000000000, 0x00000000000003ff,
         0x00000000ffff0000, 0x0000000000000200, 0xffffffff00000000, 0x000000000fffffff,
         0xfffffc0000000000, 0xffc0000000000000, 0x00000000000fffff, 0x2000000000000000,
         0x00000000003c0000, 0x000003ff00000000, 0x00000000fffeff00, 0xfffe0000000003ff,
         0x003f000000000000},
        {1, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 4, 5, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
        {1,  2,  3, 0,  0, 0, 0,  4,  5,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0, 0,  0, 0, 6,  7,  8,  9, 0,  10, 11, 0,  12, 13, 14, 0,  15, 16, 0,  0,  0,
         0,  17, 0, 0,  0, 0, 0,  18, 0,  0, 19, 20, 0,  21, 0,  22, 0,  0,  0,  0,  0,  23, 0,
         0,  0,  0, 0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  24, 0,  0,  0,
         0,  0,  0, 25, 0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  26, 0,  0,  0,  0,  0,
         0,  0,  0, 0,  0, 0, 0,  0,  27, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0, 0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0, 0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0, 0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0, 0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0, 28, 0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  29, 0,  0,  0,  0,  0,  0,
         0,  0,  0, 0,  0, 0, 0,  0,  0,  0, 30, 0,  31, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0, 0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0, 0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0, 0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0, 0,  0, 0, 32, 0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  33, 34, 0,
         35, 0,  0, 0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  36},
        {0x0000000000000000, 0x000fffffffffff80, 0x01e0000000000000, 0x0000000000000c00,
         0x0ffffffe00000000, 0x0000000f00000000, 0xfe000000ff000000, 0x0000ff8000000000,
         0xf800000000000000, 0x000000000fc00000, 0x3000000000000000, 0xfffffffffffcffff,
         0x60000000000001ff, 0x00000000e0000000, 0x0000f80000000000, 0xff000000ff000000,
         0x0000fe0000000000, 0xfc00000000000000, 0x7fffffff00000000, 0x0000007fe0000000,
         0x00000000001e0000, 0x0000000000000fe0, 0x0000003ffffc0000, 0x001ffffe00000000,
         0x0c00000000000000, 0x0007fc0000000000, 0x00001ffffc000000, 0x00000000001fffff,
         0x00000003f8000000, 0x00000000007fffff, 0x000fffff00000000, 0x01ffffff00000000,
         0x000000000000ff80, 0xfffe000000000000, 0x001eefffffffffff, 0x3fffbffffffffffe,
         0x0000000000001fff}};
    static constexpr flat_array<10> cat_pf{
        {0x2E21, 0x2E1D, 0x2E03, 0x2E05, 0x2E0A, 0x2E0D, 0x2019, 0x203A, 0x00BB, 0x201D}};
    static constexpr range_array cat_lt = {
        0x00000000, 0x0001C501, 0x0001C600, 0x0001C801, 0x0001C900, 0x0001CB01, 0x0001CC00,
        0x0001F201, 0x0001F300, 0x001F8801, 0x001F9000, 0x001F9801, 0x001FA000, 0x001FA801,
        0x001FB000, 0x001FBC01, 0x001FBD00, 0x001FCC01, 0x001FCD00, 0x001FFC01, 0x001FFD00};
    static constexpr bool_trie<32, 991, 1, 0, 32, 255, 1, 0, 264, 30, 26, 8> cat_lm{
        {0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0xffff000000000000, 0x0000501f0003ffc3,
         0x0000000000000000, 0x0410000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000002000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000001, 0x0000000000000000, 0x0000006000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0430000000000000},
        {1,  1,  2,  1, 3, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  4,
         1,  4,  1,  1, 1, 1, 1,  1,  1,  5,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  6,  1,  7,  1, 1, 1, 1, 1,  1, 1,  1,  8,  1,
         1,  1,  1,  1, 1, 9, 1,  1,  10, 11, 12, 1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 13, 14, 1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 15, 1, 1,  1,  16, 1,
         1,  16, 1,  1, 1, 1, 1,  1,  1,  17, 1,  18, 19, 1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  20, 1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  9,  1,  1, 1, 1, 21, 22, 23, 1,  24, 25, 26, 27, 1,  1, 1, 1, 1, 1,  1, 28, 1,  25, 1,
         29, 1,  30, 1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1,  1, 1,  1,  1,  1,
         1,  1,  1,  1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  25, 31, 1},
        {0x0000011004000000, 0x0000000000000000, 0x0000000000000200, 0x0002000000000000,
         0x0000000000000040, 0x1000000000000000, 0x0000000000800000, 0x0000000000000008,
         0x0000008000000000, 0x3f00000000000000, 0xfffff00000000000, 0x010007ffffffffff,
         0xfffffffff8000000, 0x8002000000000000, 0x000000001fff0000, 0x3000000000000000,
         0x0000800000000000, 0x083e000000000020, 0x0000000060000000, 0x7000000000000000,
         0x0000000000200000, 0x0000000000001000, 0x8000000000000000, 0x0000000030000000,
         0x00000000ff800000, 0x0001000000000000, 0x0000000000000100, 0x031c000000000000,
         0x0000004000008000, 0x0018000020000000, 0x00000200f0000000, 0x00000000c0000000},
        {1, 1, 1, 1, 1, 2, 1, 1, 1, 3, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7},
        {0x0000000000000000, 0x07fdffffffffffbf, 0x000000000000000f, 0x00000000fff80000,
         0x0000000b00000000, 0x6fef000000000000, 0x3f80000000000000, 0x0000000000000800}};
    static constexpr bool_trie<32, 991, 1, 0, 75, 255, 1, 0, 513, 7, 56, 66> cat_mn{
        {0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0xffffffffffffffff, 0x0000ffffffffffff, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x00000000000000f8, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0xbffffffffffe0000, 0x00000000000000b6,
         0x0000000007ff0000, 0x00010000fffff800, 0x0000000000000000, 0x00003d9f9fc00000,
         0xffff000000020000, 0x00000000000007ff, 0x0001ffc000000000, 0x200ff80000000000},
        {1,  2,  3,  4,  5,  6,  7,  8,  9,  8,  10, 11, 12, 13, 14, 15, 16, 11, 17, 18, 19, 20, 21,
         22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 34, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 35, 36, 37, 38, 39, 33, 40, 33, 41, 33,
         33, 33, 42, 43, 44, 45, 46, 47, 48, 49, 50, 33, 33, 51, 33, 33, 33, 52, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 53, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 54, 33, 55, 33, 56, 33, 33, 33, 33, 33, 33, 33, 33, 57, 33,
         58, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 59, 60, 61, 33, 33, 33, 33, 62, 33, 33, 63, 64,
         65, 66, 67, 68, 69, 70, 71, 33, 33, 33, 72, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
         33, 33, 33, 33, 33, 73, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 74, 33, 33, 33, 33, 33,
         33, 33},
        {0x00003eeffbc00000, 0x000000000e000000, 0x00000000ff000000, 0xfffffffbfffffc00,
         0x1400000000000007, 0x0000000c00fe21fe, 0x1000000000000002, 0x4000000c0000201e,
         0x1000000000000006, 0x0023000000023986, 0xfc00000c000021be, 0x9000000000000002,
         0x0000000c0060201e, 0x0000000000000004, 0x0000000000002001, 0xd000000000000011,
         0x0000000c00603dc1, 0x0000000c00003040, 0x1800000000000003, 0x0000000c0000201e,
         0x0000000000000002, 0x00000000005c0400, 0x07f2000000000000, 0x0000000000007f80,
         0x1ff2000000000000, 0x0000000000003f00, 0x02a0000003000000, 0x7ffe000000000000,
         0x1ffffffffeffe0df, 0x0000000000000040, 0x66fde00000000000, 0x001e0001c3000000,
         0x0000000020002064, 0x0000000000000000, 0x00000000e0000000, 0x000c0000001c0000,
         0x000c0000000c0000, 0x3fb0000000000000, 0x00000000200ffe40, 0x000000000000b800,
         0x0000020000000060, 0x0e04018700000000, 0x0000000009800000, 0x9ff81fe57f400000,
         0xbfff000000000000, 0x0000000000007fff, 0x17d000000000000f, 0x000ff80000000004,
         0x00003b3c00000003, 0x0003a34000000000, 0x00cff00000000000, 0x031021fdfff70000,
         0xffffffffffffffff, 0x0001ffe21fff0000, 0x0003800000000000, 0x8000000000000000,
         0xffffffff00000000, 0x00003c0000000000, 0x0000000006000000, 0x3ff0800000000000,
         0x00000000c0000000, 0x0003000000000000, 0x0000106000000844, 0x8003ffff00000030,
         0x00003fc000000000, 0x000000000003ff80, 0x33c8000000000007, 0x0000002000000000,
         0x00667e0000000000, 0x1000000000001008, 0xc19d000000000000, 0x0040300000000002,
         0x0000212000000000, 0x0000000040000000, 0x0000ffff0000ffff},
        {1, 2, 2, 2, 2, 3, 2, 2, 2, 2, 4, 5, 6, 7, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
        {1,  0,  0,  0,  2,  0,  3,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 4,  0,  0,  5,  0,  0,  0,  0,  0,  0,  0,  0,  6,
         0,  0,  0,  0,  0,  7,  0,  0,  8,  9, 0,  10, 11, 12, 13, 14, 15, 16, 17, 18, 0,  0,  19,
         20, 21, 0,  0,  22, 23, 24, 25, 0,  0, 26, 27, 28, 29, 30, 0,  31, 0,  0,  0,  32, 0,  0,
         0,  33, 34, 0,  35, 36, 37, 38, 0,  0, 0,  0,  0,  39, 0,  40, 0,  41, 42, 43, 0,  0,  0,
         0,  44, 0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  45, 46,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  47, 48, 49, 0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         50, 0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  51, 52, 0,  0,  0, 0,  0,  0,  0,  53, 54, 0,  0,  55, 0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  56, 57, 58, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  59, 0,  0,  0,  46, 0, 0,  0,  0,  0,  60, 61, 0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  62, 0,  63, 0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  64, 64, 64, 65},
        {0x0000000000000000, 0x2000000000000000, 0x0000000100000000, 0x07c0000000000000,
         0x870000000000f06e, 0x0000006000000000, 0x000000f000000000, 0x0000180000000000,
         0x000000000001ffc0, 0x000000000000003c, 0xff00000000000002, 0x801900000000007f,
         0x0678000000000003, 0x0000000000000004, 0x001fef8000000007, 0x0008000000000000,
         0x7fc0000000000003, 0x0000000000009e00, 0x40d3800000000000, 0x000007f880000000,
         0x1800000000000003, 0x001f1fc000000001, 0xff00000000000000, 0x000000004000005c,
         0x85f8000000000000, 0x000000000000000d, 0xb03c000000000000, 0x0000000030000001,
         0xa7f8000000000000, 0x0000000000000001, 0x00bf280000000000, 0x00000fbce0000000,
         0x06ff800000000000, 0x5800000000000000, 0x0000000000000008, 0x000000010cf00000,
         0x79f80000000007fe, 0x000000000e7e0080, 0x00000000037ffc00, 0xbf7f000000000000,
         0x006dfcfffffc0000, 0xb47e000000000000, 0x00000000000000bf, 0x0000000000a30000,
         0x0018000000000000, 0x001f000000000000, 0x007f000000000000, 0x0000000000008000,
         0x0000000000078000, 0x0000001000000000, 0x0000000060000000, 0xffff3fffffffffff,
         0x000000000000007f, 0xf800038000000000, 0x00003c0000000fe7, 0x000000000000001c,
         0xf87fffffffffffff, 0x00201fffffffffff, 0x0000fffef8000010, 0x000007dbf9ffff7f,
         0x0000400000000000, 0x0000f00000000000, 0x00000000007f0000, 0x00000000000007f0,
         0xffffffffffffffff, 0x0000ffffffffffff}};
    static constexpr range_array cat_me = {0x00000000, 0x00048801, 0x00048A00, 0x001ABE01,
                                           0x001ABF00, 0x0020DD01, 0x0020E100, 0x0020E201,
                                           0x0020E500, 0x00A67001, 0x00A67300};
    static constexpr bool_trie<0, 652, 4, 336, 43, 13, 1, 242, 134, 64, 58, 33> cat_mc{
        {},
        {1,  2,  3,  4,  5,  6,  5,  7,  8, 4, 9,  10, 11, 12, 8, 13, 3, 14, 15, 16, 0,  0,  0,
         0,  9,  17, 0,  0,  18, 19, 20, 0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 21, 0,  22, 23, 0, 0,  0, 0,  24, 0,  0,  0,  25,
         26, 0,  0,  27, 28, 29, 30, 31, 0, 0, 32, 0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  33, 0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 34, 0, 35, 36, 0,  37, 38, 6,
         39, 40, 0,  41, 0,  0,  0,  42},
        {0x0000000000000000, 0xc800000000000008, 0x000000000000de01, 0xc00000000000000c,
         0x0000000000801981, 0xc000000000000008, 0x0000000000000001, 0x0000000000001a01,
         0x400000000000000c, 0xc000000000000000, 0x0000000000801dc6, 0x000000000000000e,
         0x000000000000001e, 0x0000000000600d9f, 0x0000000000801dc1, 0x000000000000000c,
         0x000c0000ff038000, 0x8000000000000000, 0x1902180000000000, 0x00003f9c00c00000,
         0x000000001c009f98, 0x0010000000200000, 0xc040000000000000, 0x00000000000001bf,
         0x01fb0e7800000000, 0x0000000006000000, 0x0007e01a00a00000, 0xe820000000000010,
         0x000000000000001b, 0x000004c200000004, 0x000c5c8000000000, 0x00300ff000000000,
         0x0080000200000000, 0x0000c00000000000, 0x0000009800000000, 0xfff0000000000003,
         0x000000000000000f, 0x00000000000c0000, 0xcc30000000000008, 0x0019800000000000,
         0x2800000000002000, 0x0020c80000000000, 0x000016d800000000},
        {1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 3},
        {1, 0,  2,  0,  3,  4,  5, 6, 7,  0,  0,  8,  9,  10, 0, 0, 11, 12, 13, 14, 0, 0, 15,
         0, 16, 0,  17, 0,  18, 0, 0, 0,  19, 0,  0,  0,  20, 1, 0, 21, 22, 23, 24, 0, 0, 0,
         0, 0,  25, 0,  26, 0,  0, 0, 27, 0,  0,  0,  0,  28, 0, 0, 0,  0,  0,  0,  0, 0, 0,
         0, 0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,
         0, 0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0,  0, 0, 0,
         0, 0,  0,  0,  0,  0,  0, 0, 0,  0,  29, 30, 31, 0,  0, 0, 0,  0,  32},
        {0x0000000000000000, 0x0000000000000005, 0x0187000000000004, 0x0000100000000000,
         0x0000000000000060, 0x8038000000000004, 0x0000000000004001, 0x002c700000000000,
         0x0000000700000000, 0xc00000000000000c, 0x0000000c0080399e, 0x00e0000000000000,
         0x0000000000000023, 0x7a07000000000000, 0x0000000000000002, 0x4f03800000000000,
         0x5807000000000000, 0x0040d00000000000, 0x0000004300000000, 0x0100700000000000,
         0x21bf000000000000, 0x00000010f00e0000, 0x0200000000000000, 0x0000000001800000,
         0x0000000000800000, 0x4000800000000000, 0x0012020000000000, 0x0000000000587c00,
         0x0060000000000000, 0xfffffffffffe0000, 0x00000000000000ff, 0x0003000000000000,
         0x0007e06000000000}};
    static constexpr range_array cat_nl = {
        0x00000000, 0x0016EE01, 0x0016F100, 0x00216001, 0x00218300, 0x00218501, 0x00218900,
        0x00300701, 0x00300800, 0x00302101, 0x00302A00, 0x00303801, 0x00303B00, 0x00A6E601,
        0x00A6F000, 0x01014001, 0x01017500, 0x01034101, 0x01034200, 0x01034A01, 0x01034B00,
        0x0103D101, 0x0103D600, 0x01240001, 0x01246F00};
    static constexpr flat_array<1> cat_zl{{0x2028}};
    static constexpr flat_array<1> cat_zp{{0x2029}};
    static constexpr range_array cat_cs = {0x00000000, 0x00D80001, 0x00E00000};
    static constexpr range_array cat_co = {0x00000000, 0x00E00001, 0x00F90000, 0x0F000001,
                                           0x0FFFFE00, 0x10000001, 0x10FFFE00};
    constexpr category get_category(char32_t c) {
        if(cat_co.lookup(c))
            return category::co;
        if(cat_lo.lookup(c))
            return category::lo;
        if(cat_so.lookup(c))
            return category::so;
        if(cat_ll.lookup(c))
            return category::ll;
        if(cat_cs.lookup(c))
            return category::cs;
        if(cat_mn.lookup(c))
            return category::mn;
        if(cat_lu.lookup(c))
            return category::lu;
        if(cat_sm.lookup(c))
            return category::sm;
        if(cat_no.lookup(c))
            return category::no;
        if(cat_nd.lookup(c))
            return category::nd;
        if(cat_po.lookup(c))
            return category::po;
        if(cat_mc.lookup(c))
            return category::mc;
        if(cat_lm.lookup(c))
            return category::lm;
        if(cat_nl.lookup(c))
            return category::nl;
        if(cat_cf.lookup(c))
            return category::cf;
        if(cat_sk.lookup(c))
            return category::sk;
        if(cat_ps.lookup(c))
            return category::ps;
        if(cat_pe.lookup(c))
            return category::pe;
        if(cat_cc.lookup(c))
            return category::cc;
        if(cat_sc.lookup(c))
            return category::sc;
        if(cat_lt.lookup(c))
            return category::lt;
        if(cat_pd.lookup(c))
            return category::pd;
        if(cat_zs.lookup(c))
            return category::zs;
        if(cat_me.lookup(c))
            return category::me;
        if(cat_pi.lookup(c))
            return category::pi;
        if(cat_pf.lookup(c))
            return category::pf;
        if(cat_pc.lookup(c))
            return category::pc;
        if(cat_zp.lookup(c))
            return category::zp;
        if(cat_zl.lookup(c))
            return category::zl;
        return category::cn;
    }
}    // namespace detail::tables
template<>
constexpr bool cp_category_is<category::co>(char32_t c) {
    return detail::tables::cat_co.lookup(c);
}
template<>
constexpr bool cp_category_is<category::lo>(char32_t c) {
    return detail::tables::cat_lo.lookup(c);
}
template<>
constexpr bool cp_category_is<category::so>(char32_t c) {
    return detail::tables::cat_so.lookup(c);
}
template<>
constexpr bool cp_category_is<category::ll>(char32_t c) {
    return detail::tables::cat_ll.lookup(c);
}
template<>
constexpr bool cp_category_is<category::cs>(char32_t c) {
    return detail::tables::cat_cs.lookup(c);
}
template<>
constexpr bool cp_category_is<category::mn>(char32_t c) {
    return detail::tables::cat_mn.lookup(c);
}
template<>
constexpr bool cp_category_is<category::lu>(char32_t c) {
    return detail::tables::cat_lu.lookup(c);
}
template<>
constexpr bool cp_category_is<category::sm>(char32_t c) {
    return detail::tables::cat_sm.lookup(c);
}
template<>
constexpr bool cp_category_is<category::no>(char32_t c) {
    return detail::tables::cat_no.lookup(c);
}
template<>
constexpr bool cp_category_is<category::nd>(char32_t c) {
    return detail::tables::cat_nd.lookup(c);
}
template<>
constexpr bool cp_category_is<category::po>(char32_t c) {
    return detail::tables::cat_po.lookup(c);
}
template<>
constexpr bool cp_category_is<category::mc>(char32_t c) {
    return detail::tables::cat_mc.lookup(c);
}
template<>
constexpr bool cp_category_is<category::lm>(char32_t c) {
    return detail::tables::cat_lm.lookup(c);
}
template<>
constexpr bool cp_category_is<category::nl>(char32_t c) {
    return detail::tables::cat_nl.lookup(c);
}
template<>
constexpr bool cp_category_is<category::cf>(char32_t c) {
    return detail::tables::cat_cf.lookup(c);
}
template<>
constexpr bool cp_category_is<category::sk>(char32_t c) {
    return detail::tables::cat_sk.lookup(c);
}
template<>
constexpr bool cp_category_is<category::ps>(char32_t c) {
    return detail::tables::cat_ps.lookup(c);
}
template<>
constexpr bool cp_category_is<category::pe>(char32_t c) {
    return detail::tables::cat_pe.lookup(c);
}
template<>
constexpr bool cp_category_is<category::cc>(char32_t c) {
    return detail::tables::cat_cc.lookup(c);
}
template<>
constexpr bool cp_category_is<category::sc>(char32_t c) {
    return detail::tables::cat_sc.lookup(c);
}
template<>
constexpr bool cp_category_is<category::lt>(char32_t c) {
    return detail::tables::cat_lt.lookup(c);
}
template<>
constexpr bool cp_category_is<category::pd>(char32_t c) {
    return detail::tables::cat_pd.lookup(c);
}
template<>
constexpr bool cp_category_is<category::zs>(char32_t c) {
    return detail::tables::cat_zs.lookup(c);
}
template<>
constexpr bool cp_category_is<category::me>(char32_t c) {
    return detail::tables::cat_me.lookup(c);
}
template<>
constexpr bool cp_category_is<category::pi>(char32_t c) {
    return detail::tables::cat_pi.lookup(c);
}
template<>
constexpr bool cp_category_is<category::pf>(char32_t c) {
    return detail::tables::cat_pf.lookup(c);
}
template<>
constexpr bool cp_category_is<category::pc>(char32_t c) {
    return detail::tables::cat_pc.lookup(c);
}
template<>
constexpr bool cp_category_is<category::zp>(char32_t c) {
    return detail::tables::cat_zp.lookup(c);
}
template<>
constexpr bool cp_category_is<category::zl>(char32_t c) {
    return detail::tables::cat_zl.lookup(c);
}
template<>
constexpr bool cp_category_is<category::cased_letter>(char32_t c) {
    if(detail::tables::cat_ll.lookup(c))
        return true;
    if(detail::tables::cat_lu.lookup(c))
        return true;
    if(detail::tables::cat_lt.lookup(c))
        return true;
    return false;
}
template<>
constexpr bool cp_category_is<category::letter>(char32_t c) {
    if(detail::tables::cat_lo.lookup(c))
        return true;
    if(detail::tables::cat_ll.lookup(c))
        return true;
    if(detail::tables::cat_lu.lookup(c))
        return true;
    if(detail::tables::cat_lm.lookup(c))
        return true;
    if(detail::tables::cat_lt.lookup(c))
        return true;
    return false;
}
template<>
constexpr bool cp_category_is<category::mark>(char32_t c) {
    if(detail::tables::cat_mn.lookup(c))
        return true;
    if(detail::tables::cat_mc.lookup(c))
        return true;
    if(detail::tables::cat_me.lookup(c))
        return true;
    return false;
}
template<>
constexpr bool cp_category_is<category::number>(char32_t c) {
    if(detail::tables::cat_no.lookup(c))
        return true;
    if(detail::tables::cat_nd.lookup(c))
        return true;
    if(detail::tables::cat_nl.lookup(c))
        return true;
    return false;
}
template<>
constexpr bool cp_category_is<category::punctuation>(char32_t c) {
    if(detail::tables::cat_po.lookup(c))
        return true;
    if(detail::tables::cat_ps.lookup(c))
        return true;
    if(detail::tables::cat_pe.lookup(c))
        return true;
    if(detail::tables::cat_pd.lookup(c))
        return true;
    if(detail::tables::cat_pi.lookup(c))
        return true;
    if(detail::tables::cat_pf.lookup(c))
        return true;
    if(detail::tables::cat_pc.lookup(c))
        return true;
    return false;
}
template<>
constexpr bool cp_category_is<category::symbol>(char32_t c) {
    if(detail::tables::cat_so.lookup(c))
        return true;
    if(detail::tables::cat_sm.lookup(c))
        return true;
    if(detail::tables::cat_sk.lookup(c))
        return true;
    if(detail::tables::cat_sc.lookup(c))
        return true;
    return false;
}
template<>
constexpr bool cp_category_is<category::separator>(char32_t c) {
    if(detail::tables::cat_zs.lookup(c))
        return true;
    if(detail::tables::cat_zp.lookup(c))
        return true;
    if(detail::tables::cat_zl.lookup(c))
        return true;
    return false;
}
template<>
constexpr bool cp_category_is<category::other>(char32_t c) {
    if(detail::tables::cat_co.lookup(c))
        return true;
    if(detail::tables::cat_cs.lookup(c))
        return true;
    if(detail::tables::cat_cf.lookup(c))
        return true;
    if(detail::tables::cat_cc.lookup(c))
        return true;
    return false;
}
template<>
constexpr bool cp_category_is<category::unassigned>(char32_t c) {
    return cp_category(c) == category::unassigned;
}
namespace detail::tables {
    static constexpr string_with_idx blocks_names[] = {
        string_with_idx{"adlam", 290},
        string_with_idx{"aegean_numbers", 167},
        string_with_idx{"ahom", 232},
        string_with_idx{"alchemical", 303},
        string_with_idx{"alchemical_symbols", 303},
        string_with_idx{"alphabetic_pf", 155},
        string_with_idx{"alphabetic_presentation_forms", 155},
        string_with_idx{"anatolian_hieroglyphs", 254},
        string_with_idx{"ancient_greek_music", 277},
        string_with_idx{"ancient_greek_musical_notation", 277},
        string_with_idx{"ancient_greek_numbers", 168},
        string_with_idx{"ancient_symbols", 169},
        string_with_idx{"arabic", 13},
        string_with_idx{"arabic_ext_a", 22},
        string_with_idx{"arabic_ext_b", 21},
        string_with_idx{"arabic_extended_a", 22},
        string_with_idx{"arabic_extended_b", 21},
        string_with_idx{"arabic_math", 293},
        string_with_idx{"arabic_mathematical_alphabetic_symbols", 293},
        string_with_idx{"arabic_pf_a", 156},
        string_with_idx{"arabic_pf_b", 162},
        string_with_idx{"arabic_presentation_forms_a", 156},
        string_with_idx{"arabic_presentation_forms_b", 162},
        string_with_idx{"arabic_sup", 15},
        string_with_idx{"arabic_supplement", 15},
        string_with_idx{"armenian", 11},
        string_with_idx{"arrows", 79},
        string_with_idx{"ascii", 1},
        string_with_idx{"avestan", 201},
        string_with_idx{"balinese", 59},
        string_with_idx{"bamum", 127},
        string_with_idx{"bamum_sup", 255},
        string_with_idx{"bamum_supplement", 255},
        string_with_idx{"basic_latin", 1},
        string_with_idx{"bassa_vah", 258},
        string_with_idx{"batak", 61},
        string_with_idx{"bengali", 24},
        string_with_idx{"bhaiksuki", 241},
        string_with_idx{"block_elements", 86},
        string_with_idx{"bopomofo", 111},
        string_with_idx{"bopomofo_ext", 114},
        string_with_idx{"bopomofo_extended", 114},
        string_with_idx{"box_drawing", 85},
        string_with_idx{"brahmi", 215},
        string_with_idx{"braille", 92},
        string_with_idx{"braille_patterns", 92},
        string_with_idx{"buginese", 56},
        string_with_idx{"buhid", 47},
        string_with_idx{"byzantine_music", 275},
        string_with_idx{"byzantine_musical_symbols", 275},
        string_with_idx{"carian", 172},
        string_with_idx{"caucasian_albanian", 184},
        string_with_idx{"chakma", 218},
        string_with_idx{"cham", 140},
        string_with_idx{"cherokee", 41},
        string_with_idx{"cherokee_sup", 146},
        string_with_idx{"cherokee_supplement", 146},
        string_with_idx{"chess_symbols", 307},
        string_with_idx{"chorasmian", 213},
        string_with_idx{"cjk", 121},
        string_with_idx{"cjk_compat", 118},
        string_with_idx{"cjk_compat_forms", 160},
        string_with_idx{"cjk_compat_ideographs", 154},
        string_with_idx{"cjk_compat_ideographs_sup", 315},
        string_with_idx{"cjk_compatibility", 118},
        string_with_idx{"cjk_compatibility_forms", 160},
        string_with_idx{"cjk_compatibility_ideographs", 154},
        string_with_idx{"cjk_compatibility_ideographs_supplement", 315},
        string_with_idx{"cjk_ext_a", 119},
        string_with_idx{"cjk_ext_b", 310},
        string_with_idx{"cjk_ext_c", 311},
        string_with_idx{"cjk_ext_d", 312},
        string_with_idx{"cjk_ext_e", 313},
        string_with_idx{"cjk_ext_f", 314},
        string_with_idx{"cjk_ext_g", 316},
        string_with_idx{"cjk_radicals_sup", 105},
        string_with_idx{"cjk_radicals_supplement", 105},
        string_with_idx{"cjk_strokes", 115},
        string_with_idx{"cjk_symbols", 108},
        string_with_idx{"cjk_symbols_and_punctuation", 108},
        string_with_idx{"cjk_unified_ideographs", 121},
        string_with_idx{"cjk_unified_ideographs_extension_a", 119},
        string_with_idx{"cjk_unified_ideographs_extension_b", 310},
        string_with_idx{"cjk_unified_ideographs_extension_c", 311},
        string_with_idx{"cjk_unified_ideographs_extension_d", 312},
        string_with_idx{"cjk_unified_ideographs_extension_e", 313},
        string_with_idx{"cjk_unified_ideographs_extension_f", 314},
        string_with_idx{"cjk_unified_ideographs_extension_g", 316},
        string_with_idx{"combining_diacritical_marks", 7},
        string_with_idx{"combining_diacritical_marks_extended", 58},
        string_with_idx{"combining_diacritical_marks_for_symbols", 76},
        string_with_idx{"combining_diacritical_marks_supplement", 70},
        string_with_idx{"combining_half_marks", 159},
        string_with_idx{"common_indic_number_forms", 131},
        string_with_idx{"compat_jamo", 112},
        string_with_idx{"control_pictures", 82},
        string_with_idx{"coptic", 99},
        string_with_idx{"coptic_epact_numbers", 173},
        string_with_idx{"counting_rod", 280},
        string_with_idx{"counting_rod_numerals", 280},
        string_with_idx{"cuneiform", 248},
        string_with_idx{"cuneiform_numbers", 249},
        string_with_idx{"cuneiform_numbers_and_punctuation", 249},
        string_with_idx{"currency_symbols", 75},
        string_with_idx{"cypriot_syllabary", 188},
        string_with_idx{"cypro_minoan", 251},
        string_with_idx{"cyrillic", 9},
        string_with_idx{"cyrillic_ext_a", 103},
        string_with_idx{"cyrillic_ext_b", 126},
        string_with_idx{"cyrillic_ext_c", 64},
        string_with_idx{"cyrillic_extended_a", 103},
        string_with_idx{"cyrillic_extended_b", 126},
        string_with_idx{"cyrillic_extended_c", 64},
        string_with_idx{"cyrillic_sup", 10},
        string_with_idx{"cyrillic_supplement", 10},
        string_with_idx{"deseret", 179},
        string_with_idx{"devanagari", 23},
        string_with_idx{"devanagari_ext", 134},
        string_with_idx{"devanagari_extended", 134},
        string_with_idx{"diacriticals", 7},
        string_with_idx{"diacriticals_ext", 58},
        string_with_idx{"diacriticals_for_symbols", 76},
        string_with_idx{"diacriticals_sup", 70},
        string_with_idx{"dingbats", 89},
        string_with_idx{"dives_akuru", 235},
        string_with_idx{"dogra", 233},
        string_with_idx{"domino", 295},
        string_with_idx{"domino_tiles", 295},
        string_with_idx{"duployan", 272},
        string_with_idx{"early_dynastic_cuneiform", 250},
        string_with_idx{"egyptian_hieroglyph_format_controls", 253},
        string_with_idx{"egyptian_hieroglyphs", 252},
        string_with_idx{"elbasan", 183},
        string_with_idx{"elymaic", 214},
        string_with_idx{"emoticons", 300},
        string_with_idx{"enclosed_alphanum", 84},
        string_with_idx{"enclosed_alphanum_sup", 297},
        string_with_idx{"enclosed_alphanumeric_supplement", 297},
        string_with_idx{"enclosed_alphanumerics", 84},
        string_with_idx{"enclosed_cjk", 117},
        string_with_idx{"enclosed_cjk_letters_and_months", 117},
        string_with_idx{"enclosed_ideographic_sup", 298},
        string_with_idx{"enclosed_ideographic_supplement", 298},
        string_with_idx{"ethiopic", 39},
        string_with_idx{"ethiopic_ext", 102},
        string_with_idx{"ethiopic_ext_a", 144},
        string_with_idx{"ethiopic_ext_b", 288},
        string_with_idx{"ethiopic_extended", 102},
        string_with_idx{"ethiopic_extended_a", 144},
        string_with_idx{"ethiopic_extended_b", 288},
        string_with_idx{"ethiopic_sup", 40},
        string_with_idx{"ethiopic_supplement", 40},
        string_with_idx{"general_punctuation", 73},
        string_with_idx{"geometric_shapes", 87},
        string_with_idx{"geometric_shapes_ext", 304},
        string_with_idx{"geometric_shapes_extended", 304},
        string_with_idx{"georgian", 37},
        string_with_idx{"georgian_ext", 65},
        string_with_idx{"georgian_extended", 65},
        string_with_idx{"georgian_sup", 100},
        string_with_idx{"georgian_supplement", 100},
        string_with_idx{"glagolitic", 97},
        string_with_idx{"glagolitic_sup", 284},
        string_with_idx{"glagolitic_supplement", 284},
        string_with_idx{"gothic", 175},
        string_with_idx{"grantha", 225},
        string_with_idx{"greek", 8},
        string_with_idx{"greek_and_coptic", 8},
        string_with_idx{"greek_ext", 72},
        string_with_idx{"greek_extended", 72},
        string_with_idx{"gujarati", 26},
        string_with_idx{"gunjala_gondi", 244},
        string_with_idx{"gurmukhi", 25},
        string_with_idx{"half_and_full_forms", 163},
        string_with_idx{"half_marks", 159},
        string_with_idx{"halfwidth_and_fullwidth_forms", 163},
        string_with_idx{"hangul", 148},
        string_with_idx{"hangul_compatibility_jamo", 112},
        string_with_idx{"hangul_jamo", 38},
        string_with_idx{"hangul_jamo_extended_a", 137},
        string_with_idx{"hangul_jamo_extended_b", 149},
        string_with_idx{"hangul_syllables", 148},
        string_with_idx{"hanifi_rohingya", 207},
        string_with_idx{"hanunoo", 46},
        string_with_idx{"hatran", 192},
        string_with_idx{"hebrew", 12},
        string_with_idx{"high_private_use_surrogates", 151},
        string_with_idx{"high_pu_surrogates", 151},
        string_with_idx{"high_surrogates", 150},
        string_with_idx{"hiragana", 109},
        string_with_idx{"idc", 107},
        string_with_idx{"ideographic_description_characters", 107},
        string_with_idx{"ideographic_symbols", 262},
        string_with_idx{"ideographic_symbols_and_punctuation", 262},
        string_with_idx{"imperial_aramaic", 189},
        string_with_idx{"indic_number_forms", 131},
        string_with_idx{"indic_siyaq_numbers", 291},
        string_with_idx{"inscriptional_pahlavi", 203},
        string_with_idx{"inscriptional_parthian", 202},
        string_with_idx{"ipa_ext", 5},
        string_with_idx{"ipa_extensions", 5},
        string_with_idx{"jamo", 38},
        string_with_idx{"jamo_ext_a", 137},
        string_with_idx{"jamo_ext_b", 149},
        string_with_idx{"javanese", 138},
        string_with_idx{"kaithi", 216},
        string_with_idx{"kana_ext_a", 269},
        string_with_idx{"kana_ext_b", 267},
        string_with_idx{"kana_extended_a", 269},
        string_with_idx{"kana_extended_b", 267},
        string_with_idx{"kana_sup", 268},
        string_with_idx{"kana_supplement", 268},
        string_with_idx{"kanbun", 113},
        string_with_idx{"kangxi", 106},
        string_with_idx{"kangxi_radicals", 106},
        string_with_idx{"kannada", 30},
        string_with_idx{"katakana", 110},
        string_with_idx{"katakana_ext", 116},
        string_with_idx{"katakana_phonetic_extensions", 116},
        string_with_idx{"kayah_li", 135},
        string_with_idx{"kharoshthi", 197},
        string_with_idx{"khitan_small_script", 265},
        string_with_idx{"khmer", 49},
        string_with_idx{"khmer_symbols", 55},
        string_with_idx{"khojki", 222},
        string_with_idx{"khudawadi", 224},
        string_with_idx{"lao", 34},
        string_with_idx{"latin_1_sup", 2},
        string_with_idx{"latin_1_supplement", 2},
        string_with_idx{"latin_ext_a", 3},
        string_with_idx{"latin_ext_additional", 71},
        string_with_idx{"latin_ext_b", 4},
        string_with_idx{"latin_ext_c", 98},
        string_with_idx{"latin_ext_d", 129},
        string_with_idx{"latin_ext_e", 145},
        string_with_idx{"latin_ext_f", 187},
        string_with_idx{"latin_ext_g", 283},
        string_with_idx{"latin_extended_a", 3},
        string_with_idx{"latin_extended_additional", 71},
        string_with_idx{"latin_extended_b", 4},
        string_with_idx{"latin_extended_c", 98},
        string_with_idx{"latin_extended_d", 129},
        string_with_idx{"latin_extended_e", 145},
        string_with_idx{"latin_extended_f", 187},
        string_with_idx{"latin_extended_g", 283},
        string_with_idx{"lepcha", 62},
        string_with_idx{"letterlike_symbols", 77},
        string_with_idx{"limbu", 52},
        string_with_idx{"linear_a", 186},
        string_with_idx{"linear_b_ideograms", 166},
        string_with_idx{"linear_b_syllabary", 165},
        string_with_idx{"lisu", 124},
        string_with_idx{"lisu_sup", 246},
        string_with_idx{"lisu_supplement", 246},
        string_with_idx{"low_surrogates", 152},
        string_with_idx{"lycian", 171},
        string_with_idx{"lydian", 194},
        string_with_idx{"mahajani", 219},
        string_with_idx{"mahjong", 294},
        string_with_idx{"mahjong_tiles", 294},
        string_with_idx{"makasar", 245},
        string_with_idx{"malayalam", 31},
        string_with_idx{"mandaic", 19},
        string_with_idx{"manichaean", 200},
        string_with_idx{"marchen", 242},
        string_with_idx{"masaram_gondi", 243},
        string_with_idx{"math_alphanum", 281},
        string_with_idx{"math_operators", 80},
        string_with_idx{"mathematical_alphanumeric_symbols", 281},
        string_with_idx{"mathematical_operators", 80},
        string_with_idx{"mayan_numerals", 278},
        string_with_idx{"medefaidrin", 260},
        string_with_idx{"meetei_mayek", 147},
        string_with_idx{"meetei_mayek_ext", 143},
        string_with_idx{"meetei_mayek_extensions", 143},
        string_with_idx{"mende_kikakui", 289},
        string_with_idx{"meroitic_cursive", 196},
        string_with_idx{"meroitic_hieroglyphs", 195},
        string_with_idx{"miao", 261},
        string_with_idx{"misc_arrows", 96},
        string_with_idx{"misc_math_symbols_a", 90},
        string_with_idx{"misc_math_symbols_b", 94},
        string_with_idx{"misc_pictographs", 299},
        string_with_idx{"misc_symbols", 88},
        string_with_idx{"misc_technical", 81},
        string_with_idx{"miscellaneous_mathematical_symbols_a", 90},
        string_with_idx{"miscellaneous_mathematical_symbols_b", 94},
        string_with_idx{"miscellaneous_symbols", 88},
        string_with_idx{"miscellaneous_symbols_and_arrows", 96},
        string_with_idx{"miscellaneous_symbols_and_pictographs", 299},
        string_with_idx{"miscellaneous_technical", 81},
        string_with_idx{"modi", 229},
        string_with_idx{"modifier_letters", 6},
        string_with_idx{"modifier_tone_letters", 128},
        string_with_idx{"mongolian", 50},
        string_with_idx{"mongolian_sup", 230},
        string_with_idx{"mongolian_supplement", 230},
        string_with_idx{"mro", 256},
        string_with_idx{"multani", 223},
        string_with_idx{"music", 276},
        string_with_idx{"musical_symbols", 276},
        string_with_idx{"myanmar", 36},
        string_with_idx{"myanmar_ext_a", 141},
        string_with_idx{"myanmar_ext_b", 139},
        string_with_idx{"myanmar_extended_a", 141},
        string_with_idx{"myanmar_extended_b", 139},
        string_with_idx{"nabataean", 191},
        string_with_idx{"nandinagari", 236},
        string_with_idx{"nb", 0},
        string_with_idx{"new_tai_lue", 54},
        string_with_idx{"newa", 226},
        string_with_idx{"nko", 17},
        string_with_idx{"no_block", 0},
        string_with_idx{"number_forms", 78},
        string_with_idx{"nushu", 271},
        string_with_idx{"nyiakeng_puachue_hmong", 285},
        string_with_idx{"ocr", 83},
        string_with_idx{"ogham", 43},
        string_with_idx{"ol_chiki", 63},
        string_with_idx{"old_hungarian", 206},
        string_with_idx{"old_italic", 174},
        string_with_idx{"old_north_arabian", 199},
        string_with_idx{"old_permic", 176},
        string_with_idx{"old_persian", 178},
        string_with_idx{"old_sogdian", 210},
        string_with_idx{"old_south_arabian", 198},
        string_with_idx{"old_turkic", 205},
        string_with_idx{"old_uyghur", 212},
        string_with_idx{"optical_character_recognition", 83},
        string_with_idx{"oriya", 27},
        string_with_idx{"ornamental_dingbats", 301},
        string_with_idx{"osage", 182},
        string_with_idx{"osmanya", 181},
        string_with_idx{"ottoman_siyaq_numbers", 292},
        string_with_idx{"pahawh_hmong", 259},
        string_with_idx{"palmyrene", 190},
        string_with_idx{"pau_cin_hau", 240},
        string_with_idx{"phags_pa", 132},
        string_with_idx{"phaistos", 170},
        string_with_idx{"phaistos_disc", 170},
        string_with_idx{"phoenician", 193},
        string_with_idx{"phonetic_ext", 68},
        string_with_idx{"phonetic_ext_sup", 69},
        string_with_idx{"phonetic_extensions", 68},
        string_with_idx{"phonetic_extensions_supplement", 69},
        string_with_idx{"playing_cards", 296},
        string_with_idx{"private_use_area", 153},
        string_with_idx{"psalter_pahlavi", 204},
        string_with_idx{"pua", 153},
        string_with_idx{"punctuation", 73},
        string_with_idx{"rejang", 136},
        string_with_idx{"rumi", 208},
        string_with_idx{"rumi_numeral_symbols", 208},
        string_with_idx{"runic", 44},
        string_with_idx{"samaritan", 18},
        string_with_idx{"saurashtra", 133},
        string_with_idx{"sharada", 220},
        string_with_idx{"shavian", 180},
        string_with_idx{"shorthand_format_controls", 273},
        string_with_idx{"siddham", 228},
        string_with_idx{"sinhala", 32},
        string_with_idx{"sinhala_archaic_numbers", 221},
        string_with_idx{"small_form_variants", 161},
        string_with_idx{"small_forms", 161},
        string_with_idx{"small_kana_ext", 270},
        string_with_idx{"small_kana_extension", 270},
        string_with_idx{"sogdian", 211},
        string_with_idx{"sora_sompeng", 217},
        string_with_idx{"soyombo", 238},
        string_with_idx{"spacing_modifier_letters", 6},
        string_with_idx{"specials", 164},
        string_with_idx{"sundanese", 60},
        string_with_idx{"sundanese_sup", 66},
        string_with_idx{"sundanese_supplement", 66},
        string_with_idx{"sup_arrows_a", 91},
        string_with_idx{"sup_arrows_b", 93},
        string_with_idx{"sup_arrows_c", 305},
        string_with_idx{"sup_math_operators", 95},
        string_with_idx{"sup_pua_a", 319},
        string_with_idx{"sup_pua_b", 320},
        string_with_idx{"sup_punctuation", 104},
        string_with_idx{"sup_symbols_and_pictographs", 306},
        string_with_idx{"super_and_sub", 74},
        string_with_idx{"superscripts_and_subscripts", 74},
        string_with_idx{"supplemental_arrows_a", 91},
        string_with_idx{"supplemental_arrows_b", 93},
        string_with_idx{"supplemental_arrows_c", 305},
        string_with_idx{"supplemental_mathematical_operators", 95},
        string_with_idx{"supplemental_punctuation", 104},
        string_with_idx{"supplemental_symbols_and_pictographs", 306},
        string_with_idx{"supplementary_private_use_area_a", 319},
        string_with_idx{"supplementary_private_use_area_b", 320},
        string_with_idx{"sutton_signwriting", 282},
        string_with_idx{"syloti_nagri", 130},
        string_with_idx{"symbols_and_pictographs_ext_a", 308},
        string_with_idx{"symbols_and_pictographs_extended_a", 308},
        string_with_idx{"symbols_for_legacy_computing", 309},
        string_with_idx{"syriac", 14},
        string_with_idx{"syriac_sup", 20},
        string_with_idx{"syriac_supplement", 20},
        string_with_idx{"tagalog", 45},
        string_with_idx{"tagbanwa", 48},
        string_with_idx{"tags", 317},
        string_with_idx{"tai_le", 53},
        string_with_idx{"tai_tham", 57},
        string_with_idx{"tai_viet", 142},
        string_with_idx{"tai_xuan_jing", 279},
        string_with_idx{"tai_xuan_jing_symbols", 279},
        string_with_idx{"takri", 231},
        string_with_idx{"tamil", 28},
        string_with_idx{"tamil_sup", 247},
        string_with_idx{"tamil_supplement", 247},
        string_with_idx{"tangsa", 257},
        string_with_idx{"tangut", 263},
        string_with_idx{"tangut_components", 264},
        string_with_idx{"tangut_sup", 266},
        string_with_idx{"tangut_supplement", 266},
        string_with_idx{"telugu", 29},
        string_with_idx{"thaana", 16},
        string_with_idx{"thai", 33},
        string_with_idx{"tibetan", 35},
        string_with_idx{"tifinagh", 101},
        string_with_idx{"tirhuta", 227},
        string_with_idx{"toto", 286},
        string_with_idx{"transport_and_map", 302},
        string_with_idx{"transport_and_map_symbols", 302},
        string_with_idx{"ucas", 42},
        string_with_idx{"ucas_ext", 51},
        string_with_idx{"ucas_ext_a", 239},
        string_with_idx{"ugaritic", 177},
        string_with_idx{"unified_canadian_aboriginal_syllabics", 42},
        string_with_idx{"unified_canadian_aboriginal_syllabics_extended", 51},
        string_with_idx{"unified_canadian_aboriginal_syllabics_extended_a", 239},
        string_with_idx{"vai", 125},
        string_with_idx{"variation_selectors", 157},
        string_with_idx{"variation_selectors_supplement", 318},
        string_with_idx{"vedic_ext", 67},
        string_with_idx{"vedic_extensions", 67},
        string_with_idx{"vertical_forms", 158},
        string_with_idx{"vithkuqi", 185},
        string_with_idx{"vs", 157},
        string_with_idx{"vs_sup", 318},
        string_with_idx{"wancho", 287},
        string_with_idx{"warang_citi", 234},
        string_with_idx{"yezidi", 209},
        string_with_idx{"yi_radicals", 123},
        string_with_idx{"yi_syllables", 122},
        string_with_idx{"yijing", 120},
        string_with_idx{"yijing_hexagram_symbols", 120},
        string_with_idx{"zanabazar_square", 237},
        string_with_idx{"znamenny_music", 274},
        string_with_idx{"znamenny_musical_notation", 274}};
    static constexpr const compact_range block_data = {
        0x00000001, 0x00008001, 0x00010001, 0x00018001, 0x00025001, 0x0002B001, 0x00030001,
        0x00037001, 0x00040001, 0x00050001, 0x00053001, 0x00059001, 0x00060001, 0x00070001,
        0x00075001, 0x00078001, 0x0007C001, 0x00080001, 0x00084001, 0x00086001, 0x00087001,
        0x0008A001, 0x00090001, 0x00098001, 0x000A0001, 0x000A8001, 0x000B0001, 0x000B8001,
        0x000C0001, 0x000C8001, 0x000D0001, 0x000D8001, 0x000E0001, 0x000E8001, 0x000F0001,
        0x00100001, 0x0010A001, 0x00110001, 0x00120001, 0x00138001, 0x0013A001, 0x00140001,
        0x00168001, 0x0016A001, 0x00170001, 0x00172001, 0x00174001, 0x00176001, 0x00178001,
        0x00180001, 0x0018B001, 0x00190001, 0x00195001, 0x00198001, 0x0019E001, 0x001A0001,
        0x001A2001, 0x001AB001, 0x001B0001, 0x001B8001, 0x001BC001, 0x001C0001, 0x001C5001,
        0x001C8001, 0x001C9001, 0x001CC001, 0x001CD001, 0x001D0001, 0x001D8001, 0x001DC001,
        0x001E0001, 0x001F0001, 0x00200001, 0x00207001, 0x0020A001, 0x0020D001, 0x00210001,
        0x00215001, 0x00219001, 0x00220001, 0x00230001, 0x00240001, 0x00244001, 0x00246001,
        0x00250001, 0x00258001, 0x0025A001, 0x00260001, 0x00270001, 0x0027C001, 0x0027F001,
        0x00280001, 0x00290001, 0x00298001, 0x002A0001, 0x002B0001, 0x002C0001, 0x002C6001,
        0x002C8001, 0x002D0001, 0x002D3001, 0x002D8001, 0x002DE001, 0x002E0001, 0x002E8001,
        0x002F0001, 0x002FE000, 0x002FF002, 0x00300002, 0x00304002, 0x0030A002, 0x00310002,
        0x00313002, 0x00319002, 0x0031A002, 0x0031C002, 0x0031F002, 0x00320002, 0x00330002,
        0x00340002, 0x004DC002, 0x004E0002, 0x00A00002, 0x00A49002, 0x00A4D002, 0x00A50002,
        0x00A64002, 0x00A6A002, 0x00A70002, 0x00A72002, 0x00A80002, 0x00A83002, 0x00A84002,
        0x00A88002, 0x00A8E002, 0x00A90002, 0x00A93002, 0x00A96002, 0x00A98002, 0x00A9E002,
        0x00AA0002, 0x00AA6002, 0x00AA8002, 0x00AAE002, 0x00AB0002, 0x00AB3002, 0x00AB7002,
        0x00ABC002, 0x00AC0002, 0x00D7B002, 0x00D80002, 0x00DB8002, 0x00DC0002, 0x00E00002,
        0x00F90002, 0x00FB0002, 0x00FB5002, 0x00FE0002, 0x00FE1002, 0x00FE2002, 0x00FE3002,
        0x00FE5002, 0x00FE7002, 0x00FF0002, 0x00FFF002, 0x01000002, 0x01008002, 0x01010002,
        0x01014002, 0x01019002, 0x0101D002, 0x01020000, 0x01028003, 0x0102A003, 0x0102E003,
        0x01030003, 0x01033003, 0x01035003, 0x01038003, 0x0103A003, 0x0103E000, 0x01040004,
        0x01045004, 0x01048004, 0x0104B004, 0x01050004, 0x01053004, 0x01057004, 0x0105C000,
        0x01060005, 0x01078005, 0x0107C000, 0x01080006, 0x01084006, 0x01086006, 0x01088006,
        0x0108B000, 0x0108E007, 0x01090007, 0x01092007, 0x01094000, 0x01098008, 0x0109A008,
        0x010A0008, 0x010A6008, 0x010A8008, 0x010AA000, 0x010AC009, 0x010B0009, 0x010B4009,
        0x010B6009, 0x010B8009, 0x010BB000, 0x010C000A, 0x010C5000, 0x010C800B, 0x010D000B,
        0x010D4000, 0x010E600C, 0x010E800C, 0x010EC000, 0x010F000D, 0x010F300D, 0x010F700D,
        0x010FB00D, 0x010FE00D, 0x0110000D, 0x0110800D, 0x0110D00D, 0x0111000D, 0x0111500D,
        0x0111800D, 0x0111E00D, 0x0112000D, 0x01125000, 0x0112800E, 0x0112B00E, 0x0113000E,
        0x01138000, 0x0114000F, 0x0114800F, 0x0114E000, 0x01158010, 0x01160010, 0x01166010,
        0x01168010, 0x0116D000, 0x01170011, 0x01175000, 0x01180012, 0x01185000, 0x0118A013,
        0x01190013, 0x01196000, 0x0119A014, 0x011A0014, 0x011A5014, 0x011AB014, 0x011AC014,
        0x011B0000, 0x011C0015, 0x011C7015, 0x011CC000, 0x011D0016, 0x011D6016, 0x011DB000,
        0x011EE017, 0x011F0000, 0x011FB018, 0x011FC018, 0x01200018, 0x01240018, 0x01248018,
        0x01255000, 0x012F9019, 0x01300019, 0x01343019, 0x01344000, 0x0144001A, 0x01468000,
        0x0168001B, 0x016A401B, 0x016A701B, 0x016AD01B, 0x016B001B, 0x016B9000, 0x016E401C,
        0x016EA000, 0x016F001D, 0x016FA000, 0x016FE01E, 0x0170001E, 0x0188001E, 0x018B001E,
        0x018D001E, 0x018D8000, 0x01AFF01F, 0x01B0001F, 0x01B1001F, 0x01B1301F, 0x01B1701F,
        0x01B30000, 0x01BC0020, 0x01BCA020, 0x01BCB000, 0x01CF0021, 0x01CFD000, 0x01D00022,
        0x01D10022, 0x01D20022, 0x01D25000, 0x01D2E023, 0x01D30023, 0x01D36023, 0x01D38000,
        0x01D40024, 0x01D80024, 0x01DAB000, 0x01DF0025, 0x01E00025, 0x01E03000, 0x01E10026,
        0x01E15000, 0x01E29027, 0x01E2C027, 0x01E30000, 0x01E7E028, 0x01E80028, 0x01E8E000,
        0x01E90029, 0x01E96000, 0x01EC702A, 0x01ECC000, 0x01ED002B, 0x01ED5000, 0x01EE002C,
        0x01EF0000, 0x01F0002D, 0x01F0302D, 0x01F0A02D, 0x01F1002D, 0x01F2002D, 0x01F3002D,
        0x01F6002D, 0x01F6502D, 0x01F6802D, 0x01F7002D, 0x01F7802D, 0x01F8002D, 0x01F9002D,
        0x01FA002D, 0x01FA702D, 0x01FB002D, 0x01FC0000, 0x0200002E, 0x02A6E000, 0x02A7002F,
        0x02B7402F, 0x02B8202F, 0x02CEB02F, 0x02EBF000, 0x02F80030, 0x02FA2000, 0x03000031,
        0x03135000, 0x0E000032, 0x0E008000, 0x0E010033, 0x0E01F000, 0x0F000034, 0x10000034,
        0xFFFFFFFF};
    static constexpr string_with_idx scripts_names[] = {
        string_with_idx{"adlam", 0},
        string_with_idx{"adlm", 0},
        string_with_idx{"aghb", 1},
        string_with_idx{"ahom", 2},
        string_with_idx{"anatolian_hieroglyphs", 52},
        string_with_idx{"arab", 3},
        string_with_idx{"arabic", 3},
        string_with_idx{"armenian", 5},
        string_with_idx{"armi", 4},
        string_with_idx{"armn", 5},
        string_with_idx{"avestan", 6},
        string_with_idx{"avst", 6},
        string_with_idx{"bali", 7},
        string_with_idx{"balinese", 7},
        string_with_idx{"bamu", 8},
        string_with_idx{"bamum", 8},
        string_with_idx{"bass", 9},
        string_with_idx{"bassa_vah", 9},
        string_with_idx{"batak", 10},
        string_with_idx{"batk", 10},
        string_with_idx{"beng", 11},
        string_with_idx{"bengali", 11},
        string_with_idx{"bhaiksuki", 12},
        string_with_idx{"bhks", 12},
        string_with_idx{"bopo", 13},
        string_with_idx{"bopomofo", 13},
        string_with_idx{"brah", 14},
        string_with_idx{"brahmi", 14},
        string_with_idx{"brai", 15},
        string_with_idx{"braille", 15},
        string_with_idx{"bugi", 16},
        string_with_idx{"buginese", 16},
        string_with_idx{"buhd", 17},
        string_with_idx{"buhid", 17},
        string_with_idx{"cakm", 18},
        string_with_idx{"canadian_aboriginal", 19},
        string_with_idx{"cans", 19},
        string_with_idx{"cari", 20},
        string_with_idx{"carian", 20},
        string_with_idx{"caucasian_albanian", 1},
        string_with_idx{"chakma", 18},
        string_with_idx{"cham", 21},
        string_with_idx{"cher", 22},
        string_with_idx{"cherokee", 22},
        string_with_idx{"chorasmian", 23},
        string_with_idx{"chrs", 23},
        string_with_idx{"common", 161},
        string_with_idx{"copt", 24},
        string_with_idx{"coptic", 24},
        string_with_idx{"cpmn", 25},
        string_with_idx{"cprt", 26},
        string_with_idx{"cuneiform", 156},
        string_with_idx{"cypriot", 26},
        string_with_idx{"cypro_minoan", 25},
        string_with_idx{"cyrillic", 27},
        string_with_idx{"cyrl", 27},
        string_with_idx{"deseret", 31},
        string_with_idx{"deva", 28},
        string_with_idx{"devanagari", 28},
        string_with_idx{"diak", 29},
        string_with_idx{"dives_akuru", 29},
        string_with_idx{"dogr", 30},
        string_with_idx{"dogra", 30},
        string_with_idx{"dsrt", 31},
        string_with_idx{"dupl", 32},
        string_with_idx{"duployan", 32},
        string_with_idx{"egyp", 33},
        string_with_idx{"egyptian_hieroglyphs", 33},
        string_with_idx{"elba", 34},
        string_with_idx{"elbasan", 34},
        string_with_idx{"elym", 35},
        string_with_idx{"elymaic", 35},
        string_with_idx{"ethi", 36},
        string_with_idx{"ethiopic", 36},
        string_with_idx{"geor", 37},
        string_with_idx{"georgian", 37},
        string_with_idx{"glag", 38},
        string_with_idx{"glagolitic", 38},
        string_with_idx{"gong", 39},
        string_with_idx{"gonm", 40},
        string_with_idx{"goth", 41},
        string_with_idx{"gothic", 41},
        string_with_idx{"gran", 42},
        string_with_idx{"grantha", 42},
        string_with_idx{"greek", 43},
        string_with_idx{"grek", 43},
        string_with_idx{"gujarati", 44},
        string_with_idx{"gujr", 44},
        string_with_idx{"gunjala_gondi", 39},
        string_with_idx{"gurmukhi", 45},
        string_with_idx{"guru", 45},
        string_with_idx{"han", 47},
        string_with_idx{"hang", 46},
        string_with_idx{"hangul", 46},
        string_with_idx{"hani", 47},
        string_with_idx{"hanifi_rohingya", 116},
        string_with_idx{"hano", 48},
        string_with_idx{"hanunoo", 48},
        string_with_idx{"hatr", 49},
        string_with_idx{"hatran", 49},
        string_with_idx{"hebr", 50},
        string_with_idx{"hebrew", 50},
        string_with_idx{"hira", 51},
        string_with_idx{"hiragana", 51},
        string_with_idx{"hluw", 52},
        string_with_idx{"hmng", 53},
        string_with_idx{"hmnp", 54},
        string_with_idx{"hrkt", 55},
        string_with_idx{"hung", 56},
        string_with_idx{"imperial_aramaic", 4},
        string_with_idx{"inherited", 160},
        string_with_idx{"inscriptional_pahlavi", 110},
        string_with_idx{"inscriptional_parthian", 114},
        string_with_idx{"ital", 57},
        string_with_idx{"java", 58},
        string_with_idx{"javanese", 58},
        string_with_idx{"kaithi", 66},
        string_with_idx{"kali", 59},
        string_with_idx{"kana", 60},
        string_with_idx{"kannada", 65},
        string_with_idx{"katakana", 60},
        string_with_idx{"katakana_or_hiragana", 55},
        string_with_idx{"kayah_li", 59},
        string_with_idx{"khar", 61},
        string_with_idx{"kharoshthi", 61},
        string_with_idx{"khitan_small_script", 64},
        string_with_idx{"khmer", 62},
        string_with_idx{"khmr", 62},
        string_with_idx{"khoj", 63},
        string_with_idx{"khojki", 63},
        string_with_idx{"khudawadi", 125},
        string_with_idx{"kits", 64},
        string_with_idx{"knda", 65},
        string_with_idx{"kthi", 66},
        string_with_idx{"lana", 67},
        string_with_idx{"lao", 68},
        string_with_idx{"laoo", 68},
        string_with_idx{"latin", 69},
        string_with_idx{"latn", 69},
        string_with_idx{"lepc", 70},
        string_with_idx{"lepcha", 70},
        string_with_idx{"limb", 71},
        string_with_idx{"limbu", 71},
        string_with_idx{"lina", 72},
        string_with_idx{"linb", 73},
        string_with_idx{"linear_a", 72},
        string_with_idx{"linear_b", 73},
        string_with_idx{"lisu", 74},
        string_with_idx{"lyci", 75},
        string_with_idx{"lycian", 75},
        string_with_idx{"lydi", 76},
        string_with_idx{"lydian", 76},
        string_with_idx{"mahajani", 77},
        string_with_idx{"mahj", 77},
        string_with_idx{"maka", 78},
        string_with_idx{"makasar", 78},
        string_with_idx{"malayalam", 86},
        string_with_idx{"mand", 79},
        string_with_idx{"mandaic", 79},
        string_with_idx{"mani", 80},
        string_with_idx{"manichaean", 80},
        string_with_idx{"marc", 81},
        string_with_idx{"marchen", 81},
        string_with_idx{"masaram_gondi", 40},
        string_with_idx{"medefaidrin", 82},
        string_with_idx{"medf", 82},
        string_with_idx{"meetei_mayek", 90},
        string_with_idx{"mend", 83},
        string_with_idx{"mende_kikakui", 83},
        string_with_idx{"merc", 84},
        string_with_idx{"mero", 85},
        string_with_idx{"meroitic_cursive", 84},
        string_with_idx{"meroitic_hieroglyphs", 85},
        string_with_idx{"miao", 113},
        string_with_idx{"mlym", 86},
        string_with_idx{"modi", 87},
        string_with_idx{"mong", 88},
        string_with_idx{"mongolian", 88},
        string_with_idx{"mro", 89},
        string_with_idx{"mroo", 89},
        string_with_idx{"mtei", 90},
        string_with_idx{"mult", 91},
        string_with_idx{"multani", 91},
        string_with_idx{"myanmar", 92},
        string_with_idx{"mymr", 92},
        string_with_idx{"nabataean", 95},
        string_with_idx{"nand", 93},
        string_with_idx{"nandinagari", 93},
        string_with_idx{"narb", 94},
        string_with_idx{"nbat", 95},
        string_with_idx{"new_tai_lue", 137},
        string_with_idx{"newa", 96},
        string_with_idx{"nko", 97},
        string_with_idx{"nkoo", 97},
        string_with_idx{"nshu", 98},
        string_with_idx{"nushu", 98},
        string_with_idx{"nyiakeng_puachue_hmong", 54},
        string_with_idx{"ogam", 99},
        string_with_idx{"ogham", 99},
        string_with_idx{"ol_chiki", 100},
        string_with_idx{"olck", 100},
        string_with_idx{"old_hungarian", 56},
        string_with_idx{"old_italic", 57},
        string_with_idx{"old_north_arabian", 94},
        string_with_idx{"old_permic", 108},
        string_with_idx{"old_persian", 155},
        string_with_idx{"old_sogdian", 128},
        string_with_idx{"old_south_arabian", 119},
        string_with_idx{"old_turkic", 101},
        string_with_idx{"old_uyghur", 105},
        string_with_idx{"oriya", 102},
        string_with_idx{"orkh", 101},
        string_with_idx{"orya", 102},
        string_with_idx{"osage", 103},
        string_with_idx{"osge", 103},
        string_with_idx{"osma", 104},
        string_with_idx{"osmanya", 104},
        string_with_idx{"ougr", 105},
        string_with_idx{"pahawh_hmong", 53},
        string_with_idx{"palm", 106},
        string_with_idx{"palmyrene", 106},
        string_with_idx{"pau_cin_hau", 107},
        string_with_idx{"pauc", 107},
        string_with_idx{"perm", 108},
        string_with_idx{"phag", 109},
        string_with_idx{"phags_pa", 109},
        string_with_idx{"phli", 110},
        string_with_idx{"phlp", 111},
        string_with_idx{"phnx", 112},
        string_with_idx{"phoenician", 112},
        string_with_idx{"plrd", 113},
        string_with_idx{"prti", 114},
        string_with_idx{"psalter_pahlavi", 111},
        string_with_idx{"rejang", 115},
        string_with_idx{"rjng", 115},
        string_with_idx{"rohg", 116},
        string_with_idx{"runic", 117},
        string_with_idx{"runr", 117},
        string_with_idx{"samaritan", 118},
        string_with_idx{"samr", 118},
        string_with_idx{"sarb", 119},
        string_with_idx{"saur", 120},
        string_with_idx{"saurashtra", 120},
        string_with_idx{"sgnw", 121},
        string_with_idx{"sharada", 123},
        string_with_idx{"shavian", 122},
        string_with_idx{"shaw", 122},
        string_with_idx{"shrd", 123},
        string_with_idx{"sidd", 124},
        string_with_idx{"siddham", 124},
        string_with_idx{"signwriting", 121},
        string_with_idx{"sind", 125},
        string_with_idx{"sinh", 126},
        string_with_idx{"sinhala", 126},
        string_with_idx{"sogd", 127},
        string_with_idx{"sogdian", 127},
        string_with_idx{"sogo", 128},
        string_with_idx{"sora", 129},
        string_with_idx{"sora_sompeng", 129},
        string_with_idx{"soyo", 130},
        string_with_idx{"soyombo", 130},
        string_with_idx{"sund", 131},
        string_with_idx{"sundanese", 131},
        string_with_idx{"sylo", 132},
        string_with_idx{"syloti_nagri", 132},
        string_with_idx{"syrc", 133},
        string_with_idx{"syriac", 133},
        string_with_idx{"tagalog", 143},
        string_with_idx{"tagb", 134},
        string_with_idx{"tagbanwa", 134},
        string_with_idx{"tai_le", 136},
        string_with_idx{"tai_tham", 67},
        string_with_idx{"tai_viet", 140},
        string_with_idx{"takr", 135},
        string_with_idx{"takri", 135},
        string_with_idx{"tale", 136},
        string_with_idx{"talu", 137},
        string_with_idx{"tamil", 138},
        string_with_idx{"taml", 138},
        string_with_idx{"tang", 139},
        string_with_idx{"tangsa", 148},
        string_with_idx{"tangut", 139},
        string_with_idx{"tavt", 140},
        string_with_idx{"telu", 141},
        string_with_idx{"telugu", 141},
        string_with_idx{"tfng", 142},
        string_with_idx{"tglg", 143},
        string_with_idx{"thaa", 144},
        string_with_idx{"thaana", 144},
        string_with_idx{"thai", 145},
        string_with_idx{"tibetan", 146},
        string_with_idx{"tibt", 146},
        string_with_idx{"tifinagh", 142},
        string_with_idx{"tirh", 147},
        string_with_idx{"tirhuta", 147},
        string_with_idx{"tnsa", 148},
        string_with_idx{"toto", 149},
        string_with_idx{"ugar", 150},
        string_with_idx{"ugaritic", 150},
        string_with_idx{"unknown", 162},
        string_with_idx{"vai", 151},
        string_with_idx{"vaii", 151},
        string_with_idx{"vith", 152},
        string_with_idx{"vithkuqi", 152},
        string_with_idx{"wancho", 154},
        string_with_idx{"wara", 153},
        string_with_idx{"warang_citi", 153},
        string_with_idx{"wcho", 154},
        string_with_idx{"xpeo", 155},
        string_with_idx{"xsux", 156},
        string_with_idx{"yezi", 157},
        string_with_idx{"yezidi", 157},
        string_with_idx{"yi", 158},
        string_with_idx{"yiii", 158},
        string_with_idx{"zanabazar_square", 159},
        string_with_idx{"zanb", 159},
        string_with_idx{"zinh", 160},
        string_with_idx{"zyyy", 161},
        string_with_idx{"zzzz", 162}};
    template<auto N>
    struct script_data;
    template<>
    struct script_data<0> {
        static constexpr const compact_range scripts_data = {
            0x000000A1, 0x00004145, 0x00005BA1, 0x00006145, 0x00007BA1, 0x0000AA45, 0x0000ABA1,
            0x0000BA45, 0x0000BBA1, 0x0000C045, 0x0000D7A1, 0x0000D845, 0x0000F7A1, 0x0000F845,
            0x0002B9A1, 0x0002E045, 0x0002E5A1, 0x0002EA0D, 0x0002ECA1, 0x000300A0, 0x0003702B,
            0x000374A1, 0x0003752B, 0x000378A2, 0x00037A2B, 0x00037EA1, 0x00037F2B, 0x000380A2,
            0x0003842B, 0x000385A1, 0x0003862B, 0x000387A1, 0x0003882B, 0x00038BA2, 0x00038C2B,
            0x00038DA2, 0x00038E2B, 0x0003A2A2, 0x0003A32B, 0x0003E218, 0x0003F02B, 0x0004001B,
            0x000485A0, 0x0004871B, 0x000530A2, 0x00053105, 0x000557A2, 0x00055905, 0x00058BA2,
            0x00058D05, 0x000590A2, 0x00059132, 0x0005C8A2, 0x0005D032, 0x0005EBA2, 0x0005EF32,
            0x0005F5A2, 0x00060003, 0x000605A1, 0x00060603, 0x00060CA1, 0x00060D03, 0x00061BA1,
            0x00061C03, 0x00061FA1, 0x00062003, 0x000640A1, 0x00064103, 0x00064BA0, 0x00065603,
            0x000670A0, 0x00067103, 0x0006DDA1, 0x0006DE03, 0x00070085, 0x00070EA2, 0x00070F85,
            0x00074BA2, 0x00074D85, 0x00075003, 0x00078090, 0x0007B2A2, 0x0007C061, 0x0007FBA2,
            0x0007FD61, 0x00080076, 0x00082EA2, 0x00083076, 0x00083FA2, 0x0008404F, 0x00085CA2,
            0x00085E4F, 0x00085FA2, 0x00086085, 0x00086BA2, 0x00087003, 0x00088FA2, 0x00089003,
            0x000892A2, 0x00089803, 0x0008E2A1, 0x0008E303, 0x0009001C, 0x000951A0, 0x0009551C,
            0x000964A1, 0x0009661C, 0x0009800B, 0x000984A2, 0x0009850B, 0x00098DA2, 0x00098F0B,
            0x000991A2, 0x0009930B, 0x0009A9A2, 0x0009AA0B, 0x0009B1A2, 0x0009B20B, 0x0009B3A2,
            0x0009B60B, 0x0009BAA2, 0x0009BC0B, 0x0009C5A2, 0x0009C70B, 0x0009C9A2, 0x0009CB0B,
            0x0009CFA2, 0x0009D70B, 0x0009D8A2, 0x0009DC0B, 0x0009DEA2, 0x0009DF0B, 0x0009E4A2,
            0x0009E60B, 0x0009FFA2, 0x000A012D, 0x000A04A2, 0x000A052D, 0x000A0BA2, 0x000A0F2D,
            0x000A11A2, 0x000A132D, 0x000A29A2, 0x000A2A2D, 0x000A31A2, 0x000A322D, 0x000A34A2,
            0x000A352D, 0x000A37A2, 0x000A382D, 0x000A3AA2, 0x000A3C2D, 0x000A3DA2, 0x000A3E2D,
            0x000A43A2, 0x000A472D, 0x000A49A2, 0x000A4B2D, 0x000A4EA2, 0x000A512D, 0x000A52A2,
            0x000A592D, 0x000A5DA2, 0x000A5E2D, 0x000A5FA2, 0x000A662D, 0x000A77A2, 0x000A812C,
            0x000A84A2, 0x000A852C, 0x000A8EA2, 0x000A8F2C, 0x000A92A2, 0x000A932C, 0x000AA9A2,
            0x000AAA2C, 0x000AB1A2, 0x000AB22C, 0x000AB4A2, 0x000AB52C, 0x000ABAA2, 0x000ABC2C,
            0x000AC6A2, 0x000AC72C, 0x000ACAA2, 0x000ACB2C, 0x000ACEA2, 0x000AD02C, 0x000AD1A2,
            0x000AE02C, 0x000AE4A2, 0x000AE62C, 0x000AF2A2, 0x000AF92C, 0x000B00A2, 0x000B0166,
            0x000B04A2, 0x000B0566, 0x000B0DA2, 0x000B0F66, 0x000B11A2, 0x000B1366, 0x000B29A2,
            0x000B2A66, 0x000B31A2, 0x000B3266, 0x000B34A2, 0x000B3566, 0x000B3AA2, 0x000B3C66,
            0x000B45A2, 0x000B4766, 0x000B49A2, 0x000B4B66, 0x000B4EA2, 0x000B5566, 0x000B58A2,
            0x000B5C66, 0x000B5EA2, 0x000B5F66, 0x000B64A2, 0x000B6666, 0x000B78A2, 0x000B828A,
            0x000B84A2, 0x000B858A, 0x000B8BA2, 0x000B8E8A, 0x000B91A2, 0x000B928A, 0x000B96A2,
            0x000B998A, 0x000B9BA2, 0x000B9C8A, 0x000B9DA2, 0x000B9E8A, 0x000BA0A2, 0x000BA38A,
            0x000BA5A2, 0x000BA88A, 0x000BABA2, 0x000BAE8A, 0x000BBAA2, 0x000BBE8A, 0x000BC3A2,
            0x000BC68A, 0x000BC9A2, 0x000BCA8A, 0x000BCEA2, 0x000BD08A, 0x000BD1A2, 0x000BD78A,
            0x000BD8A2, 0x000BE68A, 0x000BFBA2, 0x000C008D, 0x000C0DA2, 0x000C0E8D, 0x000C11A2,
            0x000C128D, 0x000C29A2, 0x000C2A8D, 0x000C3AA2, 0x000C3C8D, 0x000C45A2, 0x000C468D,
            0x000C49A2, 0x000C4A8D, 0x000C4EA2, 0x000C558D, 0x000C57A2, 0x000C588D, 0x000C5BA2,
            0x000C5D8D, 0x000C5EA2, 0x000C608D, 0x000C64A2, 0x000C668D, 0x000C70A2, 0x000C778D,
            0x000C8041, 0x000C8DA2, 0x000C8E41, 0x000C91A2, 0x000C9241, 0x000CA9A2, 0x000CAA41,
            0x000CB4A2, 0x000CB541, 0x000CBAA2, 0x000CBC41, 0x000CC5A2, 0x000CC641, 0x000CC9A2,
            0x000CCA41, 0x000CCEA2, 0x000CD541, 0x000CD7A2, 0x000CDD41, 0x000CDFA2, 0x000CE041,
            0x000CE4A2, 0x000CE641, 0x000CF0A2, 0x000CF141, 0x000CF3A2, 0x000D0056, 0x000D0DA2,
            0x000D0E56, 0x000D11A2, 0x000D1256, 0x000D45A2, 0x000D4656, 0x000D49A2, 0x000D4A56,
            0x000D50A2, 0x000D5456, 0x000D64A2, 0x000D6656, 0x000D80A2, 0x000D817E, 0x000D84A2,
            0x000D857E, 0x000D97A2, 0x000D9A7E, 0x000DB2A2, 0x000DB37E, 0x000DBCA2, 0x000DBD7E,
            0x000DBEA2, 0x000DC07E, 0x000DC7A2, 0x000DCA7E, 0x000DCBA2, 0x000DCF7E, 0x000DD5A2,
            0x000DD67E, 0x000DD7A2, 0x000DD87E, 0x000DE0A2, 0x000DE67E, 0x000DF0A2, 0x000DF27E,
            0x000DF5A2, 0x000E0191, 0x000E3BA2, 0x000E3FA1, 0x000E4091, 0x000E5CA2, 0x000E8144,
            0x000E83A2, 0x000E8444, 0x000E85A2, 0x000E8644, 0x000E8BA2, 0x000E8C44, 0x000EA4A2,
            0x000EA544, 0x000EA6A2, 0x000EA744, 0x000EBEA2, 0x000EC044, 0x000EC5A2, 0x000EC644,
            0x000EC7A2, 0x000EC844, 0x000ECEA2, 0x000ED044, 0x000EDAA2, 0x000EDC44, 0x000EE0A2,
            0x000F0092, 0x000F48A2, 0x000F4992, 0x000F6DA2, 0x000F7192, 0x000F98A2, 0x000F9992,
            0x000FBDA2, 0x000FBE92, 0x000FCDA2, 0x000FCE92, 0x000FD5A1, 0x000FD992, 0x000FDBA2,
            0x0010005C, 0x0010A025, 0x0010C6A2, 0x0010C725, 0x0010C8A2, 0x0010CD25, 0x0010CEA2,
            0x0010D025, 0x0010FBA1, 0x0010FC25, 0x0011002E, 0x00120024, 0x001249A2, 0x00124A24,
            0x00124EA2, 0x00125024, 0x001257A2, 0x00125824, 0x001259A2, 0x00125A24, 0x00125EA2,
            0x00126024, 0x001289A2, 0x00128A24, 0x00128EA2, 0x00129024, 0x0012B1A2, 0x0012B224,
            0x0012B6A2, 0x0012B824, 0x0012BFA2, 0x0012C024, 0x0012C1A2, 0x0012C224, 0x0012C6A2,
            0x0012C824, 0x0012D7A2, 0x0012D824, 0x001311A2, 0x00131224, 0x001316A2, 0x00131824,
            0x00135BA2, 0x00135D24, 0x00137DA2, 0x00138024, 0x00139AA2, 0x0013A016, 0x0013F6A2,
            0x0013F816, 0x0013FEA2, 0x00140013, 0x00168063, 0x00169DA2, 0x0016A075, 0x0016EBA1,
            0x0016EE75, 0x0016F9A2, 0x0017008F, 0x001716A2, 0x00171F8F, 0x00172030, 0x001735A1,
            0x001737A2, 0x00174011, 0x001754A2, 0x00176086, 0x00176DA2, 0x00176E86, 0x001771A2,
            0x00177286, 0x001774A2, 0x0017803E, 0x0017DEA2, 0x0017E03E, 0x0017EAA2, 0x0017F03E,
            0x0017FAA2, 0x00180058, 0x001802A1, 0x00180458, 0x001805A1, 0x00180658, 0x00181AA2,
            0x00182058, 0x001879A2, 0x00188058, 0x0018ABA2, 0x0018B013, 0x0018F6A2, 0x00190047,
            0x00191FA2, 0x00192047, 0x00192CA2, 0x00193047, 0x00193CA2, 0x00194047, 0x001941A2,
            0x00194447, 0x00195088, 0x00196EA2, 0x00197088, 0x001975A2, 0x00198089, 0x0019ACA2,
            0x0019B089, 0x0019CAA2, 0x0019D089, 0x0019DBA2, 0x0019DE89, 0x0019E03E, 0x001A0010,
            0x001A1CA2, 0x001A1E10, 0x001A2043, 0x001A5FA2, 0x001A6043, 0x001A7DA2, 0x001A7F43,
            0x001A8AA2, 0x001A9043, 0x001A9AA2, 0x001AA043, 0x001AAEA2, 0x001AB0A0, 0x001ACFA2,
            0x001B0007, 0x001B4DA2, 0x001B5007, 0x001B7FA2, 0x001B8083, 0x001BC00A, 0x001BF4A2,
            0x001BFC0A, 0x001C0046, 0x001C38A2, 0x001C3B46, 0x001C4AA2, 0x001C4D46, 0x001C5064,
            0x001C801B, 0x001C89A2, 0x001C9025, 0x001CBBA2, 0x001CBD25, 0x001CC083, 0x001CC8A2,
            0x001CD0A0, 0x001CD3A1, 0x001CD4A0, 0x001CE1A1, 0x001CE2A0, 0x001CE9A1, 0x001CEDA0,
            0x001CEEA1, 0x001CF4A0, 0x001CF5A1, 0x001CF8A0, 0x001CFAA1, 0x001CFBA2, 0x001D0045,
            0x001D262B, 0x001D2B1B, 0x001D2C45, 0x001D5D2B, 0x001D6245, 0x001D662B, 0x001D6B45,
            0x001D781B, 0x001D7945, 0x001DBF2B, 0x001DC0A0, 0x001E0045, 0x001F002B, 0x001F16A2,
            0x001F182B, 0x001F1EA2, 0x001F202B, 0x001F46A2, 0x001F482B, 0x001F4EA2, 0x001F502B,
            0x001F58A2, 0x001F592B, 0x001F5AA2, 0x001F5B2B, 0x001F5CA2, 0x001F5D2B, 0x001F5EA2,
            0x001F5F2B, 0x001F7EA2, 0x001F802B, 0x001FB5A2, 0x001FB62B, 0x001FC5A2, 0x001FC62B,
            0x001FD4A2, 0x001FD62B, 0x001FDCA2, 0x001FDD2B, 0x001FF0A2, 0x001FF22B, 0x001FF5A2,
            0x001FF62B, 0x001FFFA2, 0x002000A1, 0x00200CA0, 0x00200EA1, 0x002065A2, 0x002066A1,
            0x00207145, 0x002072A2, 0x002074A1, 0x00207F45, 0x002080A1, 0x00208FA2, 0x00209045,
            0x00209DA2, 0x0020A0A1, 0x0020C1A2, 0x0020D0A0, 0x0020F1A2, 0x002100A1, 0x0021262B,
            0x002127A1, 0x00212A45, 0x00212CA1, 0x00213245, 0x002133A1, 0x00214E45, 0x00214FA1,
            0x00216045, 0x002189A1, 0x00218CA2, 0x002190A1, 0x002427A2, 0x002440A1, 0x00244BA2,
            0x002460A1, 0x0028000F, 0x002900A1, 0x002B74A2, 0x002B76A1, 0x002B96A2, 0x002B97A1,
            0x002C0026, 0x002C6045, 0x002C8018, 0x002CF4A2, 0x002CF918, 0x002D0025, 0x002D26A2,
            0x002D2725, 0x002D28A2, 0x002D2D25, 0x002D2EA2, 0x002D308E, 0x002D68A2, 0x002D6F8E,
            0x002D71A2, 0x002D7F8E, 0x002D8024, 0x002D97A2, 0x002DA024, 0x002DA7A2, 0x002DA824,
            0x002DAFA2, 0x002DB024, 0x002DB7A2, 0x002DB824, 0x002DBFA2, 0x002DC024, 0x002DC7A2,
            0x002DC824, 0x002DCFA2, 0x002DD024, 0x002DD7A2, 0x002DD824, 0x002DDFA2, 0x002DE01B,
            0x002E00A1, 0x002E5EA2, 0x002E802F, 0x002E9AA2, 0x002E9B2F, 0x002EF4A2, 0x002F002F,
            0x002FD6A2, 0x002FF0A1, 0x002FFCA2, 0x003000A1, 0x0030052F, 0x003006A1, 0x0030072F,
            0x003008A1, 0x0030212F, 0x00302AA0, 0x00302E2E, 0x003030A1, 0x0030382F, 0x00303CA1,
            0x003040A2, 0x00304133, 0x003097A2, 0x003099A0, 0x00309BA1, 0x00309D33, 0x0030A0A1,
            0x0030A13C, 0x0030FBA1, 0x0030FD3C, 0x003100A2, 0x0031050D, 0x003130A2, 0x0031312E,
            0x00318FA2, 0x003190A1, 0x0031A00D, 0x0031C0A1, 0x0031E4A2, 0x0031F03C, 0x0032002E,
            0x00321FA2, 0x003220A1, 0x0032602E, 0x00327FA1, 0x0032D03C, 0x0032FFA1, 0x0033003C,
            0x003358A1, 0x0034002F, 0x004DC0A1, 0x004E002F, 0x00A0009E, 0x00A48DA2, 0x00A4909E,
            0x00A4C7A2, 0x00A4D04A, 0x00A50097, 0x00A62CA2, 0x00A6401B, 0x00A6A008, 0x00A6F8A2,
            0x00A700A1, 0x00A72245, 0x00A788A1, 0x00A78B45, 0x00A7CBA2, 0x00A7D045, 0x00A7D2A2,
            0x00A7D345, 0x00A7D4A2, 0x00A7D545, 0x00A7DAA2, 0x00A7F245, 0x00A80084, 0x00A82DA2,
            0x00A830A1, 0x00A83AA2, 0x00A8406D, 0x00A878A2, 0x00A88078, 0x00A8C6A2, 0x00A8CE78,
            0x00A8DAA2, 0x00A8E01C, 0x00A9003B, 0x00A92EA1, 0x00A92F3B, 0x00A93073, 0x00A954A2,
            0x00A95F73, 0x00A9602E, 0x00A97DA2, 0x00A9803A, 0x00A9CEA2, 0x00A9CFA1, 0x00A9D03A,
            0x00A9DAA2, 0x00A9DE3A, 0x00A9E05C, 0x00A9FFA2, 0x00AA0015, 0x00AA37A2, 0x00AA4015,
            0x00AA4EA2, 0x00AA5015, 0x00AA5AA2, 0x00AA5C15, 0x00AA605C, 0x00AA808C, 0x00AAC3A2,
            0x00AADB8C, 0x00AAE05A, 0x00AAF7A2, 0x00AB0124, 0x00AB07A2, 0x00AB0924, 0x00AB0FA2,
            0x00AB1124, 0x00AB17A2, 0x00AB2024, 0x00AB27A2, 0x00AB2824, 0x00AB2FA2, 0x00AB3045,
            0x00AB5BA1, 0x00AB5C45, 0x00AB652B, 0x00AB6645, 0x00AB6AA1, 0x00AB6CA2, 0x00AB7016,
            0x00ABC05A, 0x00ABEEA2, 0x00ABF05A, 0x00ABFAA2, 0x00AC002E, 0x00D7A4A2, 0x00D7B02E,
            0x00D7C7A2, 0x00D7CB2E, 0x00D7FCA2, 0x00F9002F, 0x00FA6EA2, 0x00FA702F, 0x00FADAA2,
            0x00FB0045, 0x00FB07A2, 0x00FB1305, 0x00FB18A2, 0x00FB1D32, 0x00FB37A2, 0x00FB3832,
            0x00FB3DA2, 0x00FB3E32, 0x00FB3FA2, 0x00FB4032, 0x00FB42A2, 0x00FB4332, 0x00FB45A2,
            0x00FB4632, 0x00FB5003, 0x00FBC3A2, 0x00FBD303, 0x00FD3EA1, 0x00FD4003, 0x00FD90A2,
            0x00FD9203, 0x00FDC8A2, 0x00FDCF03, 0x00FDD0A2, 0x00FDF003, 0x00FE00A0, 0x00FE10A1,
            0x00FE1AA2, 0x00FE20A0, 0x00FE2E1B, 0x00FE30A1, 0x00FE53A2, 0x00FE54A1, 0x00FE67A2,
            0x00FE68A1, 0x00FE6CA2, 0x00FE7003, 0x00FE75A2, 0x00FE7603, 0x00FEFDA2, 0x00FEFFA1,
            0x00FF00A2, 0x00FF01A1, 0x00FF2145, 0x00FF3BA1, 0x00FF4145, 0x00FF5BA1, 0x00FF663C,
            0x00FF70A1, 0x00FF713C, 0x00FF9EA1, 0x00FFA02E, 0x00FFBFA2, 0x00FFC22E, 0x00FFC8A2,
            0x00FFCA2E, 0x00FFD0A2, 0x00FFD22E, 0x00FFD8A2, 0x00FFDA2E, 0x00FFDDA2, 0x00FFE0A1,
            0x00FFE7A2, 0x00FFE8A1, 0x00FFEFA2, 0x00FFF9A1, 0x00FFFEA2, 0x01000049, 0x01000CA2,
            0x01000D49, 0x010027A2, 0x01002849, 0x01003BA2, 0x01003C49, 0x01003EA2, 0x01003F49,
            0x01004EA2, 0x01005049, 0x01005EA2, 0x01008049, 0x0100FBA2, 0x010100A1, 0x010103A2,
            0x010107A1, 0x010134A2, 0x010137A1, 0x0101402B, 0x01018FA2, 0x010190A1, 0x01019DA2,
            0x0101A02B, 0x0101A1A2, 0x0101D0A1, 0x0101FDA0, 0x0101FEA2, 0x0102804B, 0x01029DA2,
            0x0102A014, 0x0102D1A2, 0x0102E0A0, 0x0102E1A1, 0x0102FCA2, 0x01030039, 0x010324A2,
            0x01032D39, 0x01033029, 0x01034BA2, 0x0103506C, 0x01037BA2, 0x01038096, 0x01039EA2,
            0x01039F96, 0x0103A09B, 0x0103C4A2, 0x0103C89B, 0x0103D6A2, 0x0104001F, 0x0104507A,
            0x01048068, 0x01049EA2, 0x0104A068, 0x0104AAA2, 0x0104B067, 0x0104D4A2, 0x0104D867,
            0x0104FCA2, 0x01050022, 0x010528A2, 0x01053001, 0x010564A2, 0x01056F01, 0x01057098,
            0x01057BA2, 0x01057C98, 0x01058BA2, 0x01058C98, 0x010593A2, 0x01059498, 0x010596A2,
            0x01059798, 0x0105A2A2, 0x0105A398, 0x0105B2A2, 0x0105B398, 0x0105BAA2, 0x0105BB98,
            0x0105BDA2, 0x01060048, 0x010737A2, 0x01074048, 0x010756A2, 0x01076048, 0x010768A2,
            0x01078045, 0x010786A2, 0x01078745, 0x0107B1A2, 0x0107B245, 0x0107BBA2, 0x0108001A,
            0x010806A2, 0x0108081A, 0x010809A2, 0x01080A1A, 0x010836A2, 0x0108371A, 0x010839A2,
            0x01083C1A, 0x01083DA2, 0x01083F1A, 0x01084004, 0x010856A2, 0x01085704, 0x0108606A,
            0x0108805F, 0x01089FA2, 0x0108A75F, 0x0108B0A2, 0x0108E031, 0x0108F3A2, 0x0108F431,
            0x0108F6A2, 0x0108FB31, 0x01090070, 0x01091CA2, 0x01091F70, 0x0109204C, 0x01093AA2,
            0x01093F4C, 0x010940A2, 0x01098055, 0x0109A054, 0x0109B8A2, 0x0109BC54, 0x0109D0A2,
            0x0109D254, 0x010A003D, 0x010A04A2, 0x010A053D, 0x010A07A2, 0x010A0C3D, 0x010A14A2,
            0x010A153D, 0x010A18A2, 0x010A193D, 0x010A36A2, 0x010A383D, 0x010A3BA2, 0x010A3F3D,
            0x010A49A2, 0x010A503D, 0x010A59A2, 0x010A6077, 0x010A805E, 0x010AA0A2, 0x010AC050,
            0x010AE7A2, 0x010AEB50, 0x010AF7A2, 0x010B0006, 0x010B36A2, 0x010B3906, 0x010B4072,
            0x010B56A2, 0x010B5872, 0x010B606E, 0x010B73A2, 0x010B786E, 0x010B806F, 0x010B92A2,
            0x010B996F, 0x010B9DA2, 0x010BA96F, 0x010BB0A2, 0x010C0065, 0x010C49A2, 0x010C8038,
            0x010CB3A2, 0x010CC038, 0x010CF3A2, 0x010CFA38, 0x010D0074, 0x010D28A2, 0x010D3074,
            0x010D3AA2, 0x010E6003, 0x010E7FA2, 0x010E809D, 0x010EAAA2, 0x010EAB9D, 0x010EAEA2,
            0x010EB09D, 0x010EB2A2, 0x010F0080, 0x010F28A2, 0x010F307F, 0x010F5AA2, 0x010F7069,
            0x010F8AA2, 0x010FB017, 0x010FCCA2, 0x010FE023, 0x010FF7A2, 0x0110000E, 0x01104EA2,
            0x0110520E, 0x011076A2, 0x01107F0E, 0x01108042, 0x0110C3A2, 0x0110CD42, 0x0110CEA2,
            0x0110D081, 0x0110E9A2, 0x0110F081, 0x0110FAA2, 0x01110012, 0x011135A2, 0x01113612,
            0x011148A2, 0x0111504D, 0x011177A2, 0x0111807B, 0x0111E0A2, 0x0111E17E, 0x0111F5A2,
            0x0112003F, 0x011212A2, 0x0112133F, 0x01123FA2, 0x0112805B, 0x011287A2, 0x0112885B,
            0x011289A2, 0x01128A5B, 0x01128EA2, 0x01128F5B, 0x01129EA2, 0x01129F5B, 0x0112AAA2,
            0x0112B07D, 0x0112EBA2, 0x0112F07D, 0x0112FAA2, 0x0113002A, 0x011304A2, 0x0113052A,
            0x01130DA2, 0x01130F2A, 0x011311A2, 0x0113132A, 0x011329A2, 0x01132A2A, 0x011331A2,
            0x0113322A, 0x011334A2, 0x0113352A, 0x01133AA2, 0x01133BA0, 0x01133C2A, 0x011345A2,
            0x0113472A, 0x011349A2, 0x01134B2A, 0x01134EA2, 0x0113502A, 0x011351A2, 0x0113572A,
            0x011358A2, 0x01135D2A, 0x011364A2, 0x0113662A, 0x01136DA2, 0x0113702A, 0x011375A2,
            0x01140060, 0x01145CA2, 0x01145D60, 0x011462A2, 0x01148093, 0x0114C8A2, 0x0114D093,
            0x0114DAA2, 0x0115807C, 0x0115B6A2, 0x0115B87C, 0x0115DEA2, 0x01160057, 0x011645A2,
            0x01165057, 0x01165AA2, 0x01166058, 0x01166DA2, 0x01168087, 0x0116BAA2, 0x0116C087,
            0x0116CAA2, 0x01170002, 0x01171BA2, 0x01171D02, 0x01172CA2, 0x01173002, 0x011747A2,
            0x0118001E, 0x01183CA2, 0x0118A099, 0x0118F3A2, 0x0118FF99, 0x0119001D, 0x011907A2,
            0x0119091D, 0x01190AA2, 0x01190C1D, 0x011914A2, 0x0119151D, 0x011917A2, 0x0119181D,
            0x011936A2, 0x0119371D, 0x011939A2, 0x01193B1D, 0x011947A2, 0x0119501D, 0x01195AA2,
            0x0119A05D, 0x0119A8A2, 0x0119AA5D, 0x0119D8A2, 0x0119DA5D, 0x0119E5A2, 0x011A009F,
            0x011A48A2, 0x011A5082, 0x011AA3A2, 0x011AB013, 0x011AC06B, 0x011AF9A2, 0x011C000C,
            0x011C09A2, 0x011C0A0C, 0x011C37A2, 0x011C380C, 0x011C46A2, 0x011C500C, 0x011C6DA2,
            0x011C7051, 0x011C90A2, 0x011C9251, 0x011CA8A2, 0x011CA951, 0x011CB7A2, 0x011D0028,
            0x011D07A2, 0x011D0828, 0x011D0AA2, 0x011D0B28, 0x011D37A2, 0x011D3A28, 0x011D3BA2,
            0x011D3C28, 0x011D3EA2, 0x011D3F28, 0x011D48A2, 0x011D5028, 0x011D5AA2, 0x011D6027,
            0x011D66A2, 0x011D6727, 0x011D69A2, 0x011D6A27, 0x011D8FA2, 0x011D9027, 0x011D92A2,
            0x011D9327, 0x011D99A2, 0x011DA027, 0x011DAAA2, 0x011EE04E, 0x011EF9A2, 0x011FB04A,
            0x011FB1A2, 0x011FC08A, 0x011FF2A2, 0x011FFF8A, 0x0120009C, 0x01239AA2, 0x0124009C,
            0x01246FA2, 0x0124709C, 0x012475A2, 0x0124809C, 0x012544A2, 0x012F9019, 0x012FF3A2,
            0x01300021, 0x01342FA2, 0x01343021, 0x013439A2, 0x01440034, 0x014647A2, 0x01680008,
            0x016A39A2, 0x016A4059, 0x016A5FA2, 0x016A6059, 0x016A6AA2, 0x016A6E59, 0x016A7094,
            0x016ABFA2, 0x016AC094, 0x016ACAA2, 0x016AD009, 0x016AEEA2, 0x016AF009, 0x016AF6A2,
            0x016B0035, 0x016B46A2, 0x016B5035, 0x016B5AA2, 0x016B5B35, 0x016B62A2, 0x016B6335,
            0x016B78A2, 0x016B7D35, 0x016B90A2, 0x016E4052, 0x016E9BA2, 0x016F0071, 0x016F4BA2,
            0x016F4F71, 0x016F88A2, 0x016F8F71, 0x016FA0A2, 0x016FE08B, 0x016FE162, 0x016FE22F,
            0x016FE440, 0x016FE5A2, 0x016FF02F, 0x016FF2A2, 0x0170008B, 0x0187F8A2, 0x0188008B,
            0x018B0040, 0x018CD6A2, 0x018D008B, 0x018D09A2, 0x01AFF03C, 0x01AFF4A2, 0x01AFF53C,
            0x01AFFCA2, 0x01AFFD3C, 0x01AFFFA2, 0x01B0003C, 0x01B00133, 0x01B1203C, 0x01B123A2,
            0x01B15033, 0x01B153A2, 0x01B1643C, 0x01B168A2, 0x01B17062, 0x01B2FCA2, 0x01BC0020,
            0x01BC6BA2, 0x01BC7020, 0x01BC7DA2, 0x01BC8020, 0x01BC89A2, 0x01BC9020, 0x01BC9AA2,
            0x01BC9C20, 0x01BCA0A1, 0x01BCA4A2, 0x01CF00A0, 0x01CF2EA2, 0x01CF30A0, 0x01CF47A2,
            0x01CF50A1, 0x01CFC4A2, 0x01D000A1, 0x01D0F6A2, 0x01D100A1, 0x01D127A2, 0x01D129A1,
            0x01D167A0, 0x01D16AA1, 0x01D17BA0, 0x01D183A1, 0x01D185A0, 0x01D18CA1, 0x01D1AAA0,
            0x01D1AEA1, 0x01D1EBA2, 0x01D2002B, 0x01D246A2, 0x01D2E0A1, 0x01D2F4A2, 0x01D300A1,
            0x01D357A2, 0x01D360A1, 0x01D379A2, 0x01D400A1, 0x01D455A2, 0x01D456A1, 0x01D49DA2,
            0x01D49EA1, 0x01D4A0A2, 0x01D4A2A1, 0x01D4A3A2, 0x01D4A5A1, 0x01D4A7A2, 0x01D4A9A1,
            0x01D4ADA2, 0x01D4AEA1, 0x01D4BAA2, 0x01D4BBA1, 0x01D4BCA2, 0x01D4BDA1, 0x01D4C4A2,
            0x01D4C5A1, 0x01D506A2, 0x01D507A1, 0x01D50BA2, 0x01D50DA1, 0x01D515A2, 0x01D516A1,
            0x01D51DA2, 0x01D51EA1, 0x01D53AA2, 0x01D53BA1, 0x01D53FA2, 0x01D540A1, 0x01D545A2,
            0x01D546A1, 0x01D547A2, 0x01D54AA1, 0x01D551A2, 0x01D552A1, 0x01D6A6A2, 0x01D6A8A1,
            0x01D7CCA2, 0x01D7CEA1, 0x01D80079, 0x01DA8CA2, 0x01DA9B79, 0x01DAA0A2, 0x01DAA179,
            0x01DAB0A2, 0x01DF0045, 0x01DF1FA2, 0x01E00026, 0x01E007A2, 0x01E00826, 0x01E019A2,
            0x01E01B26, 0x01E022A2, 0x01E02326, 0x01E025A2, 0x01E02626, 0x01E02BA2, 0x01E10036,
            0x01E12DA2, 0x01E13036, 0x01E13EA2, 0x01E14036, 0x01E14AA2, 0x01E14E36, 0x01E150A2,
            0x01E29095, 0x01E2AFA2, 0x01E2C09A, 0x01E2FAA2, 0x01E2FF9A, 0x01E300A2, 0x01E7E024,
            0x01E7E7A2, 0x01E7E824, 0x01E7ECA2, 0x01E7ED24, 0x01E7EFA2, 0x01E7F024, 0x01E7FFA2,
            0x01E80053, 0x01E8C5A2, 0x01E8C753, 0x01E8D7A2, 0x01E90000, 0x01E94CA2, 0x01E95000,
            0x01E95AA2, 0x01E95E00, 0x01E960A2, 0x01EC71A1, 0x01ECB5A2, 0x01ED01A1, 0x01ED3EA2,
            0x01EE0003, 0x01EE04A2, 0x01EE0503, 0x01EE20A2, 0x01EE2103, 0x01EE23A2, 0x01EE2403,
            0x01EE25A2, 0x01EE2703, 0x01EE28A2, 0x01EE2903, 0x01EE33A2, 0x01EE3403, 0x01EE38A2,
            0x01EE3903, 0x01EE3AA2, 0x01EE3B03, 0x01EE3CA2, 0x01EE4203, 0x01EE43A2, 0x01EE4703,
            0x01EE48A2, 0x01EE4903, 0x01EE4AA2, 0x01EE4B03, 0x01EE4CA2, 0x01EE4D03, 0x01EE50A2,
            0x01EE5103, 0x01EE53A2, 0x01EE5403, 0x01EE55A2, 0x01EE5703, 0x01EE58A2, 0x01EE5903,
            0x01EE5AA2, 0x01EE5B03, 0x01EE5CA2, 0x01EE5D03, 0x01EE5EA2, 0x01EE5F03, 0x01EE60A2,
            0x01EE6103, 0x01EE63A2, 0x01EE6403, 0x01EE65A2, 0x01EE6703, 0x01EE6BA2, 0x01EE6C03,
            0x01EE73A2, 0x01EE7403, 0x01EE78A2, 0x01EE7903, 0x01EE7DA2, 0x01EE7E03, 0x01EE7FA2,
            0x01EE8003, 0x01EE8AA2, 0x01EE8B03, 0x01EE9CA2, 0x01EEA103, 0x01EEA4A2, 0x01EEA503,
            0x01EEAAA2, 0x01EEAB03, 0x01EEBCA2, 0x01EEF003, 0x01EEF2A2, 0x01F000A1, 0x01F02CA2,
            0x01F030A1, 0x01F094A2, 0x01F0A0A1, 0x01F0AFA2, 0x01F0B1A1, 0x01F0C0A2, 0x01F0C1A1,
            0x01F0D0A2, 0x01F0D1A1, 0x01F0F6A2, 0x01F100A1, 0x01F1AEA2, 0x01F1E6A1, 0x01F20033,
            0x01F201A1, 0x01F203A2, 0x01F210A1, 0x01F23CA2, 0x01F240A1, 0x01F249A2, 0x01F250A1,
            0x01F252A2, 0x01F260A1, 0x01F266A2, 0x01F300A1, 0x01F6D8A2, 0x01F6DDA1, 0x01F6EDA2,
            0x01F6F0A1, 0x01F6FDA2, 0x01F700A1, 0x01F774A2, 0x01F780A1, 0x01F7D9A2, 0x01F7E0A1,
            0x01F7ECA2, 0x01F7F0A1, 0x01F7F1A2, 0x01F800A1, 0x01F80CA2, 0x01F810A1, 0x01F848A2,
            0x01F850A1, 0x01F85AA2, 0x01F860A1, 0x01F888A2, 0x01F890A1, 0x01F8AEA2, 0x01F8B0A1,
            0x01F8B2A2, 0x01F900A1, 0x01FA54A2, 0x01FA60A1, 0x01FA6EA2, 0x01FA70A1, 0x01FA75A2,
            0x01FA78A1, 0x01FA7DA2, 0x01FA80A1, 0x01FA87A2, 0x01FA90A1, 0x01FAADA2, 0x01FAB0A1,
            0x01FABBA2, 0x01FAC0A1, 0x01FAC6A2, 0x01FAD0A1, 0x01FADAA2, 0x01FAE0A1, 0x01FAE8A2,
            0x01FAF0A1, 0x01FAF7A2, 0x01FB00A1, 0x01FB93A2, 0x01FB94A1, 0x01FBCBA2, 0x01FBF0A1,
            0x01FBFAA2, 0x0200002F, 0x02A6E0A2, 0x02A7002F, 0x02B739A2, 0x02B7402F, 0x02B81EA2,
            0x02B8202F, 0x02CEA2A2, 0x02CEB02F, 0x02EBE1A2, 0x02F8002F, 0x02FA1EA2, 0x0300002F,
            0x03134BA2, 0x0E0001A1, 0x0E0002A2, 0x0E0020A1, 0x0E0080A2, 0x0E0100A0, 0x0E01F0A2,
            0xFFFFFFFF};
    };
    template<>
    struct script_data<1> {
        static constexpr const compact_range scripts_data = {
            0x000000A2, 0x0003422B, 0x000343A2, 0x0003452B, 0x000346A2, 0x00036345, 0x000370A2,
            0x0004831B, 0x000488A2, 0x00060C03, 0x00060DA2, 0x00061B03, 0x00061DA2, 0x00061F00,
            0x000620A2, 0x00064000, 0x000641A2, 0x00064B03, 0x000656A2, 0x00066003, 0x00066AA2,
            0x00067003, 0x000671A2, 0x0006D403, 0x0006D5A2, 0x0009510B, 0x000953A2, 0x0009640B,
            0x0009661C, 0x000970A2, 0x0009E60B, 0x0009F0A2, 0x000A662D, 0x000A70A2, 0x000AE62C,
            0x000AF0A2, 0x000BE62A, 0x000BF4A2, 0x000CE641, 0x000CF0A2, 0x00104012, 0x00104AA2,
            0x0010FB25, 0x0010FCA2, 0x00173511, 0x001737A2, 0x00180258, 0x001804A2, 0x00180558,
            0x001806A2, 0x001CD00B, 0x001CD11C, 0x001CD20B, 0x001CD31C, 0x001CD50B, 0x001CD71C,
            0x001CD80B, 0x001CD91C, 0x001CE10B, 0x001CE21C, 0x001CEA0B, 0x001CEB1C, 0x001CED0B,
            0x001CEE1C, 0x001CF20B, 0x001CF31C, 0x001CF50B, 0x001CF81C, 0x001CFA5D, 0x001CFBA2,
            0x001DC02B, 0x001DC2A2, 0x001DF81B, 0x001DF9A2, 0x001DFA85, 0x001DFBA2, 0x00202F45,
            0x002030A2, 0x0020F01C, 0x0020F1A2, 0x002E431B, 0x002E44A2, 0x0030010D, 0x003004A2,
            0x0030062F, 0x003007A2, 0x0030080D, 0x003012A2, 0x0030130D, 0x003020A2, 0x00302A0D,
            0x00302EA2, 0x0030300D, 0x00303133, 0x003036A2, 0x0030370D, 0x003038A2, 0x00303C2F,
            0x003040A2, 0x00309933, 0x00309DA2, 0x0030A033, 0x0030A1A2, 0x0030FB0D, 0x0030FC33,
            0x0030FDA2, 0x0031902F, 0x0031A0A2, 0x0031C02F, 0x0031E4A2, 0x0032202F, 0x003248A2,
            0x0032802F, 0x0032B1A2, 0x0032C02F, 0x0032CCA2, 0x0032FF2F, 0x003300A2, 0x0033582F,
            0x003371A2, 0x00337B2F, 0x003380A2, 0x0033E02F, 0x0033FFA2, 0x00A66F1B, 0x00A670A2,
            0x00A7002F, 0x00A708A2, 0x00A8301C, 0x00A83AA2, 0x00A8F10B, 0x00A8F2A2, 0x00A8F31C,
            0x00A8F4A2, 0x00A92E3B, 0x00A92FA2, 0x00A9CF10, 0x00A9D0A2, 0x00FD3E03, 0x00FD40A2,
            0x00FDF203, 0x00FDF3A2, 0x00FDFD03, 0x00FDFEA2, 0x00FE450D, 0x00FE47A2, 0x00FF610D,
            0x00FF66A2, 0x00FF7033, 0x00FF71A2, 0x00FF9E33, 0x00FFA0A2, 0x01010019, 0x0101021A,
            0x010103A2, 0x0101071A, 0x010134A2, 0x0101371A, 0x010140A2, 0x0102E003, 0x0102FCA2,
            0x010AF250, 0x010AF3A2, 0x0113012A, 0x011302A2, 0x0113032A, 0x011304A2, 0x01133B2A,
            0x01133DA2, 0x011FD02A, 0x011FD2A2, 0x011FD32A, 0x011FD4A2, 0x01BCA020, 0x01BCA4A2,
            0x01D3602F, 0x01D372A2, 0x01F2502F, 0x01F252A2, 0xFFFFFFFF};
    };
    template<>
    struct script_data<2> {
        static constexpr const compact_range scripts_data = {
            0x000000A2, 0x0004836C, 0x00048426, 0x00048545, 0x00048726, 0x000488A2, 0x00060C61,
            0x00060DA2, 0x00061B61, 0x00061C85, 0x00061DA2, 0x00061F03, 0x000620A2, 0x00064003,
            0x000641A2, 0x00064B85, 0x000656A2, 0x00066090, 0x00066AA2, 0x00067085, 0x000671A2,
            0x0006D474, 0x0006D5A2, 0x0009511C, 0x000953A2, 0x0009641C, 0x0009661E, 0x000970A2,
            0x0009E612, 0x0009F0A2, 0x000A665B, 0x000A70A2, 0x000AE63F, 0x000AF0A2, 0x000BE68A,
            0x000BF4A2, 0x000CE65D, 0x000CF0A2, 0x0010405C, 0x00104AA2, 0x0010FB45, 0x0010FCA2,
            0x00173530, 0x001737A2, 0x0018026D, 0x001804A2, 0x0018056D, 0x001806A2, 0x001CD01C,
            0x001CD1A2, 0x001CD21C, 0x001CD32A, 0x001CD4A2, 0x001CD51C, 0x001CD77B, 0x001CD81C,
            0x001CD97B, 0x001CDA41, 0x001CDBA2, 0x001CDC7B, 0x001CDEA2, 0x001CE07B, 0x001CE11C,
            0x001CE2A2, 0x001CE95D, 0x001CEA1C, 0x001CEBA2, 0x001CED1C, 0x001CEEA2, 0x001CF21C,
            0x001CF32A, 0x001CF51C, 0x001CF7A2, 0x001CF82A, 0x001CFAA2, 0x001DF885, 0x001DF9A2,
            0x00202F58, 0x002030A2, 0x0020F02A, 0x0020F1A2, 0x002E4326, 0x002E44A2, 0x0030012E,
            0x003004A2, 0x0030082E, 0x003012A2, 0x0030132E, 0x003020A2, 0x00302A2F, 0x00302EA2,
            0x0030302E, 0x0030313C, 0x003036A2, 0x0030372E, 0x003038A2, 0x00303C33, 0x00303EA2,
            0x0030993C, 0x00309DA2, 0x0030A03C, 0x0030A1A2, 0x0030FB2E, 0x0030FC3C, 0x0030FDA2,
            0x00A66F26, 0x00A670A2, 0x00A70045, 0x00A708A2, 0x00A8301E, 0x00A83AA2, 0x00A8F11C,
            0x00A8F2A2, 0x00A8F38A, 0x00A8F4A2, 0x00A92E45, 0x00A92FA2, 0x00A9CF3A, 0x00A9D0A2,
            0x00FD3E61, 0x00FD40A2, 0x00FDF290, 0x00FDF3A2, 0x00FDFD90, 0x00FDFEA2, 0x00FE452E,
            0x00FE47A2, 0x00FF612E, 0x00FF66A2, 0x00FF703C, 0x00FF71A2, 0x00FF9E3C, 0x00FFA0A2,
            0x0101001A, 0x01010249, 0x010103A2, 0x01010748, 0x010134A2, 0x01013749, 0x010140A2,
            0x0102E018, 0x0102FCA2, 0x010AF269, 0x010AF3A2, 0x0113018A, 0x011302A2, 0x0113038A,
            0x011304A2, 0x01133B8A, 0x01133DA2, 0x011FD08A, 0x011FD2A2, 0x011FD38A, 0x011FD4A2,
            0xFFFFFFFF};
    };
    template<>
    struct script_data<3> {
        static constexpr const compact_range scripts_data = {
            0x000000A2, 0x00060C74, 0x00060DA2, 0x00061B74, 0x00061C90, 0x00061DA2, 0x00061F61,
            0x000620A2, 0x0006404F, 0x000641A2, 0x0006609D, 0x00066AA2, 0x0009512A, 0x000953A2,
            0x0009641E, 0x00096642, 0x000970A2, 0x0009E684, 0x0009F0A2, 0x00104088, 0x00104AA2,
            0x00173586, 0x001737A2, 0x001CD02A, 0x001CD1A2, 0x001CD22A, 0x001CD3A2, 0x001CDA56,
            0x001CDBA2, 0x001CF22A, 0x001CF3A2, 0x001CF441, 0x001CF5A2, 0x0020F045, 0x0020F1A2,
            0x0030012F, 0x003004A2, 0x0030082F, 0x003012A2, 0x0030132F, 0x003020A2, 0x0030302F,
            0x003031A2, 0x0030372F, 0x003038A2, 0x00303C3C, 0x00303EA2, 0x0030FB2F, 0x0030FCA2,
            0x00A8302C, 0x00A83AA2, 0x00A92E5C, 0x00A92FA2, 0x00FE452F, 0x00FE47A2, 0x00FF612F,
            0x00FF66A2, 0x01010049, 0x010102A2, 0x01010749, 0x010134A2, 0xFFFFFFFF};
    };
    template<>
    struct script_data<4> {
        static constexpr const compact_range scripts_data = {
            0x000000A2, 0x00060C85, 0x00060DA2, 0x00061B85, 0x00061CA2, 0x00061F74, 0x000620A2,
            0x00064050, 0x000641A2, 0x0009512C, 0x000953A2, 0x00096427, 0x0009664D, 0x000970A2,
            0x0017358F, 0x001737A2, 0x001CD041, 0x001CD1A2, 0x001CD241, 0x001CD3A2, 0x001CDA66,
            0x001CDBA2, 0x001CF241, 0x001CF3A2, 0x00300133, 0x003004A2, 0x00300833, 0x003012A2,
            0x00301333, 0x003020A2, 0x00303033, 0x003031A2, 0x00303733, 0x003038A2, 0x0030FB33,
            0x0030FCA2, 0x00A8302D, 0x00A83AA2, 0x00FE4533, 0x00FE47A2, 0x00FF6133, 0x00FF66A2,
            0xFFFFFFFF};
    };
    template<>
    struct script_data<5> {
        static constexpr const compact_range scripts_data = {
            0x000000A2, 0x00060C90, 0x00060DA2, 0x00061B90, 0x00061CA2, 0x00061F85,
            0x000620A2, 0x00064069, 0x000641A2, 0x0009512D, 0x000953A2, 0x00096428,
            0x000966A2, 0x001CDA8A, 0x001CDBA2, 0x001CF25D, 0x001CF3A2, 0x0030013C,
            0x003004A2, 0x0030083C, 0x003012A2, 0x0030133C, 0x003020A2, 0x0030303C,
            0x003031A2, 0x0030373C, 0x003038A2, 0x0030FB3C, 0x0030FCA2, 0x00A8303F,
            0x00A83AA2, 0x00FE453C, 0x00FE47A2, 0x00FF613C, 0x00FF66A2, 0xFFFFFFFF};
    };
    template<>
    struct script_data<6> {
        static constexpr const compact_range scripts_data = {
            0x000000A2, 0x00060C9D, 0x00060DA2, 0x00061B9D, 0x00061CA2, 0x00061F90, 0x000620A2,
            0x0006406F, 0x000641A2, 0x00095141, 0x000953A2, 0x0009642A, 0x000966A2, 0x001CDA8D,
            0x001CDBA2, 0x001CF266, 0x001CF3A2, 0x0030019E, 0x003003A2, 0x0030089E, 0x003012A2,
            0x0030149E, 0x00301CA2, 0x0030FB9E, 0x0030FCA2, 0x00A83041, 0x00A83642, 0x00A83AA2,
            0x00FF619E, 0x00FF66A2, 0xFFFFFFFF};
    };
    template<>
    struct script_data<7> {
        static constexpr const compact_range scripts_data = {
            0x000000A2, 0x00061F9D, 0x000620A2, 0x00064074, 0x000641A2,
            0x00095145, 0x000953A2, 0x0009642C, 0x000966A2, 0x001CF28D,
            0x001CF3A2, 0x00A83042, 0x00A8364D, 0x00A83AA2, 0xFFFFFFFF};
    };
    template<>
    struct script_data<8> {
        static constexpr const compact_range scripts_data = {
            0x000000A2, 0x0006407F, 0x000641A2, 0x00095156, 0x000953A2, 0x0009642D, 0x000966A2,
            0x001CF293, 0x001CF3A2, 0x00A8304D, 0x00A83657, 0x00A83AA2, 0xFFFFFFFF};
    };
    template<>
    struct script_data<9> {
        static constexpr const compact_range scripts_data = {
            0x000000A2, 0x00064085, 0x000641A2, 0x00095166, 0x000953A2, 0x00096441,
            0x000966A2, 0x00A83056, 0x00A83357, 0x00A8367D, 0x00A83AA2, 0xFFFFFFFF};
    };
    template<>
    struct script_data<10> {
        static constexpr const compact_range scripts_data = {
            0x000000A2, 0x0009517B, 0x0009528A, 0x000953A2, 0x0009644D, 0x00096547,
            0x000966A2, 0x00A83057, 0x00A8335D, 0x00A83687, 0x00A83AA2, 0xFFFFFFFF};
    };
    template<>
    struct script_data<11> {
        static constexpr const compact_range scripts_data = {
            0x000000A2, 0x0009518A, 0x0009528D, 0x000953A2, 0x00096456, 0x0009654D,
            0x000966A2, 0x00A8305D, 0x00A8337D, 0x00A83693, 0x00A83AA2, 0xFFFFFFFF};
    };
    template<>
    struct script_data<12> {
        static constexpr const compact_range scripts_data = {
            0x000000A2, 0x0009518D, 0x00095293, 0x000953A2, 0x0009645D, 0x00096556,
            0x000966A2, 0x00A8307D, 0x00A83387, 0x00A836A2, 0xFFFFFFFF};
    };
    template<>
    struct script_data<13> {
        static constexpr const compact_range scripts_data = {
            0x000000A2, 0x00095193, 0x000952A2, 0x00096466, 0x0009655D,
            0x000966A2, 0x00A83087, 0x00A83393, 0x00A836A2, 0xFFFFFFFF};
    };
    template<>
    struct script_data<14> {
        static constexpr const compact_range scripts_data = {
            0x000000A2, 0x0009647D, 0x00096566, 0x000966A2, 0x00A83093, 0x00A833A2, 0xFFFFFFFF};
    };
    template<>
    struct script_data<15> {
        static constexpr const compact_range scripts_data = {0x000000A2, 0x0009647E, 0x0009657D,
                                                             0x000966A2, 0xFFFFFFFF};
    };
    template<>
    struct script_data<16> {
        static constexpr const compact_range scripts_data = {0x000000A2, 0x00096484, 0x0009657E,
                                                             0x000966A2, 0xFFFFFFFF};
    };
    template<>
    struct script_data<17> {
        static constexpr const compact_range scripts_data = {0x000000A2, 0x00096487, 0x00096584,
                                                             0x000966A2, 0xFFFFFFFF};
    };
    template<>
    struct script_data<18> {
        static constexpr const compact_range scripts_data = {0x000000A2, 0x0009648A, 0x00096587,
                                                             0x000966A2, 0xFFFFFFFF};
    };
    template<>
    struct script_data<19> {
        static constexpr const compact_range scripts_data = {0x000000A2, 0x0009648D, 0x0009658A,
                                                             0x000966A2, 0xFFFFFFFF};
    };
    template<>
    struct script_data<20> {
        static constexpr const compact_range scripts_data = {0x000000A2, 0x00096493, 0x0009658D,
                                                             0x000966A2, 0xFFFFFFFF};
    };
    template<>
    struct script_data<21> {
        static constexpr const compact_range scripts_data = {0x000000A2, 0x00096593, 0x000966A2,
                                                             0xFFFFFFFF};
    };
    template<auto N>
    constexpr script cp_script(char32_t cp) {
        if(cp > 0x10FFFF)
            return script::unknown;

        uni::script sc = static_cast<uni::script>(
            script_data<N>::scripts_data.value(cp, uint8_t(script::unknown)));
        return sc;
    }
    constexpr script get_cp_script(char32_t cp, int idx) {
        switch(idx) {
            case 0: return cp_script<0>(cp);
            case 1: return cp_script<1>(cp);
            case 2: return cp_script<2>(cp);
            case 3: return cp_script<3>(cp);
            case 4: return cp_script<4>(cp);
            case 5: return cp_script<5>(cp);
            case 6: return cp_script<6>(cp);
            case 7: return cp_script<7>(cp);
            case 8: return cp_script<8>(cp);
            case 9: return cp_script<9>(cp);
            case 10: return cp_script<10>(cp);
            case 11: return cp_script<11>(cp);
            case 12: return cp_script<12>(cp);
            case 13: return cp_script<13>(cp);
            case 14: return cp_script<14>(cp);
            case 15: return cp_script<15>(cp);
            case 16: return cp_script<16>(cp);
            case 17: return cp_script<17>(cp);
            case 18: return cp_script<18>(cp);
            case 19: return cp_script<19>(cp);
            case 20: return cp_script<20>(cp);
            case 21: return cp_script<21>(cp);
        }
        return script::zzzz;
    }
    static constexpr compact_list numeric_data8 = {
        0x00003000, 0x00003101, 0x00003202, 0x00003303, 0x00003404, 0x00003505, 0x00003606,
        0x00003707, 0x00003808, 0x00003909, 0x0000B202, 0x0000B303, 0x0000B901, 0x0000BC01,
        0x0000BD01, 0x0000BE03, 0x00066000, 0x00066101, 0x00066202, 0x00066303, 0x00066404,
        0x00066505, 0x00066606, 0x00066707, 0x00066808, 0x00066909, 0x0006F000, 0x0006F101,
        0x0006F202, 0x0006F303, 0x0006F404, 0x0006F505, 0x0006F606, 0x0006F707, 0x0006F808,
        0x0006F909, 0x0007C000, 0x0007C101, 0x0007C202, 0x0007C303, 0x0007C404, 0x0007C505,
        0x0007C606, 0x0007C707, 0x0007C808, 0x0007C909, 0x00096600, 0x00096701, 0x00096802,
        0x00096903, 0x00096A04, 0x00096B05, 0x00096C06, 0x00096D07, 0x00096E08, 0x00096F09,
        0x0009E600, 0x0009E701, 0x0009E802, 0x0009E903, 0x0009EA04, 0x0009EB05, 0x0009EC06,
        0x0009ED07, 0x0009EE08, 0x0009EF09, 0x0009F401, 0x0009F501, 0x0009F603, 0x0009F701,
        0x0009F803, 0x0009F910, 0x000A6600, 0x000A6701, 0x000A6802, 0x000A6903, 0x000A6A04,
        0x000A6B05, 0x000A6C06, 0x000A6D07, 0x000A6E08, 0x000A6F09, 0x000AE600, 0x000AE701,
        0x000AE802, 0x000AE903, 0x000AEA04, 0x000AEB05, 0x000AEC06, 0x000AED07, 0x000AEE08,
        0x000AEF09, 0x000B6600, 0x000B6701, 0x000B6802, 0x000B6903, 0x000B6A04, 0x000B6B05,
        0x000B6C06, 0x000B6D07, 0x000B6E08, 0x000B6F09, 0x000B7201, 0x000B7301, 0x000B7403,
        0x000B7501, 0x000B7601, 0x000B7703, 0x000BE600, 0x000BE701, 0x000BE802, 0x000BE903,
        0x000BEA04, 0x000BEB05, 0x000BEC06, 0x000BED07, 0x000BEE08, 0x000BEF09, 0x000BF00A,
        0x000BF164, 0x000C6600, 0x000C6701, 0x000C6802, 0x000C6903, 0x000C6A04, 0x000C6B05,
        0x000C6C06, 0x000C6D07, 0x000C6E08, 0x000C6F09, 0x000C7800, 0x000C7901, 0x000C7A02,
        0x000C7B03, 0x000C7C01, 0x000C7D02, 0x000C7E03, 0x000CE600, 0x000CE701, 0x000CE802,
        0x000CE903, 0x000CEA04, 0x000CEB05, 0x000CEC06, 0x000CED07, 0x000CEE08, 0x000CEF09,
        0x000D5801, 0x000D5901, 0x000D5A03, 0x000D5B01, 0x000D5C01, 0x000D5D03, 0x000D5E01,
        0x000D6600, 0x000D6701, 0x000D6802, 0x000D6903, 0x000D6A04, 0x000D6B05, 0x000D6C06,
        0x000D6D07, 0x000D6E08, 0x000D6F09, 0x000D700A, 0x000D7164, 0x000D7301, 0x000D7401,
        0x000D7503, 0x000D7601, 0x000D7701, 0x000D7803, 0x000DE600, 0x000DE701, 0x000DE802,
        0x000DE903, 0x000DEA04, 0x000DEB05, 0x000DEC06, 0x000DED07, 0x000DEE08, 0x000DEF09,
        0x000E5000, 0x000E5101, 0x000E5202, 0x000E5303, 0x000E5404, 0x000E5505, 0x000E5606,
        0x000E5707, 0x000E5808, 0x000E5909, 0x000ED000, 0x000ED101, 0x000ED202, 0x000ED303,
        0x000ED404, 0x000ED505, 0x000ED606, 0x000ED707, 0x000ED808, 0x000ED909, 0x000F2000,
        0x000F2101, 0x000F2202, 0x000F2303, 0x000F2404, 0x000F2505, 0x000F2606, 0x000F2707,
        0x000F2808, 0x000F2909, 0x000F2A01, 0x000F2B03, 0x000F2C05, 0x000F2D07, 0x000F2E09,
        0x000F2F0B, 0x000F300D, 0x000F310F, 0x000F3211, 0x00104000, 0x00104101, 0x00104202,
        0x00104303, 0x00104404, 0x00104505, 0x00104606, 0x00104707, 0x00104808, 0x00104909,
        0x00109000, 0x00109101, 0x00109202, 0x00109303, 0x00109404, 0x00109505, 0x00109606,
        0x00109707, 0x00109808, 0x00109909, 0x00136901, 0x00136A02, 0x00136B03, 0x00136C04,
        0x00136D05, 0x00136E06, 0x00136F07, 0x00137008, 0x00137109, 0x0013720A, 0x00137314,
        0x0013741E, 0x00137528, 0x00137632, 0x0013773C, 0x00137846, 0x00137950, 0x00137A5A,
        0x00137B64, 0x0016EE11, 0x0016EF12, 0x0016F013, 0x0017E000, 0x0017E101, 0x0017E202,
        0x0017E303, 0x0017E404, 0x0017E505, 0x0017E606, 0x0017E707, 0x0017E808, 0x0017E909,
        0x0017F000, 0x0017F101, 0x0017F202, 0x0017F303, 0x0017F404, 0x0017F505, 0x0017F606,
        0x0017F707, 0x0017F808, 0x0017F909, 0x00181000, 0x00181101, 0x00181202, 0x00181303,
        0x00181404, 0x00181505, 0x00181606, 0x00181707, 0x00181808, 0x00181909, 0x00194600,
        0x00194701, 0x00194802, 0x00194903, 0x00194A04, 0x00194B05, 0x00194C06, 0x00194D07,
        0x00194E08, 0x00194F09, 0x0019D000, 0x0019D101, 0x0019D202, 0x0019D303, 0x0019D404,
        0x0019D505, 0x0019D606, 0x0019D707, 0x0019D808, 0x0019D909, 0x0019DA01, 0x001A8000,
        0x001A8101, 0x001A8202, 0x001A8303, 0x001A8404, 0x001A8505, 0x001A8606, 0x001A8707,
        0x001A8808, 0x001A8909, 0x001A9000, 0x001A9101, 0x001A9202, 0x001A9303, 0x001A9404,
        0x001A9505, 0x001A9606, 0x001A9707, 0x001A9808, 0x001A9909, 0x001B5000, 0x001B5101,
        0x001B5202, 0x001B5303, 0x001B5404, 0x001B5505, 0x001B5606, 0x001B5707, 0x001B5808,
        0x001B5909, 0x001BB000, 0x001BB101, 0x001BB202, 0x001BB303, 0x001BB404, 0x001BB505,
        0x001BB606, 0x001BB707, 0x001BB808, 0x001BB909, 0x001C4000, 0x001C4101, 0x001C4202,
        0x001C4303, 0x001C4404, 0x001C4505, 0x001C4606, 0x001C4707, 0x001C4808, 0x001C4909,
        0x001C5000, 0x001C5101, 0x001C5202, 0x001C5303, 0x001C5404, 0x001C5505, 0x001C5606,
        0x001C5707, 0x001C5808, 0x001C5909, 0x00207000, 0x00207404, 0x00207505, 0x00207606,
        0x00207707, 0x00207808, 0x00207909, 0x00208000, 0x00208101, 0x00208202, 0x00208303,
        0x00208404, 0x00208505, 0x00208606, 0x00208707, 0x00208808, 0x00208909, 0x00215001,
        0x00215101, 0x00215201, 0x00215301, 0x00215402, 0x00215501, 0x00215602, 0x00215703,
        0x00215804, 0x00215901, 0x00215A05, 0x00215B01, 0x00215C03, 0x00215D05, 0x00215E07,
        0x00215F01, 0x00216001, 0x00216102, 0x00216203, 0x00216304, 0x00216405, 0x00216506,
        0x00216607, 0x00216708, 0x00216809, 0x0021690A, 0x00216A0B, 0x00216B0C, 0x00216C32,
        0x00216D64, 0x00217001, 0x00217102, 0x00217203, 0x00217304, 0x00217405, 0x00217506,
        0x00217607, 0x00217708, 0x00217809, 0x0021790A, 0x00217A0B, 0x00217B0C, 0x00217C32,
        0x00217D64, 0x00218506, 0x00218632, 0x00218900, 0x00246001, 0x00246102, 0x00246203,
        0x00246304, 0x00246405, 0x00246506, 0x00246607, 0x00246708, 0x00246809, 0x0024690A,
        0x00246A0B, 0x00246B0C, 0x00246C0D, 0x00246D0E, 0x00246E0F, 0x00246F10, 0x00247011,
        0x00247112, 0x00247213, 0x00247314, 0x00247401, 0x00247502, 0x00247603, 0x00247704,
        0x00247805, 0x00247906, 0x00247A07, 0x00247B08, 0x00247C09, 0x00247D0A, 0x00247E0B,
        0x00247F0C, 0x0024800D, 0x0024810E, 0x0024820F, 0x00248310, 0x00248411, 0x00248512,
        0x00248613, 0x00248714, 0x00248801, 0x00248902, 0x00248A03, 0x00248B04, 0x00248C05,
        0x00248D06, 0x00248E07, 0x00248F08, 0x00249009, 0x0024910A, 0x0024920B, 0x0024930C,
        0x0024940D, 0x0024950E, 0x0024960F, 0x00249710, 0x00249811, 0x00249912, 0x00249A13,
        0x00249B14, 0x0024EA00, 0x0024EB0B, 0x0024EC0C, 0x0024ED0D, 0x0024EE0E, 0x0024EF0F,
        0x0024F010, 0x0024F111, 0x0024F212, 0x0024F313, 0x0024F414, 0x0024F501, 0x0024F602,
        0x0024F703, 0x0024F804, 0x0024F905, 0x0024FA06, 0x0024FB07, 0x0024FC08, 0x0024FD09,
        0x0024FE0A, 0x0024FF00, 0x00277601, 0x00277702, 0x00277803, 0x00277904, 0x00277A05,
        0x00277B06, 0x00277C07, 0x00277D08, 0x00277E09, 0x00277F0A, 0x00278001, 0x00278102,
        0x00278203, 0x00278304, 0x00278405, 0x00278506, 0x00278607, 0x00278708, 0x00278809,
        0x0027890A, 0x00278A01, 0x00278B02, 0x00278C03, 0x00278D04, 0x00278E05, 0x00278F06,
        0x00279007, 0x00279108, 0x00279209, 0x0027930A, 0x002CFD01, 0x00300700, 0x00302101,
        0x00302202, 0x00302303, 0x00302404, 0x00302505, 0x00302606, 0x00302707, 0x00302808,
        0x00302909, 0x0030380A, 0x00303914, 0x00303A1E, 0x00319201, 0x00319302, 0x00319403,
        0x00319504, 0x00322001, 0x00322102, 0x00322203, 0x00322304, 0x00322405, 0x00322506,
        0x00322607, 0x00322708, 0x00322809, 0x0032290A, 0x0032480A, 0x00324914, 0x00324A1E,
        0x00324B28, 0x00324C32, 0x00324D3C, 0x00324E46, 0x00324F50, 0x00325115, 0x00325216,
        0x00325317, 0x00325418, 0x00325519, 0x0032561A, 0x0032571B, 0x0032581C, 0x0032591D,
        0x00325A1E, 0x00325B1F, 0x00325C20, 0x00325D21, 0x00325E22, 0x00325F23, 0x00328001,
        0x00328102, 0x00328203, 0x00328304, 0x00328405, 0x00328506, 0x00328607, 0x00328708,
        0x00328809, 0x0032890A, 0x0032B124, 0x0032B225, 0x0032B326, 0x0032B427, 0x0032B528,
        0x0032B629, 0x0032B72A, 0x0032B82B, 0x0032B92C, 0x0032BA2D, 0x0032BB2E, 0x0032BC2F,
        0x0032BD30, 0x0032BE31, 0x0032BF32, 0x00340505, 0x00348302, 0x00382A05, 0x003B4D07,
        0x004E0001, 0x004E0307, 0x004E0903, 0x004E5D09, 0x004E8C02, 0x004E9405, 0x004E9604,
        0x004EC00A, 0x004EE803, 0x004F0D05, 0x004F7064, 0x00516902, 0x00516B08, 0x00516D06,
        0x0053410A, 0x00534414, 0x0053451E, 0x00534C28, 0x0053C103, 0x0053C203, 0x0053C303,
        0x0053C403, 0x0056DB04, 0x0058F101, 0x0058F901, 0x005E7A01, 0x005EFE09, 0x005EFF14,
        0x005F0C01, 0x005F0D02, 0x005F0E03, 0x005F1002, 0x0062FE0A, 0x00634C08, 0x0067D207,
        0x006F0607, 0x00739609, 0x00767E64, 0x00808604, 0x008CAE02, 0x008CB302, 0x008D3002,
        0x00964606, 0x00964C64, 0x00967806, 0x0096F600, 0x00A62000, 0x00A62101, 0x00A62202,
        0x00A62303, 0x00A62404, 0x00A62505, 0x00A62606, 0x00A62707, 0x00A62808, 0x00A62909,
        0x00A6E601, 0x00A6E702, 0x00A6E803, 0x00A6E904, 0x00A6EA05, 0x00A6EB06, 0x00A6EC07,
        0x00A6ED08, 0x00A6EE09, 0x00A6EF00, 0x00A83001, 0x00A83101, 0x00A83203, 0x00A83301,
        0x00A83401, 0x00A83503, 0x00A8D000, 0x00A8D101, 0x00A8D202, 0x00A8D303, 0x00A8D404,
        0x00A8D505, 0x00A8D606, 0x00A8D707, 0x00A8D808, 0x00A8D909, 0x00A90000, 0x00A90101,
        0x00A90202, 0x00A90303, 0x00A90404, 0x00A90505, 0x00A90606, 0x00A90707, 0x00A90808,
        0x00A90909, 0x00A9D000, 0x00A9D101, 0x00A9D202, 0x00A9D303, 0x00A9D404, 0x00A9D505,
        0x00A9D606, 0x00A9D707, 0x00A9D808, 0x00A9D909, 0x00A9F000, 0x00A9F101, 0x00A9F202,
        0x00A9F303, 0x00A9F404, 0x00A9F505, 0x00A9F606, 0x00A9F707, 0x00A9F808, 0x00A9F909,
        0x00AA5000, 0x00AA5101, 0x00AA5202, 0x00AA5303, 0x00AA5404, 0x00AA5505, 0x00AA5606,
        0x00AA5707, 0x00AA5808, 0x00AA5909, 0x00ABF000, 0x00ABF101, 0x00ABF202, 0x00ABF303,
        0x00ABF404, 0x00ABF505, 0x00ABF606, 0x00ABF707, 0x00ABF808, 0x00ABF909, 0x00F96B03,
        0x00F9730A, 0x00F97802, 0x00F9B200, 0x00F9D106, 0x00F9D306, 0x00F9FD0A, 0x00FF1000,
        0x00FF1101, 0x00FF1202, 0x00FF1303, 0x00FF1404, 0x00FF1505, 0x00FF1606, 0x00FF1707,
        0x00FF1808, 0x00FF1909, 0x01010701, 0x01010802, 0x01010903, 0x01010A04, 0x01010B05,
        0x01010C06, 0x01010D07, 0x01010E08, 0x01010F09, 0x0101100A, 0x01011114, 0x0101121E,
        0x01011328, 0x01011432, 0x0101153C, 0x01011646, 0x01011750, 0x0101185A, 0x01011964,
        0x01014001, 0x01014101, 0x01014201, 0x01014305, 0x01014432, 0x01014805, 0x0101490A,
        0x01014A32, 0x01014B64, 0x01014F05, 0x0101500A, 0x01015132, 0x01015264, 0x0101570A,
        0x01015801, 0x01015901, 0x01015A01, 0x01015B02, 0x01015C02, 0x01015D02, 0x01015E02,
        0x01015F05, 0x0101600A, 0x0101610A, 0x0101620A, 0x0101630A, 0x0101640A, 0x0101651E,
        0x01016632, 0x01016732, 0x01016832, 0x01016932, 0x01016A64, 0x01017305, 0x01017432,
        0x01017501, 0x01017601, 0x01017702, 0x01017803, 0x01018A00, 0x01018B01, 0x0102E101,
        0x0102E202, 0x0102E303, 0x0102E404, 0x0102E505, 0x0102E606, 0x0102E707, 0x0102E808,
        0x0102E909, 0x0102EA0A, 0x0102EB14, 0x0102EC1E, 0x0102ED28, 0x0102EE32, 0x0102EF3C,
        0x0102F046, 0x0102F150, 0x0102F25A, 0x0102F364, 0x01032001, 0x01032105, 0x0103220A,
        0x01032332, 0x0103415A, 0x0103D101, 0x0103D202, 0x0103D30A, 0x0103D414, 0x0103D564,
        0x0104A000, 0x0104A101, 0x0104A202, 0x0104A303, 0x0104A404, 0x0104A505, 0x0104A606,
        0x0104A707, 0x0104A808, 0x0104A909, 0x01085801, 0x01085902, 0x01085A03, 0x01085B0A,
        0x01085C14, 0x01085D64, 0x01087901, 0x01087A02, 0x01087B03, 0x01087C04, 0x01087D05,
        0x01087E0A, 0x01087F14, 0x0108A701, 0x0108A802, 0x0108A903, 0x0108AA04, 0x0108AB04,
        0x0108AC05, 0x0108AD0A, 0x0108AE14, 0x0108AF64, 0x0108FB01, 0x0108FC05, 0x0108FD0A,
        0x0108FE14, 0x0108FF64, 0x01091601, 0x0109170A, 0x01091814, 0x01091964, 0x01091A02,
        0x01091B03, 0x0109BC0B, 0x0109BD01, 0x0109C001, 0x0109C102, 0x0109C203, 0x0109C304,
        0x0109C405, 0x0109C506, 0x0109C607, 0x0109C708, 0x0109C809, 0x0109C90A, 0x0109CA14,
        0x0109CB1E, 0x0109CC28, 0x0109CD32, 0x0109CE3C, 0x0109CF46, 0x0109D264, 0x0109F601,
        0x0109F702, 0x0109F803, 0x0109F904, 0x0109FA05, 0x0109FB06, 0x0109FC07, 0x0109FD08,
        0x0109FE09, 0x0109FF0A, 0x010A4001, 0x010A4102, 0x010A4203, 0x010A4304, 0x010A440A,
        0x010A4514, 0x010A4664, 0x010A4801, 0x010A7D01, 0x010A7E32, 0x010A9D01, 0x010A9E0A,
        0x010A9F14, 0x010AEB01, 0x010AEC05, 0x010AED0A, 0x010AEE14, 0x010AEF64, 0x010B5801,
        0x010B5902, 0x010B5A03, 0x010B5B04, 0x010B5C0A, 0x010B5D14, 0x010B5E64, 0x010B7801,
        0x010B7902, 0x010B7A03, 0x010B7B04, 0x010B7C0A, 0x010B7D14, 0x010B7E64, 0x010BA901,
        0x010BAA02, 0x010BAB03, 0x010BAC04, 0x010BAD0A, 0x010BAE14, 0x010BAF64, 0x010CFA01,
        0x010CFB05, 0x010CFC0A, 0x010CFD32, 0x010CFE64, 0x010D3000, 0x010D3101, 0x010D3202,
        0x010D3303, 0x010D3404, 0x010D3505, 0x010D3606, 0x010D3707, 0x010D3808, 0x010D3909,
        0x010E6001, 0x010E6102, 0x010E6203, 0x010E6304, 0x010E6405, 0x010E6506, 0x010E6607,
        0x010E6708, 0x010E6809, 0x010E690A, 0x010E6A14, 0x010E6B1E, 0x010E6C28, 0x010E6D32,
        0x010E6E3C, 0x010E6F46, 0x010E7050, 0x010E715A, 0x010E7264, 0x010E7B01, 0x010E7C01,
        0x010E7D01, 0x010E7E02, 0x010F1D01, 0x010F1E02, 0x010F1F03, 0x010F2004, 0x010F2105,
        0x010F220A, 0x010F2314, 0x010F241E, 0x010F2564, 0x010F2601, 0x010F5101, 0x010F520A,
        0x010F5314, 0x010F5464, 0x010FC501, 0x010FC602, 0x010FC703, 0x010FC804, 0x010FC90A,
        0x010FCA14, 0x010FCB64, 0x01105201, 0x01105302, 0x01105403, 0x01105504, 0x01105605,
        0x01105706, 0x01105807, 0x01105908, 0x01105A09, 0x01105B0A, 0x01105C14, 0x01105D1E,
        0x01105E28, 0x01105F32, 0x0110603C, 0x01106146, 0x01106250, 0x0110635A, 0x01106464,
        0x01106600, 0x01106701, 0x01106802, 0x01106903, 0x01106A04, 0x01106B05, 0x01106C06,
        0x01106D07, 0x01106E08, 0x01106F09, 0x0110F000, 0x0110F101, 0x0110F202, 0x0110F303,
        0x0110F404, 0x0110F505, 0x0110F606, 0x0110F707, 0x0110F808, 0x0110F909, 0x01113600,
        0x01113701, 0x01113802, 0x01113903, 0x01113A04, 0x01113B05, 0x01113C06, 0x01113D07,
        0x01113E08, 0x01113F09, 0x0111D000, 0x0111D101, 0x0111D202, 0x0111D303, 0x0111D404,
        0x0111D505, 0x0111D606, 0x0111D707, 0x0111D808, 0x0111D909, 0x0111E101, 0x0111E202,
        0x0111E303, 0x0111E404, 0x0111E505, 0x0111E606, 0x0111E707, 0x0111E808, 0x0111E909,
        0x0111EA0A, 0x0111EB14, 0x0111EC1E, 0x0111ED28, 0x0111EE32, 0x0111EF3C, 0x0111F046,
        0x0111F150, 0x0111F25A, 0x0111F364, 0x0112F000, 0x0112F101, 0x0112F202, 0x0112F303,
        0x0112F404, 0x0112F505, 0x0112F606, 0x0112F707, 0x0112F808, 0x0112F909, 0x01145000,
        0x01145101, 0x01145202, 0x01145303, 0x01145404, 0x01145505, 0x01145606, 0x01145707,
        0x01145808, 0x01145909, 0x0114D000, 0x0114D101, 0x0114D202, 0x0114D303, 0x0114D404,
        0x0114D505, 0x0114D606, 0x0114D707, 0x0114D808, 0x0114D909, 0x01165000, 0x01165101,
        0x01165202, 0x01165303, 0x01165404, 0x01165505, 0x01165606, 0x01165707, 0x01165808,
        0x01165909, 0x0116C000, 0x0116C101, 0x0116C202, 0x0116C303, 0x0116C404, 0x0116C505,
        0x0116C606, 0x0116C707, 0x0116C808, 0x0116C909, 0x01173000, 0x01173101, 0x01173202,
        0x01173303, 0x01173404, 0x01173505, 0x01173606, 0x01173707, 0x01173808, 0x01173909,
        0x01173A0A, 0x01173B14, 0x0118E000, 0x0118E101, 0x0118E202, 0x0118E303, 0x0118E404,
        0x0118E505, 0x0118E606, 0x0118E707, 0x0118E808, 0x0118E909, 0x0118EA0A, 0x0118EB14,
        0x0118EC1E, 0x0118ED28, 0x0118EE32, 0x0118EF3C, 0x0118F046, 0x0118F150, 0x0118F25A,
        0x01195000, 0x01195101, 0x01195202, 0x01195303, 0x01195404, 0x01195505, 0x01195606,
        0x01195707, 0x01195808, 0x01195909, 0x011C5000, 0x011C5101, 0x011C5202, 0x011C5303,
        0x011C5404, 0x011C5505, 0x011C5606, 0x011C5707, 0x011C5808, 0x011C5909, 0x011C5A01,
        0x011C5B02, 0x011C5C03, 0x011C5D04, 0x011C5E05, 0x011C5F06, 0x011C6007, 0x011C6108,
        0x011C6209, 0x011C630A, 0x011C6414, 0x011C651E, 0x011C6628, 0x011C6732, 0x011C683C,
        0x011C6946, 0x011C6A50, 0x011C6B5A, 0x011C6C64, 0x011D5000, 0x011D5101, 0x011D5202,
        0x011D5303, 0x011D5404, 0x011D5505, 0x011D5606, 0x011D5707, 0x011D5808, 0x011D5909,
        0x011DA000, 0x011DA101, 0x011DA202, 0x011DA303, 0x011DA404, 0x011DA505, 0x011DA606,
        0x011DA707, 0x011DA808, 0x011DA909, 0x011FC001, 0x011FC101, 0x011FC201, 0x011FC301,
        0x011FC401, 0x011FC501, 0x011FC603, 0x011FC703, 0x011FC801, 0x011FC901, 0x011FCA01,
        0x011FCB01, 0x011FCC01, 0x011FCD03, 0x011FCE03, 0x011FCF01, 0x011FD001, 0x011FD101,
        0x011FD201, 0x011FD303, 0x011FD401, 0x01240002, 0x01240103, 0x01240204, 0x01240305,
        0x01240406, 0x01240507, 0x01240608, 0x01240709, 0x01240803, 0x01240904, 0x01240A05,
        0x01240B06, 0x01240C07, 0x01240D08, 0x01240E09, 0x01240F04, 0x01241005, 0x01241106,
        0x01241207, 0x01241308, 0x01241409, 0x01241501, 0x01241602, 0x01241703, 0x01241804,
        0x01241905, 0x01241A06, 0x01241B07, 0x01241C08, 0x01241D09, 0x01241E01, 0x01241F02,
        0x01242003, 0x01242104, 0x01242205, 0x01242302, 0x01242403, 0x01242503, 0x01242604,
        0x01242705, 0x01242806, 0x01242907, 0x01242A08, 0x01242B09, 0x01242C01, 0x01242D02,
        0x01242E03, 0x01242F03, 0x01243004, 0x01243105, 0x01243401, 0x01243502, 0x01243603,
        0x01243703, 0x01243804, 0x01243905, 0x01243A03, 0x01243B03, 0x01243C04, 0x01243D04,
        0x01243E04, 0x01243F04, 0x01244006, 0x01244107, 0x01244207, 0x01244307, 0x01244408,
        0x01244508, 0x01244609, 0x01244709, 0x01244809, 0x01244909, 0x01244A02, 0x01244B03,
        0x01244C04, 0x01244D05, 0x01244E06, 0x01244F01, 0x01245002, 0x01245103, 0x01245204,
        0x01245304, 0x01245405, 0x01245505, 0x01245602, 0x01245703, 0x01245801, 0x01245902,
        0x01245A01, 0x01245B02, 0x01245C05, 0x01245D01, 0x01245E02, 0x01245F01, 0x01246001,
        0x01246101, 0x01246201, 0x01246301, 0x01246401, 0x01246501, 0x01246602, 0x01246728,
        0x01246832, 0x01246904, 0x01246A05, 0x01246B06, 0x01246C07, 0x01246D08, 0x01246E09,
        0x016A6000, 0x016A6101, 0x016A6202, 0x016A6303, 0x016A6404, 0x016A6505, 0x016A6606,
        0x016A6707, 0x016A6808, 0x016A6909, 0x016AC000, 0x016AC101, 0x016AC202, 0x016AC303,
        0x016AC404, 0x016AC505, 0x016AC606, 0x016AC707, 0x016AC808, 0x016AC909, 0x016B5000,
        0x016B5101, 0x016B5202, 0x016B5303, 0x016B5404, 0x016B5505, 0x016B5606, 0x016B5707,
        0x016B5808, 0x016B5909, 0x016B5B0A, 0x016B5C64, 0x016E8000, 0x016E8101, 0x016E8202,
        0x016E8303, 0x016E8404, 0x016E8505, 0x016E8606, 0x016E8707, 0x016E8808, 0x016E8909,
        0x016E8A0A, 0x016E8B0B, 0x016E8C0C, 0x016E8D0D, 0x016E8E0E, 0x016E8F0F, 0x016E9010,
        0x016E9111, 0x016E9212, 0x016E9313, 0x016E9401, 0x016E9502, 0x016E9603, 0x01D2E000,
        0x01D2E101, 0x01D2E202, 0x01D2E303, 0x01D2E404, 0x01D2E505, 0x01D2E606, 0x01D2E707,
        0x01D2E808, 0x01D2E909, 0x01D2EA0A, 0x01D2EB0B, 0x01D2EC0C, 0x01D2ED0D, 0x01D2EE0E,
        0x01D2EF0F, 0x01D2F010, 0x01D2F111, 0x01D2F212, 0x01D2F313, 0x01D36001, 0x01D36102,
        0x01D36203, 0x01D36304, 0x01D36405, 0x01D36506, 0x01D36607, 0x01D36708, 0x01D36809,
        0x01D3690A, 0x01D36A14, 0x01D36B1E, 0x01D36C28, 0x01D36D32, 0x01D36E3C, 0x01D36F46,
        0x01D37050, 0x01D3715A, 0x01D37201, 0x01D37302, 0x01D37403, 0x01D37504, 0x01D37605,
        0x01D37701, 0x01D37805, 0x01D7CE00, 0x01D7CF01, 0x01D7D002, 0x01D7D103, 0x01D7D204,
        0x01D7D305, 0x01D7D406, 0x01D7D507, 0x01D7D608, 0x01D7D709, 0x01D7D800, 0x01D7D901,
        0x01D7DA02, 0x01D7DB03, 0x01D7DC04, 0x01D7DD05, 0x01D7DE06, 0x01D7DF07, 0x01D7E008,
        0x01D7E109, 0x01D7E200, 0x01D7E301, 0x01D7E402, 0x01D7E503, 0x01D7E604, 0x01D7E705,
        0x01D7E806, 0x01D7E907, 0x01D7EA08, 0x01D7EB09, 0x01D7EC00, 0x01D7ED01, 0x01D7EE02,
        0x01D7EF03, 0x01D7F004, 0x01D7F105, 0x01D7F206, 0x01D7F307, 0x01D7F408, 0x01D7F509,
        0x01D7F600, 0x01D7F701, 0x01D7F802, 0x01D7F903, 0x01D7FA04, 0x01D7FB05, 0x01D7FC06,
        0x01D7FD07, 0x01D7FE08, 0x01D7FF09, 0x01E14000, 0x01E14101, 0x01E14202, 0x01E14303,
        0x01E14404, 0x01E14505, 0x01E14606, 0x01E14707, 0x01E14808, 0x01E14909, 0x01E2F000,
        0x01E2F101, 0x01E2F202, 0x01E2F303, 0x01E2F404, 0x01E2F505, 0x01E2F606, 0x01E2F707,
        0x01E2F808, 0x01E2F909, 0x01E8C701, 0x01E8C802, 0x01E8C903, 0x01E8CA04, 0x01E8CB05,
        0x01E8CC06, 0x01E8CD07, 0x01E8CE08, 0x01E8CF09, 0x01E95000, 0x01E95101, 0x01E95202,
        0x01E95303, 0x01E95404, 0x01E95505, 0x01E95606, 0x01E95707, 0x01E95808, 0x01E95909,
        0x01EC7101, 0x01EC7202, 0x01EC7303, 0x01EC7404, 0x01EC7505, 0x01EC7606, 0x01EC7707,
        0x01EC7808, 0x01EC7909, 0x01EC7A0A, 0x01EC7B14, 0x01EC7C1E, 0x01EC7D28, 0x01EC7E32,
        0x01EC7F3C, 0x01EC8046, 0x01EC8150, 0x01EC825A, 0x01EC8364, 0x01ECA301, 0x01ECA402,
        0x01ECA503, 0x01ECA604, 0x01ECA705, 0x01ECA806, 0x01ECA907, 0x01ECAA08, 0x01ECAB09,
        0x01ECAD01, 0x01ECAE01, 0x01ECAF03, 0x01ECB101, 0x01ECB202, 0x01ED0101, 0x01ED0202,
        0x01ED0303, 0x01ED0404, 0x01ED0505, 0x01ED0606, 0x01ED0707, 0x01ED0808, 0x01ED0909,
        0x01ED0A0A, 0x01ED0B14, 0x01ED0C1E, 0x01ED0D28, 0x01ED0E32, 0x01ED0F3C, 0x01ED1046,
        0x01ED1150, 0x01ED125A, 0x01ED1364, 0x01ED2F02, 0x01ED3003, 0x01ED3104, 0x01ED3205,
        0x01ED3306, 0x01ED3407, 0x01ED3508, 0x01ED3609, 0x01ED370A, 0x01ED3C01, 0x01ED3D01,
        0x01F10000, 0x01F10100, 0x01F10201, 0x01F10302, 0x01F10403, 0x01F10504, 0x01F10605,
        0x01F10706, 0x01F10807, 0x01F10908, 0x01F10A09, 0x01F10B00, 0x01F10C00, 0x01FBF000,
        0x01FBF101, 0x01FBF202, 0x01FBF303, 0x01FBF404, 0x01FBF505, 0x01FBF606, 0x01FBF707,
        0x01FBF808, 0x01FBF909, 0x02000107, 0x02006404, 0x0200E204, 0x02012105, 0x02092A01,
        0x0209831E, 0x02098C28, 0x02099C28, 0x020AEA06, 0x020AFD03, 0x020B1903, 0x02239002,
        0x02299803, 0x023B1B03, 0x02626D04, 0x02F89009};
    static constexpr uni::detail::pair<char32_t, int16_t> numeric_data16[] = {
        uni::detail::pair<char32_t, int16_t>{0x0BF2, 1000},
        uni::detail::pair<char32_t, int16_t>{0x0D72, 1000},
        uni::detail::pair<char32_t, int16_t>{0x0F33, -1},
        uni::detail::pair<char32_t, int16_t>{0x137C, 10000},
        uni::detail::pair<char32_t, int16_t>{0x216E, 500},
        uni::detail::pair<char32_t, int16_t>{0x216F, 1000},
        uni::detail::pair<char32_t, int16_t>{0x217E, 500},
        uni::detail::pair<char32_t, int16_t>{0x217F, 1000},
        uni::detail::pair<char32_t, int16_t>{0x2180, 1000},
        uni::detail::pair<char32_t, int16_t>{0x2181, 5000},
        uni::detail::pair<char32_t, int16_t>{0x2182, 10000},
        uni::detail::pair<char32_t, int16_t>{0x4E07, 10000},
        uni::detail::pair<char32_t, int16_t>{0x4EDF, 1000},
        uni::detail::pair<char32_t, int16_t>{0x5343, 1000},
        uni::detail::pair<char32_t, int16_t>{0x842C, 10000},
        uni::detail::pair<char32_t, int16_t>{0x9621, 1000},
        uni::detail::pair<char32_t, int16_t>{0x1011A, 200},
        uni::detail::pair<char32_t, int16_t>{0x1011B, 300},
        uni::detail::pair<char32_t, int16_t>{0x1011C, 400},
        uni::detail::pair<char32_t, int16_t>{0x1011D, 500},
        uni::detail::pair<char32_t, int16_t>{0x1011E, 600},
        uni::detail::pair<char32_t, int16_t>{0x1011F, 700},
        uni::detail::pair<char32_t, int16_t>{0x10120, 800},
        uni::detail::pair<char32_t, int16_t>{0x10121, 900},
        uni::detail::pair<char32_t, int16_t>{0x10122, 1000},
        uni::detail::pair<char32_t, int16_t>{0x10123, 2000},
        uni::detail::pair<char32_t, int16_t>{0x10124, 3000},
        uni::detail::pair<char32_t, int16_t>{0x10125, 4000},
        uni::detail::pair<char32_t, int16_t>{0x10126, 5000},
        uni::detail::pair<char32_t, int16_t>{0x10127, 6000},
        uni::detail::pair<char32_t, int16_t>{0x10128, 7000},
        uni::detail::pair<char32_t, int16_t>{0x10129, 8000},
        uni::detail::pair<char32_t, int16_t>{0x1012A, 9000},
        uni::detail::pair<char32_t, int16_t>{0x1012B, 10000},
        uni::detail::pair<char32_t, int16_t>{0x1012C, 20000},
        uni::detail::pair<char32_t, int16_t>{0x1012D, 30000},
        uni::detail::pair<char32_t, int16_t>{0x10145, 500},
        uni::detail::pair<char32_t, int16_t>{0x10146, 5000},
        uni::detail::pair<char32_t, int16_t>{0x1014C, 500},
        uni::detail::pair<char32_t, int16_t>{0x1014D, 1000},
        uni::detail::pair<char32_t, int16_t>{0x1014E, 5000},
        uni::detail::pair<char32_t, int16_t>{0x10153, 500},
        uni::detail::pair<char32_t, int16_t>{0x10154, 1000},
        uni::detail::pair<char32_t, int16_t>{0x10155, 10000},
        uni::detail::pair<char32_t, int16_t>{0x1016B, 300},
        uni::detail::pair<char32_t, int16_t>{0x1016C, 500},
        uni::detail::pair<char32_t, int16_t>{0x1016D, 500},
        uni::detail::pair<char32_t, int16_t>{0x1016E, 500},
        uni::detail::pair<char32_t, int16_t>{0x1016F, 500},
        uni::detail::pair<char32_t, int16_t>{0x10170, 500},
        uni::detail::pair<char32_t, int16_t>{0x10171, 1000},
        uni::detail::pair<char32_t, int16_t>{0x10172, 5000},
        uni::detail::pair<char32_t, int16_t>{0x102F4, 200},
        uni::detail::pair<char32_t, int16_t>{0x102F5, 300},
        uni::detail::pair<char32_t, int16_t>{0x102F6, 400},
        uni::detail::pair<char32_t, int16_t>{0x102F7, 500},
        uni::detail::pair<char32_t, int16_t>{0x102F8, 600},
        uni::detail::pair<char32_t, int16_t>{0x102F9, 700},
        uni::detail::pair<char32_t, int16_t>{0x102FA, 800},
        uni::detail::pair<char32_t, int16_t>{0x102FB, 900},
        uni::detail::pair<char32_t, int16_t>{0x1034A, 900},
        uni::detail::pair<char32_t, int16_t>{0x1085E, 1000},
        uni::detail::pair<char32_t, int16_t>{0x1085F, 10000},
        uni::detail::pair<char32_t, int16_t>{0x109D3, 200},
        uni::detail::pair<char32_t, int16_t>{0x109D4, 300},
        uni::detail::pair<char32_t, int16_t>{0x109D5, 400},
        uni::detail::pair<char32_t, int16_t>{0x109D6, 500},
        uni::detail::pair<char32_t, int16_t>{0x109D7, 600},
        uni::detail::pair<char32_t, int16_t>{0x109D8, 700},
        uni::detail::pair<char32_t, int16_t>{0x109D9, 800},
        uni::detail::pair<char32_t, int16_t>{0x109DA, 900},
        uni::detail::pair<char32_t, int16_t>{0x109DB, 1000},
        uni::detail::pair<char32_t, int16_t>{0x109DC, 2000},
        uni::detail::pair<char32_t, int16_t>{0x109DD, 3000},
        uni::detail::pair<char32_t, int16_t>{0x109DE, 4000},
        uni::detail::pair<char32_t, int16_t>{0x109DF, 5000},
        uni::detail::pair<char32_t, int16_t>{0x109E0, 6000},
        uni::detail::pair<char32_t, int16_t>{0x109E1, 7000},
        uni::detail::pair<char32_t, int16_t>{0x109E2, 8000},
        uni::detail::pair<char32_t, int16_t>{0x109E3, 9000},
        uni::detail::pair<char32_t, int16_t>{0x109E4, 10000},
        uni::detail::pair<char32_t, int16_t>{0x109E5, 20000},
        uni::detail::pair<char32_t, int16_t>{0x109E6, 30000},
        uni::detail::pair<char32_t, int16_t>{0x10A47, 1000},
        uni::detail::pair<char32_t, int16_t>{0x10B5F, 1000},
        uni::detail::pair<char32_t, int16_t>{0x10B7F, 1000},
        uni::detail::pair<char32_t, int16_t>{0x10CFF, 1000},
        uni::detail::pair<char32_t, int16_t>{0x10E73, 200},
        uni::detail::pair<char32_t, int16_t>{0x10E74, 300},
        uni::detail::pair<char32_t, int16_t>{0x10E75, 400},
        uni::detail::pair<char32_t, int16_t>{0x10E76, 500},
        uni::detail::pair<char32_t, int16_t>{0x10E77, 600},
        uni::detail::pair<char32_t, int16_t>{0x10E78, 700},
        uni::detail::pair<char32_t, int16_t>{0x10E79, 800},
        uni::detail::pair<char32_t, int16_t>{0x10E7A, 900},
        uni::detail::pair<char32_t, int16_t>{0x11065, 1000},
        uni::detail::pair<char32_t, int16_t>{0x111F4, 1000},
        uni::detail::pair<char32_t, int16_t>{0x16B5D, 10000},
        uni::detail::pair<char32_t, int16_t>{0x1EC84, 200},
        uni::detail::pair<char32_t, int16_t>{0x1EC85, 300},
        uni::detail::pair<char32_t, int16_t>{0x1EC86, 400},
        uni::detail::pair<char32_t, int16_t>{0x1EC87, 500},
        uni::detail::pair<char32_t, int16_t>{0x1EC88, 600},
        uni::detail::pair<char32_t, int16_t>{0x1EC89, 700},
        uni::detail::pair<char32_t, int16_t>{0x1EC8A, 800},
        uni::detail::pair<char32_t, int16_t>{0x1EC8B, 900},
        uni::detail::pair<char32_t, int16_t>{0x1EC8C, 1000},
        uni::detail::pair<char32_t, int16_t>{0x1EC8D, 2000},
        uni::detail::pair<char32_t, int16_t>{0x1EC8E, 3000},
        uni::detail::pair<char32_t, int16_t>{0x1EC8F, 4000},
        uni::detail::pair<char32_t, int16_t>{0x1EC90, 5000},
        uni::detail::pair<char32_t, int16_t>{0x1EC91, 6000},
        uni::detail::pair<char32_t, int16_t>{0x1EC92, 7000},
        uni::detail::pair<char32_t, int16_t>{0x1EC93, 8000},
        uni::detail::pair<char32_t, int16_t>{0x1EC94, 9000},
        uni::detail::pair<char32_t, int16_t>{0x1EC95, 10000},
        uni::detail::pair<char32_t, int16_t>{0x1EC96, 20000},
        uni::detail::pair<char32_t, int16_t>{0x1EC97, 30000},
        uni::detail::pair<char32_t, int16_t>{0x1ECB3, 10000},
        uni::detail::pair<char32_t, int16_t>{0x1ED14, 200},
        uni::detail::pair<char32_t, int16_t>{0x1ED15, 300},
        uni::detail::pair<char32_t, int16_t>{0x1ED16, 400},
        uni::detail::pair<char32_t, int16_t>{0x1ED17, 500},
        uni::detail::pair<char32_t, int16_t>{0x1ED18, 600},
        uni::detail::pair<char32_t, int16_t>{0x1ED19, 700},
        uni::detail::pair<char32_t, int16_t>{0x1ED1A, 800},
        uni::detail::pair<char32_t, int16_t>{0x1ED1B, 900},
        uni::detail::pair<char32_t, int16_t>{0x1ED1C, 1000},
        uni::detail::pair<char32_t, int16_t>{0x1ED1D, 2000},
        uni::detail::pair<char32_t, int16_t>{0x1ED1E, 3000},
        uni::detail::pair<char32_t, int16_t>{0x1ED1F, 4000},
        uni::detail::pair<char32_t, int16_t>{0x1ED20, 5000},
        uni::detail::pair<char32_t, int16_t>{0x1ED21, 6000},
        uni::detail::pair<char32_t, int16_t>{0x1ED22, 7000},
        uni::detail::pair<char32_t, int16_t>{0x1ED23, 8000},
        uni::detail::pair<char32_t, int16_t>{0x1ED24, 9000},
        uni::detail::pair<char32_t, int16_t>{0x1ED25, 10000},
        uni::detail::pair<char32_t, int16_t>{0x1ED26, 20000},
        uni::detail::pair<char32_t, int16_t>{0x1ED27, 30000},
        uni::detail::pair<char32_t, int16_t>{0x1ED38, 400},
        uni::detail::pair<char32_t, int16_t>{0x1ED39, 600},
        uni::detail::pair<char32_t, int16_t>{0x1ED3A, 2000},
        uni::detail::pair<char32_t, int16_t>{0x1ED3B, 10000},
    };
    static constexpr uni::detail::pair<char32_t, int32_t> numeric_data32[] = {
        uni::detail::pair<char32_t, int32_t>{0x2187, 50000},
        uni::detail::pair<char32_t, int32_t>{0x2188, 100000},
        uni::detail::pair<char32_t, int32_t>{0x4EBF, 100000000},
        uni::detail::pair<char32_t, int32_t>{0x5104, 100000000},
        uni::detail::pair<char32_t, int32_t>{0x1012E, 40000},
        uni::detail::pair<char32_t, int32_t>{0x1012F, 50000},
        uni::detail::pair<char32_t, int32_t>{0x10130, 60000},
        uni::detail::pair<char32_t, int32_t>{0x10131, 70000},
        uni::detail::pair<char32_t, int32_t>{0x10132, 80000},
        uni::detail::pair<char32_t, int32_t>{0x10133, 90000},
        uni::detail::pair<char32_t, int32_t>{0x10147, 50000},
        uni::detail::pair<char32_t, int32_t>{0x10156, 50000},
        uni::detail::pair<char32_t, int32_t>{0x109E7, 40000},
        uni::detail::pair<char32_t, int32_t>{0x109E8, 50000},
        uni::detail::pair<char32_t, int32_t>{0x109E9, 60000},
        uni::detail::pair<char32_t, int32_t>{0x109EA, 70000},
        uni::detail::pair<char32_t, int32_t>{0x109EB, 80000},
        uni::detail::pair<char32_t, int32_t>{0x109EC, 90000},
        uni::detail::pair<char32_t, int32_t>{0x109ED, 100000},
        uni::detail::pair<char32_t, int32_t>{0x109EE, 200000},
        uni::detail::pair<char32_t, int32_t>{0x109EF, 300000},
        uni::detail::pair<char32_t, int32_t>{0x109F0, 400000},
        uni::detail::pair<char32_t, int32_t>{0x109F1, 500000},
        uni::detail::pair<char32_t, int32_t>{0x109F2, 600000},
        uni::detail::pair<char32_t, int32_t>{0x109F3, 700000},
        uni::detail::pair<char32_t, int32_t>{0x109F4, 800000},
        uni::detail::pair<char32_t, int32_t>{0x109F5, 900000},
        uni::detail::pair<char32_t, int32_t>{0x12432, 216000},
        uni::detail::pair<char32_t, int32_t>{0x12433, 432000},
        uni::detail::pair<char32_t, int32_t>{0x16B5E, 1000000},
        uni::detail::pair<char32_t, int32_t>{0x16B5F, 100000000},
        uni::detail::pair<char32_t, int32_t>{0x1EC98, 40000},
        uni::detail::pair<char32_t, int32_t>{0x1EC99, 50000},
        uni::detail::pair<char32_t, int32_t>{0x1EC9A, 60000},
        uni::detail::pair<char32_t, int32_t>{0x1EC9B, 70000},
        uni::detail::pair<char32_t, int32_t>{0x1EC9C, 80000},
        uni::detail::pair<char32_t, int32_t>{0x1EC9D, 90000},
        uni::detail::pair<char32_t, int32_t>{0x1EC9E, 100000},
        uni::detail::pair<char32_t, int32_t>{0x1EC9F, 200000},
        uni::detail::pair<char32_t, int32_t>{0x1ECA0, 100000},
        uni::detail::pair<char32_t, int32_t>{0x1ECA1, 10000000},
        uni::detail::pair<char32_t, int32_t>{0x1ECA2, 20000000},
        uni::detail::pair<char32_t, int32_t>{0x1ECB4, 100000},
        uni::detail::pair<char32_t, int32_t>{0x1ED28, 40000},
        uni::detail::pair<char32_t, int32_t>{0x1ED29, 50000},
        uni::detail::pair<char32_t, int32_t>{0x1ED2A, 60000},
        uni::detail::pair<char32_t, int32_t>{0x1ED2B, 70000},
        uni::detail::pair<char32_t, int32_t>{0x1ED2C, 80000},
        uni::detail::pair<char32_t, int32_t>{0x1ED2D, 90000},
    };
    static constexpr uni::detail::pair<char32_t, int64_t> numeric_data64[] = {
        uni::detail::pair<char32_t, int64_t>{0x5146, 1000000000000},
        uni::detail::pair<char32_t, int64_t>{0x16B60, 10000000000},
        uni::detail::pair<char32_t, int64_t>{0x16B61, 1000000000000},
    };
    static constexpr uni::detail::pair<char32_t, int16_t> numeric_data_d[] = {
        uni::detail::pair<char32_t, int16_t>{0x00BC, 4},
        uni::detail::pair<char32_t, int16_t>{0x00BD, 2},
        uni::detail::pair<char32_t, int16_t>{0x00BE, 4},
        uni::detail::pair<char32_t, int16_t>{0x09F4, 16},
        uni::detail::pair<char32_t, int16_t>{0x09F5, 8},
        uni::detail::pair<char32_t, int16_t>{0x09F6, 16},
        uni::detail::pair<char32_t, int16_t>{0x09F7, 4},
        uni::detail::pair<char32_t, int16_t>{0x09F8, 4},
        uni::detail::pair<char32_t, int16_t>{0x0B72, 4},
        uni::detail::pair<char32_t, int16_t>{0x0B73, 2},
        uni::detail::pair<char32_t, int16_t>{0x0B74, 4},
        uni::detail::pair<char32_t, int16_t>{0x0B75, 16},
        uni::detail::pair<char32_t, int16_t>{0x0B76, 8},
        uni::detail::pair<char32_t, int16_t>{0x0B77, 16},
        uni::detail::pair<char32_t, int16_t>{0x0D58, 160},
        uni::detail::pair<char32_t, int16_t>{0x0D59, 40},
        uni::detail::pair<char32_t, int16_t>{0x0D5A, 80},
        uni::detail::pair<char32_t, int16_t>{0x0D5B, 20},
        uni::detail::pair<char32_t, int16_t>{0x0D5C, 10},
        uni::detail::pair<char32_t, int16_t>{0x0D5D, 20},
        uni::detail::pair<char32_t, int16_t>{0x0D5E, 5},
        uni::detail::pair<char32_t, int16_t>{0x0D73, 4},
        uni::detail::pair<char32_t, int16_t>{0x0D74, 2},
        uni::detail::pair<char32_t, int16_t>{0x0D75, 4},
        uni::detail::pair<char32_t, int16_t>{0x0D76, 16},
        uni::detail::pair<char32_t, int16_t>{0x0D77, 8},
        uni::detail::pair<char32_t, int16_t>{0x0D78, 16},
        uni::detail::pair<char32_t, int16_t>{0x0F2A, 2},
        uni::detail::pair<char32_t, int16_t>{0x0F2B, 2},
        uni::detail::pair<char32_t, int16_t>{0x0F2C, 2},
        uni::detail::pair<char32_t, int16_t>{0x0F2D, 2},
        uni::detail::pair<char32_t, int16_t>{0x0F2E, 2},
        uni::detail::pair<char32_t, int16_t>{0x0F2F, 2},
        uni::detail::pair<char32_t, int16_t>{0x0F30, 2},
        uni::detail::pair<char32_t, int16_t>{0x0F31, 2},
        uni::detail::pair<char32_t, int16_t>{0x0F32, 2},
        uni::detail::pair<char32_t, int16_t>{0x0F33, 2},
        uni::detail::pair<char32_t, int16_t>{0x2150, 7},
        uni::detail::pair<char32_t, int16_t>{0x2151, 9},
        uni::detail::pair<char32_t, int16_t>{0x2152, 10},
        uni::detail::pair<char32_t, int16_t>{0x2153, 3},
        uni::detail::pair<char32_t, int16_t>{0x2154, 3},
        uni::detail::pair<char32_t, int16_t>{0x2155, 5},
        uni::detail::pair<char32_t, int16_t>{0x2156, 5},
        uni::detail::pair<char32_t, int16_t>{0x2157, 5},
        uni::detail::pair<char32_t, int16_t>{0x2158, 5},
        uni::detail::pair<char32_t, int16_t>{0x2159, 6},
        uni::detail::pair<char32_t, int16_t>{0x215A, 6},
        uni::detail::pair<char32_t, int16_t>{0x215B, 8},
        uni::detail::pair<char32_t, int16_t>{0x215C, 8},
        uni::detail::pair<char32_t, int16_t>{0x215D, 8},
        uni::detail::pair<char32_t, int16_t>{0x215E, 8},
        uni::detail::pair<char32_t, int16_t>{0x2CFD, 2},
        uni::detail::pair<char32_t, int16_t>{0xA830, 4},
        uni::detail::pair<char32_t, int16_t>{0xA831, 2},
        uni::detail::pair<char32_t, int16_t>{0xA832, 4},
        uni::detail::pair<char32_t, int16_t>{0xA833, 16},
        uni::detail::pair<char32_t, int16_t>{0xA834, 8},
        uni::detail::pair<char32_t, int16_t>{0xA835, 16},
        uni::detail::pair<char32_t, int16_t>{0x10140, 4},
        uni::detail::pair<char32_t, int16_t>{0x10141, 2},
        uni::detail::pair<char32_t, int16_t>{0x10175, 2},
        uni::detail::pair<char32_t, int16_t>{0x10176, 2},
        uni::detail::pair<char32_t, int16_t>{0x10177, 3},
        uni::detail::pair<char32_t, int16_t>{0x10178, 4},
        uni::detail::pair<char32_t, int16_t>{0x1018B, 4},
        uni::detail::pair<char32_t, int16_t>{0x109BC, 12},
        uni::detail::pair<char32_t, int16_t>{0x109BD, 2},
        uni::detail::pair<char32_t, int16_t>{0x109F6, 12},
        uni::detail::pair<char32_t, int16_t>{0x109F7, 12},
        uni::detail::pair<char32_t, int16_t>{0x109F8, 12},
        uni::detail::pair<char32_t, int16_t>{0x109F9, 12},
        uni::detail::pair<char32_t, int16_t>{0x109FA, 12},
        uni::detail::pair<char32_t, int16_t>{0x109FB, 12},
        uni::detail::pair<char32_t, int16_t>{0x109FC, 12},
        uni::detail::pair<char32_t, int16_t>{0x109FD, 12},
        uni::detail::pair<char32_t, int16_t>{0x109FE, 12},
        uni::detail::pair<char32_t, int16_t>{0x109FF, 12},
        uni::detail::pair<char32_t, int16_t>{0x10A48, 2},
        uni::detail::pair<char32_t, int16_t>{0x10E7B, 2},
        uni::detail::pair<char32_t, int16_t>{0x10E7C, 4},
        uni::detail::pair<char32_t, int16_t>{0x10E7D, 3},
        uni::detail::pair<char32_t, int16_t>{0x10E7E, 3},
        uni::detail::pair<char32_t, int16_t>{0x10F26, 2},
        uni::detail::pair<char32_t, int16_t>{0x11FC0, 320},
        uni::detail::pair<char32_t, int16_t>{0x11FC1, 160},
        uni::detail::pair<char32_t, int16_t>{0x11FC2, 80},
        uni::detail::pair<char32_t, int16_t>{0x11FC3, 64},
        uni::detail::pair<char32_t, int16_t>{0x11FC4, 40},
        uni::detail::pair<char32_t, int16_t>{0x11FC5, 32},
        uni::detail::pair<char32_t, int16_t>{0x11FC6, 80},
        uni::detail::pair<char32_t, int16_t>{0x11FC7, 64},
        uni::detail::pair<char32_t, int16_t>{0x11FC8, 20},
        uni::detail::pair<char32_t, int16_t>{0x11FC9, 16},
        uni::detail::pair<char32_t, int16_t>{0x11FCA, 16},
        uni::detail::pair<char32_t, int16_t>{0x11FCB, 10},
        uni::detail::pair<char32_t, int16_t>{0x11FCC, 8},
        uni::detail::pair<char32_t, int16_t>{0x11FCD, 20},
        uni::detail::pair<char32_t, int16_t>{0x11FCE, 16},
        uni::detail::pair<char32_t, int16_t>{0x11FCF, 5},
        uni::detail::pair<char32_t, int16_t>{0x11FD0, 4},
        uni::detail::pair<char32_t, int16_t>{0x11FD1, 2},
        uni::detail::pair<char32_t, int16_t>{0x11FD2, 2},
        uni::detail::pair<char32_t, int16_t>{0x11FD3, 4},
        uni::detail::pair<char32_t, int16_t>{0x11FD4, 320},
        uni::detail::pair<char32_t, int16_t>{0x1245A, 3},
        uni::detail::pair<char32_t, int16_t>{0x1245B, 3},
        uni::detail::pair<char32_t, int16_t>{0x1245C, 6},
        uni::detail::pair<char32_t, int16_t>{0x1245D, 3},
        uni::detail::pair<char32_t, int16_t>{0x1245E, 3},
        uni::detail::pair<char32_t, int16_t>{0x1245F, 8},
        uni::detail::pair<char32_t, int16_t>{0x12460, 4},
        uni::detail::pair<char32_t, int16_t>{0x12461, 6},
        uni::detail::pair<char32_t, int16_t>{0x12462, 4},
        uni::detail::pair<char32_t, int16_t>{0x12463, 4},
        uni::detail::pair<char32_t, int16_t>{0x12464, 2},
        uni::detail::pair<char32_t, int16_t>{0x12465, 3},
        uni::detail::pair<char32_t, int16_t>{0x12466, 3},
        uni::detail::pair<char32_t, int16_t>{0x1ECAD, 4},
        uni::detail::pair<char32_t, int16_t>{0x1ECAE, 2},
        uni::detail::pair<char32_t, int16_t>{0x1ECAF, 4},
        uni::detail::pair<char32_t, int16_t>{0x1ED3C, 2},
        uni::detail::pair<char32_t, int16_t>{0x1ED3D, 6},
        uni::detail::pair<char32_t, int16_t>{0x110000, 0}};
    static constexpr bool_trie<32, 991, 1, 0, 112, 255, 1, 0, 1407, 1, 0, 160> prop_assigned{
        {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
         0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
         0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
         0xffffffffffffffff, 0xfcffffffffffffff, 0xfffffffbffffd7f0, 0xffffffffffffffff,
         0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
         0xfffeffffffffffff, 0xfffffffffe7fffff, 0xfffffffffffee7ff, 0x001f87ffffff00ff,
         0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
         0xffffffffffffbfff, 0xffffffffffffe7ff, 0x0003ffffffffffff, 0xe7ffffffffffffff},
        {1,   2,   3,   3,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,
         17,  18,  19,  20,  21,  22,  23,  24,  25,  3,   26,  27,  28,  3,   3,   3,   29,  3,
         3,   3,   3,   3,   30,  31,  32,  33,  34,  35,  36,  3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   37,  38,  39,  40,  3,   41,  35,  38,  42,  43,  44,  45,  46,  47,  48,
         49,  50,  51,  3,   52,  3,   53,  54,  55,  56,  57,  3,   3,   3,   3,   3,   3,   3,
         3,   58,  59,  60,  61,  3,   62,  63,  64,  3,   3,   65,  3,   3,   3,   3,   3,   3,
         3,   3,   3,   66,  67,  3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   68,  69,  3,   3,
         3,   3,   70,  71,  72,  73,  74,  3,   75,  76,  77,  3,   3,   3,   78,  3,   79,  80,
         3,   81,  3,   82,  83,  84,  3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   85,  86,  3,   3,   3,
         3,   87,  3,   3,   88,  3,   3,   3,   89,  90,  88,  3,   91,  3,   92,  3,   93,  94,
         95,  3,   96,  97,  46,  3,   98,  3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,
         3,   83,  99,  100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
         100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
         100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
         100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
         100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
         100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
         100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
         100, 100, 100, 100, 100, 100, 100, 100, 100, 3,   3,   3,   3,   3,   101, 3,   102, 103,
         104, 3,   105, 3,   3,   3,   3,   3,   3,   106, 107, 35,  108, 3,   109, 79,  3,   110,
         111},
        {0x7fff3fffffffffff, 0xffff07ff4fffffff, 0xffffffffff037fff, 0xffffffffffffffff,
         0xf3c5fdfffff99fef, 0x7fffffcfb080799f, 0xd36dfdfffff987ee, 0x007fffc05e023987,
         0xf3edfdfffffbbfee, 0xfe03ffcf00013bbf, 0xf3edfdfffff99fee, 0x00ffffcfb0e0399f,
         0xc3ffc718d63dc7ec, 0x07ffffc000813dc7, 0xf3fffdfffffddfff, 0xff80ffcf27603ddf,
         0xf3effdfffffddfff, 0x0006ffcf60603ddf, 0xfffffffffffddfff, 0xffffffcffff0fddf,
         0x2ffbfffffc7fffee, 0x001cffc0ff5f847f, 0x87fffffffffffffe, 0x000000000fffffff,
         0x3fffffaffffff7d6, 0x00000000f3ff3f5f, 0xfffe1ffffffffeff, 0xdffffffffeffffff,
         0x0000000007ffdfff, 0xffffffffffff20bf, 0xffffffff3d7f3dff, 0x7f3dffffffff3dff,
         0xffffffffff7fff3d, 0xffffffffff3dffff, 0x1fffffffe7ffffff, 0xffffffff03ffffff,
         0x3f3fffffffffffff, 0xffffffff1fffffff, 0x01ffffffffffffff, 0x007fffff803fffff,
         0x000ddfff000fffff, 0x03ff03ff3fffffff, 0xffff07ffffffffff, 0x003fffffffffffff,
         0x0fff0fff7fffffff, 0x001f3ffffffffff1, 0xffff0fffffffffff, 0xffffffffc7ff03ff,
         0xffffffffcfffffff, 0x9fffffff7fffffff, 0xffff3fff03ff03ff, 0x0000000000007fff,
         0x7fffffffffff1fff, 0xf00fffffffffffff, 0xf8ffffffffffffff, 0xffffffffffffe3ff,
         0xe7ffffffffff01ff, 0x07ffffffffff00ff, 0xffffffff3f3fffff, 0x3fffffffaaff3f3f,
         0xffdfffffffffffff, 0x7fdcffffefcfffdf, 0xfff3ffdfffffffff, 0xffffffff1fff7fff,
         0x0001ffffffff0001, 0xffffffffffff0fff, 0x0000007fffffffff, 0xffffffff000007ff,
         0xffcfffffffffffff, 0xffffffffffbfffff, 0xfe0fffffffffffff, 0xffff20bfffffffff,
         0x800180ffffffffff, 0x7f7f7f7f007fffff, 0xffffffff7f7f7f7f, 0x000000003fffffff,
         0xfffffffffbffffff, 0x000fffffffffffff, 0x0fff0000003fffff, 0xfffffffffffffffe,
         0xfffffffffe7fffff, 0xfffeffffffffffe0, 0xffffffffffff7fff, 0xffff000fffffffff,
         0xffffffff7fffffff, 0xffffffffffff1fff, 0xffffffffffff007f, 0x00000fffffffffff,
         0x00ffffffffffffff, 0xfffc000003eb07ff, 0x03ff1fffffffffff, 0xffffffff03ffc03f,
         0x1fffffff800fffff, 0x7fffffffc3ffbfff, 0x007fffffffffffff, 0xfffffffff3ff3fff,
         0x007ffffff8000007, 0xffff7f7f007e7e7e, 0x03ff3fffffffffff, 0x0ffffffffffff87f,
         0x0000000000000000, 0xffff3fffffffffff, 0x0000000003ffffff, 0x5f7fffffe0f8007f,
         0xffffffffffffffdb, 0xfffffffffff80007, 0xfffffffffffcffff, 0xffff0000000080ff,
         0xffdf0f7ffff7ffff, 0x9fffffffffffffff, 0x7fffffffffffffff, 0x3e007f7f1cfcfcfc},
        {1,  2,  3, 4,  5,  6, 7,  8, 5, 9, 10, 11, 12, 13, 14, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 15,
         16, 17, 7, 18, 19, 7, 20, 5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 21,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5},
        {1,   2,   3,   4,   2,   5,   6,   7,   7,   8,   9,   10,  11,  12,  13,  2,   2,   14,
         15,  16,  17,  18,  7,   2,   2,   2,   2,   19,  20,  21,  7,   22,  23,  24,  25,  26,
         7,   27,  28,  29,  30,  31,  32,  33,  34,  35,  7,   2,   36,  37,  38,  39,  7,   7,
         7,   7,   40,  41,  7,   16,  42,  43,  44,  2,   45,  2,   46,  47,  48,  2,   49,  50,
         7,   51,  52,  53,  54,  7,   7,   2,   55,  2,   56,  7,   7,   57,  58,  2,   59,  60,
         61,  62,  63,  7,   7,   64,  7,   65,  66,  67,  68,  69,  70,  2,   71,  72,  73,  7,
         7,   7,   7,   74,  75,  76,  7,   77,  78,  79,  7,   7,   7,   7,   80,  7,   7,   81,
         82,  2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   83,  7,   2,
         84,  2,   2,   2,   85,  7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   86,  37,  2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   87,  7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   2,   2,   2,   2,   2,   2,   2,   2,   2,   63,  7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   2,   2,   2,   2,   2,   2,   2,   2,   73,  88,  89,  90,  2,   91,  92,  7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   2,   93,  7,   2,   94,  95,  96,  2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   97,  2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   98,  36,  7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   99,  2,   2,   2,   2,   100, 101, 2,   2,   2,
         2,   2,   64,  7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   2,   102, 103, 7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   104, 105, 2,   85,  2,   2,   2,   106, 107, 2,   2,
         108, 2,   109, 7,   110, 2,   111, 7,   7,   2,   112, 113, 114, 115, 116, 2,   2,   2,
         2,   117, 2,   2,   2,   2,   118, 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   119,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   120,
         7,   7,   7,   121, 7,   7,   7,   122, 123, 7,   7,   7,   7,   124, 125, 7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   126, 2,
         2,   2,   127, 2,   128, 7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   129, 130,
         7,   131, 7,   7,   7,   132, 133, 134, 135, 7,   7,   7,   7,   136, 2,   137, 138, 2,
         2,   139, 140, 141, 142, 7,   7,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   143, 2,   144, 2,   145, 146, 147, 148, 7,   2,   2,   2,   2,   2,
         149, 150, 151, 2,   2,   152, 153, 7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   31,  2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   73,  2,   2,   2,   154, 2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   155, 2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   156, 7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   2,   2,   2,   2,   2,   2,   2,   2,   58,  7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   157, 7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   158, 2,   7,   7,   2,   2,   2,
         159, 7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7},
        {0xb7ffff7fffffefff, 0x000000003fff3fff, 0xffffffffffffffff, 0x07ffffffffffffff,
         0xff8fffffffffff87, 0x000000011fff7fff, 0x3fffffffffff0000, 0x0000000000000000,
         0xffffffff1fffffff, 0x0fffffff0001ffff, 0xffffe00fffffffff, 0x07ffffffffff07ff,
         0xffffffffbfffffff, 0x00000000003fff0f, 0xffff03ff3fffffff, 0x0fffffffff0fffff,
         0xffff00ffffffffff, 0xf7ff800fffffffff, 0x1bfbfffbffb7f7ff, 0x007fffffffffffff,
         0x000000ff003fffff, 0x07fdffffffffffbf, 0x91bffffffffffd3f, 0xffffffffffbfffff,
         0x0000ff807fffffff, 0xf837ffff00000000, 0x83ffffff8fffffff, 0xf0ffffffffffffff,
         0xfffffffffffcffff, 0x873ffffffeeff06f, 0xffffffff01ff01ff, 0x00000000ffffffff,
         0x007ff87fffffffff, 0xfe3fffffffffffff, 0xff07ffffff3fffff, 0x0000fe001e03ffff,
         0x00000000000001ff, 0x0007ffffffffffff, 0xfc07ffffffffffff, 0x03ff00ffffffffff,
         0x7fffffff00000000, 0x00033bffffffffff, 0xffff000003ffffff, 0xffff0000000003ff,
         0x007fffff00000fff, 0x803ffffffffc3fff, 0x03ff01ffffff2007, 0xffdfffffffffffff,
         0x007fffffffff00ff, 0x001ffffeffffffff, 0x7ffffffffffbffff, 0xffff03ffbfffbd7f,
         0x03ff07ffffffffff, 0xfbedfdfffff99fef, 0x001f1fcfe081399f, 0x00000003efffffff,
         0x0000000003ff00ff, 0xff3fffffffffffff, 0x000000003fffffff, 0x00001fff03ff001f,
         0x03ffffffffffffff, 0x00000000000003ff, 0xffff0fffe7ffffff, 0x000000000000007f,
         0x0fffffffffffffff, 0xffffffff00000000, 0x8007ffffffffffff, 0xf9bfffffff6ff27f,
         0x0000000003ff007f, 0xfffffcff00000000, 0x0000001ffcffffff, 0xffffffffffff00ff,
         0xffff0007ffffffff, 0x01ffffffffffffff, 0xff7ffffffffffdff, 0xffff1fffffff003f,
         0x007ffefffffcffff, 0xb47ffffffffffb7f, 0xfffffdbf03ff00ff, 0x000003ff01fb7fff,
         0x01ffffff00000000, 0x0001000000000000, 0x8003ffffffffffff, 0x0000000003ffffff,
         0x001f7fffffffffff, 0x000000000000000f, 0xffffffffffff0000, 0x01ff7fffffffffff,
         0xffffc3ff7fffffff, 0x7fffffffffffffff, 0x003f3fffffff03ff, 0xe0fffffbfbff003f,
         0x000000000000ffff, 0x0000000007ffffff, 0xffffffffffff87ff, 0x00000000ffff80ff,
         0x0003001f00000000, 0x00ffffffffffffff, 0x00000000003fffff, 0x6fef000000000000,
         0x00000007ffffffff, 0xffff00f000070000, 0x1fff07ffffffffff, 0x0000000ff3ff01ff,
         0xffff3fffffffffff, 0xffffffffffff007f, 0x003fffffffffffff, 0xfffffe7fffffffff,
         0x000007ffffffffff, 0x000000000000003f, 0x000fffff00000000, 0x01ffffff007fffff,
         0xffffffffffdfffff, 0xebffde64dfffffff, 0xffffffffffffffef, 0x7bffffffdfdfe7bf,
         0xfffffffffffdfc5f, 0xffffff3fffffffff, 0xffffffffffffcfff, 0x0000fffef8000fff,
         0x000000007fffffff, 0x000007dbf9ffff7f, 0x3fff1fffffffffff, 0x000000000000c3ff,
         0x00007fffffff0000, 0x83ffffffffffffff, 0x7fff6f7f00000000, 0x00000000007fff9f,
         0x00000000c3ff0fff, 0xfffe000000000000, 0x001fffffffffffff, 0x3ffffffffffffffe,
         0x0af7fe96ffffffef, 0x5ef7f796aa96ea84, 0x0ffffbee0ffffbff, 0x0003000000000000,
         0xffff0fffffffffff, 0xfffe7fff000fffff, 0x003ffffffffefffe, 0x00003fffffffffff,
         0xffffffc000000000, 0x0fffffffffff0007, 0x0000003f000301ff, 0x1fff1fffe0ffffff,
         0x000fffffffffffff, 0x00010fff01ffffff, 0xffffffffffff0fff, 0xffffffff03ff00ff,
         0x00033fffffff00ff, 0x1f1f3fff000fffff, 0x07ff1fffffff007f, 0x007f00ff03ff003f,
         0xfffffffffff7ffff, 0x03ff0000000007ff, 0xffffffff3fffffff, 0xffff0003ffffffff,
         0x00000001ffffffff, 0x00000000000007ff, 0xffffffff00000002, 0x0000ffffffffffff}};
}    // namespace detail::tables
enum class property {
    ahex,
    ascii_hex_digit = ahex,
    alpha,
    alphabetic = alpha,
    bidi_c,
    bidi_control = bidi_c,
    bidi_m,
    bidi_mirrored = bidi_m,
    cased,
    ci,
    case_ignorable = ci,
    dash,
    dep,
    deprecated = dep,
    di,
    default_ignorable_code_point = di,
    dia,
    diacritic = dia,
    emoji,
    emoji_component,
    emoji_modifier,
    emoji_modifier_base,
    emoji_presentation,
    ext,
    extender = ext,
    extended_pictographic,
    gr_base,
    grapheme_base = gr_base,
    gr_ext,
    grapheme_extend = gr_ext,
    hex,
    hex_digit = hex,
    idc,
    id_continue = idc,
    ideo,
    ideographic = ideo,
    ids,
    id_start = ids,
    idsb,
    ids_binary_operator = idsb,
    idst,
    ids_trinary_operator = idst,
    join_c,
    join_control = join_c,
    loe,
    logical_order_exception = loe,
    lower,
    lowercase = lower,
    math,
    nchar,
    noncharacter_code_point = nchar,
    pat_syn,
    pattern_syntax = pat_syn,
    pat_ws,
    pattern_white_space = pat_ws,
    pcm,
    prepended_concatenation_mark = pcm,
    qmark,
    quotation_mark = qmark,
    radical,
    ri,
    regional_indicator = ri,
    sd,
    soft_dotted = sd,
    sterm,
    sentence_terminal = sterm,
    term,
    terminal_punctuation = term,
    uideo,
    unified_ideograph = uideo,
    upper,
    uppercase = upper,
    vs,
    variation_selector = vs,
    wspace,
    white_space = wspace,
    space = wspace,
    xidc,
    xid_continue = xidc,
    xids,
    xid_start = xids,
    max
};
namespace detail::tables {
    static constexpr range_array prop_ahex_data = {0x00000000, 0x00003001, 0x00003A00, 0x00004101,
                                                   0x00004700, 0x00006101, 0x00006700};
    static constexpr bool_trie<32, 991, 1, 0, 133, 255, 1, 0, 1279, 1, 0, 127> prop_alpha_data{
        {0x0000000000000000, 0x07fffffe07fffffe, 0x0420040000000000, 0xff7fffffff7fffff,
         0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
         0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0x0000501f0003ffc3,
         0x0000000000000000, 0xbcdf000000000020, 0xfffffffbffffd740, 0xffbfffffffffffff,
         0xffffffffffffffff, 0xffffffffffffffff, 0xfffffffffffffc03, 0xffffffffffffffff,
         0xfffeffffffffffff, 0xffffffff027fffff, 0xbfff0000000001ff, 0x000787ffffff00b6,
         0xffffffff07ff0000, 0xffffc000feffffff, 0xffffffffffffffff, 0x9c00e1fe1fefffff,
         0xffffffffffff0000, 0xffffffffffffe000, 0x0003ffffffffffff, 0x043007fffffffc00},
        {1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12, 13,  14,  15,  16,  17,  18,
         19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30, 31,  32,  33,  34,  35,  36,
         36,  36,  36,  36,  37,  38,  39,  40,  41,  42,  43,  44, 36,  36,  36,  36,  36,  36,
         36,  36,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54, 55,  56,  57,  58,  59,  60,
         61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  36, 36,  36,  72,  36,  36,  36,
         36,  73,  74,  75,  76,  31,  77,  78,  31,  79,  80,  81, 31,  31,  31,  31,  31,  31,
         31,  31,  31,  31,  31,  82,  83,  31,  31,  31,  31,  31, 31,  31,  31,  31,  31,  31,
         31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31, 31,  31,  31,  31,  31,  36,
         36,  36,  84,  85,  86,  87,  88,  89,  31,  31,  31,  31, 31,  31,  31,  90,  44,  91,
         92,  93,  36,  94,  95,  31,  31,  31,  31,  31,  31,  31, 31,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  31,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  96,  97,  36,  36,  36,
         36,  98,  99,  36,  100, 101, 36,  102, 103, 104, 105, 36, 106, 107, 108, 109, 110, 68,
         111, 112, 113, 114, 115, 36,  116, 36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36,  36, 36,  36,  36,  36,  36,  36,
         36,  117, 118, 31,  31,  31,  31,  31,  31,  31,  31,  31, 31,  31,  31,  31,  31,  31,
         31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31, 31,  31,  31,  31,  31,  31,
         31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31, 31,  31,  31,  31,  31,  31,
         31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31, 31,  31,  31,  31,  31,  31,
         31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31, 31,  31,  31,  31,  31,  31,
         31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31, 31,  31,  31,  31,  31,  31,
         31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31,  31, 31,  31,  31,  31,  31,  31,
         31,  31,  31,  31,  31,  31,  31,  31,  31,  36,  36,  36, 36,  36,  119, 36,  120, 121,
         122, 123, 124, 36,  36,  36,  36,  125, 33,  126, 127, 31, 128, 36,  129, 130, 131, 112,
         132},
        {0x00001ffffcffffff, 0xffff07ff01ffffff, 0xffffffff00007eff, 0xffff03f8fff003ff,
         0xefffffffffffffff, 0xfffe000fffe1dfff, 0xe3c5fdfffff99fef, 0x1003000fb080599f,
         0xc36dfdfffff987ee, 0x003f00005e021987, 0xe3edfdfffffbbfee, 0x1e00000f00011bbf,
         0xe3edfdfffff99fee, 0x0002000fb0c0199f, 0xc3ffc718d63dc7ec, 0x0000000000811dc7,
         0xe3fffdfffffddfef, 0x0000000f27601ddf, 0xe3effdfffffddfef, 0x0006000f60601ddf,
         0xe7fffffffffddfff, 0xfc00000f80f05ddf, 0x2ffbfffffc7fffee, 0x000c0000ff5f807f,
         0x07fffffffffffffe, 0x000000000000207f, 0x3bffffaffffff7d6, 0x00000000f000205f,
         0x0000000000000001, 0xfffe1ffffffffeff, 0x1ffffffffeffff03, 0x0000000000000000,
         0xf97fffffffffffff, 0xffffffffffff0000, 0xffffffff3c00ffff, 0xf7ffffffffff20bf,
         0xffffffffffffffff, 0xffffffff3d7f3dff, 0x7f3dffffffff3dff, 0xffffffffff7fff3d,
         0xffffffffff3dffff, 0x0000000007ffffff, 0xffffffff0000ffff, 0x3f3fffffffffffff,
         0xfffffffffffffffe, 0xffff9fffffffffff, 0xffffffff07fffffe, 0x01ffc7ffffffffff,
         0x000fffff800fffff, 0x000ddfff000fffff, 0xffcfffffffffffff, 0x00000000108001ff,
         0xffffffff00000000, 0x01ffffffffffffff, 0xffff07ffffffffff, 0x003fffffffffffff,
         0x01ff0fff7fffffff, 0x001f3fffffff0000, 0xffff0fffffffffff, 0x00000000000003ff,
         0xffffffff0fffffff, 0x001ffffe7fffffff, 0x8000008000000000, 0x0000000000007001,
         0xffefffffffffffff, 0x0000000000001fef, 0xfc00f3ffffffffff, 0x0003ffbfffffffff,
         0x007fffffffffffff, 0x3ffffffffc00e000, 0xe7ffffffffff01ff, 0x046fde0000000000,
         0x001fff8000000000, 0xffffffff3f3fffff, 0x3fffffffaaff3f3f, 0x5fdfffffffffffff,
         0x1fdc1fff0fcf1fdc, 0x8002000000000000, 0x000000001fff0000, 0xf3ffbd503e2ffc84,
         0xffffffff000043e0, 0x00000000000001ff, 0xffc0000000000000, 0x000003ffffffffff,
         0x000c781fffffffff, 0xffff20bfffffffff, 0x000080ffffffffff, 0x7f7f7f7f007fffff,
         0xffffffff7f7f7f7f, 0x0000800000000000, 0x1f3e03fe000000e0, 0xfffffffee07fffff,
         0xf7ffffffffffffff, 0xfffeffffffffffe0, 0xffffffff00007fff, 0xffff000000000000,
         0x0000000000001fff, 0x3fffffffffff0000, 0x00000c00ffff1fff, 0x8ff07fffffffffff,
         0x0000ffffffffffff, 0xfffffffcff800000, 0xfffffffffffff9ff, 0xfffc000003eb07ff,
         0x000000ffffffffbf, 0x000fffffffffffff, 0xe8fc00000000002f, 0xffff07fffffffc00,
         0x1fffffff0007ffff, 0xfff7ffffffffffff, 0x7c00ffff00008000, 0xfc7fffff00003fff,
         0x7fffffffffffffff, 0x003cffff38000005, 0xffff7f7f007e7e7e, 0xffff03fff7ffffff,
         0x000007ffffffffff, 0xffff000fffffffff, 0x0ffffffffffff87f, 0xffff3fffffffffff,
         0x0000000003ffffff, 0x5f7ffdffe0f8007f, 0xffffffffffffffdb, 0x0003ffffffffffff,
         0xfffffffffff80000, 0x3fffffffffffffff, 0xfffffffffffcffff, 0x0fff0000000000ff,
         0xffdf000000000000, 0x1fffffffffffffff, 0x07fffffe00000000, 0xffffffc007fffffe,
         0x000000001cfcfcfc},
        {1,  2,  3, 4,  5,  6, 7,  8, 5, 9, 10, 5, 11, 12, 13, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 14,
         15, 16, 7, 17, 18, 7, 19, 5, 5, 5, 5,  5, 5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5, 5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5, 5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5, 5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5, 5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5, 5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5, 5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5, 5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5, 5,  5,  5,  5, 5, 5, 5, 5, 5},
        {1,   2,   3,   4,  5,   4,  4,   4,   4,   6,   7,   8,   9,   10,  11, 2,  2,  12,  13,
         14,  15,  16,  4,  2,   2,  2,   2,   17,  18,  19,  4,   20,  21,  22, 23, 24, 4,   25,
         4,   26,  27,  28, 29,  30, 31,  32,  4,   2,   33,  34,  34,  35,  4,  4,  4,  4,   4,
         36,  4,   37,  38, 39,  40, 2,   41,  42,  43,  34,  44,  2,   45,  46, 4,  47, 48,  49,
         50,  4,   4,   2,  51,  2,  52,  4,   4,   53,  54,  55,  56,  57,  4,  58, 59, 4,   4,
         60,  4,   61,  62, 63,  64, 65,  66,  67,  68,  69,  60,  4,   4,   4,  4,  70, 71,  72,
         4,   73,  74,  75, 4,   4,  4,   4,   76,  4,   4,   77,  4,   2,   2,  2,  2,  2,   2,
         2,   2,   2,   2,  2,   2,  2,   2,   78,  4,   2,   79,  2,   2,   2,  80, 4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   68,
         81,  2,   2,   2,  2,   2,  2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  79,  4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   2,  2,   2,   2,   2,   2,   2,   2,   2,   59, 4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  2,  2,   2,
         2,   2,   2,   2,  2,   60, 82,  55,  83,  84,  85,  86,  4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   2,  4,   4,  2,   87,  88,  89,  2,   2,   2,   2,   2,  2,  2,  2,   2,
         2,   2,   2,   2,  2,   2,  2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,   2,
         2,   2,   2,   2,  2,   2,  2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,   2,
         2,   2,   2,   2,  2,   2,  2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,   2,
         2,   2,   2,   2,  2,   2,  2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,   2,
         2,   2,   2,   2,  2,   2,  2,   2,   2,   2,   90,  2,   2,   2,   2,  2,  2,  2,   2,
         2,   2,   2,   2,  2,   2,  2,   2,   2,   2,   2,   91,  33,  4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   92,  2,   2,   2,  2,  93, 94,  2,
         2,   2,   2,   2,  95,  4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   2,  96,  97, 4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  2,  98,  99,
         100, 101, 102, 2,  2,   2,  2,   103, 104, 105, 106, 107, 108, 4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   22, 4,   4,  4,   109, 4,   4,   4,   110, 111, 4,   4,  4,  4,  83,  112,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         113, 2,   2,   2,  114, 2,  115, 4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  116, 117, 118, 4,   4,   4,   4,   4,   4,  4,  4,  4,   119,
         120, 121, 4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   2,   2,  2,   2,  2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,   2,
         2,   2,   2,   2,  2,   2,  2,   2,   2,   2,   122, 2,   2,   2,   2,  2,  2,  2,   2,
         2,   2,   2,   2,  2,   2,  2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,   2,
         2,   2,   2,   2,  2,   2,  2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,   2,
         2,   2,   2,   2,  2,   2,  2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,   60,
         2,   2,   2,   10, 2,   2,  2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,   2,
         2,   2,   2,   2,  2,   2,  2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,   2,
         2,   2,   2,   2,  2,   2,  2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,   2,
         2,   2,   2,   2,  2,   2,  2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,   2,
         2,   2,   2,   2,  2,   2,  2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  123, 2,
         2,   2,   2,   2,  2,   2,  2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,   2,
         2,   2,   2,   2,  2,   2,  2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,   2,
         2,   2,   2,   2,  2,   2,  2,   2,   2,   2,   2,   2,   2,   124, 4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   2,  2,   2,   2,   2,   2,   2,   2,   125, 4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   2,
         2,   2,   2,   2,  2,   2,  2,   2,   2,   2,   2,   2,   126, 4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,   4,
         4,   4,   4,   4,  4,   4},
        {0xb7ffff7fffffefff, 0x000000003fff3fff, 0xffffffffffffffff, 0x07ffffffffffffff,
         0x0000000000000000, 0x001fffffffffffff, 0xffffffff1fffffff, 0x000000000001ffff,
         0xffffe000ffffffff, 0x07ffffffffff07ff, 0xffffffff3fffffff, 0x00000000003eff0f,
         0xffff00003fffffff, 0x0fffffffff0fffff, 0xffff00ffffffffff, 0xf7ff000fffffffff,
         0x1bfbfffbffb7f7ff, 0x007fffffffffffff, 0x000000ff003fffff, 0x07fdffffffffffbf,
         0x91bffffffffffd3f, 0x007fffff003fffff, 0x000000007fffffff, 0x0037ffff00000000,
         0x03ffffff003fffff, 0xc0ffffffffffffff, 0x003ffffffeeff06f, 0x1fffffff00000000,
         0x000000001fffffff, 0x0000001ffffffeff, 0x003fffffffffffff, 0x0007ffff003fffff,
         0x000000000003ffff, 0x00000000000001ff, 0x0007ffffffffffff, 0x000000ffffffffff,
         0x00031bffffffffff, 0xffff00801fffffff, 0xffff00000000003f, 0xffff000000000003,
         0x007fffff0000001f, 0x003e00000000003f, 0x01fffffffffffffc, 0x000001ffffff0004,
         0x0047ffffffff00f0, 0x000000001400c01e, 0x409ffffffffbffff, 0xffff01ffbfffbd7f,
         0x000001ffffffffff, 0xe3edfdfffff99fef, 0x0000000fe081199f, 0x00000003800007bb,
         0x00000000000000b3, 0x7f3fffffffffffff, 0x000000003f000000, 0x7fffffffffffffff,
         0x0000000000000011, 0x013fffffffffffff, 0x000007ffe7ffffff, 0x000000000000007f,
         0x01ffffffffffffff, 0xffffffff00000000, 0x80000000ffffffff, 0x99bfffffff6ff27f,
         0x0000000000000007, 0xfffffcff00000000, 0x0000001afcffffff, 0x7fe7ffffffffffff,
         0xffffffffffff0000, 0xffff000020ffffff, 0x7f7ffffffffffdff, 0xfffc000000000001,
         0x007ffefffffcffff, 0xb47ffffffffffb7f, 0xfffffdbf000000cb, 0x00000000017b7fff,
         0x007fffff00000000, 0x0001000000000000, 0x0000000003ffffff, 0x00007fffffffffff,
         0x000000000000000f, 0x0001ffffffffffff, 0xffff00007fffffff, 0x00003fffffff0000,
         0x0000ffffffffffff, 0xe0fffff80000000f, 0x000000000000ffff, 0xffffffffffff87ff,
         0x00000000ffff80ff, 0x0003000b00000000, 0x00ffffffffffffff, 0x00000000003fffff,
         0x6fef000000000000, 0x00000007ffffffff, 0xffff00f000070000, 0x0fffffffffffffff,
         0x1fff07ffffffffff, 0x0000000043ff01ff, 0xffffffffffdfffff, 0xebffde64dfffffff,
         0xffffffffffffffef, 0x7bffffffdfdfe7bf, 0xfffffffffffdfc5f, 0xffffff3fffffffff,
         0xf7fffffff7fffffd, 0xffdfffffffdfffff, 0xffff7fffffff7fff, 0xfffffdfffffffdff,
         0x0000000000000ff7, 0x000007dbf9ffff7f, 0x3f801fffffffffff, 0x0000000000004000,
         0x00000fffffffffff, 0x7fff6f7f00000000, 0x000000000000001f, 0x000000000000088f,
         0x0af7fe96ffffffef, 0x5ef7f796aa96ea84, 0x0ffffbee0ffffbff, 0xffff000000000000,
         0xffff03ffffff03ff, 0x00000000000003ff, 0x00000000ffffffff, 0xffff0003ffffffff,
         0x00000001ffffffff, 0x000000003fffffff, 0x00000000000007ff}};
    static constexpr range_array prop_bidi_c_data = {0x00000000, 0x00061C01, 0x00061D00,
                                                     0x00200E01, 0x00201000, 0x00202A01,
                                                     0x00202F00, 0x00206601, 0x00206A00};
    static constexpr bool_trie<32, 962, 28, 2, 27, 1, 13, 242, 5, 91, 32, 6> prop_bidi_m_data{
        {0x5000030000000000, 0x2800000028000000, 0x0800080000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000},
        {1, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  2,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 3, 4, 5, 0,  0,  6,  0, 0, 7,  8,
         9, 10, 11, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 12, 0,  13, 0, 0, 0,  0,
         0, 0,  14, 15, 16, 17, 18, 19, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0,  21, 22, 0, 0, 0,  0,
         0, 0,  23, 0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0,  0,
         0, 0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  24, 0, 0, 25, 26},
        {0x0000000000000000, 0x3c00000000000000, 0x0000000018000000, 0x0600000000000000,
         0x6000000000000060, 0x0000000000006000, 0x0000000000000001, 0xfa0ff857bc623f1e,
         0xffffcff5803c1fff, 0xc1ffffcc01079fff, 0xffff3fffffc33e00, 0x0000060300000f00,
         0x003fff0000000000, 0x0000fffc70783b79, 0x0100fffdf9fffff8, 0x33f0033a1f37c23f,
         0x70307a53dffffc00, 0xfe19bc3001800000, 0xffffbfcfffffffff, 0x2f88707c507fffff,
         0x4000000000000000, 0x000003ff3000363c, 0x000000001fe00000, 0x000000000ff3ff00,
         0x000000307e000000, 0x2800000050000300, 0x0000000da8000000},
        {1},
        {1, 2, 3, 4, 5},
        {0x0000000000000000, 0x0000000008000000, 0x0000000000200000, 0x0000000000008000,
         0x0000000000000200, 0x0000000000000008}};
    static constexpr bool_trie<0, 969, 5, 18, 11, 1, 13, 242, 3, 69, 56, 4> prop_ce_data{
        {},
        {1, 0, 2, 3, 4, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 9, 10},
        {0x0000000000000000, 0x00000000ff000000, 0x00000000b0000000, 0x0048000000000000,
         0x000000004e000000, 0x0000000030000000, 0x0140020010842008, 0x0200108420080000,
         0x0000000010000000, 0x5f7ffc00a0000000, 0x0000000000007fdb},
        {1},
        {1, 2, 3},
        {0x0000000000000000, 0x0000001fc0000000, 0xf800000000000000, 0x0000000000000001}};
    static constexpr bool_trie<32, 969, 5, 18, 21, 19, 13, 224, 100, 69, 23, 6> prop_comp_ex_data{
        {0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x401000000000001b, 0x0000000000000080, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000},
        {1,  0,  2,  3,  4,  0,  0, 0, 5, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  6,  7,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 8, 9, 10, 11, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0,  0,  13,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  14, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0, 0, 15, 15, 15,
         15, 16, 17, 15, 18, 19, 20},
        {0x0000000000000000, 0x00000000ff000000, 0x00000000b0000000, 0x0048000000000000,
         0x000000004e000000, 0x0000000030000000, 0x0168020010842008, 0x0200108420080002,
         0x2aaa000000000000, 0x4800000000000000, 0x2a00c80808080a00, 0x0000000000000003,
         0x00000c4000000000, 0x0000060000000000, 0x0000000010000000, 0xffffffffffffffff,
         0xfffffc657fe53fff, 0xffff3fffffffffff, 0x0000000003ffffff, 0x5f7ffc00a0000000,
         0x0000000000007fdb},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
        {1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 5},
        {0x0000000000000000, 0x0000001fc0000000, 0xf800000000000000, 0x0000000000000001,
         0xffffffffffffffff, 0x000000003fffffff}};
    static constexpr flat_array<30> prop_dash_data{
        {0x1400, 0x1806, 0x058A, 0x208B, 0xFF0D, 0x2010, 0x2011, 0x2012,  0x2013, 0x2014,
         0x2015, 0x2212, 0x2E17, 0x2E1A, 0x301C, 0x30A0, 0x002D, 0x10EAD, 0x3030, 0xFE31,
         0xFE32, 0x2E3A, 0x2E3B, 0x05BE, 0x2E40, 0x2053, 0xFE58, 0x2E5D,  0xFE63, 0x207B}};
    static constexpr flat_array<15> prop_dep_data{{0xE0001, 0x17A3, 0x17A4, 0x0149, 0x206A, 0x206B,
                                                   0x206C, 0x206D, 0x206E, 0x206F, 0x2329, 0x232A,
                                                   0x0673, 0x0F77, 0x0F79}};
    static constexpr bool_trie<32, 991, 1, 0, 64, 255, 1, 0, 475, 11, 26, 42> prop_dia_data{
        {0x0000000000000000, 0x0000000140000000, 0x0190810000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0xffff000000000000, 0xffffffffffffffff,
         0xffffffffffffffff, 0x04300007e0ff7fff, 0x0000000000000030, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x00000000000000f8, 0x0000000000000000,
         0x0000000000000000, 0x0000000002000000, 0xbffffffbfffe0000, 0x0000000000000016,
         0x0000000000000000, 0x000000000187f800, 0x0000000000000000, 0x00001c6180000000,
         0xffff000000000000, 0x00000000000007ff, 0x0001ffc000000000, 0x003ff80000000000},
        {1,  2,  3,  4,  5,  4,  6,  4,  6,  4,  7,  4,  8,  1,  6,  4,  6,  4,  6, 9, 6, 1,  10,
         1,  11, 12, 13, 14, 1,  15, 16, 17, 18, 19, 1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 20, 1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  21, 1,  1,  22, 1,  1, 1, 1, 23, 1,
         1,  1,  1,  24, 25, 26, 27, 28, 29, 1,  30, 31, 1,  32, 33, 34, 1,  35, 1, 1, 1, 1,  1,
         1,  36, 37, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  38, 1,  1,  1,  1,  39, 1,  1,  1,  1,  1, 1, 1, 40, 1,
         41, 4,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  42, 43, 44, 45, 1,  46, 47, 1, 1, 1, 48, 49,
         50, 51, 52, 1,  53, 54, 55, 1,  56, 1,  57, 1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1,  1,
         1,  1,  1,  1,  1,  58, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  59, 1, 1, 1, 60, 61,
         62, 63},
        {0x0000000003000000, 0x0000000000000000, 0x00000000ff000000, 0x7ffffff80007fe00,
         0x1000000000000000, 0x00020000001e2000, 0x0000000000002000, 0xe000000000002000,
         0x0000000000202000, 0x1800000000000000, 0x0000000000000400, 0x0000000000005f80,
         0x0400000000000000, 0x0000000000001f00, 0xc2a0000003000000, 0x00000000000000dc,
         0x0000000000000040, 0x0680000000000000, 0x00003e1800000000, 0x000000000c00bf80,
         0x00000000e0000000, 0x0000000000300000, 0x00000000200ffe00, 0x0e00000000000000,
         0x9fe0000000000000, 0x7fff000000000000, 0x0000000000000ffe, 0x0010000000000000,
         0x000ff80000000010, 0x00000c0000000000, 0x00c0000000000000, 0x3f00000000000000,
         0x039021ffffff0000, 0xfffff00000000000, 0x000007ffffffffff, 0xffe000000000fff0,
         0xa000000000000000, 0x6000e000e000e003, 0x0003800000000000, 0x0000800000000000,
         0x0000fc0000000000, 0x000000001e000000, 0xb000800000000000, 0x0000000030000000,
         0x0003000000000000, 0x00000003ffffffff, 0x0000000000000700, 0x0300000000000000,
         0x0003ffff00000010, 0x0000780000000000, 0x0000000000080000, 0x0008000000000000,
         0x0000002000000001, 0x3800000000000000, 0x8000000000000000, 0x0040000000000007,
         0x00000e00f8000000, 0x0000300000000000, 0x0000000040000000, 0x0000ffff00000000,
         0x4000000000000000, 0x0001000000000001, 0x00000000c0000000, 0x0000000800000000},
        {1, 2, 2, 2, 2, 3, 2, 2, 2, 4, 2, 5, 6, 7, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
        {1,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  2,  0,  0,  0,  0,
         0,  0, 0, 0,  0,  0,  0, 0,  3,  0,  0,  0,  0,  0,  0, 0,  0,  4,  0,  0,  0,  0,  0,  0,
         0,  0, 5, 6,  0,  0,  7, 8,  0,  9,  10, 0,  11, 12, 0, 0,  13, 14, 15, 0,  0,  0,  16, 0,
         17, 0, 0, 18, 19, 18, 0, 20, 0,  21, 0,  0,  0,  8,  0, 0,  0,  22, 23, 0,  1,  24, 25, 26,
         0,  0, 0, 0,  0,  18, 0, 0,  0,  0,  27, 28, 0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0, 0, 0,  0,  0,  0, 0,  29, 30, 0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0, 0, 31, 32, 0,  0, 0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  33, 0,  0,  0,
         0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0, 0, 0,  0,  0,  0, 0,  0,  34, 35, 0,  0,  0,  0, 0,  0,  0,  36, 37, 0,  0,  0,  0,
         0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0, 0, 0,  0,  0,  0, 0,  0,  30, 0,  0,  0,  0,  0, 38, 39, 0,  0,  0,  0,  0,  0,  0,
         0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,  0,  0, 0,  40, 0,  41},
        {0x0000000000000000, 0x0000000100000000, 0x07fdffffffffffbf, 0x0000006000000000,
         0x000000fc00000000, 0x000000000001ffc0, 0x000000000000003c, 0x0001000000000040,
         0x0600000000000000, 0x0018000000000000, 0x0008000000000000, 0x0000000000001c01,
         0x0060000000000000, 0x0000060000000000, 0x1000000000000000, 0x001f1fc000002000,
         0x0000000000000044, 0x000000000000000c, 0x8000000000000000, 0x0000000000000001,
         0x00c0000000000000, 0x0000080000000000, 0x6000000000000000, 0x0000000000000008,
         0x0010000000000000, 0x0000000000000080, 0x0000000002000000, 0x0000000000000034,
         0x0000000000800000, 0x001f000000000000, 0x007f000000000000, 0x00000000ffff8000,
         0x0003000000000000, 0x6fef000000000000, 0xffff3fffffffffff, 0x000000000000007f,
         0xf807e38000000000, 0x00003c0000000fe7, 0x0000400000000000, 0x0000f00000000000,
         0x00000000007f0000, 0x0000000000000770}};
    static constexpr bool_trie<32, 75, 96, 821, 22, 1, 15, 240, 44, 64, 20, 25> prop_emoji_data{
        {0x03ff040800000000, 0x0000000000000000, 0x0000420000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000},
        {1,  2,  0,  0,  3,  0,  4, 0, 0, 0, 0, 0,  5, 0, 0,  6, 0, 0, 0, 7,  0,  0, 8, 9, 10,
         11, 12, 13, 14, 15, 16, 0, 0, 0, 0, 0, 17, 0, 0, 0,  0, 0, 0, 0, 18, 19, 0, 0, 0, 0,
         0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 20, 0, 0, 0, 0, 0,  0,  0, 0, 0, 21},
        {0x0000000000000000, 0x1000000000000000, 0x0000000000000200, 0x0200000400000000,
         0x0000060003f00000, 0x000001000c000000, 0x070ffe0000008000, 0x0000000000000004,
         0x00400c0000000000, 0x7800000000000001, 0x0700c44d2132401f, 0xc8000169800fff05,
         0x60030c831afc0000, 0x27bf0600001ac130, 0x001801022054bf24, 0x0000001800b85090,
         0x8001000200e00000, 0x0030000000000000, 0x00000000180000e0, 0x0000000000210000,
         0x2001000000000000, 0x0000000002800000},
        {1},
        {1,  0,  0, 2,  0, 3,  4, 5, 6, 7,  0, 0, 8, 9, 10, 11, 9, 9, 9, 12, 13, 14,
         15, 16, 9, 17, 9, 18, 0, 0, 0, 19, 0, 0, 0, 0, 20, 21, 9, 9, 0, 22, 23, 24},
        {0x0000000000000000, 0x0000000000000010, 0x0000000000008000, 0xc003000000000000,
         0x0000000007fe4000, 0xffffffc000000000, 0x07fc800004000006, 0x0000000000030000,
         0xfffffff3ffffffff, 0xffffffffffffffff, 0xffffffffcecfffff, 0xffb9ffffffffffff,
         0xbfffffffffffffff, 0x3fffffffffffffff, 0x07f980ffffff7e00, 0x1006013000613c80,
         0xfc08810a700e001c, 0x000000000000ffff, 0x1ff91a3fe0e7f83f, 0x00010fff00000000,
         0xf7fffffffffff000, 0xffffffffffffffbf, 0x1f1f000000000000, 0x07ff1fffffff007f,
         0x007f00ff03ff003f}};
    static constexpr range_array prop_emoji_component_data = {
        0x00000000, 0x00002301, 0x00002400, 0x00002A01, 0x00002B00, 0x00003001, 0x00003A00,
        0x00200D01, 0x00200E00, 0x0020E301, 0x0020E400, 0x00FE0F01, 0x00FE1000, 0x01F1E601,
        0x01F20000, 0x01F3FB01, 0x01F40000, 0x01F9B001, 0x01F9B400, 0x0E002001, 0x0E008000};
    static constexpr range_array prop_emoji_modifier_data = {0x00000000, 0x01F3FB01, 0x01F40000};
    static constexpr bool_trie<0, 5, 120, 867, 4, 1, 15, 240, 30, 78, 20, 15>
        prop_emoji_modifier_base_data{
            {},
            {1, 0, 0, 2, 3},
            {0x0000000000000000, 0x0000000020000000, 0x0200000000000000, 0x0000000000003c00},
            {1},
            {1, 2, 0, 3, 4, 0, 0, 5,  6,  0,  0,  7, 8, 9, 0,
             0, 0, 0, 0, 0, 0, 0, 10, 11, 12, 13, 0, 0, 0, 14},
            {0x0000000000000000, 0x0000000000000020, 0x0000000000001c9c, 0x11ffffc00001ffcc,
             0x00000400000280ee, 0x0430000000000000, 0x0000000000610000, 0x000000000000f8e0,
             0x0070000800000000, 0x0000000000001001, 0x73ff0040ff009000, 0x0080000000000000,
             0x0b60000000000000, 0x000000003ffee000, 0x007f000000000038}};
    static constexpr bool_trie<0, 34, 108, 850, 13, 1, 15, 240, 44, 64, 20, 27>
        prop_emoji_presentation_data{
            {},
            {1, 0,  0, 2, 0, 0, 0, 0, 0, 0, 0, 3, 4, 5, 6, 7,  8,
             9, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 12},
            {0x0000000000000000, 0x000000000c000000, 0x00091e0000000000, 0x6000000000000000,
             0x0000000000300000, 0x80000000000fff00, 0x60000c0200080000, 0x242c040000104030,
             0x0000010000000c20, 0x0000000000b85000, 0x8001000000e00000, 0x0000000018000000,
             0x0000000000210000},
            {1},
            {1,  0,  0,  2,  0,  0,  3, 4, 5, 6,  0, 0, 7, 8, 9,  10, 11, 12, 13, 14, 15, 16,
             17, 18, 13, 19, 13, 20, 0, 0, 0, 21, 0, 0, 0, 0, 22, 23, 13, 13, 0,  24, 25, 26},
            {0x0000000000000000, 0x0000000000000010, 0x0000000000008000, 0x0000000007fe4000,
             0xffffffc000000000, 0x077c800004000002, 0x0000000000030000, 0xffbfe001ffffffff,
             0xdfffffffffffffff, 0xffffffff000fffff, 0xff11ffff000f87ff, 0x7fffffffffffffff,
             0xfffffffffffffffd, 0xffffffffffffffff, 0x9fffffffffffffff, 0x3fffffffffffffff,
             0x040000ffffff7800, 0x0000001000600000, 0xf800000000000000, 0x000000000000ffff,
             0x1ff01800e0e7103f, 0x00010fff00000000, 0xf7fffffffffff000, 0xffffffffffffffbf,
             0x1f1f000000000000, 0x07ff1fffffff007f, 0x007f00ff03ff003f}};
    static constexpr flat_array<50> prop_ext_data{
        {0x10781, 0x10782, 0x3005,  0x180A,  0xA60C, 0xA015, 0x11A98, 0x309D,  0x309E,  0x1AA7,
         0x3031,  0x3032,  0x3033,  0x3034,  0x3035, 0x1C36, 0x00B7,  0x1E13C, 0x1E13D, 0x0640,
         0x16B42, 0x1843,  0x16B43, 0x1E944, 0x0E46, 0x0EC6, 0x115C6, 0x115C7, 0x115C8, 0x1E945,
         0x1E946, 0xA9CF,  0x02D0,  0x02D1,  0x0B55, 0xAADD, 0x1135D, 0x16FE0, 0x16FE1, 0x16FE3,
         0xA9E6,  0xAA70,  0xFF70,  0xAAF3,  0xAAF4, 0x07FA, 0x1C7B,  0x30FC,  0x30FD,  0x30FE}};
    static constexpr bool_trie<32, 75, 96, 821, 22, 1, 15, 240, 44, 64, 20, 22>
        prop_extended_pictographic_data{
            {0x0000000000000000, 0x0000000000000000, 0x0000420000000000, 0x0000000000000000,
             0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
             0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
             0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
             0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
             0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
             0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
             0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000},
            {1,  2,  0,  0,  3,  0,  4, 0, 0, 0, 0, 0,  5, 0, 6,  7, 0, 0, 0, 8,  0,  0, 9, 10, 11,
             12, 13, 12, 14, 15, 16, 0, 0, 0, 0, 0, 17, 0, 0, 0,  0, 0, 0, 0, 18, 19, 0, 0, 0,  0,
             0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 20, 0, 0, 0, 0, 0,  0,  0, 0, 0,  21},
            {0x0000000000000000, 0x1000000000000000, 0x0000000000000200, 0x0200000400000000,
             0x0000060003f00000, 0x000001000c000000, 0x0000000000000100, 0x070ffe0000008000,
             0x0000000000000004, 0x00400c0000000000, 0x7800000000000001, 0xfffffffffff7ffbf,
             0xffffffffffffffff, 0xffffffffffff003f, 0x001801022057ff3f, 0x000000f800b85090,
             0x8001000200e00000, 0x0030000000000000, 0x00000000180000e0, 0x0000000000210000,
             0x2001000000000000, 0x0000000002800000},
            {1},
            {1, 2, 3, 4,  5, 6,  7, 0, 8, 9,  0, 0, 2,  2, 2,  10, 2, 2, 2, 2,  11, 12,
             2, 2, 2, 13, 2, 14, 0, 0, 0, 15, 0, 0, 16, 0, 17, 18, 2, 2, 2, 19, 20, 21},
            {0x0000000000000000, 0xffff0fffffffffff, 0xffffffffffffffff, 0xfffe7fff000fffff,
             0x003ffffffffefffe, 0x000080000000e000, 0xc003f00000000000, 0x0000200007fe4000,
             0x07fc800004000006, 0x0000003f00030000, 0x07ffffffffffffff, 0x3fffffffffffffff,
             0xffffffffffffffc0, 0x000000000000ffff, 0x1fff1fffe0ffffff, 0x00010fff01e00000,
             0x0003000000000000, 0xf7fffffffffff000, 0xffffffffffffffbf, 0x1f1f3fff000fffff,
             0x07ff1fffffff007f, 0x007f00ff03ff003f}};
    static constexpr bool_trie<32, 991, 1, 0, 131, 255, 1, 0, 1343, 1, 0, 168> prop_gr_base_data{
        {0xffffffff00000000, 0x7fffffffffffffff, 0xffffdfff00000000, 0xffffffffffffffff,
         0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
         0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
         0x0000000000000000, 0xfcff000000000000, 0xfffffffbffffd7f0, 0xffffffffffffffff,
         0xffffffffffffffff, 0xffffffffffffffff, 0xfffffffffffffc07, 0xffffffffffffffff,
         0xfffeffffffffffff, 0xfffffffffe7fffff, 0x400000000000e7ff, 0x001f87ffffff0049,
         0xffffffffe800ffc0, 0xfffeffff000007ff, 0xffffffffffffffff, 0xffffc260403fffff,
         0x0000fffffffd3fff, 0xffffffffffffe000, 0x0002003fffffffff, 0xc7f007ffffffffff},
        {1,   2,   3,   4,   5,   6,  7,   8,  9,   10,  11,  12,  13,  14,  15,  16,  17,  18,
         19,  20,  21,  22,  23,  24, 25,  26, 27,  28,  29,  30,  31,  32,  33,  34,  35,  36,
         36,  36,  36,  36,  37,  38, 39,  40, 41,  42,  43,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  44,  45,  46, 47,  48, 49,  50,  45,  51,  52,  53,  54,  55,  56,  57,
         58,  59,  60,  61,  62,  63, 64,  65, 66,  67,  68,  36,  36,  36,  60,  36,  36,  36,
         36,  69,  70,  71,  72,  73, 74,  75, 76,  36,  36,  77,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  78,  79,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  80,  81,  36,  36,
         36,  36,  82,  83,  84,  85, 86,  36, 87,  88,  89,  36,  36,  36,  90,  91,  92,  93,
         36,  94,  36,  95,  96,  97, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  98,  99,  36,  36,  36,
         36,  100, 101, 102, 103, 36, 36,  36, 104, 105, 106, 36,  107, 108, 109, 110, 111, 112,
         113, 114, 115, 116, 55,  36, 117, 36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  36,  36,  36,  36,  36, 36,  36, 36,  36,  36,  36,  36,  36,  36,  36,  36,  36,
         36,  96,  118, 60,  60,  60, 60,  60, 60,  60,  60,  60,  60,  60,  60,  60,  60,  60,
         60,  60,  60,  60,  60,  60, 60,  60, 60,  60,  60,  60,  60,  60,  60,  60,  60,  60,
         60,  60,  60,  60,  60,  60, 60,  60, 60,  60,  60,  60,  60,  60,  60,  60,  60,  60,
         60,  60,  60,  60,  60,  60, 60,  60, 60,  60,  60,  60,  60,  60,  60,  60,  60,  60,
         60,  60,  60,  60,  60,  60, 60,  60, 60,  60,  60,  60,  60,  60,  60,  60,  60,  60,
         60,  60,  60,  60,  60,  60, 60,  60, 60,  60,  60,  60,  60,  60,  60,  60,  60,  60,
         60,  60,  60,  60,  60,  60, 60,  60, 60,  60,  60,  60,  60,  60,  60,  60,  60,  60,
         60,  60,  60,  60,  60,  60, 60,  60, 60,  36,  36,  36,  36,  36,  119, 36,  120, 121,
         122, 36,  123, 36,  36,  36, 36,  36, 36,  124, 125, 126, 127, 36,  128, 92,  36,  129,
         130},
        {0x7fff0110043fffff, 0xffff07ff41ffffff, 0xffffffff00007fff, 0x00000000000003ff,
         0xebfffffffffffff8, 0xfffffff3ff01de01, 0xa3c5fdfffff99fed, 0x3fffffc3b0005981,
         0xc36dfdfffff987e8, 0x005cffc05e000001, 0xe3edfdfffffbbfe8, 0x0203ffc300011a01,
         0x23edfdfffff99fec, 0x00ffffc3b0001981, 0x83ffc718d63dc7e8, 0x07ffffc000011dc6,
         0x23fffdfffffddfee, 0xff80ffc32700001e, 0x63effdfffffddffd, 0x0006ffc360000d9b,
         0xa7fffffffffddffc, 0xffffffc3ff70ddc1, 0x2ffbfffffc7fffec, 0x001cffc07f03007f,
         0x800dfffffffffffe, 0x000000000fff807f, 0x200dffaffffff7d6, 0x00000000f3ff005f,
         0xfd5ffffffcffffff, 0x80001ffffffffeff, 0xc000000000001f20, 0x0000000007ffdfbf,
         0x99021fffffffffff, 0xffe1fffe3cffffff, 0xffffffffdfffdf9b, 0xffffffffffff20bf,
         0xffffffffffffffff, 0xffffffff3d7f3dff, 0x7f3dffffffff3dff, 0xffffffffff7fff3d,
         0xffffffffff3dffff, 0x1fffffff07ffffff, 0xffffffff03ffffff, 0x3f3fffffffffffff,
         0xffffffff1fffffff, 0x01ffffffffffffff, 0x0073ffff8023ffff, 0x0001dfff0003ffff,
         0xc04fffffffffffff, 0x03ff03ff1ff001bf, 0xffffffff03ff07ff, 0xffff05ffffffff9f,
         0x003fffffffffffff, 0x01fb0e787fffffff, 0x001f3ffffffffff1, 0xffff0fffffffffff,
         0xffffffffc7ff03ff, 0xffffffffc67fffff, 0x0007e01a00bfffff, 0x00003fff03ff03ff,
         0x0000000000000000, 0xe80ffffffffffff0, 0x7ff007ffffff1ffb, 0xffffc4c3fffffffc,
         0xf00c5cbfffffffff, 0xf8300fffffffffff, 0xffffffffffffe3ff, 0xe7ffffffffff01ff,
         0x04efde02000800ff, 0xffffffff3f3fffff, 0x3fffffffaaff3f3f, 0xffdfffffffffffff,
         0x7fdcffffefcfffdf, 0xffff80ffffff07ff, 0xfff30000ffffffff, 0xffffffff1fff7fff,
         0x0000000000000001, 0xffffffffffff0fff, 0x0000007fffffffff, 0xffffffff000007ff,
         0xffcfffffffffffff, 0xffffffffffbfffff, 0xfe0c7fffffffffff, 0xffff20bfffffffff,
         0x000180ffffffffff, 0x7f7f7f7f007fffff, 0x000000007f7f7f7f, 0x000000003fffffff,
         0xfffffffffbffffff, 0x000fffffffffffff, 0x0fff0000003fffff, 0xffff03ffffffffff,
         0xfffffffffffffffe, 0xfffffffff87fffff, 0xfffeffffffffffe0, 0xffffffffffff7fff,
         0xffff000fffffffff, 0xffffffff7fffffff, 0xffffffffffff1fff, 0xffffffffffff007f,
         0x00000fffffffffff, 0xc0087fffffffffff, 0xffffffff3fffffff, 0x00fcffffffffffff,
         0xfffc000003eb07ff, 0x03ff0f9ffffff7bb, 0x00ffffffffffffff, 0x7ffc000003ffc00f,
         0xffffc03fffffffff, 0x1fffffff800c007f, 0xcc37fffffffffff8, 0x7fffffdfc3ffbfff,
         0x001981ffffffffff, 0xeffffffff3ff2ff7, 0x3e62ffffffffffff, 0x003fcffff8000005,
         0xffff7f7f007e7e7e, 0x03ff1edfffffffff, 0x0ffffffffffff87f, 0xffff3fffffffffff,
         0x0000000003ffffff, 0x5f7fffffa0f8007f, 0xffffffffffffffdb, 0xfffffffffff80007,
         0xfffffffffffcffff, 0xffff0000000080ff, 0xffff000003ff0000, 0xffdf0f7ffff7ffff,
         0x1fffffffffffffff, 0x7fffffff3fffffff, 0x30007f7f1cfcfcfc},
        {1,  2,  3, 4,  5,  6, 7,  8, 5, 9, 10, 11, 12, 13, 14, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 15,
         16, 17, 7, 18, 19, 7, 20, 5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5},
        {1,   2,   3,   4,   2,   5,   6,   7,   7,   8,   9,   10,  11,  12,  13,  2,   2,   14,
         15,  16,  17,  18,  7,   2,   2,   2,   2,   19,  20,  21,  7,   22,  23,  24,  25,  26,
         7,   27,  28,  29,  30,  31,  32,  33,  34,  35,  7,   2,   36,  37,  38,  39,  7,   7,
         7,   7,   40,  41,  7,   16,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,
         7,   54,  55,  56,  57,  7,   7,   58,  59,  60,  61,  7,   7,   62,  63,  64,  65,  66,
         67,  68,  69,  7,   7,   70,  7,   71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  7,
         7,   7,   7,   81,  82,  83,  7,   84,  85,  86,  7,   7,   7,   7,   87,  7,   7,   88,
         89,  2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   90,  7,   2,
         91,  2,   2,   2,   92,  7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   93,  37,  2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   94,  7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   2,   2,   2,   2,   2,   2,   2,   2,   2,   69,  7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   2,   2,   2,   2,   2,   2,   2,   2,   80,  95,  96,  97,  98,  99,  100, 7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   2,   101, 7,   2,   102, 103, 104, 2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   58,  2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   105, 36,  7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   106, 2,   2,   2,   2,   107, 108, 2,   2,   2,
         2,   2,   109, 7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   2,   110, 111, 7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   93,  2,   92,  2,   2,   2,   112, 113, 114, 115,
         116, 2,   117, 7,   118, 2,   119, 7,   7,   2,   120, 121, 122, 123, 124, 2,   2,   2,
         2,   125, 2,   2,   2,   2,   126, 2,   2,   2,   2,   2,   2,   2,   2,   127, 128, 129,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   130,
         7,   7,   7,   7,   7,   7,   7,   131, 132, 7,   7,   7,   7,   133, 134, 7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   135, 2,
         2,   2,   136, 2,   137, 7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   138, 139,
         7,   140, 7,   7,   7,   141, 142, 143, 144, 7,   7,   7,   7,   145, 2,   146, 147, 2,
         2,   148, 149, 150, 151, 7,   7,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   152, 2,   153, 2,   154, 155, 156, 157, 7,   2,   2,   2,   2,   2,
         158, 159, 160, 2,   2,   161, 162, 7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   31,  2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   80,  2,   2,   2,   163, 2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   164, 2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   165, 7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   2,   2,   2,   2,   2,   2,   2,   2,   166, 7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   167, 7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
         7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7},
        {0xb7ffff7fffffefff, 0x000000003fff3fff, 0xffffffffffffffff, 0x07ffffffffffffff,
         0xff8fffffffffff87, 0x000000011fff7fff, 0x1fffffffffff0000, 0x0000000000000000,
         0xffffffff1fffffff, 0x0ffffffe0001ffff, 0xffffe00fffffffff, 0x003fffffffff07ff,
         0xffffffffbfffffff, 0x00000000003fff0f, 0xffff03ff3fffffff, 0x0fffffffff0fffff,
         0xffff00ffffffffff, 0xf7ff800fffffffff, 0x1bfbfffbffb7f7ff, 0x007fffffffffffff,
         0x000000ff003fffff, 0x07fdffffffffffbf, 0x91bffffffffffd3f, 0xffffffffffbfffff,
         0x0000ff807fffffff, 0xf837ffff00000000, 0x83ffffff8fffffff, 0xf0ffffffffffffff,
         0xfffffffffffcffff, 0x003ffffffeef0001, 0xffffffff01ff01ff, 0x00000000ffffffff,
         0x007ff81fffffffff, 0xfe3fffffffffffff, 0xff07ffffff3fffff, 0x0000fe001e03ffff,
         0x00000000000001ff, 0x0007ffffffffffff, 0xfc07ffffffffffff, 0x03ff000fffffffff,
         0x7fffffff00000000, 0x000323ffffffffff, 0xffff000003fe003f, 0xffff0000000003c3,
         0x007fffff00000fff, 0x00fffffffffffffd, 0x0026fffffffc3f80, 0xd987fffffffffffc,
         0x03ff01ffffff0003, 0xffc0107ffffffff8, 0x0077ffffffff00ff, 0x803ffffffffffffc,
         0x001ffffeffff61ff, 0x3f2c7ffffffbffff, 0xffff03ffbfffbd7f, 0x03ff00077fffffff,
         0xa3edfdfffff99fec, 0x0000000fe001399e, 0x00ffffffffffffff, 0x00000003afffffa3,
         0x5a06ffffffffffff, 0x0000000003ff00f2, 0x4f037fffffffffff, 0x000000000ffffffe,
         0x5807ffffffffffff, 0x00001fff03ff001e, 0x0340d7ffffffffff, 0x00000000000003ff,
         0xffff004307ffffff, 0x000000000000007f, 0x09007fffffffffff, 0xffffffff00000000,
         0x8007ffffffffffff, 0xa1beffffff6ff27f, 0x0000000003ff0077, 0xfffffcff00000000,
         0x0000001ef00fffff, 0x8607fffffffff801, 0xfffffffff181007f, 0xffff0007fc8003ff,
         0x01ffffffffffffff, 0x4000fffffffffdff, 0xffff1fffffff003f, 0x001202000000ffff,
         0x0001fffffffffb7f, 0xfffffdbf03ff0040, 0x000003ff01587fff, 0x01e7ffff00000000,
         0x0001000000000000, 0x8003ffffffffffff, 0x0000000003ffffff, 0x001f7fffffffffff,
         0x000000000000000f, 0xffffffffffff0000, 0x00007fffffffffff, 0xffffc3ff7fffffff,
         0x7fffffffffffffff, 0x00203fffffff03ff, 0xff80ffffffffffff, 0xe0fffffbfbff003f,
         0x000000000000ffff, 0x0000000007ffffff, 0xffffffffffff07ff, 0x00000000fff800ff,
         0x0003000f00000000, 0x00000000003fffff, 0x6fef000000000000, 0x00000007ffffffff,
         0xffff00f000070000, 0x0fffffffffffffff, 0x1fff07ffffffffff, 0x0000000093ff01ff,
         0x003fffffffffffff, 0xfffffe7fffffffff, 0x00003c5fffffffff, 0xffffc3fffffff018,
         0x000007ffffffffff, 0x0000000000000023, 0x000fffff00000000, 0x01ffffff007fffff,
         0xffffffffffdfffff, 0xebffde64dfffffff, 0xffffffffffffffef, 0x7bffffffdfdfe7bf,
         0xfffffffffffdfc5f, 0xffffff3fffffffff, 0xffffffffffffcfff, 0x0780000000000000,
         0xffdfe00000000000, 0x0000000000000fef, 0x000000007fffffff, 0x3f801fffffffffff,
         0x000000000000c3ff, 0x00003fffffff0000, 0x83ff0fffffffffff, 0x7fff6f7f00000000,
         0x000000000000ff9f, 0x00000000c3ff080f, 0xfffe000000000000, 0x001fffffffffffff,
         0x3ffffffffffffffe, 0x0af7fe96ffffffef, 0x5ef7f796aa96ea84, 0x0ffffbee0ffffbff,
         0x0003000000000000, 0xffff0fffffffffff, 0xfffe7fff000fffff, 0x003ffffffffefffe,
         0x00003fffffffffff, 0xffffffc000000000, 0x0fffffffffff0007, 0x0000003f000301ff,
         0x1fff1fffe0ffffff, 0x000fffffffffffff, 0x00010fff01ffffff, 0xffffffffffff0fff,
         0xffffffff03ff00ff, 0x00033fffffff00ff, 0x1f1f3fff000fffff, 0x07ff1fffffff007f,
         0x007f00ff03ff003f, 0xfffffffffff7ffff, 0x03ff0000000007ff, 0xffffffff3fffffff,
         0xffff0003ffffffff, 0x00000001ffffffff, 0x000000003fffffff, 0x00000000000007ff}};
    static constexpr range_array prop_hex_data = {
        0x00000000, 0x00003001, 0x00003A00, 0x00004101, 0x00004700, 0x00006101, 0x00006700,
        0x00FF1001, 0x00FF1A00, 0x00FF2101, 0x00FF2700, 0x00FF4101, 0x00FF4700};
    static constexpr range_array prop_ideo_data = {
        0x00000000, 0x00300601, 0x00300800, 0x00302101, 0x00302A00, 0x00303801, 0x00303B00,
        0x00340001, 0x004DC000, 0x004E0001, 0x00A00000, 0x00F90001, 0x00FA6E00, 0x00FA7001,
        0x00FADA00, 0x016FE401, 0x016FE500, 0x01700001, 0x0187F800, 0x01880001, 0x018CD600,
        0x018D0001, 0x018D0900, 0x01B17001, 0x01B2FC00, 0x02000001, 0x02A6E000, 0x02A70001,
        0x02B73900, 0x02B74001, 0x02B81E00, 0x02B82001, 0x02CEA200, 0x02CEB001, 0x02EBE100,
        0x02F80001, 0x02FA1E00, 0x03000001, 0x03134B00};
    static constexpr range_array prop_idsb_data = {0x00000000, 0x002FF001, 0x002FF200, 0x002FF401,
                                                   0x002FFC00};
    static constexpr flat_array<2> prop_idst_data{{0x2FF2, 0x2FF3}};
    static constexpr flat_array<2> prop_join_c_data{{0x200C, 0x200D}};
    static constexpr range_array prop_loe_data = {0x00000000, 0x000E4001, 0x000E4500, 0x000EC001,
                                                  0x000EC500, 0x0019B501, 0x0019B800, 0x0019BA01,
                                                  0x0019BB00, 0x00AAB501, 0x00AAB700, 0x00AAB901,
                                                  0x00AABA00, 0x00AABB01, 0x00AABD00};
    static constexpr bool_trie<32, 991, 1, 0, 57, 255, 1, 0, 378, 13, 57, 48> prop_oalpha_data{
        {0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000020, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0xbfff000000000000, 0x00000000000000b6,
         0x0000000007ff0000, 0x00010000fefff800, 0x0000000000000000, 0x0000219e1fc00000,
         0xffff000000020000, 0x0000000000000000, 0x0001ffc000000000, 0x0000000000000000},
        {1,  1,  2,  3,  4,  5,  6,  5,  7,  5,  8,  5,  9,  10, 11, 12, 13, 5,  13, 12, 14, 15, 16,
         17, 18, 19, 18, 1,  20, 21, 1,  22, 23, 24, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  25, 25, 26, 27, 1,  1,  28, 1,  29, 1,
         1,  1,  30, 31, 32, 33, 34, 35, 36, 37, 38, 1,  1,  1,  1,  1,  1,  39, 1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  26, 40,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  41, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  42, 43, 1,  1,  1,  1,  1,  44, 1,  45, 46, 47,
         48, 49, 50, 51, 52, 53, 54, 1,  1,  1,  55, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1,  1,  1,  1,  56, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         1,  1},
        {0x00001eeff8c00000, 0x0000000000000000, 0xffff03f8fff00000, 0xcc0000000000000f,
         0x0000000c00e0dfff, 0xc00000000000000e, 0x0000000c0080199f, 0x0023000000021987,
         0x1c00000c00001bbf, 0x0000000c00c0199f, 0xc000000000000004, 0x0000000000801dc7,
         0xc00000000000000f, 0x0000000c00601ddf, 0x0000000c00801ddf, 0x000000000000000e,
         0x000c0000ff5f8000, 0x07f2000000000000, 0x0000000000002000, 0x1bf2000000000000,
         0xfffe000000000000, 0x1ffffffffeffe003, 0x797ff80000000000, 0x001e3f9dc3c00000,
         0x000000003c00bffc, 0x000c0000000c0000, 0xffc0000000000000, 0x00000000000001ff,
         0x0000020000000060, 0x01ff0fff00000000, 0x000000000f800000, 0x001ffffe7fe00000,
         0x8000000000000000, 0x0000000000007001, 0xffe000000000001f, 0x000000000000000f,
         0x000033fe00000007, 0x0003ff8000000000, 0x007ffff000000000, 0x001fff8000000000,
         0x000003ffffffffff, 0xffffffff00000000, 0x0ff0000000000000, 0x00000000c0000000,
         0x000000f800000804, 0xfff0000000000003, 0x800000000000002f, 0x000007c000000000,
         0x000000000007ff80, 0xfff000000000000f, 0x0000002000000000, 0x007ffe0000000000,
         0x3800000000003008, 0x419d000000000000, 0x0020f80000000000, 0x000007f800000000,
         0x0000000040000000},
        {1, 2, 2, 2, 2, 3, 2, 2, 2, 2, 4, 2, 2, 5, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
        {1,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  2,  0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0,  3,  0,  0, 0,  0,  0,  4,  0,  0,
         0,  0,  0,  5,  6,  7,  8, 9,  10, 11, 12, 13, 0, 0, 14, 15, 16, 0, 0,  17, 18, 19, 20, 0,
         0,  21, 22, 23, 24, 25, 0, 26, 0,  0,  0,  27, 0, 0, 0,  28, 29, 0, 30, 31, 32, 33, 0,  0,
         0,  0,  0,  34, 0,  35, 0, 36, 37, 38, 0,  0,  0, 0, 39, 0,  0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,
         40, 41, 42, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  43, 0, 0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0, 0,  44, 0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0,  45, 0,  0,  0,  0, 0, 0,  0,  0,  0, 0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0, 0,  0,  0,  0,  0,  0, 0, 0,  19, 46, 47},
        {0x0000000000000000, 0x07c0000000000000, 0x000000000000f06e, 0x000000f000000000,
         0x0000180000000000, 0xff00000000000007, 0x001800000000003f, 0x01ff000000000004,
         0x0000000000000004, 0x0007ff8000000007, 0x0000000000000060, 0xfff8000000000007,
         0x000000000000c000, 0x409ff00000000000, 0x000001ff80000000, 0xc00000000000000f,
         0x0000000c0080199f, 0xffe0000000000000, 0x000000000000003b, 0xffff000000000000,
         0x0000000000000003, 0x7f3f800000000000, 0x0000000030000000, 0x7fff000000000000,
         0x0000000000000001, 0x003ff80000000000, 0x000007ffe0000000, 0x01fff00000000000,
         0x19bf000000000000, 0x0000000000000005, 0x00000010fcfe0000, 0x7be00000000007fe,
         0x000000000ffe0000, 0x0000000000fffc00, 0x7f7f800000000000, 0x007ffefffffc0000,
         0xb47e000000000000, 0x000000000000008b, 0x00000000007b7c00, 0x0078000000000000,
         0xfffffffffffe8000, 0x00000000000780ff, 0x0003000000000000, 0x0000000040000000,
         0x000007dbf9ffff7f, 0x0000000000000080, 0xffff03ffffff03ff, 0x00000000000003ff}};
    static constexpr flat_array<7> prop_odi_data{
        {0x1160, 0xFFA0, 0x3164, 0x034F, 0x17B4, 0x17B5, 0x115F}};
    static constexpr bool_trie<0, 985, 6, 1, 9, 208, 1, 47, 118, 76, 62, 9> prop_ogr_ext_data{
        {},
        {1, 2, 0, 0, 0, 0, 1, 2, 1, 2, 0, 0, 0, 3, 1, 2, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8},
        {0x0000000000000000, 0x4000000000000000, 0x0000000000800000, 0x0000000000600004,
         0x0000000080008000, 0x0020000000000000, 0x0000000000001000, 0x0000c00000000000,
         0x00000000c0000000},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3},
        {1, 2, 0, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 8},
        {0x0000000000000000, 0x4000000000000000, 0x0000000000800000, 0x2001000000000000,
         0x0000800000000000, 0x0001000000000000, 0x0007c02000000000, 0xffffffff00000000,
         0xffffffffffffffff}};
    static constexpr range_array prop_oidc_data = {0x00000000, 0x0000B701, 0x0000B800,
                                                   0x00038701, 0x00038800, 0x00136901,
                                                   0x00137200, 0x0019DA01, 0x0019DB00};
    static constexpr flat_array<6> prop_oids_data{{0x1885, 0x1886, 0x212E, 0x2118, 0x309B, 0x309C}};
    static constexpr range_array prop_olower_data = {
        0x00000000, 0x0000AA01, 0x0000AB00, 0x0000BA01, 0x0000BB00, 0x0002B001, 0x0002B900,
        0x0002C001, 0x0002C200, 0x0002E001, 0x0002E500, 0x00034501, 0x00034600, 0x00037A01,
        0x00037B00, 0x001D2C01, 0x001D6B00, 0x001D7801, 0x001D7900, 0x001D9B01, 0x001DC000,
        0x00207101, 0x00207200, 0x00207F01, 0x00208000, 0x00209001, 0x00209D00, 0x00217001,
        0x00218000, 0x0024D001, 0x0024EA00, 0x002C7C01, 0x002C7E00, 0x00A69C01, 0x00A69E00,
        0x00A77001, 0x00A77100, 0x00A7F801, 0x00A7FA00, 0x00AB5C01, 0x00AB6000, 0x01078001,
        0x01078100, 0x01078301, 0x01078600, 0x01078701, 0x0107B100, 0x0107B201, 0x0107BB00};
    static constexpr bool_trie<32, 893, 96, 3, 21, 2, 13, 241, 107, 80, 5, 16> prop_omath_data{
        {0x0000000000000000, 0x0000000040000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0033000000270000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000},
        {1, 2, 3, 4, 5,  6, 7, 8, 0, 0, 0, 0,  9,  0, 10, 11, 0, 0, 0, 0, 0, 0, 12, 13, 14, 15, 0,
         0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 17, 18, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,
         0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,  0,  0, 0,  0,  0, 0, 0, 0, 0, 0, 0,  0,  0,  19, 0,
         0, 20},
        {0x0000000000000000, 0x001c000000400000, 0x6000001e00000001, 0x0000000000006000,
         0x0000f8621fff0000, 0xf1fbb3103e2ffc84, 0x00000000000003e0, 0xf0c33eb6f3e00000,
         0x000000302feb3fff, 0x0000000000000f00, 0x00b0000000000000, 0x0000000400010000,
         0xf07fc00300000000, 0x00001f94000f8cc1, 0x0000000000000060, 0x0000600f00000005,
         0x0000ffc000000060, 0x0000000001fffff8, 0x300000000f000000, 0x0000010a00000000,
         0x5000000000000000},
        {1, 2},
        {1, 2, 3, 4, 5, 6, 1, 1, 1, 1, 7, 8, 9, 10, 11, 12, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 13, 14, 15},
        {0x0000000000000000, 0xffffffffffffffff, 0xffffffffffdfffff, 0xebffde64dfffffff,
         0xffffffffffffffef, 0x7bffffffdfdfe7bf, 0xfffffffffffdfc5f, 0xffffff3fffffffff,
         0xf7fffffff7fffffd, 0xffdfffffffdfffff, 0xffff7fffffff7fff, 0xfffffdfffffffdff,
         0xffffffffffffcff7, 0x0af7fe96ffffffef, 0x5ef7f796aa96ea84, 0x0ffffbee0ffffbff}};
    static constexpr range_array prop_oupper_data = {0x00000000, 0x00216001, 0x00217000, 0x0024B601,
                                                     0x0024D000, 0x01F13001, 0x01F14A00, 0x01F15001,
                                                     0x01F16A00, 0x01F17001, 0x01F18A00};
    static constexpr bool_trie<32, 890, 96, 6, 15, 0, 0, 0, 0, 0, 0, 0> prop_pat_syn_data{
        {0xfc00fffe00000000, 0x7800000178000001, 0x88435afe00000000, 0x0080000000800000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000},
        {1, 2, 0, 0, 0, 0, 3,  4, 4, 4, 4, 4, 4, 4, 4,  4, 5, 6,  0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4,
         7, 8, 4, 4, 4, 4, 4,  4, 4, 4, 4, 4, 4, 4, 4,  4, 9, 10, 4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 11,
         0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 13, 0, 0, 0,  0, 14},
        {0x0000000000000000, 0x7fff00ffffff0000, 0x000000007feffffe, 0xffffffffffff0000,
         0xffffffffffffffff, 0x0000007fffffffff, 0x00000000000007ff, 0x003fffffffffffff,
         0xfffffffffff00000, 0xffcfffffffffffff, 0xffffffffffbfffff, 0x000000003fffffff,
         0x00010001ffffff0e, 0xc000000000000000, 0x0000000000000060},
        {},
        {},
        {}};
    static constexpr flat_array<11> prop_pat_ws_data{
        {0x0020, 0x0085, 0x2028, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x200E, 0x200F, 0x2029}};
    static constexpr flat_array<13> prop_pcm_data{{0x0600, 0x0601, 0x0602, 0x0603, 0x0604, 0x0605,
                                                   0x08E2, 0x110CD, 0x070F, 0x0890, 0x0891, 0x110BD,
                                                   0x06DD}};
    static constexpr range_array prop_qmark_data = {
        0x00000000, 0x00002201, 0x00002300, 0x00002701, 0x00002800, 0x0000AB01, 0x0000AC00,
        0x0000BB01, 0x0000BC00, 0x00201801, 0x00202000, 0x00203901, 0x00203B00, 0x002E4201,
        0x002E4300, 0x00300C01, 0x00301000, 0x00301D01, 0x00302000, 0x00FE4101, 0x00FE4500,
        0x00FF0201, 0x00FF0300, 0x00FF0701, 0x00FF0800, 0x00FF6201, 0x00FF6400};
    static constexpr range_array prop_radical_data = {
        0x00000000, 0x002E8001, 0x002E9A00, 0x002E9B01, 0x002EF400, 0x002F0001, 0x002FD600};
    static constexpr range_array prop_ri_data = {0x00000000, 0x01F1E601, 0x01F20000};
    static constexpr flat_array<47> prop_sd_data{
        {0x1D48A, 0x1D48B, 0x1D58E, 0x1D58F, 0x1D692, 0x1D693, 0x1D96,  0x1DF1A, 0x029D, 0x1D422,
         0x1D423, 0x1DA4,  0x1D526, 0x1D527, 0x1DA8,  0x1D62A, 0x1D62B, 0x1E2D,  0x012F, 0x02B2,
         0x1D4BE, 0x1D4BF, 0x1D5C2, 0x1D5C3, 0x2148,  0x2149,  0x0249,  0x1ECB,  0x0456, 0x1D456,
         0x0458,  0x1D457, 0x1D55A, 0x1D55B, 0x1D65E, 0x1D65F, 0x1D62,  0x0268,  0x0069, 0x006A,
         0x2071,  0x1D4F2, 0x03F3,  0x1D4F3, 0x1D5F6, 0x1D5F7, 0x2C7C}};
    static constexpr bool_trie<32, 991, 1, 0, 30, 255, 1, 0, 322, 41, 21, 25> prop_sterm_data{
        {0x8000400200000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000200, 0x0000000000000000,
         0x00000000e0000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000100000,
         0x0000000000000007, 0x0000000000000000, 0x0000000000000000, 0x0200000000000000},
        {1,  1,  1,  1, 2,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  3, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  4, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  5,  1, 1, 6,  1, 1, 1,  7,  1, 1,  1, 1,  8,  1, 1,  1,  1, 9,  1,
         1,  10, 1,  1, 11, 12, 1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 13, 14, 1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  15, 16, 1, 1,  1,  1,  1, 1, 17, 1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  18, 1,  1, 1,  1,  19, 1, 1, 20, 1, 1, 1,  1,  1, 21, 1, 19, 22, 1, 1,  23, 1, 24, 1,
         25, 1,  1,  1, 26, 1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 1,  1, 1, 1,  1,  1, 1,  1, 1,  1,  1, 1,  1,  1, 1,  1,
         1,  1,  1,  1, 1,  1,  1,  1, 1, 27, 1, 1, 28, 29, 1, 1},
        {0x6280000000000000, 0x0000000000000000, 0x0000003000000000, 0x0000000000000c00,
         0x0000018400000000, 0x0000400000000000, 0x0060000000000000, 0x0000000000000208,
         0x0000000000000030, 0x00000f0000000000, 0x60000000cc000000, 0x1800000000000000,
         0xc000000000000000, 0x3000000000000000, 0x0000000000000380, 0x1000400000000000,
         0x0000000000180000, 0x0000000000000004, 0x8000000000000000, 0x000000000000c000,
         0x0088000000000000, 0x00c0000000000000, 0x0000800000000000, 0x0000000000000300,
         0x00000000e0000000, 0x0003000000000000, 0x0000080000000000, 0x0000000000c40000,
         0x0000000080004002, 0x0000000200000000},
        {1, 2, 2, 2, 2, 3, 2, 2, 2, 2, 4, 2, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
        {1, 0,  0, 0, 0, 0, 0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0, 0,  0, 0,  2,  3,  0,  0,  4,
         5, 6,  0, 7, 0, 8, 9, 0,  10, 0,  0,  0, 0, 0, 0,  11, 0, 0,  0, 0,  0,  12, 0,  13, 0,
         0, 14, 0, 0, 0, 0, 0, 0,  0,  0,  15, 0, 0, 0, 16, 17, 0, 0,  0, 0,  0,  0,  13, 0,  0,
         0, 0,  0, 0, 0, 0, 0, 18, 0,  0,  0,  0, 0, 0, 0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0, 0,  0, 0, 0, 0, 0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0, 0,  0, 0, 0, 0, 0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0, 0,  0, 0, 0, 0, 0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0, 0,  0, 0, 0, 0, 0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0, 19, 0, 20, 18, 21, 0,  0,  0,
         0, 0,  0, 0, 0, 0, 0, 0,  0,  22, 0,  0, 0, 0, 0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0, 0,  0, 0, 0, 0, 0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0, 0,  0, 0, 0, 0, 0, 0,  0,  0,  0,  0, 0, 0, 0,  23, 0, 0,  0, 0,  0,  0,  0,  0,  0,
         0, 0,  0, 0, 0, 0, 0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0, 0,  0, 0,  0,  0,  0,  0,  0,
         0, 0,  0, 0, 0, 0, 0, 0,  0,  0,  0,  0, 0, 0, 0,  0,  0, 0,  0, 0,  0,  24},
        {0x0000000000000000, 0x0000000000c00000, 0x0000000003e00000, 0x00000000000003c0,
         0x0000000000000180, 0xc000000000000000, 0x0000000000000003, 0x000000000000000e,
         0x00000000c0002060, 0x1b00000000000000, 0x0000020000000000, 0x0000000000001800,
         0x0000000000fffe0c, 0x0000000000000006, 0x7000000000000000, 0x0000000000000050,
         0x000000000000000c, 0x0000000018000000, 0x0180000000000000, 0x0000c00000000000,
         0x0020000000000000, 0x0000000000000010, 0x0000000001000000, 0x0000000080000000,
         0x0000000000000100}};
    static constexpr bool_trie<32, 991, 1, 0, 34, 255, 1, 0, 413, 14, 21, 33> prop_term_data{
        {0x8c00500200000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x4000000000000000, 0x0000000000000080, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
         0x0000000000000000, 0x0000000000000000, 0x0000000000000200, 0x0000000000000008,
         0x00000000e8001000, 0x0000000000000000, 0x0000000000000000, 0x0000000000100000,
         0x00000000000017ff, 0x0000000000000000, 0x0000000000000000, 0x0300000000000000},
        {1,  2,  2,  2, 3,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  4,
         2,  2,  5,  2, 2,  2,  2,  6, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  7, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  8,  2, 9, 10, 2, 2, 11, 12, 2, 2,  2, 2,  13, 2, 2,  2,  2, 14, 2,
         2,  15, 2,  2, 16, 17, 2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 18, 19, 2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  20, 21, 2, 2,  2,  2,  2, 2, 22, 2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  17, 2,  2, 2,  2,  23, 2, 2, 24, 2, 2, 2,  2,  2, 25, 2, 26, 27, 2, 2,  19, 2, 28, 2,
         29, 2,  2,  2, 30, 2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 2,  2, 2, 2,  2,  2, 2,  2, 2,  2,  2, 2,  2,  2, 2,  2,
         2,  2,  2,  2, 2,  2,  2,  2, 2, 31, 2, 2, 32, 33, 2, 2},
        {0x7fff000000000000, 0x0000000040000000, 0x0000000000000000, 0x0000003000000000,
         0x000000000c000000, 0x000000000007e100, 0x0000000000000c00, 0x000001fe00000000,
         0x0000400000000000, 0x0000380000000000, 0x0060000000000000, 0x0000000004700000,
         0x000000000000033c, 0x0000000000000030, 0x00000f0000000000, 0x60000000ec000000,
         0xf800000000000000, 0xc000000000000000, 0x3000000000000000, 0x0000000000000380,
         0x1000400000000000, 0x000000000018d002, 0x0000000000000006, 0x000000000000e000,
         0x00f8000000000000, 0x00c0000000000000, 0x000000000000c000, 0x0000800000000000,
         0x00000000e0000000, 0x0003000080000000, 0x0000080000000000, 0x0000000000f70000,
         0x000000008c005002, 0x0000001200000000},
        {1, 2, 3, 3, 3, 4, 3, 3, 3, 3, 5, 3, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
         3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
         3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
         3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
         3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
         3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
         3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
         3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
         3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
        {1,  2,  0, 0,  0, 0,  0,  0, 0,  0,  0, 0, 0, 0,  0,  0,  0, 0, 0, 3, 0, 0,  1, 0,  0, 0,
         0,  4,  0, 5,  6, 0,  7,  0, 0,  0,  0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 8,  9, 0,  0, 10,
         11, 12, 0, 13, 0, 14, 15, 0, 16, 0,  0, 0, 0, 0,  0,  17, 0, 0, 0, 0, 0, 18, 0, 19, 0, 0,
         20, 0,  0, 0,  0, 0,  0,  0, 0,  21, 0, 0, 0, 22, 23, 0,  0, 0, 0, 0, 0, 24, 0, 0,  0, 0,
         0,  0,  0, 0,  0, 25, 0,  0, 0,  0,  0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0,  0, 0,  0, 0,
         0,  26, 0, 0,  0, 0,  0,  0, 0,  0,  0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0,  0, 0,  0, 0,
         0,  0,  0, 0,  0, 0,  0,  0, 0,  0,  0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0,  0, 0,  0, 0,
         0,  0,  0, 0,  0, 0,  0,  0, 0,  0,  0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0,  0, 0,  0, 0,
         0,  0,  0, 0,  0, 0,  0,  0, 0,  0,  0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0,  0, 0,  0, 0,
         0,  0,  0, 0,  0, 0,  0,  0, 0,  0,  0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0,  0, 0,  0, 0,
         0,  0,  0, 0,  0, 0,  0,  0, 0,  0,  0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0,  0, 27, 0, 28,
         29, 30, 0, 0,  0, 0,  0,  0, 0,  0,  0, 0, 0, 0,  31, 0,  0, 0, 0, 0, 0, 0,  0, 0,  0, 0,
         0,  0,  0, 0,  0, 0,  0,  0, 0,  0,  0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0,  0, 0,  0, 0,
         0,  0,  0, 0,  0, 0,  0,  0, 0,  0,  0, 0, 0, 0,  0,  0,  0, 0, 1, 0, 0, 0,  0, 0,  0, 0,
         0,  0,  0, 0,  0, 0,  0,  0, 0,  0,  0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0,  0, 0,  0, 0,
         0,  0,  0, 0,  0, 0,  0,  0, 0,  0,  0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0,  32},
        {0x0000000000000000, 0x0000000080000000, 0x0000000000010000, 0x0000000000800000,
         0x0000000000c00000, 0x003f000000000000, 0xfc00000000000000, 0x000000001e000000,
         0x0000000003e00000, 0x00000000000003c0, 0x0000000000003f80, 0xc000000000000000,
         0x0000000000000003, 0x000000000000000e, 0x00000000c0002060, 0x1f00000000000000,
         0x0000020000000000, 0x000000000c003800, 0x0000000000fffe3c, 0x0000000000000006,
         0x7000000000000000, 0x0000000000000050, 0x000000000000000c, 0x0000000618000000,
         0x000200000000000e, 0x0180000000000000, 0x001f000000000000, 0x0000c00000000000,
         0x0020000000000000, 0x0380000000000000, 0x0000000000000010, 0x0000000001800000,
         0x0000000000000780}};
    static constexpr range_array prop_uideo_data = {
        0x00000000, 0x00340001, 0x004DC000, 0x004E0001, 0x00A00000, 0x00FA0E01, 0x00FA1000,
        0x00FA1101, 0x00FA1200, 0x00FA1301, 0x00FA1500, 0x00FA1F01, 0x00FA2000, 0x00FA2101,
        0x00FA2200, 0x00FA2301, 0x00FA2500, 0x00FA2701, 0x00FA2A00, 0x02000001, 0x02A6E000,
        0x02A70001, 0x02B73900, 0x02B74001, 0x02B81E00, 0x02B82001, 0x02CEA200, 0x02CEB001,
        0x02EBE100, 0x03000001, 0x03134B00};
    static constexpr range_array prop_vs_data = {0x00000000, 0x00180B01, 0x00180E00,
                                                 0x00180F01, 0x00181000, 0x00FE0001,
                                                 0x00FE1000, 0x0E010001, 0x0E01F000};
    static constexpr range_array prop_wspace_data = {
        0x00000000, 0x00000901, 0x00000E00, 0x00002001, 0x00002100, 0x00008501, 0x00008600,
        0x0000A001, 0x0000A100, 0x00168001, 0x00168100, 0x00200001, 0x00200B00, 0x00202801,
        0x00202A00, 0x00202F01, 0x00203000, 0x00205F01, 0x00206000, 0x00300001, 0x00300100};
    static constexpr bool_trie<32, 991, 1, 0, 127, 255, 1, 0, 1407, 1, 0, 134> prop_xidc_data{
        {0x03ff000000000000, 0x07fffffe87fffffe, 0x04a0040000000000, 0xff7fffffff7fffff,
         0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
         0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0x0000501f0003ffc3,
         0xffffffffffffffff, 0xb8dfffffffffffff, 0xfffffffbffffd7c0, 0xffbfffffffffffff,
         0xffffffffffffffff, 0xffffffffffffffff, 0xfffffffffffffcfb, 0xffffffffffffffff,
         0xfffeffffffffffff, 0xffffffff027fffff, 0xbffffffffffe01ff, 0x000787ffffff00b6,
         0xffffffff07ff0000, 0xffffc3ffffffffff, 0xffffffffffffffff, 0x9ffffdff9fefffff,
         0xffffffffffff0000, 0xffffffffffffe7ff, 0x0003ffffffffffff, 0x243fffffffffffff},
        {1,   2,   3,   4,   5,   6,  7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  17,  18,
         19,  20,  21,  22,  23,  24, 25,  26,  27,  28,  29,  30,  31,  4,   32,  33,  34,  4,
         4,   4,   4,   4,   35,  36, 37,  38,  39,  40,  41,  42,  4,   4,   4,   4,   4,   4,
         4,   4,   43,  44,  45,  46, 47,  4,   48,  49,  50,  51,  52,  53,  54,  55,  56,  57,
         58,  59,  60,  4,   61,  4,  62,  63,  64,  65,  66,  4,   4,   4,   4,   4,   4,   4,
         4,   67,  68,  69,  70,  71, 72,  73,  74,  75,  76,  77,  78,  78,  78,  78,  78,  78,
         78,  78,  78,  78,  78,  78, 78,  78,  78,  78,  78,  78,  78,  78,  78,  78,  78,  78,
         78,  78,  78,  78,  78,  78, 78,  78,  78,  78,  78,  78,  78,  78,  78,  78,  78,  4,
         4,   4,   79,  80,  81,  82, 83,  78,  78,  78,  78,  78,  78,  78,  78,  84,  42,  85,
         86,  87,  4,   88,  89,  78, 78,  78,  78,  78,  78,  78,  78,  4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   78,  4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   90,  91,  4,   4,   4,
         4,   92,  93,  4,   94,  95, 4,   96,  97,  98,  62,  4,   99,  100, 101, 4,   102, 103,
         104, 4,   105, 106, 107, 4,  108, 4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
         4,   109, 110, 78,  78,  78, 78,  78,  78,  78,  78,  78,  78,  78,  78,  78,  78,  78,
         78,  78,  78,  78,  78,  78, 78,  78,  78,  78,  78,  78,  78,  78,  78,  78,  78,  78,
         78,  78,  78,  78,  78,  78, 78,  78,  78,  78,  78,  78,  78,  78,  78,  78,  78,  78,
         78,  78,  78,  78,  78,  78, 78,  78,  78,  78,  78,  78,  78,  78,  78,  78,  78,  78,
         78,  78,  78,  78,  78,  78, 78,  78,  78,  78,  78,  78,  78,  78,  78,  78,  78,  78,
         78,  78,  78,  78,  78,  78, 78,  78,  78,  78,  78,  78,  78,  78,  78,  78,  78,  78,
         78,  78,  78,  78,  78,  78, 78,  78,  78,  78,  78,  78,  78,  78,  78,  78,  78,  78,
         78,  78,  78,  78,  78,  78, 78,  78,  78,  4,   4,   4,   4,   4,   100, 4,   111, 112,
         113, 94,  114, 4,   115, 4,  4,   116, 117, 118, 119, 120, 121, 4,   122, 123, 124, 125,
         126},
        {0x00003fffffffffff, 0xffff07ff0fffffff, 0xffffffffff007eff, 0xfffffffbffffffff,
         0xffffffffffffffff, 0xfffeffcfffffffff, 0xf3c5fdfffff99fef, 0x5003ffcfb080799f,
         0xd36dfdfffff987ee, 0x003fffc05e023987, 0xf3edfdfffffbbfee, 0xfe00ffcf00013bbf,
         0xf3edfdfffff99fee, 0x0002ffcfb0e0399f, 0xc3ffc718d63dc7ec, 0x0000ffc000813dc7,
         0xf3fffdfffffddfff, 0x0000ffcf27603ddf, 0xf3effdfffffddfef, 0x0006ffcf60603ddf,
         0xfffffffffffddfff, 0xfc00ffcf80f07ddf, 0x2ffbfffffc7fffee, 0x000cffc0ff5f847f,
         0x07fffffffffffffe, 0x0000000003ff7fff, 0x3fffffaffffff7d6, 0x00000000f3ff3f5f,
         0xc2a003ff03000001, 0xfffe1ffffffffeff, 0x1ffffffffeffffdf, 0x0000000000000040,
         0xffffffffffff03ff, 0xffffffff3fffffff, 0xf7ffffffffff20bf, 0xffffffff3d7f3dff,
         0x7f3dffffffff3dff, 0xffffffffff7fff3d, 0xffffffffff3dffff, 0x0003fe00e7ffffff,
         0xffffffff0000ffff, 0x3f3fffffffffffff, 0xfffffffffffffffe, 0xffff9fffffffffff,
         0xffffffff07fffffe, 0x01ffc7ffffffffff, 0x001fffff803fffff, 0x000ddfff000fffff,
         0x000003ff308fffff, 0xffffffff03ffb800, 0x01ffffffffffffff, 0xffff07ffffffffff,
         0x003fffffffffffff, 0x0fff0fff7fffffff, 0x001f3fffffffffc0, 0xffff0fffffffffff,
         0x0000000007ff03ff, 0xffffffff0fffffff, 0x9fffffff7fffffff, 0xbfff008003ff03ff,
         0x0000000000007fff, 0x000ff80003ff1fff, 0x000fffffffffffff, 0x00ffffffffffffff,
         0x3fffffffffffe3ff, 0xe7ffffffffff01ff, 0x07fffffffff70000, 0xffffffff3f3fffff,
         0x3fffffffaaff3f3f, 0x5fdfffffffffffff, 0x1fdc1fff0fcf1fdc, 0x8000000000000000,
         0x8002000000100001, 0x000000001fff0000, 0x0001ffe21fff0000, 0xf3fffd503f2ffc84,
         0xffffffff000043e0, 0x00000000000001ff, 0x0000000000000000, 0x000ff81fffffffff,
         0xffff20bfffffffff, 0x800080ffffffffff, 0x7f7f7f7f007fffff, 0xffffffff7f7f7f7f,
         0x1f3efffe000000e0, 0xfffffffee67fffff, 0xf7ffffffffffffff, 0xfffeffffffffffe0,
         0xffffffff00007fff, 0xffff000000000000, 0x0000000000001fff, 0x3fffffffffff0000,
         0x00000fffffff1fff, 0xbff0ffffffffffff, 0x0003ffffffffffff, 0xfffffffcff800000,
         0xfffffffffffff9ff, 0xfffc000003eb07ff, 0x000010ffffffffff, 0xe8ffffff03ff003f,
         0xffff3fffffffffff, 0x1fffffff000fffff, 0x7fffffff03ff8001, 0x007fffffffffffff,
         0xfc7fffff03ff3fff, 0x007cffff38000007, 0xffff7f7f007e7e7e, 0xffff03fff7ffffff,
         0x03ff37ffffffffff, 0xffff000fffffffff, 0x0ffffffffffff87f, 0x0000000003ffffff,
         0x5f7ffdffe0f8007f, 0xffffffffffffffdb, 0xfffffffffff80000, 0xfffffff03fffffff,
         0x3fffffffffffffff, 0xffffffffffff0000, 0xfffffffffffcffff, 0x03ff0000000000ff,
         0x0018ffff0000ffff, 0xaa8a00000000e000, 0x1fffffffffffffff, 0x87fffffe03ff0000,
         0xffffffc007fffffe, 0x7fffffffffffffff, 0x000000001cfcfcfc},
        {1,  2,  3, 4,  5,  6, 7,  8, 5, 9, 10, 11, 12, 13, 14, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 15,
         16, 17, 7, 18, 19, 7, 20, 5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 21,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5,  5,  5,  5,  5, 5, 5, 5, 5, 5},
        {1,   2,   3,  4,  5,  4,   6,   4,   4,   7,   8,   9,   10, 11,  12,  2,   2,   13,  14,
         15,  16,  17, 4,  2,  2,   2,   2,   18,  19,  20,  4,   21, 22,  23,  24,  25,  4,   26,
         4,   27,  28, 29, 30, 31,  32,  33,  4,   2,   34,  35,  35, 36,  4,   4,   4,   4,   4,
         37,  4,   38, 39, 40, 41,  2,   42,  3,   43,  44,  45,  2,  46,  47,  4,   48,  49,  50,
         51,  4,   4,  2,  52, 2,   53,  4,   4,   54,  55,  2,   56, 57,  58,  59,  60,  4,   4,
         3,   4,   61, 62, 63, 64,  65,  66,  67,  68,  69,  57,  4,  4,   4,   4,   70,  71,  72,
         4,   73,  74, 75, 4,  4,   4,   4,   76,  4,   4,   77,  4,  2,   2,   2,   2,   2,   2,
         2,   2,   2,  2,  2,  2,   2,   2,   78,  4,   2,   79,  2,  2,   2,   80,  4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   81,
         82,  2,   2,  2,  2,  2,   2,   2,   2,   2,   2,   2,   2,  2,   2,   2,   2,   79,  4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  2,   2,   2,   2,   2,   2,   2,   2,  2,   60,  4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   2,   2,   2,
         2,   2,   2,  2,  2,  57,  83,  67,  84,  18,  85,  86,  4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  2,  4,  4,   2,   87,  88,  89,  2,   2,   2,  2,   2,   2,   2,   2,   2,
         2,   2,   2,  2,  2,  2,   2,   2,   2,   2,   2,   2,   2,  2,   2,   2,   2,   2,   2,
         2,   2,   2,  2,  2,  2,   2,   2,   2,   2,   2,   2,   2,  2,   2,   2,   2,   2,   2,
         2,   2,   2,  2,  2,  2,   2,   2,   2,   2,   2,   2,   2,  2,   2,   2,   2,   2,   2,
         2,   2,   2,  2,  2,  2,   2,   2,   2,   2,   2,   2,   2,  2,   2,   2,   2,   2,   2,
         2,   2,   2,  2,  2,  2,   2,   2,   2,   2,   90,  2,   2,  2,   2,   2,   2,   2,   2,
         2,   2,   2,  2,  2,  2,   2,   2,   2,   2,   2,   91,  34, 4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   92,  2,  2,   2,   2,   93,  94,  2,
         2,   2,   2,  2,  95, 4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  2,  96, 97,  4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  98, 60, 4,   4,   4,   4,   4,   4,   4,   99, 100, 4,   4,   101, 4,   4,
         4,   4,   4,  4,  2,  102, 103, 104, 105, 106, 2,   2,   2,  2,   107, 108, 109, 110, 111,
         112, 4,   4,  4,  4,  4,   4,   4,   4,   113, 114, 115, 4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   23,  4,   4,  4,   116, 4,   4,   4,   117,
         118, 4,   4,  4,  4,  119, 120, 4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   121, 2,   2,   2,   122, 2,  123, 4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  124, 125, 126, 4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   127, 4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   2,   2,   2,   2,  2,   2,   2,   2,   2,   2,
         2,   2,   2,  2,  2,  2,   2,   2,   2,   2,   2,   2,   2,  2,   2,   2,   2,   128, 2,
         2,   2,   2,  2,  2,  2,   2,   2,   2,   2,   2,   2,   2,  2,   2,   2,   2,   2,   2,
         2,   2,   2,  2,  2,  2,   2,   2,   2,   2,   2,   2,   2,  2,   2,   2,   2,   2,   2,
         2,   2,   2,  2,  2,  2,   2,   2,   2,   2,   2,   2,   2,  2,   2,   2,   2,   2,   2,
         2,   2,   2,  2,  2,  2,   57,  2,   2,   2,   11,  2,   2,  2,   2,   2,   2,   2,   2,
         2,   2,   2,  2,  2,  2,   2,   2,   2,   2,   2,   2,   2,  2,   2,   2,   2,   2,   2,
         2,   2,   2,  2,  2,  2,   2,   2,   2,   2,   2,   2,   2,  2,   2,   2,   2,   2,   2,
         2,   2,   2,  2,  2,  2,   2,   2,   2,   2,   2,   2,   2,  2,   2,   2,   2,   2,   2,
         2,   2,   2,  2,  2,  2,   2,   2,   2,   2,   2,   2,   2,  2,   2,   2,   2,   2,   2,
         2,   2,   2,  2,  2,  129, 2,   2,   2,   2,   2,   2,   2,  2,   2,   2,   2,   2,   2,
         2,   2,   2,  2,  2,  2,   2,   2,   2,   2,   2,   2,   2,  2,   2,   2,   2,   2,   2,
         2,   2,   2,  2,  2,  2,   2,   2,   2,   2,   2,   2,   2,  2,   2,   2,   2,   2,   2,
         2,   130, 4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   2,  2,   2,   2,   2,   2,   2,
         2,   131, 4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   2,   2,   2,   2,   2,   2,   2,  2,   2,   2,   2,   2,   2,
         132, 4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   2,   2,
         2,   133, 4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4,   4,   4,  4,  4,  4,   4,   4,   4,   4,   4,   4,   4,  4,   4,   4,   4,   4,   4,
         4},
        {0xb7ffff7fffffefff, 0x000000003fff3fff, 0xffffffffffffffff, 0x07ffffffffffffff,
         0x0000000000000000, 0x001fffffffffffff, 0x2000000000000000, 0xffffffff1fffffff,
         0x000000010001ffff, 0xffffe000ffffffff, 0x07ffffffffff07ff, 0xffffffff3fffffff,
         0x00000000003eff0f, 0xffff03ff3fffffff, 0x0fffffffff0fffff, 0xffff00ffffffffff,
         0xf7ff000fffffffff, 0x1bfbfffbffb7f7ff, 0x007fffffffffffff, 0x000000ff003fffff,
         0x07fdffffffffffbf, 0x91bffffffffffd3f, 0x007fffff003fffff, 0x000000007fffffff,
         0x0037ffff00000000, 0x03ffffff003fffff, 0xc0ffffffffffffff, 0x873ffffffeeff06f,
         0x1fffffff00000000, 0x000000001fffffff, 0x0000007ffffffeff, 0x003fffffffffffff,
         0x0007ffff003fffff, 0x000000000003ffff, 0x00000000000001ff, 0x0007ffffffffffff,
         0x03ff00ffffffffff, 0x00031bffffffffff, 0xffff00801fffffff, 0xffff00000001ffff,
         0xffff00000000003f, 0x007fffff0000001f, 0x803fffc00000007f, 0x03ff01ffffff0004,
         0xffdfffffffffffff, 0x004fffffffff00f0, 0x0000000017ffde1f, 0x40fffffffffbffff,
         0xffff01ffbfffbd7f, 0x03ff07ffffffffff, 0xfbedfdfffff99fef, 0x001f1fcfe081399f,
         0x00000003c3ff07ff, 0x0000000003ff00bf, 0xff3fffffffffffff, 0x000000003f000001,
         0x0000000003ff0011, 0x01ffffffffffffff, 0x00000000000003ff, 0x03ff0fffe7ffffff,
         0x000000000000007f, 0xffffffff00000000, 0x800003ffffffffff, 0xf9bfffffff6ff27f,
         0x0000000003ff000f, 0xfffffcff00000000, 0x0000001bfcffffff, 0x7fffffffffffffff,
         0xffffffffffff0080, 0xffff000023ffffff, 0xff7ffffffffffdff, 0xfffc000003ff0001,
         0x007ffefffffcffff, 0xb47ffffffffffb7f, 0xfffffdbf03ff00ff, 0x000003ff01fb7fff,
         0x007fffff00000000, 0x0001000000000000, 0x0000000003ffffff, 0x00007fffffffffff,
         0x000000000000000f, 0xffffffffffff0000, 0x0001ffffffffffff, 0xffff03ff7fffffff,
         0x001f3fffffff03ff, 0xe0fffff803ff000f, 0x000000000000ffff, 0xffffffffffff87ff,
         0x00000000ffff80ff, 0x0003001b00000000, 0x00ffffffffffffff, 0x00000000003fffff,
         0x6fef000000000000, 0x00000007ffffffff, 0xffff00f000070000, 0x0fffffffffffffff,
         0x1fff07ffffffffff, 0x0000000063ff01ff, 0xffff3fffffffffff, 0xf807e3e000000000,
         0x00003c0000000fe7, 0x000000000000001c, 0xffffffffffdfffff, 0xebffde64dfffffff,
         0xffffffffffffffef, 0x7bffffffdfdfe7bf, 0xfffffffffffdfc5f, 0xffffff3fffffffff,
         0xf7fffffff7fffffd, 0xffdfffffffdfffff, 0xffff7fffffff7fff, 0xfffffdfffffffdff,
         0xffffffffffffcff7, 0xf87fffffffffffff, 0x00201fffffffffff, 0x0000fffef8000010,
         0x000007dbf9ffff7f, 0x3fff1fffffffffff, 0x00000000000043ff, 0x00007fffffff0000,
         0x03ffffffffffffff, 0x7fff6f7f00000000, 0x00000000007f001f, 0x0000000003ff0fff,
         0x0af7fe96ffffffef, 0x5ef7f796aa96ea84, 0x0ffffbee0ffffbff, 0x03ff000000000000,
         0x00000000ffffffff, 0xffff0003ffffffff, 0x00000001ffffffff, 0x000000003fffffff,
         0x00000000000007ff, 0x0000ffffffffffff}};
    static constexpr bool_trie<32, 991, 1, 0, 131, 255, 1, 0, 1215, 1, 0, 124> prop_xids_data{
        {0x0000000000000000, 0x07fffffe07fffffe, 0x0420040000000000, 0xff7fffffff7fffff,
         0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff,
         0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0x0000501f0003ffc3,
         0x0000000000000000, 0xb8df000000000000, 0xfffffffbffffd740, 0xffbfffffffffffff,
         0xffffffffffffffff, 0xffffffffffffffff, 0xfffffffffffffc03, 0xffffffffffffffff,
         0xfffeffffffffffff, 0xffffffff027fffff, 0x00000000000001ff, 0x000787ffffff0000,
         0xffffffff00000000, 0xfffec000000007ff, 0xffffffffffffffff, 0x9c00c060002fffff,
         0x0000fffffffd0000, 0xffffffffffffe000, 0x0002003fffffffff, 0x043007fffffffc00},
        {1,   2,   3,   4,   5,   6,  7,   8,   9,   10,  11,  12, 13,  14,  15,  16,  17,  18,
         19,  20,  21,  22,  23,  24, 23,  25,  26,  27,  28,  29, 30,  31,  32,  33,  34,  35,
         35,  35,  35,  35,  36,  37, 38,  39,  40,  41,  42,  43, 35,  35,  35,  35,  35,  35,
         35,  35,  44,  45,  46,  47, 48,  49,  50,  51,  52,  53, 54,  55,  56,  57,  3,   58,
         59,  60,  30,  61,  62,  63, 64,  65,  66,  67,  68,  35, 35,  35,  30,  35,  35,  35,
         35,  69,  70,  71,  72,  30, 73,  74,  30,  75,  76,  77, 30,  30,  30,  30,  30,  30,
         30,  30,  30,  30,  30,  30, 30,  30,  30,  30,  30,  30, 30,  30,  30,  30,  30,  30,
         30,  30,  30,  30,  30,  30, 30,  30,  30,  30,  30,  30, 30,  30,  30,  30,  30,  35,
         35,  35,  78,  79,  80,  81, 82,  30,  30,  30,  30,  30, 30,  30,  30,  83,  43,  84,
         85,  86,  35,  87,  88,  30, 30,  30,  30,  30,  30,  30, 30,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  30,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  89,  90,  35,  35,  35,
         35,  91,  92,  93,  94,  95, 35,  96,  97,  98,  49,  99, 100, 101, 102, 103, 104, 105,
         106, 107, 108, 109, 110, 35, 111, 35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35, 35,  35,  35,  35,  35,  35,
         35,  112, 113, 30,  30,  30, 30,  30,  30,  30,  30,  30, 30,  30,  30,  30,  30,  30,
         30,  30,  30,  30,  30,  30, 30,  30,  30,  30,  30,  30, 30,  30,  30,  30,  30,  30,
         30,  30,  30,  30,  30,  30, 30,  30,  30,  30,  30,  30, 30,  30,  30,  30,  30,  30,
         30,  30,  30,  30,  30,  30, 30,  30,  30,  30,  30,  30, 30,  30,  30,  30,  30,  30,
         30,  30,  30,  30,  30,  30, 30,  30,  30,  30,  30,  30, 30,  30,  30,  30,  30,  30,
         30,  30,  30,  30,  30,  30, 30,  30,  30,  30,  30,  30, 30,  30,  30,  30,  30,  30,
         30,  30,  30,  30,  30,  30, 30,  30,  30,  30,  30,  30, 30,  30,  30,  30,  30,  30,
         30,  30,  30,  30,  30,  30, 30,  30,  30,  35,  35,  35, 35,  35,  114, 35,  115, 116,
         117, 118, 119, 35,  120, 35, 35,  121, 122, 123, 124, 30, 125, 35,  126, 127, 128, 129,
         130},
        {0x00000110043fffff, 0xffff07ff01ffffff, 0xffffffff00007eff, 0x00000000000003ff,
         0x23fffffffffffff0, 0xfffe0003ff010000, 0x23c5fdfffff99fe1, 0x10030003b0004000,
         0x036dfdfffff987e0, 0x001c00005e000000, 0x23edfdfffffbbfe0, 0x0200000300010000,
         0x23edfdfffff99fe0, 0x00020003b0000000, 0x03ffc718d63dc7e8, 0x0000000000010000,
         0x23fffdfffffddfe0, 0x0000000327000000, 0x23effdfffffddfe1, 0x0006000360000000,
         0x27fffffffffddff0, 0xfc00000380704000, 0x2ffbfffffc7fffe0, 0x000000000000007f,
         0x0005fffffffffffe, 0x2005ffaffffff7d6, 0x00000000f000005f, 0x0000000000000001,
         0x00001ffffffffeff, 0x0000000000001f00, 0x0000000000000000, 0x800007ffffffffff,
         0xffe1c0623c3f0000, 0xffffffff00004003, 0xf7ffffffffff20bf, 0xffffffffffffffff,
         0xffffffff3d7f3dff, 0x7f3dffffffff3dff, 0xffffffffff7fff3d, 0xffffffffff3dffff,
         0x0000000007ffffff, 0xffffffff0000ffff, 0x3f3fffffffffffff, 0xfffffffffffffffe,
         0xffff9fffffffffff, 0xffffffff07fffffe, 0x01ffc7ffffffffff, 0x0003ffff8003ffff,
         0x0001dfff0003ffff, 0x000fffffffffffff, 0x0000000010800000, 0xffffffff00000000,
         0x01ffffffffffffff, 0xffff05ffffffffff, 0x003fffffffffffff, 0x000000007fffffff,
         0x001f3fffffff0000, 0xffff0fffffffffff, 0xffffffff007fffff, 0x00000000001fffff,
         0x0000008000000000, 0x000fffffffffffe0, 0x0000000000001fe0, 0xfc00c001fffffff8,
         0x0000003fffffffff, 0x0000000fffffffff, 0x3ffffffffc00e000, 0xe7ffffffffff01ff,
         0x046fde0000000000, 0xffffffff3f3fffff, 0x3fffffffaaff3f3f, 0x5fdfffffffffffff,
         0x1fdc1fff0fcf1fdc, 0x8002000000000000, 0x000000001fff0000, 0xf3fffd503f2ffc84,
         0xffffffff000043e0, 0x00000000000001ff, 0x000c781fffffffff, 0xffff20bfffffffff,
         0x000080ffffffffff, 0x7f7f7f7f007fffff, 0x000000007f7f7f7f, 0x1f3e03fe000000e0,
         0xfffffffee07fffff, 0xf7ffffffffffffff, 0xfffeffffffffffe0, 0xffffffff00007fff,
         0xffff000000000000, 0x0000000000001fff, 0x3fffffffffff0000, 0x00000c00ffff1fff,
         0x80007fffffffffff, 0xffffffff3fffffff, 0x0000ffffffffffff, 0xfffffffcff800000,
         0xfffffffffffff9ff, 0xfffc000003eb07ff, 0x00000007fffff7bb, 0x000ffffffffffffc,
         0x68fc000000000000, 0xffff003ffffffc00, 0x1fffffff0000007f, 0x0007fffffffffff0,
         0x7c00ffdf00008000, 0x000001ffffffffff, 0xc47fffff00000ff7, 0x3e62ffffffffffff,
         0x001c07ff38000005, 0xffff7f7f007e7e7e, 0xffff03fff7ffffff, 0x00000007ffffffff,
         0xffff000fffffffff, 0x0ffffffffffff87f, 0xffff3fffffffffff, 0x0000000003ffffff,
         0x5f7ffdffa0f8007f, 0xffffffffffffffdb, 0x0003ffffffffffff, 0xfffffffffff80000,
         0xfffffff03fffffff, 0x3fffffffffffffff, 0xffffffffffff0000, 0xfffffffffffcffff,
         0x03ff0000000000ff, 0xaa8a000000000000, 0x1fffffffffffffff, 0x07fffffe00000000,
         0xffffffc007fffffe, 0x7fffffff3fffffff, 0x000000001cfcfcfc},
        {1,  2,  3, 4,  5,  6, 7,  8, 5, 9, 10, 5, 11, 12, 5, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 13,
         14, 15, 7, 16, 17, 7, 18, 5, 5, 5, 5,  5, 5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5, 5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5, 5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5, 5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5, 5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5, 5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5, 5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5, 5,  5,  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
         5,  5,  5, 5,  5,  5, 5,  5, 5, 5, 5,  5, 5,  5,  5, 5, 5, 5, 5, 5, 5},
        {1,   2,   3,   4,   5,   4,   4,   4,   4,   6,   7,   8,   9,   10, 11, 2,  2,  12,  13,
         14,  15,  16,  4,   2,   2,   2,   2,   17,  18,  19,  4,   20,  21, 22, 23, 24, 4,   25,
         4,   26,  27,  28,  29,  30,  31,  32,  4,   2,   33,  34,  34,  35, 4,  4,  4,  4,   4,
         36,  4,   37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48, 49, 4,  50, 22,  51,
         52,  4,   4,   5,   53,  54,  55,  4,   4,   56,  57,  54,  58,  59, 4,  60, 61, 4,   4,
         62,  4,   63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  4,   4,  4,  4,  73, 74,  75,
         4,   76,  77,  78,  4,   4,   4,   4,   79,  4,   4,   80,  4,   2,  2,  2,  2,  2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   81,  4,   2,   56,  2,   2,  2,  82, 4,  4,   4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   83,
         84,  2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,  56,  4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   4,   4,   2,   2,   2,   2,   2,   2,   2,   2,   2,  61, 4,  4,  4,   4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  2,  2,   2,
         2,   2,   2,   2,   2,   72,  85,  86,  87,  54,  88,  75,  4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   2,   4,   4,   2,   89,  90,  91,  2,   2,   2,   2,  2,  2,  2,  2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,  2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,  2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,  2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,  2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   92,  2,   2,   2,  2,  2,  2,  2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   93,  33,  4,  4,  4,  4,  4,   4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   94,  2,   2,  2,  2,  95, 96,  2,
         2,   2,   2,   2,   97,  4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   2,   98,  99,  4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  2,  100, 101,
         102, 103, 104, 2,   2,   2,   2,   105, 106, 107, 108, 109, 110, 4,  4,  4,  4,  4,   4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   22,  4,   4,   4,   4,   4,   4,   4,   111, 112, 4,  4,  4,  4,  87,  62,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         113, 2,   2,   2,   114, 2,   115, 4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   4,   4,   4,   116, 117, 118, 4,   4,   4,   4,   4,  2,  2,  2,  2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,  2,   2,
         2,   2,   2,   119, 2,   2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,  2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,  2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,  2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   72,  2,   2,  2,  10, 2,  2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,  2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,  2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,  2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,  2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   120, 2,   2,   2,  2,  2,  2,  2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,  2,   2,
         2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,  2,  2,  2,  2,   2,
         2,   2,   2,   2,   2,   2,   121, 4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  2,   2,
         2,   2,   2,   2,   2,   2,   122, 4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   2,   2,   2,  2,  2,  2,  2,   2,
         2,   2,   2,   2,   2,   123, 4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4,   4,
         4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,  4,  4,  4,  4},
        {0xb7ffff7fffffefff, 0x000000003fff3fff, 0xffffffffffffffff, 0x07ffffffffffffff,
         0x0000000000000000, 0x001fffffffffffff, 0xffffffff1fffffff, 0x000000000001ffff,
         0xffffe000ffffffff, 0x003fffffffff07ff, 0xffffffff3fffffff, 0x00000000003eff0f,
         0xffff00003fffffff, 0x0fffffffff0fffff, 0xffff00ffffffffff, 0xf7ff000fffffffff,
         0x1bfbfffbffb7f7ff, 0x007fffffffffffff, 0x000000ff003fffff, 0x07fdffffffffffbf,
         0x91bffffffffffd3f, 0x007fffff003fffff, 0x000000007fffffff, 0x0037ffff00000000,
         0x03ffffff003fffff, 0xc0ffffffffffffff, 0x003ffffffeef0001, 0x1fffffff00000000,
         0x000000001fffffff, 0x0000001ffffffeff, 0x003fffffffffffff, 0x0007ffff003fffff,
         0x000000000003ffff, 0x00000000000001ff, 0x0007ffffffffffff, 0x0000000fffffffff,
         0x000303ffffffffff, 0xffff00801fffffff, 0xffff00000000003f, 0xffff000000000003,
         0x007fffff0000001f, 0x00fffffffffffff8, 0x0026000000000000, 0x0000fffffffffff8,
         0x000001ffffff0000, 0x0000007ffffffff8, 0x0047ffffffff0090, 0x0007fffffffffff8,
         0x000000001400001e, 0x00000ffffffbffff, 0xffff01ffbfffbd7f, 0x23edfdfffff99fe0,
         0x00000003e0010000, 0x0000000380000780, 0x0000ffffffffffff, 0x00000000000000b0,
         0x00007fffffffffff, 0x000000000f000000, 0x0000000000000010, 0x010007ffffffffff,
         0x0000000007ffffff, 0x000000000000007f, 0x00000fffffffffff, 0xffffffff00000000,
         0x80000000ffffffff, 0x8000ffffff6ff27f, 0x0000000000000002, 0xfffffcff00000000,
         0x0000000a0001ffff, 0x0407fffffffff801, 0xfffffffff0010000, 0xffff0000200003ff,
         0x01ffffffffffffff, 0x00007ffffffffdff, 0xfffc000000000001, 0x000000000000ffff,
         0x0001fffffffffb7f, 0xfffffdbf00000040, 0x00000000010003ff, 0x0007ffff00000000,
         0x0001000000000000, 0x0000000003ffffff, 0x000000000000000f, 0xffffffffffff0000,
         0x0001ffffffffffff, 0xffff00007fffffff, 0x7fffffffffffffff, 0x00003fffffff0000,
         0xe0fffff80000000f, 0x00000000000107ff, 0x00000000fff80000, 0x0000000b00000000,
         0x00ffffffffffffff, 0x00000000003fffff, 0x6fef000000000000, 0x00000007ffffffff,
         0xffff00f000070000, 0x0fffffffffffffff, 0x1fff07ffffffffff, 0x0000000003ff01ff,
         0xffffffffffdfffff, 0xebffde64dfffffff, 0xffffffffffffffef, 0x7bffffffdfdfe7bf,
         0xfffffffffffdfc5f, 0xffffff3fffffffff, 0xf7fffffff7fffffd, 0xffdfffffffdfffff,
         0xffff7fffffff7fff, 0xfffffdfffffffdff, 0x0000000000000ff7, 0x3f801fffffffffff,
         0x0000000000004000, 0x7fff6f7f00000000, 0x000000000000001f, 0x000000000000080f,
         0x0af7fe96ffffffef, 0x5ef7f796aa96ea84, 0x0ffffbee0ffffbff, 0x00000000ffffffff,
         0xffff0003ffffffff, 0x00000001ffffffff, 0x000000003fffffff, 0x00000000000007ff}};
}    // namespace detail::tables
template<>
constexpr bool cp_property_is<property::ahex>(char32_t c) {
    return detail::tables::prop_ahex_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::alpha>(char32_t c) {
    return detail::tables::prop_alpha_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::bidi_c>(char32_t c) {
    return detail::tables::prop_bidi_c_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::bidi_m>(char32_t c) {
    return detail::tables::prop_bidi_m_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::dash>(char32_t c) {
    return detail::tables::prop_dash_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::dep>(char32_t c) {
    return detail::tables::prop_dep_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::dia>(char32_t c) {
    return detail::tables::prop_dia_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::emoji>(char32_t c) {
    return detail::tables::prop_emoji_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::emoji_component>(char32_t c) {
    return detail::tables::prop_emoji_component_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::emoji_modifier>(char32_t c) {
    return detail::tables::prop_emoji_modifier_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::emoji_modifier_base>(char32_t c) {
    return detail::tables::prop_emoji_modifier_base_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::emoji_presentation>(char32_t c) {
    return detail::tables::prop_emoji_presentation_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::ext>(char32_t c) {
    return detail::tables::prop_ext_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::extended_pictographic>(char32_t c) {
    return detail::tables::prop_extended_pictographic_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::gr_base>(char32_t c) {
    return detail::tables::prop_gr_base_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::hex>(char32_t c) {
    return detail::tables::prop_hex_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::ideo>(char32_t c) {
    return detail::tables::prop_ideo_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::idsb>(char32_t c) {
    return detail::tables::prop_idsb_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::idst>(char32_t c) {
    return detail::tables::prop_idst_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::join_c>(char32_t c) {
    return detail::tables::prop_join_c_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::loe>(char32_t c) {
    return detail::tables::prop_loe_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::pat_syn>(char32_t c) {
    return detail::tables::prop_pat_syn_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::pat_ws>(char32_t c) {
    return detail::tables::prop_pat_ws_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::pcm>(char32_t c) {
    return detail::tables::prop_pcm_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::qmark>(char32_t c) {
    return detail::tables::prop_qmark_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::radical>(char32_t c) {
    return detail::tables::prop_radical_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::ri>(char32_t c) {
    return detail::tables::prop_ri_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::sd>(char32_t c) {
    return detail::tables::prop_sd_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::sterm>(char32_t c) {
    return detail::tables::prop_sterm_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::term>(char32_t c) {
    return detail::tables::prop_term_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::uideo>(char32_t c) {
    return detail::tables::prop_uideo_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::vs>(char32_t c) {
    return detail::tables::prop_vs_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::wspace>(char32_t c) {
    return detail::tables::prop_wspace_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::xidc>(char32_t c) {
    return detail::tables::prop_xidc_data.lookup(c);
}
template<>
constexpr bool cp_property_is<property::xids>(char32_t c) {
    return detail::tables::prop_xids_data.lookup(c);
}
}    // namespace uni

#include <iterator>

namespace uni {

constexpr category cp_category(char32_t cp) {
    if(cp > 0x10FFFF)
        return category::unassigned;
    return detail::tables::get_category(cp);
}

constexpr uni::version detail::age_from_string(std::string_view a) {
    for(std::size_t i = 0; i < std::size(detail::tables::age_strings); ++i) {
        const auto res = detail::propnamecomp(a, detail::tables::age_strings[i]);
        if(res == 0)
            return uni::version(i);
    }
    return uni::version::unassigned;
}

constexpr category detail::category_from_string(std::string_view s) {
    for(const auto& c : detail::tables::categories_names) {
        const auto res = detail::propnamecomp(s, c.name);
        if(res == 0)
            return category(c.value);
    }
    return category::unassigned;
}

constexpr block detail::block_from_string(std::string_view s) {
    for(const auto& c : detail::tables::blocks_names) {
        const auto res = detail::propnamecomp(s, c.name);
        if(res == 0)
            return block(c.value);
    }
    return block::no_block;
}

constexpr script detail::script_from_string(std::string_view s) {
    for(const auto& c : detail::tables::scripts_names) {
        const auto res = detail::propnamecomp(s, c.name);
        if(res == 0)
            return script(c.value);
    }
    return script::unknown;
}

constexpr bool detail::is_unassigned(category cat) {
    return cat == category::unassigned;
}

constexpr bool detail::is_unknown(script s) {
    return s == script::unknown;
}

constexpr bool detail::is_unknown(block b) {
    return b == block::no_block;
}

constexpr bool detail::is_unassigned(version v) {
    return v == version::unassigned;
}

constexpr script cp_script(char32_t cp) {
    return detail::tables::cp_script<0>(cp);
}

constexpr script_extensions_view::script_extensions_view(char32_t c_) : c(c_) {}

constexpr script_extensions_view::iterator::iterator(char32_t c_) :
    m_c(c_), m_script(detail::tables::get_cp_script(m_c, 1)) {
    if(m_script == script::unknown)
        m_script = detail::tables::cp_script<0>(m_c);
}

constexpr script script_extensions_view::iterator::operator*() const {
    return m_script;
}

constexpr auto script_extensions_view::iterator::operator++(int) -> iterator& {
    idx++;
    m_script = detail::tables::get_cp_script(m_c, idx);
    return *this;
}

constexpr auto script_extensions_view::iterator::operator++() -> iterator {
    auto c = *this;
    idx++;
    m_script = detail::tables::get_cp_script(m_c, idx);
    return c;
}

constexpr bool script_extensions_view::iterator::operator==(sentinel) const {
    return m_script == script::unknown;
}

constexpr bool script_extensions_view::iterator::operator!=(sentinel) const {
    return m_script != script::unknown;
}

/*constexpr bool script_extensions_view::iterator::operator==(iterator it) const {
    return m_script == it.m_script && m_c == it.m_c;
};
constexpr bool script_extensions_view::iterator::operator!=(iterator it) const {
    return !(*this == it);
};*/

constexpr script_extensions_view::iterator script_extensions_view::begin() const {
    return iterator{c};
}
constexpr script_extensions_view::sentinel script_extensions_view::end() const {
    return {};
}

constexpr script_extensions_view cp_script_extensions(char32_t cp) {
    return script_extensions_view(cp);
}

constexpr version cp_age(char32_t cp) {
    return static_cast<version>(detail::tables::age_data.value(cp, uint8_t(version::unassigned)));
}

constexpr block cp_block(char32_t cp) {
    const auto end = std::end(detail::tables::block_data._data);
    auto it = detail::upper_bound(std::begin(detail::tables::block_data._data), end, cp,
                                  [](char32_t cp_, uint32_t v) {
                                      char32_t c = (v >> 8);
                                      return cp_ < c;
                                  });
    if(it == end)
        return block::no_block;
    it--;
    auto offset = (*it) & 0xFF;
    if(offset == 0)
        return block::no_block;
    offset--;

    const auto d = std::distance(std::begin(detail::tables::block_data._data), it);
    return uni::block((d - offset) + 1);
}

template<>
constexpr bool cp_property_is<property::noncharacter_code_point>(char32_t cp) {
    return (char32_t(cp) & 0xfffe) == 0xfffe || (char32_t(cp) >= 0xfdd0 && char32_t(cp) <= 0xfdef);
}

// http://unicode.org/reports/tr44/#Lowercase
template<>
constexpr bool cp_property_is<property::lowercase>(char32_t cp) {
    return detail::tables::cat_ll.lookup(char32_t(cp)) ||
           detail::tables::prop_olower_data.lookup(char32_t(cp));
}

// http://unicode.org/reports/tr44/#Uppercase
template<>
constexpr bool cp_property_is<property::uppercase>(char32_t cp) {
    return detail::tables::cat_lu.lookup(char32_t(cp)) ||
           detail::tables::prop_oupper_data.lookup(char32_t(cp));
}

// http://unicode.org/reports/tr44/#Cased
template<>
constexpr bool cp_property_is<property::cased>(char32_t cp) {
    return cp_property_is<property::lower>(cp) || cp_property_is<property::upper>(cp) ||
           detail::tables::cat_lt.lookup(char32_t(cp));
}

// http://unicode.org/reports/tr44/#Math
template<>
constexpr bool cp_property_is<property::math>(char32_t cp) {
    return detail::tables::cat_sm.lookup(char32_t(cp)) ||
           detail::tables::prop_omath_data.lookup(cp);
}

// http://unicode.org/reports/tr44/#Case_Ignorable
template<>
constexpr bool cp_property_is<property::case_ignorable>(char32_t) {
    return false;
}

// http://unicode.org/reports/tr44/#Grapheme_Extend
template<>
constexpr bool cp_property_is<property::grapheme_extend>(char32_t cp) {
    return detail::tables::cat_me.lookup(char32_t(cp)) ||
           detail::tables::cat_mn.lookup(char32_t(cp)) ||
           detail::tables::prop_ogr_ext_data.lookup(cp);
}

constexpr bool cp_is_valid(char32_t cp) {
    return char32_t(cp) <= 0x10FFFF;
}
constexpr bool cp_is_assigned(char32_t cp) {
    return detail::tables::prop_assigned.lookup(char32_t(cp));
}

constexpr bool cp_is_ascii(char32_t cp) {
    return char32_t(cp) <= 0x7F;
}

template<>
constexpr bool cp_property_is<property::default_ignorable_code_point>(char32_t cp) {
    const auto c = char32_t(cp);
    const bool maybe = detail::tables::prop_odi_data.lookup(cp) ||
                       detail::tables::cat_cf.lookup(cp) || detail::tables::prop_vs_data.lookup(cp);
    if(!maybe)
        return false;
    // ignore (Interlinear annotation format characters
    if(c >= 0xFFF9 && c <= 0xFFFB) {
        return false;
    }
    // Ignore Egyptian hieroglyph format characters
    else if(c >= 0x13430 && c <= 0x13438) {
        return false;
    } else if(detail::tables::prop_wspace_data.lookup(cp))
        return false;
    else if(detail::tables::prop_pcm_data.lookup(cp))
        return false;
    return true;
}

// http://www.unicode.org/reports/tr31/#D1
template<>
constexpr bool cp_property_is<property::id_start>(char32_t cp) {
    const bool maybe = cp_category_is<category::letter>(cp) || detail::tables::cat_nl.lookup(cp) ||
                       detail::tables::prop_oids_data.lookup(cp);
    if(!maybe)
        return false;
    return !detail::tables::prop_pat_syn_data.lookup(cp) &&
           !detail::tables::prop_pat_ws_data.lookup(cp);
}

template<>
constexpr bool cp_property_is<property::id_continue>(char32_t cp) {
    const bool maybe = cp_category_is<category::letter>(cp) || detail::tables::cat_nl.lookup(cp) ||
                       detail::tables::prop_oids_data.lookup(cp) ||
                       detail::tables::cat_mn.lookup(cp) || detail::tables::cat_mc.lookup(cp) ||
                       detail::tables::cat_nd.lookup(cp) || detail::tables::cat_pc.lookup(cp) ||
                       detail::tables::prop_oidc_data.lookup(cp);
    if(!maybe)
        return false;
    return !detail::tables::prop_pat_syn_data.lookup(cp) &&
           !detail::tables::prop_pat_ws_data.lookup(cp);
}

namespace detail {

    template<typename Array, typename Res = long long>
    constexpr bool get_numeric_value(char32_t cp, const Array& array, Res& res) {
        auto it = detail::lower_bound(std::begin(array), std::end(array), cp,
                                      [](const auto& d, char32_t cp_) { return d.first < cp_; });
        if(it == std::end(array) || it->first != cp)
            return false;
        res = it->second;
        return true;
    }

}    // namespace detail

constexpr numeric_value cp_numeric_value(char32_t cp) {
    long long res = 0;
    if(!(detail::get_numeric_value(cp, detail::tables::numeric_data64, res) ||
             detail::get_numeric_value(cp, detail::tables::numeric_data32, res) ||
             detail::get_numeric_value(cp, detail::tables::numeric_data16, res) ||
             [&res, cp]() -> bool {
           res = detail::tables::numeric_data8.value(cp, 255);
           return res != 255;
       }())) {
        return {};
    }
    int16_t d = 1;
    detail::get_numeric_value(cp, detail::tables::numeric_data_d, d);
    return numeric_value(res, d);
}

}    // namespace uni

namespace std {
template<>
struct iterator_traits<uni::script_extensions_view::iterator> {
    using difference_type = std::ptrdiff_t;
    using value_type = uni::script;
    using pointer = uni::script*;
    using reference = uni::script;
    using iterator_category = std::forward_iterator_tag;
};
}    // namespace std

namespace uni::detail {
enum class binary_prop {
    ahex,
    alpha,
    bidi_c,
    bidi_m,
    cased,
    ci,
    dash,
    dep,
    di,
    dia,
    emoji,
    emoji_component,
    emoji_modifier,
    emoji_modifier_base,
    emoji_presentation,
    ext,
    extended_pictographic,
    gr_base,
    gr_ext,
    hex,
    idc,
    ideo,
    ids,
    idsb,
    idst,
    join_c,
    loe,
    lower,
    math,
    nchar,
    pat_syn,
    pat_ws,
    pcm,
    qmark,
    radical,
    ri,
    sd,
    sterm,
    term,
    uideo,
    upper,
    vs,
    wspace,
    xidc,
    xids,
    any,
    ascii,
    assigned,
    c,
    cc,
    cf,
    cn,
    co,
    cs,
    l,
    lc,
    ll,
    lm,
    lo,
    lt,
    lu,
    m,
    mc,
    me,
    mn,
    n,
    nd,
    nl,
    no,
    p,
    pc,
    pd,
    pe,
    pf,
    pi,
    po,
    ps,
    s,
    sc,
    sk,
    sm,
    so,
    z,
    zl,
    zp,
    zs,
    adlm,
    aghb,
    ahom,
    arab,
    armi,
    armn,
    avst,
    bali,
    bamu,
    bass,
    batk,
    beng,
    bhks,
    bopo,
    brah,
    brai,
    bugi,
    buhd,
    cakm,
    cans,
    cari,
    cham,
    cher,
    chrs,
    copt,
    cpmn,
    cprt,
    cyrl,
    deva,
    diak,
    dogr,
    dsrt,
    dupl,
    egyp,
    elba,
    elym,
    ethi,
    geor,
    glag,
    gong,
    gonm,
    goth,
    gran,
    grek,
    gujr,
    guru,
    hang,
    hani,
    hano,
    hatr,
    hebr,
    hira,
    hluw,
    hmng,
    hmnp,
    hrkt,
    hung,
    ital,
    java,
    kali,
    kana,
    khar,
    khmr,
    khoj,
    kits,
    knda,
    kthi,
    lana,
    laoo,
    latn,
    lepc,
    limb,
    lina,
    linb,
    lisu,
    lyci,
    lydi,
    mahj,
    maka,
    mand,
    mani,
    marc,
    medf,
    mend,
    merc,
    mero,
    mlym,
    modi,
    mong,
    mroo,
    mtei,
    mult,
    mymr,
    nand,
    narb,
    nbat,
    newa,
    nkoo,
    nshu,
    ogam,
    olck,
    orkh,
    orya,
    osge,
    osma,
    ougr,
    palm,
    pauc,
    perm,
    phag,
    phli,
    phlp,
    phnx,
    plrd,
    prti,
    rjng,
    rohg,
    runr,
    samr,
    sarb,
    saur,
    sgnw,
    shaw,
    shrd,
    sidd,
    sind,
    sinh,
    sogd,
    sogo,
    sora,
    soyo,
    sund,
    sylo,
    syrc,
    tagb,
    takr,
    tale,
    talu,
    taml,
    tang,
    tavt,
    telu,
    tfng,
    tglg,
    thaa,
    thai,
    tibt,
    tirh,
    tnsa,
    toto,
    ugar,
    vaii,
    vith,
    wara,
    wcho,
    xpeo,
    xsux,
    yezi,
    yiii,
    zanb,
    zinh,
    zyyy,
    zzzz,
    unknown
};

template<>
constexpr bool get_binary_prop<binary_prop::ahex>(char32_t c) {
    return cp_property_is<property::ahex>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::alpha>(char32_t c) {
    return cp_property_is<property::alpha>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::bidi_c>(char32_t c) {
    return cp_property_is<property::bidi_c>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::bidi_m>(char32_t c) {
    return cp_property_is<property::bidi_m>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::cased>(char32_t c) {
    return cp_property_is<property::cased>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::ci>(char32_t c) {
    return cp_property_is<property::ci>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::dash>(char32_t c) {
    return cp_property_is<property::dash>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::dep>(char32_t c) {
    return cp_property_is<property::dep>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::di>(char32_t c) {
    return cp_property_is<property::di>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::dia>(char32_t c) {
    return cp_property_is<property::dia>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::emoji>(char32_t c) {
    return cp_property_is<property::emoji>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::emoji_component>(char32_t c) {
    return cp_property_is<property::emoji_component>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::emoji_modifier>(char32_t c) {
    return cp_property_is<property::emoji_modifier>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::emoji_modifier_base>(char32_t c) {
    return cp_property_is<property::emoji_modifier_base>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::emoji_presentation>(char32_t c) {
    return cp_property_is<property::emoji_presentation>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::ext>(char32_t c) {
    return cp_property_is<property::ext>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::extended_pictographic>(char32_t c) {
    return cp_property_is<property::extended_pictographic>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::gr_base>(char32_t c) {
    return cp_property_is<property::gr_base>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::gr_ext>(char32_t c) {
    return cp_property_is<property::gr_ext>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::hex>(char32_t c) {
    return cp_property_is<property::hex>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::idc>(char32_t c) {
    return cp_property_is<property::idc>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::ideo>(char32_t c) {
    return cp_property_is<property::ideo>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::ids>(char32_t c) {
    return cp_property_is<property::ids>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::idsb>(char32_t c) {
    return cp_property_is<property::idsb>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::idst>(char32_t c) {
    return cp_property_is<property::idst>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::join_c>(char32_t c) {
    return cp_property_is<property::join_c>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::loe>(char32_t c) {
    return cp_property_is<property::loe>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::lower>(char32_t c) {
    return cp_property_is<property::lower>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::math>(char32_t c) {
    return cp_property_is<property::math>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::nchar>(char32_t c) {
    return cp_property_is<property::nchar>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::pat_syn>(char32_t c) {
    return cp_property_is<property::pat_syn>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::pat_ws>(char32_t c) {
    return cp_property_is<property::pat_ws>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::pcm>(char32_t c) {
    return cp_property_is<property::pcm>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::qmark>(char32_t c) {
    return cp_property_is<property::qmark>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::radical>(char32_t c) {
    return cp_property_is<property::radical>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::ri>(char32_t c) {
    return cp_property_is<property::ri>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::sd>(char32_t c) {
    return cp_property_is<property::sd>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::sterm>(char32_t c) {
    return cp_property_is<property::sterm>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::term>(char32_t c) {
    return cp_property_is<property::term>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::uideo>(char32_t c) {
    return cp_property_is<property::uideo>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::upper>(char32_t c) {
    return cp_property_is<property::upper>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::vs>(char32_t c) {
    return cp_property_is<property::vs>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::wspace>(char32_t c) {
    return cp_property_is<property::wspace>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::xidc>(char32_t c) {
    return cp_property_is<property::xidc>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::xids>(char32_t c) {
    return cp_property_is<property::xids>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::c>(char32_t c) {
    return cp_category_is<category::c>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::cc>(char32_t c) {
    return cp_category_is<category::cc>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::cf>(char32_t c) {
    return cp_category_is<category::cf>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::cn>(char32_t c) {
    return cp_category_is<category::cn>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::co>(char32_t c) {
    return cp_category_is<category::co>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::cs>(char32_t c) {
    return cp_category_is<category::cs>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::l>(char32_t c) {
    return cp_category_is<category::l>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::lc>(char32_t c) {
    return cp_category_is<category::lc>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::ll>(char32_t c) {
    return cp_category_is<category::ll>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::lm>(char32_t c) {
    return cp_category_is<category::lm>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::lo>(char32_t c) {
    return cp_category_is<category::lo>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::lt>(char32_t c) {
    return cp_category_is<category::lt>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::lu>(char32_t c) {
    return cp_category_is<category::lu>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::m>(char32_t c) {
    return cp_category_is<category::m>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::mc>(char32_t c) {
    return cp_category_is<category::mc>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::me>(char32_t c) {
    return cp_category_is<category::me>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::mn>(char32_t c) {
    return cp_category_is<category::mn>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::n>(char32_t c) {
    return cp_category_is<category::n>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::nd>(char32_t c) {
    return cp_category_is<category::nd>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::nl>(char32_t c) {
    return cp_category_is<category::nl>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::no>(char32_t c) {
    return cp_category_is<category::no>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::p>(char32_t c) {
    return cp_category_is<category::p>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::pc>(char32_t c) {
    return cp_category_is<category::pc>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::pd>(char32_t c) {
    return cp_category_is<category::pd>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::pe>(char32_t c) {
    return cp_category_is<category::pe>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::pf>(char32_t c) {
    return cp_category_is<category::pf>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::pi>(char32_t c) {
    return cp_category_is<category::pi>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::po>(char32_t c) {
    return cp_category_is<category::po>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::ps>(char32_t c) {
    return cp_category_is<category::ps>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::s>(char32_t c) {
    return cp_category_is<category::s>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::sc>(char32_t c) {
    return cp_category_is<category::sc>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::sk>(char32_t c) {
    return cp_category_is<category::sk>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::sm>(char32_t c) {
    return cp_category_is<category::sm>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::so>(char32_t c) {
    return cp_category_is<category::so>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::z>(char32_t c) {
    return cp_category_is<category::z>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::zl>(char32_t c) {
    return cp_category_is<category::zl>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::zp>(char32_t c) {
    return cp_category_is<category::zp>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::zs>(char32_t c) {
    return cp_category_is<category::zs>(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::adlm>(char32_t c) {
    return cp_script(c) == script::adlm;
}

template<>
constexpr bool get_binary_prop<binary_prop::aghb>(char32_t c) {
    return cp_script(c) == script::aghb;
}

template<>
constexpr bool get_binary_prop<binary_prop::ahom>(char32_t c) {
    return cp_script(c) == script::ahom;
}

template<>
constexpr bool get_binary_prop<binary_prop::arab>(char32_t c) {
    return cp_script(c) == script::arab;
}

template<>
constexpr bool get_binary_prop<binary_prop::armi>(char32_t c) {
    return cp_script(c) == script::armi;
}

template<>
constexpr bool get_binary_prop<binary_prop::armn>(char32_t c) {
    return cp_script(c) == script::armn;
}

template<>
constexpr bool get_binary_prop<binary_prop::avst>(char32_t c) {
    return cp_script(c) == script::avst;
}

template<>
constexpr bool get_binary_prop<binary_prop::bali>(char32_t c) {
    return cp_script(c) == script::bali;
}

template<>
constexpr bool get_binary_prop<binary_prop::bamu>(char32_t c) {
    return cp_script(c) == script::bamu;
}

template<>
constexpr bool get_binary_prop<binary_prop::bass>(char32_t c) {
    return cp_script(c) == script::bass;
}

template<>
constexpr bool get_binary_prop<binary_prop::batk>(char32_t c) {
    return cp_script(c) == script::batk;
}

template<>
constexpr bool get_binary_prop<binary_prop::beng>(char32_t c) {
    return cp_script(c) == script::beng;
}

template<>
constexpr bool get_binary_prop<binary_prop::bhks>(char32_t c) {
    return cp_script(c) == script::bhks;
}

template<>
constexpr bool get_binary_prop<binary_prop::bopo>(char32_t c) {
    return cp_script(c) == script::bopo;
}

template<>
constexpr bool get_binary_prop<binary_prop::brah>(char32_t c) {
    return cp_script(c) == script::brah;
}

template<>
constexpr bool get_binary_prop<binary_prop::brai>(char32_t c) {
    return cp_script(c) == script::brai;
}

template<>
constexpr bool get_binary_prop<binary_prop::bugi>(char32_t c) {
    return cp_script(c) == script::bugi;
}

template<>
constexpr bool get_binary_prop<binary_prop::buhd>(char32_t c) {
    return cp_script(c) == script::buhd;
}

template<>
constexpr bool get_binary_prop<binary_prop::cakm>(char32_t c) {
    return cp_script(c) == script::cakm;
}

template<>
constexpr bool get_binary_prop<binary_prop::cans>(char32_t c) {
    return cp_script(c) == script::cans;
}

template<>
constexpr bool get_binary_prop<binary_prop::cari>(char32_t c) {
    return cp_script(c) == script::cari;
}

template<>
constexpr bool get_binary_prop<binary_prop::cham>(char32_t c) {
    return cp_script(c) == script::cham;
}

template<>
constexpr bool get_binary_prop<binary_prop::cher>(char32_t c) {
    return cp_script(c) == script::cher;
}

template<>
constexpr bool get_binary_prop<binary_prop::chrs>(char32_t c) {
    return cp_script(c) == script::chrs;
}

template<>
constexpr bool get_binary_prop<binary_prop::copt>(char32_t c) {
    return cp_script(c) == script::copt;
}

template<>
constexpr bool get_binary_prop<binary_prop::cpmn>(char32_t c) {
    return cp_script(c) == script::cpmn;
}

template<>
constexpr bool get_binary_prop<binary_prop::cprt>(char32_t c) {
    return cp_script(c) == script::cprt;
}

template<>
constexpr bool get_binary_prop<binary_prop::cyrl>(char32_t c) {
    return cp_script(c) == script::cyrl;
}

template<>
constexpr bool get_binary_prop<binary_prop::deva>(char32_t c) {
    return cp_script(c) == script::deva;
}

template<>
constexpr bool get_binary_prop<binary_prop::diak>(char32_t c) {
    return cp_script(c) == script::diak;
}

template<>
constexpr bool get_binary_prop<binary_prop::dogr>(char32_t c) {
    return cp_script(c) == script::dogr;
}

template<>
constexpr bool get_binary_prop<binary_prop::dsrt>(char32_t c) {
    return cp_script(c) == script::dsrt;
}

template<>
constexpr bool get_binary_prop<binary_prop::dupl>(char32_t c) {
    return cp_script(c) == script::dupl;
}

template<>
constexpr bool get_binary_prop<binary_prop::egyp>(char32_t c) {
    return cp_script(c) == script::egyp;
}

template<>
constexpr bool get_binary_prop<binary_prop::elba>(char32_t c) {
    return cp_script(c) == script::elba;
}

template<>
constexpr bool get_binary_prop<binary_prop::elym>(char32_t c) {
    return cp_script(c) == script::elym;
}

template<>
constexpr bool get_binary_prop<binary_prop::ethi>(char32_t c) {
    return cp_script(c) == script::ethi;
}

template<>
constexpr bool get_binary_prop<binary_prop::geor>(char32_t c) {
    return cp_script(c) == script::geor;
}

template<>
constexpr bool get_binary_prop<binary_prop::glag>(char32_t c) {
    return cp_script(c) == script::glag;
}

template<>
constexpr bool get_binary_prop<binary_prop::gong>(char32_t c) {
    return cp_script(c) == script::gong;
}

template<>
constexpr bool get_binary_prop<binary_prop::gonm>(char32_t c) {
    return cp_script(c) == script::gonm;
}

template<>
constexpr bool get_binary_prop<binary_prop::goth>(char32_t c) {
    return cp_script(c) == script::goth;
}

template<>
constexpr bool get_binary_prop<binary_prop::gran>(char32_t c) {
    return cp_script(c) == script::gran;
}

template<>
constexpr bool get_binary_prop<binary_prop::grek>(char32_t c) {
    return cp_script(c) == script::grek;
}

template<>
constexpr bool get_binary_prop<binary_prop::gujr>(char32_t c) {
    return cp_script(c) == script::gujr;
}

template<>
constexpr bool get_binary_prop<binary_prop::guru>(char32_t c) {
    return cp_script(c) == script::guru;
}

template<>
constexpr bool get_binary_prop<binary_prop::hang>(char32_t c) {
    return cp_script(c) == script::hang;
}

template<>
constexpr bool get_binary_prop<binary_prop::hani>(char32_t c) {
    return cp_script(c) == script::hani;
}

template<>
constexpr bool get_binary_prop<binary_prop::hano>(char32_t c) {
    return cp_script(c) == script::hano;
}

template<>
constexpr bool get_binary_prop<binary_prop::hatr>(char32_t c) {
    return cp_script(c) == script::hatr;
}

template<>
constexpr bool get_binary_prop<binary_prop::hebr>(char32_t c) {
    return cp_script(c) == script::hebr;
}

template<>
constexpr bool get_binary_prop<binary_prop::hira>(char32_t c) {
    return cp_script(c) == script::hira;
}

template<>
constexpr bool get_binary_prop<binary_prop::hluw>(char32_t c) {
    return cp_script(c) == script::hluw;
}

template<>
constexpr bool get_binary_prop<binary_prop::hmng>(char32_t c) {
    return cp_script(c) == script::hmng;
}

template<>
constexpr bool get_binary_prop<binary_prop::hmnp>(char32_t c) {
    return cp_script(c) == script::hmnp;
}

template<>
constexpr bool get_binary_prop<binary_prop::hrkt>(char32_t c) {
    return cp_script(c) == script::hrkt;
}

template<>
constexpr bool get_binary_prop<binary_prop::hung>(char32_t c) {
    return cp_script(c) == script::hung;
}

template<>
constexpr bool get_binary_prop<binary_prop::ital>(char32_t c) {
    return cp_script(c) == script::ital;
}

template<>
constexpr bool get_binary_prop<binary_prop::java>(char32_t c) {
    return cp_script(c) == script::java;
}

template<>
constexpr bool get_binary_prop<binary_prop::kali>(char32_t c) {
    return cp_script(c) == script::kali;
}

template<>
constexpr bool get_binary_prop<binary_prop::kana>(char32_t c) {
    return cp_script(c) == script::kana;
}

template<>
constexpr bool get_binary_prop<binary_prop::khar>(char32_t c) {
    return cp_script(c) == script::khar;
}

template<>
constexpr bool get_binary_prop<binary_prop::khmr>(char32_t c) {
    return cp_script(c) == script::khmr;
}

template<>
constexpr bool get_binary_prop<binary_prop::khoj>(char32_t c) {
    return cp_script(c) == script::khoj;
}

template<>
constexpr bool get_binary_prop<binary_prop::kits>(char32_t c) {
    return cp_script(c) == script::kits;
}

template<>
constexpr bool get_binary_prop<binary_prop::knda>(char32_t c) {
    return cp_script(c) == script::knda;
}

template<>
constexpr bool get_binary_prop<binary_prop::kthi>(char32_t c) {
    return cp_script(c) == script::kthi;
}

template<>
constexpr bool get_binary_prop<binary_prop::lana>(char32_t c) {
    return cp_script(c) == script::lana;
}

template<>
constexpr bool get_binary_prop<binary_prop::laoo>(char32_t c) {
    return cp_script(c) == script::laoo;
}

template<>
constexpr bool get_binary_prop<binary_prop::latn>(char32_t c) {
    return cp_script(c) == script::latn;
}

template<>
constexpr bool get_binary_prop<binary_prop::lepc>(char32_t c) {
    return cp_script(c) == script::lepc;
}

template<>
constexpr bool get_binary_prop<binary_prop::limb>(char32_t c) {
    return cp_script(c) == script::limb;
}

template<>
constexpr bool get_binary_prop<binary_prop::lina>(char32_t c) {
    return cp_script(c) == script::lina;
}

template<>
constexpr bool get_binary_prop<binary_prop::linb>(char32_t c) {
    return cp_script(c) == script::linb;
}

template<>
constexpr bool get_binary_prop<binary_prop::lisu>(char32_t c) {
    return cp_script(c) == script::lisu;
}

template<>
constexpr bool get_binary_prop<binary_prop::lyci>(char32_t c) {
    return cp_script(c) == script::lyci;
}

template<>
constexpr bool get_binary_prop<binary_prop::lydi>(char32_t c) {
    return cp_script(c) == script::lydi;
}

template<>
constexpr bool get_binary_prop<binary_prop::mahj>(char32_t c) {
    return cp_script(c) == script::mahj;
}

template<>
constexpr bool get_binary_prop<binary_prop::maka>(char32_t c) {
    return cp_script(c) == script::maka;
}

template<>
constexpr bool get_binary_prop<binary_prop::mand>(char32_t c) {
    return cp_script(c) == script::mand;
}

template<>
constexpr bool get_binary_prop<binary_prop::mani>(char32_t c) {
    return cp_script(c) == script::mani;
}

template<>
constexpr bool get_binary_prop<binary_prop::marc>(char32_t c) {
    return cp_script(c) == script::marc;
}

template<>
constexpr bool get_binary_prop<binary_prop::medf>(char32_t c) {
    return cp_script(c) == script::medf;
}

template<>
constexpr bool get_binary_prop<binary_prop::mend>(char32_t c) {
    return cp_script(c) == script::mend;
}

template<>
constexpr bool get_binary_prop<binary_prop::merc>(char32_t c) {
    return cp_script(c) == script::merc;
}

template<>
constexpr bool get_binary_prop<binary_prop::mero>(char32_t c) {
    return cp_script(c) == script::mero;
}

template<>
constexpr bool get_binary_prop<binary_prop::mlym>(char32_t c) {
    return cp_script(c) == script::mlym;
}

template<>
constexpr bool get_binary_prop<binary_prop::modi>(char32_t c) {
    return cp_script(c) == script::modi;
}

template<>
constexpr bool get_binary_prop<binary_prop::mong>(char32_t c) {
    return cp_script(c) == script::mong;
}

template<>
constexpr bool get_binary_prop<binary_prop::mroo>(char32_t c) {
    return cp_script(c) == script::mroo;
}

template<>
constexpr bool get_binary_prop<binary_prop::mtei>(char32_t c) {
    return cp_script(c) == script::mtei;
}

template<>
constexpr bool get_binary_prop<binary_prop::mult>(char32_t c) {
    return cp_script(c) == script::mult;
}

template<>
constexpr bool get_binary_prop<binary_prop::mymr>(char32_t c) {
    return cp_script(c) == script::mymr;
}

template<>
constexpr bool get_binary_prop<binary_prop::nand>(char32_t c) {
    return cp_script(c) == script::nand;
}

template<>
constexpr bool get_binary_prop<binary_prop::narb>(char32_t c) {
    return cp_script(c) == script::narb;
}

template<>
constexpr bool get_binary_prop<binary_prop::nbat>(char32_t c) {
    return cp_script(c) == script::nbat;
}

template<>
constexpr bool get_binary_prop<binary_prop::newa>(char32_t c) {
    return cp_script(c) == script::newa;
}

template<>
constexpr bool get_binary_prop<binary_prop::nkoo>(char32_t c) {
    return cp_script(c) == script::nkoo;
}

template<>
constexpr bool get_binary_prop<binary_prop::nshu>(char32_t c) {
    return cp_script(c) == script::nshu;
}

template<>
constexpr bool get_binary_prop<binary_prop::ogam>(char32_t c) {
    return cp_script(c) == script::ogam;
}

template<>
constexpr bool get_binary_prop<binary_prop::olck>(char32_t c) {
    return cp_script(c) == script::olck;
}

template<>
constexpr bool get_binary_prop<binary_prop::orkh>(char32_t c) {
    return cp_script(c) == script::orkh;
}

template<>
constexpr bool get_binary_prop<binary_prop::orya>(char32_t c) {
    return cp_script(c) == script::orya;
}

template<>
constexpr bool get_binary_prop<binary_prop::osge>(char32_t c) {
    return cp_script(c) == script::osge;
}

template<>
constexpr bool get_binary_prop<binary_prop::osma>(char32_t c) {
    return cp_script(c) == script::osma;
}

template<>
constexpr bool get_binary_prop<binary_prop::ougr>(char32_t c) {
    return cp_script(c) == script::ougr;
}

template<>
constexpr bool get_binary_prop<binary_prop::palm>(char32_t c) {
    return cp_script(c) == script::palm;
}

template<>
constexpr bool get_binary_prop<binary_prop::pauc>(char32_t c) {
    return cp_script(c) == script::pauc;
}

template<>
constexpr bool get_binary_prop<binary_prop::perm>(char32_t c) {
    return cp_script(c) == script::perm;
}

template<>
constexpr bool get_binary_prop<binary_prop::phag>(char32_t c) {
    return cp_script(c) == script::phag;
}

template<>
constexpr bool get_binary_prop<binary_prop::phli>(char32_t c) {
    return cp_script(c) == script::phli;
}

template<>
constexpr bool get_binary_prop<binary_prop::phlp>(char32_t c) {
    return cp_script(c) == script::phlp;
}

template<>
constexpr bool get_binary_prop<binary_prop::phnx>(char32_t c) {
    return cp_script(c) == script::phnx;
}

template<>
constexpr bool get_binary_prop<binary_prop::plrd>(char32_t c) {
    return cp_script(c) == script::plrd;
}

template<>
constexpr bool get_binary_prop<binary_prop::prti>(char32_t c) {
    return cp_script(c) == script::prti;
}

template<>
constexpr bool get_binary_prop<binary_prop::rjng>(char32_t c) {
    return cp_script(c) == script::rjng;
}

template<>
constexpr bool get_binary_prop<binary_prop::rohg>(char32_t c) {
    return cp_script(c) == script::rohg;
}

template<>
constexpr bool get_binary_prop<binary_prop::runr>(char32_t c) {
    return cp_script(c) == script::runr;
}

template<>
constexpr bool get_binary_prop<binary_prop::samr>(char32_t c) {
    return cp_script(c) == script::samr;
}

template<>
constexpr bool get_binary_prop<binary_prop::sarb>(char32_t c) {
    return cp_script(c) == script::sarb;
}

template<>
constexpr bool get_binary_prop<binary_prop::saur>(char32_t c) {
    return cp_script(c) == script::saur;
}

template<>
constexpr bool get_binary_prop<binary_prop::sgnw>(char32_t c) {
    return cp_script(c) == script::sgnw;
}

template<>
constexpr bool get_binary_prop<binary_prop::shaw>(char32_t c) {
    return cp_script(c) == script::shaw;
}

template<>
constexpr bool get_binary_prop<binary_prop::shrd>(char32_t c) {
    return cp_script(c) == script::shrd;
}

template<>
constexpr bool get_binary_prop<binary_prop::sidd>(char32_t c) {
    return cp_script(c) == script::sidd;
}

template<>
constexpr bool get_binary_prop<binary_prop::sind>(char32_t c) {
    return cp_script(c) == script::sind;
}

template<>
constexpr bool get_binary_prop<binary_prop::sinh>(char32_t c) {
    return cp_script(c) == script::sinh;
}

template<>
constexpr bool get_binary_prop<binary_prop::sogd>(char32_t c) {
    return cp_script(c) == script::sogd;
}

template<>
constexpr bool get_binary_prop<binary_prop::sogo>(char32_t c) {
    return cp_script(c) == script::sogo;
}

template<>
constexpr bool get_binary_prop<binary_prop::sora>(char32_t c) {
    return cp_script(c) == script::sora;
}

template<>
constexpr bool get_binary_prop<binary_prop::soyo>(char32_t c) {
    return cp_script(c) == script::soyo;
}

template<>
constexpr bool get_binary_prop<binary_prop::sund>(char32_t c) {
    return cp_script(c) == script::sund;
}

template<>
constexpr bool get_binary_prop<binary_prop::sylo>(char32_t c) {
    return cp_script(c) == script::sylo;
}

template<>
constexpr bool get_binary_prop<binary_prop::syrc>(char32_t c) {
    return cp_script(c) == script::syrc;
}

template<>
constexpr bool get_binary_prop<binary_prop::tagb>(char32_t c) {
    return cp_script(c) == script::tagb;
}

template<>
constexpr bool get_binary_prop<binary_prop::takr>(char32_t c) {
    return cp_script(c) == script::takr;
}

template<>
constexpr bool get_binary_prop<binary_prop::tale>(char32_t c) {
    return cp_script(c) == script::tale;
}

template<>
constexpr bool get_binary_prop<binary_prop::talu>(char32_t c) {
    return cp_script(c) == script::talu;
}

template<>
constexpr bool get_binary_prop<binary_prop::taml>(char32_t c) {
    return cp_script(c) == script::taml;
}

template<>
constexpr bool get_binary_prop<binary_prop::tang>(char32_t c) {
    return cp_script(c) == script::tang;
}

template<>
constexpr bool get_binary_prop<binary_prop::tavt>(char32_t c) {
    return cp_script(c) == script::tavt;
}

template<>
constexpr bool get_binary_prop<binary_prop::telu>(char32_t c) {
    return cp_script(c) == script::telu;
}

template<>
constexpr bool get_binary_prop<binary_prop::tfng>(char32_t c) {
    return cp_script(c) == script::tfng;
}

template<>
constexpr bool get_binary_prop<binary_prop::tglg>(char32_t c) {
    return cp_script(c) == script::tglg;
}

template<>
constexpr bool get_binary_prop<binary_prop::thaa>(char32_t c) {
    return cp_script(c) == script::thaa;
}

template<>
constexpr bool get_binary_prop<binary_prop::thai>(char32_t c) {
    return cp_script(c) == script::thai;
}

template<>
constexpr bool get_binary_prop<binary_prop::tibt>(char32_t c) {
    return cp_script(c) == script::tibt;
}

template<>
constexpr bool get_binary_prop<binary_prop::tirh>(char32_t c) {
    return cp_script(c) == script::tirh;
}

template<>
constexpr bool get_binary_prop<binary_prop::tnsa>(char32_t c) {
    return cp_script(c) == script::tnsa;
}

template<>
constexpr bool get_binary_prop<binary_prop::toto>(char32_t c) {
    return cp_script(c) == script::toto;
}

template<>
constexpr bool get_binary_prop<binary_prop::ugar>(char32_t c) {
    return cp_script(c) == script::ugar;
}

template<>
constexpr bool get_binary_prop<binary_prop::vaii>(char32_t c) {
    return cp_script(c) == script::vaii;
}

template<>
constexpr bool get_binary_prop<binary_prop::vith>(char32_t c) {
    return cp_script(c) == script::vith;
}

template<>
constexpr bool get_binary_prop<binary_prop::wara>(char32_t c) {
    return cp_script(c) == script::wara;
}

template<>
constexpr bool get_binary_prop<binary_prop::wcho>(char32_t c) {
    return cp_script(c) == script::wcho;
}

template<>
constexpr bool get_binary_prop<binary_prop::xpeo>(char32_t c) {
    return cp_script(c) == script::xpeo;
}

template<>
constexpr bool get_binary_prop<binary_prop::xsux>(char32_t c) {
    return cp_script(c) == script::xsux;
}

template<>
constexpr bool get_binary_prop<binary_prop::yezi>(char32_t c) {
    return cp_script(c) == script::yezi;
}

template<>
constexpr bool get_binary_prop<binary_prop::yiii>(char32_t c) {
    return cp_script(c) == script::yiii;
}

template<>
constexpr bool get_binary_prop<binary_prop::zanb>(char32_t c) {
    return cp_script(c) == script::zanb;
}

template<>
constexpr bool get_binary_prop<binary_prop::zinh>(char32_t c) {
    return cp_script(c) == script::zinh;
}

template<>
constexpr bool get_binary_prop<binary_prop::zyyy>(char32_t c) {
    return cp_script(c) == script::zyyy;
}

template<>
constexpr bool get_binary_prop<binary_prop::zzzz>(char32_t c) {
    return cp_script(c) == script::zzzz;
}
namespace tables {
    static constexpr string_with_idx binary_prop_names[] = {
        string_with_idx{"adlam", 86},
        string_with_idx{"adlm", 86},
        string_with_idx{"aghb", 87},
        string_with_idx{"ahex", 0},
        string_with_idx{"ahom", 88},
        string_with_idx{"alpha", 1},
        string_with_idx{"alphabetic", 1},
        string_with_idx{"anatolian_hieroglyphs", 138},
        string_with_idx{"any", 45},
        string_with_idx{"arab", 89},
        string_with_idx{"arabic", 89},
        string_with_idx{"armenian", 91},
        string_with_idx{"armi", 90},
        string_with_idx{"armn", 91},
        string_with_idx{"ascii", 46},
        string_with_idx{"ascii_hex_digit", 0},
        string_with_idx{"assigned", 47},
        string_with_idx{"avestan", 92},
        string_with_idx{"avst", 92},
        string_with_idx{"bali", 93},
        string_with_idx{"balinese", 93},
        string_with_idx{"bamu", 94},
        string_with_idx{"bamum", 94},
        string_with_idx{"bass", 95},
        string_with_idx{"bassa_vah", 95},
        string_with_idx{"batak", 96},
        string_with_idx{"batk", 96},
        string_with_idx{"beng", 97},
        string_with_idx{"bengali", 97},
        string_with_idx{"bhaiksuki", 98},
        string_with_idx{"bhks", 98},
        string_with_idx{"bidi_c", 2},
        string_with_idx{"bidi_control", 2},
        string_with_idx{"bidi_m", 3},
        string_with_idx{"bidi_mirrored", 3},
        string_with_idx{"bopo", 99},
        string_with_idx{"bopomofo", 99},
        string_with_idx{"brah", 100},
        string_with_idx{"brahmi", 100},
        string_with_idx{"brai", 101},
        string_with_idx{"braille", 101},
        string_with_idx{"bugi", 102},
        string_with_idx{"buginese", 102},
        string_with_idx{"buhd", 103},
        string_with_idx{"buhid", 103},
        string_with_idx{"c", 48},
        string_with_idx{"cakm", 104},
        string_with_idx{"canadian_aboriginal", 105},
        string_with_idx{"cans", 105},
        string_with_idx{"cari", 106},
        string_with_idx{"carian", 106},
        string_with_idx{"case_ignorable", 5},
        string_with_idx{"cased", 4},
        string_with_idx{"cased_letter", 55},
        string_with_idx{"caucasian_albanian", 87},
        string_with_idx{"cc", 49},
        string_with_idx{"cf", 50},
        string_with_idx{"chakma", 104},
        string_with_idx{"cham", 107},
        string_with_idx{"cher", 108},
        string_with_idx{"cherokee", 108},
        string_with_idx{"chorasmian", 109},
        string_with_idx{"chrs", 109},
        string_with_idx{"ci", 5},
        string_with_idx{"close_punctuation", 72},
        string_with_idx{"cn", 51},
        string_with_idx{"co", 52},
        string_with_idx{"common", 247},
        string_with_idx{"connector_punctuation", 70},
        string_with_idx{"control", 49},
        string_with_idx{"copt", 110},
        string_with_idx{"coptic", 110},
        string_with_idx{"cpmn", 111},
        string_with_idx{"cprt", 112},
        string_with_idx{"cs", 53},
        string_with_idx{"cuneiform", 242},
        string_with_idx{"currency_symbol", 78},
        string_with_idx{"cypriot", 112},
        string_with_idx{"cypro_minoan", 111},
        string_with_idx{"cyrillic", 113},
        string_with_idx{"cyrl", 113},
        string_with_idx{"dash", 6},
        string_with_idx{"dash_punctuation", 71},
        string_with_idx{"decimal_number", 66},
        string_with_idx{"default_ignorable_code_point", 8},
        string_with_idx{"dep", 7},
        string_with_idx{"deprecated", 7},
        string_with_idx{"deseret", 117},
        string_with_idx{"deva", 114},
        string_with_idx{"devanagari", 114},
        string_with_idx{"di", 8},
        string_with_idx{"dia", 9},
        string_with_idx{"diacritic", 9},
        string_with_idx{"diak", 115},
        string_with_idx{"dives_akuru", 115},
        string_with_idx{"dogr", 116},
        string_with_idx{"dogra", 116},
        string_with_idx{"dsrt", 117},
        string_with_idx{"dupl", 118},
        string_with_idx{"duployan", 118},
        string_with_idx{"egyp", 119},
        string_with_idx{"egyptian_hieroglyphs", 119},
        string_with_idx{"elba", 120},
        string_with_idx{"elbasan", 120},
        string_with_idx{"elym", 121},
        string_with_idx{"elymaic", 121},
        string_with_idx{"emoji", 10},
        string_with_idx{"emoji_component", 11},
        string_with_idx{"emoji_modifier", 12},
        string_with_idx{"emoji_modifier_base", 13},
        string_with_idx{"emoji_presentation", 14},
        string_with_idx{"enclosing_mark", 63},
        string_with_idx{"ethi", 122},
        string_with_idx{"ethiopic", 122},
        string_with_idx{"ext", 15},
        string_with_idx{"extended_pictographic", 16},
        string_with_idx{"extender", 15},
        string_with_idx{"final_punctuation", 73},
        string_with_idx{"format", 50},
        string_with_idx{"geor", 123},
        string_with_idx{"georgian", 123},
        string_with_idx{"glag", 124},
        string_with_idx{"glagolitic", 124},
        string_with_idx{"gong", 125},
        string_with_idx{"gonm", 126},
        string_with_idx{"goth", 127},
        string_with_idx{"gothic", 127},
        string_with_idx{"gr_base", 17},
        string_with_idx{"gr_ext", 18},
        string_with_idx{"gran", 128},
        string_with_idx{"grantha", 128},
        string_with_idx{"grapheme_base", 17},
        string_with_idx{"grapheme_extend", 18},
        string_with_idx{"greek", 129},
        string_with_idx{"grek", 129},
        string_with_idx{"gujarati", 130},
        string_with_idx{"gujr", 130},
        string_with_idx{"gunjala_gondi", 125},
        string_with_idx{"gurmukhi", 131},
        string_with_idx{"guru", 131},
        string_with_idx{"han", 133},
        string_with_idx{"hang", 132},
        string_with_idx{"hangul", 132},
        string_with_idx{"hani", 133},
        string_with_idx{"hanifi_rohingya", 202},
        string_with_idx{"hano", 134},
        string_with_idx{"hanunoo", 134},
        string_with_idx{"hatr", 135},
        string_with_idx{"hatran", 135},
        string_with_idx{"hebr", 136},
        string_with_idx{"hebrew", 136},
        string_with_idx{"hex", 19},
        string_with_idx{"hex_digit", 19},
        string_with_idx{"hira", 137},
        string_with_idx{"hiragana", 137},
        string_with_idx{"hluw", 138},
        string_with_idx{"hmng", 139},
        string_with_idx{"hmnp", 140},
        string_with_idx{"hrkt", 141},
        string_with_idx{"hung", 142},
        string_with_idx{"id_continue", 20},
        string_with_idx{"id_start", 22},
        string_with_idx{"idc", 20},
        string_with_idx{"ideo", 21},
        string_with_idx{"ideographic", 21},
        string_with_idx{"ids", 22},
        string_with_idx{"ids_binary_operator", 23},
        string_with_idx{"ids_trinary_operator", 24},
        string_with_idx{"idsb", 23},
        string_with_idx{"idst", 24},
        string_with_idx{"imperial_aramaic", 90},
        string_with_idx{"inherited", 246},
        string_with_idx{"initial_punctuation", 74},
        string_with_idx{"inscriptional_pahlavi", 196},
        string_with_idx{"inscriptional_parthian", 200},
        string_with_idx{"ital", 143},
        string_with_idx{"java", 144},
        string_with_idx{"javanese", 144},
        string_with_idx{"join_c", 25},
        string_with_idx{"join_control", 25},
        string_with_idx{"kaithi", 152},
        string_with_idx{"kali", 145},
        string_with_idx{"kana", 146},
        string_with_idx{"kannada", 151},
        string_with_idx{"katakana", 146},
        string_with_idx{"katakana_or_hiragana", 141},
        string_with_idx{"kayah_li", 145},
        string_with_idx{"khar", 147},
        string_with_idx{"kharoshthi", 147},
        string_with_idx{"khitan_small_script", 150},
        string_with_idx{"khmer", 148},
        string_with_idx{"khmr", 148},
        string_with_idx{"khoj", 149},
        string_with_idx{"khojki", 149},
        string_with_idx{"khudawadi", 211},
        string_with_idx{"kits", 150},
        string_with_idx{"knda", 151},
        string_with_idx{"kthi", 152},
        string_with_idx{"l", 54},
        string_with_idx{"lana", 153},
        string_with_idx{"lao", 154},
        string_with_idx{"laoo", 154},
        string_with_idx{"latin", 155},
        string_with_idx{"latn", 155},
        string_with_idx{"lc", 55},
        string_with_idx{"lepc", 156},
        string_with_idx{"lepcha", 156},
        string_with_idx{"letter", 54},
        string_with_idx{"letter_number", 67},
        string_with_idx{"limb", 157},
        string_with_idx{"limbu", 157},
        string_with_idx{"lina", 158},
        string_with_idx{"linb", 159},
        string_with_idx{"line_separator", 83},
        string_with_idx{"linear_a", 158},
        string_with_idx{"linear_b", 159},
        string_with_idx{"lisu", 160},
        string_with_idx{"ll", 56},
        string_with_idx{"lm", 57},
        string_with_idx{"lo", 58},
        string_with_idx{"loe", 26},
        string_with_idx{"logical_order_exception", 26},
        string_with_idx{"lower", 27},
        string_with_idx{"lowercase", 27},
        string_with_idx{"lowercase_letter", 56},
        string_with_idx{"lt", 59},
        string_with_idx{"lu", 60},
        string_with_idx{"lyci", 161},
        string_with_idx{"lycian", 161},
        string_with_idx{"lydi", 162},
        string_with_idx{"lydian", 162},
        string_with_idx{"m", 61},
        string_with_idx{"mahajani", 163},
        string_with_idx{"mahj", 163},
        string_with_idx{"maka", 164},
        string_with_idx{"makasar", 164},
        string_with_idx{"malayalam", 172},
        string_with_idx{"mand", 165},
        string_with_idx{"mandaic", 165},
        string_with_idx{"mani", 166},
        string_with_idx{"manichaean", 166},
        string_with_idx{"marc", 167},
        string_with_idx{"marchen", 167},
        string_with_idx{"mark", 61},
        string_with_idx{"masaram_gondi", 126},
        string_with_idx{"math", 28},
        string_with_idx{"math_symbol", 80},
        string_with_idx{"mc", 62},
        string_with_idx{"me", 63},
        string_with_idx{"medefaidrin", 168},
        string_with_idx{"medf", 168},
        string_with_idx{"meetei_mayek", 176},
        string_with_idx{"mend", 169},
        string_with_idx{"mende_kikakui", 169},
        string_with_idx{"merc", 170},
        string_with_idx{"mero", 171},
        string_with_idx{"meroitic_cursive", 170},
        string_with_idx{"meroitic_hieroglyphs", 171},
        string_with_idx{"miao", 199},
        string_with_idx{"mlym", 172},
        string_with_idx{"mn", 64},
        string_with_idx{"modi", 173},
        string_with_idx{"modifier_letter", 57},
        string_with_idx{"modifier_symbol", 79},
        string_with_idx{"mong", 174},
        string_with_idx{"mongolian", 174},
        string_with_idx{"mro", 175},
        string_with_idx{"mroo", 175},
        string_with_idx{"mtei", 176},
        string_with_idx{"mult", 177},
        string_with_idx{"multani", 177},
        string_with_idx{"myanmar", 178},
        string_with_idx{"mymr", 178},
        string_with_idx{"n", 65},
        string_with_idx{"nabataean", 181},
        string_with_idx{"nand", 179},
        string_with_idx{"nandinagari", 179},
        string_with_idx{"narb", 180},
        string_with_idx{"nbat", 181},
        string_with_idx{"nchar", 29},
        string_with_idx{"nd", 66},
        string_with_idx{"new_tai_lue", 223},
        string_with_idx{"newa", 182},
        string_with_idx{"nko", 183},
        string_with_idx{"nkoo", 183},
        string_with_idx{"nl", 67},
        string_with_idx{"no", 68},
        string_with_idx{"noncharacter_code_point", 29},
        string_with_idx{"nonspacing_mark", 64},
        string_with_idx{"nshu", 184},
        string_with_idx{"number", 65},
        string_with_idx{"nushu", 184},
        string_with_idx{"nyiakeng_puachue_hmong", 140},
        string_with_idx{"ogam", 185},
        string_with_idx{"ogham", 185},
        string_with_idx{"ol_chiki", 186},
        string_with_idx{"olck", 186},
        string_with_idx{"old_hungarian", 142},
        string_with_idx{"old_italic", 143},
        string_with_idx{"old_north_arabian", 180},
        string_with_idx{"old_permic", 194},
        string_with_idx{"old_persian", 241},
        string_with_idx{"old_sogdian", 214},
        string_with_idx{"old_south_arabian", 205},
        string_with_idx{"old_turkic", 187},
        string_with_idx{"old_uyghur", 191},
        string_with_idx{"open_punctuation", 76},
        string_with_idx{"oriya", 188},
        string_with_idx{"orkh", 187},
        string_with_idx{"orya", 188},
        string_with_idx{"osage", 189},
        string_with_idx{"osge", 189},
        string_with_idx{"osma", 190},
        string_with_idx{"osmanya", 190},
        string_with_idx{"other", 48},
        string_with_idx{"other_letter", 58},
        string_with_idx{"other_number", 68},
        string_with_idx{"other_punctuation", 75},
        string_with_idx{"other_symbol", 81},
        string_with_idx{"ougr", 191},
        string_with_idx{"p", 69},
        string_with_idx{"pahawh_hmong", 139},
        string_with_idx{"palm", 192},
        string_with_idx{"palmyrene", 192},
        string_with_idx{"paragraph_separator", 84},
        string_with_idx{"pat_syn", 30},
        string_with_idx{"pat_ws", 31},
        string_with_idx{"pattern_syntax", 30},
        string_with_idx{"pattern_white_space", 31},
        string_with_idx{"pau_cin_hau", 193},
        string_with_idx{"pauc", 193},
        string_with_idx{"pc", 70},
        string_with_idx{"pcm", 32},
        string_with_idx{"pd", 71},
        string_with_idx{"pe", 72},
        string_with_idx{"perm", 194},
        string_with_idx{"pf", 73},
        string_with_idx{"phag", 195},
        string_with_idx{"phags_pa", 195},
        string_with_idx{"phli", 196},
        string_with_idx{"phlp", 197},
        string_with_idx{"phnx", 198},
        string_with_idx{"phoenician", 198},
        string_with_idx{"pi", 74},
        string_with_idx{"plrd", 199},
        string_with_idx{"po", 75},
        string_with_idx{"prepended_concatenation_mark", 32},
        string_with_idx{"private_use", 52},
        string_with_idx{"prti", 200},
        string_with_idx{"ps", 76},
        string_with_idx{"psalter_pahlavi", 197},
        string_with_idx{"punctuation", 69},
        string_with_idx{"qmark", 33},
        string_with_idx{"quotation_mark", 33},
        string_with_idx{"radical", 34},
        string_with_idx{"regional_indicator", 35},
        string_with_idx{"rejang", 201},
        string_with_idx{"ri", 35},
        string_with_idx{"rjng", 201},
        string_with_idx{"rohg", 202},
        string_with_idx{"runic", 203},
        string_with_idx{"runr", 203},
        string_with_idx{"s", 77},
        string_with_idx{"samaritan", 204},
        string_with_idx{"samr", 204},
        string_with_idx{"sarb", 205},
        string_with_idx{"saur", 206},
        string_with_idx{"saurashtra", 206},
        string_with_idx{"sc", 78},
        string_with_idx{"sd", 36},
        string_with_idx{"sentence_terminal", 37},
        string_with_idx{"separator", 82},
        string_with_idx{"sgnw", 207},
        string_with_idx{"sharada", 209},
        string_with_idx{"shavian", 208},
        string_with_idx{"shaw", 208},
        string_with_idx{"shrd", 209},
        string_with_idx{"sidd", 210},
        string_with_idx{"siddham", 210},
        string_with_idx{"signwriting", 207},
        string_with_idx{"sind", 211},
        string_with_idx{"sinh", 212},
        string_with_idx{"sinhala", 212},
        string_with_idx{"sk", 79},
        string_with_idx{"sm", 80},
        string_with_idx{"so", 81},
        string_with_idx{"soft_dotted", 36},
        string_with_idx{"sogd", 213},
        string_with_idx{"sogdian", 213},
        string_with_idx{"sogo", 214},
        string_with_idx{"sora", 215},
        string_with_idx{"sora_sompeng", 215},
        string_with_idx{"soyo", 216},
        string_with_idx{"soyombo", 216},
        string_with_idx{"space", 42},
        string_with_idx{"space_separator", 85},
        string_with_idx{"spacing_mark", 62},
        string_with_idx{"sterm", 37},
        string_with_idx{"sund", 217},
        string_with_idx{"sundanese", 217},
        string_with_idx{"surrogate", 53},
        string_with_idx{"sylo", 218},
        string_with_idx{"syloti_nagri", 218},
        string_with_idx{"symbol", 77},
        string_with_idx{"syrc", 219},
        string_with_idx{"syriac", 219},
        string_with_idx{"tagalog", 229},
        string_with_idx{"tagb", 220},
        string_with_idx{"tagbanwa", 220},
        string_with_idx{"tai_le", 222},
        string_with_idx{"tai_tham", 153},
        string_with_idx{"tai_viet", 226},
        string_with_idx{"takr", 221},
        string_with_idx{"takri", 221},
        string_with_idx{"tale", 222},
        string_with_idx{"talu", 223},
        string_with_idx{"tamil", 224},
        string_with_idx{"taml", 224},
        string_with_idx{"tang", 225},
        string_with_idx{"tangsa", 234},
        string_with_idx{"tangut", 225},
        string_with_idx{"tavt", 226},
        string_with_idx{"telu", 227},
        string_with_idx{"telugu", 227},
        string_with_idx{"term", 38},
        string_with_idx{"terminal_punctuation", 38},
        string_with_idx{"tfng", 228},
        string_with_idx{"tglg", 229},
        string_with_idx{"thaa", 230},
        string_with_idx{"thaana", 230},
        string_with_idx{"thai", 231},
        string_with_idx{"tibetan", 232},
        string_with_idx{"tibt", 232},
        string_with_idx{"tifinagh", 228},
        string_with_idx{"tirh", 233},
        string_with_idx{"tirhuta", 233},
        string_with_idx{"titlecase_letter", 59},
        string_with_idx{"tnsa", 234},
        string_with_idx{"toto", 235},
        string_with_idx{"ugar", 236},
        string_with_idx{"ugaritic", 236},
        string_with_idx{"uideo", 39},
        string_with_idx{"unassigned", 51},
        string_with_idx{"unified_ideograph", 39},
        string_with_idx{"unknown", 248},
        string_with_idx{"upper", 40},
        string_with_idx{"uppercase", 40},
        string_with_idx{"uppercase_letter", 60},
        string_with_idx{"vai", 237},
        string_with_idx{"vaii", 237},
        string_with_idx{"variation_selector", 41},
        string_with_idx{"vith", 238},
        string_with_idx{"vithkuqi", 238},
        string_with_idx{"vs", 41},
        string_with_idx{"wancho", 240},
        string_with_idx{"wara", 239},
        string_with_idx{"warang_citi", 239},
        string_with_idx{"wcho", 240},
        string_with_idx{"white_space", 42},
        string_with_idx{"wspace", 42},
        string_with_idx{"xid_continue", 43},
        string_with_idx{"xid_start", 44},
        string_with_idx{"xidc", 43},
        string_with_idx{"xids", 44},
        string_with_idx{"xpeo", 241},
        string_with_idx{"xsux", 242},
        string_with_idx{"yezi", 243},
        string_with_idx{"yezidi", 243},
        string_with_idx{"yi", 244},
        string_with_idx{"yiii", 244},
        string_with_idx{"z", 82},
        string_with_idx{"zanabazar_square", 245},
        string_with_idx{"zanb", 245},
        string_with_idx{"zinh", 246},
        string_with_idx{"zl", 83},
        string_with_idx{"zp", 84},
        string_with_idx{"zs", 85},
        string_with_idx{"zyyy", 247},
        string_with_idx{"zzzz", 248}};
}
}    // namespace uni::detail

// More regex support for ctre

namespace uni::detail {

constexpr binary_prop binary_prop_from_string(std::string_view s) {
    for(const auto& c : tables::binary_prop_names) {
        const auto res = propnamecomp(s, c.name);
        if(res == 0)
            return binary_prop(c.value);
    }
    return binary_prop::unknown;
}

template<>
constexpr bool get_binary_prop<binary_prop::ascii>(char32_t c) {
    return cp_is_ascii(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::assigned>(char32_t c) {
    return cp_is_assigned(c);
}

template<>
constexpr bool get_binary_prop<binary_prop::any>(char32_t c) {
    return cp_is_valid(c);
}

constexpr bool is_unknown(binary_prop s) {
    return s == binary_prop::unknown;
}

}    // namespace uni::detail
