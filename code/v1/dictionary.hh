#pragma once

#include "../string_view.hh"

#include <unordered_set>
#include <string>

namespace v1 {

class Dictionary
{
public:
	template<typename Collection>
	explicit Dictionary(const Collection& words) :
		_container(words.cbegin(), words.cend())
	{}

	bool in_dictionary(string_view word) const
	{
		return _container.find({word.data(), word.size()}) != _container.end();
	}

private:
	std::unordered_set<std::string> _container;
};

}
