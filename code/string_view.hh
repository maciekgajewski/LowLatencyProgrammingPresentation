#pragma once

#if __has_include(<experimental/string_view>)
#include <experimental/string_view>
using std::experimental::string_view;
#else
#include <string_view>
using std::string_view;
#endif
