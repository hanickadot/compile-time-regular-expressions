module;

#ifdef _MSVC_LANG
#pragma warning( disable : 5202 )
#endif

import std;

export module ctre;

#define CTRE_IN_A_MODULE
#define CTLL_IN_A_MODULE
#define UNICODE_DB_IN_A_MODULE

using std::int16_t;
using std::int32_t;
using std::int64_t;
using std::int8_t;
using std::size_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::uint8_t;

#include "ctre.hpp"
#include "unicode-db.hpp"
