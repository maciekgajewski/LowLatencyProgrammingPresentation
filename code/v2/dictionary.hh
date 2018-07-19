#pragma once

#include "../string_view.hh"

#include <unordered_set>
#include <string>
#include <vector>

namespace v2 {

class Dictionary
{
public:
	template<typename Collection>
	explicit Dictionary(const Collection& words)
		: _data(words.begin(), words.end())
		, _index(_data.begin(), _data.end())
	{
	}

	bool in_dictionary(string_view word) const
	{
		return _index.find(word) != _index.end();
	}

private:
	std::vector<std::string> _data;
	std::unordered_set<string_view> _index;
};

}
