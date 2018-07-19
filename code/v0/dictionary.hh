#pragma once

#include "../string_view.hh"

#include <set>
#include <string>

namespace v0 {

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
	std::set<std::string> _container;
};

}
