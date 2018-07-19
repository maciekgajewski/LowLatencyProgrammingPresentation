//
// Copyright (c) 2017 David Gross
// full version: https://github.com/david-grs/mtrace
// 

#pragma once

#include <utility>

extern "C"
{
#include <malloc.h>
}

namespace
{
	using malloc_hook = void*(*)(size_t, const void*);
	using free_hook = void(*)(void*, const void*);
	using realloc_hook = void*(*)(void*, size_t, const void*);
}

struct malloc_counter
{
	std::size_t malloc_calls() const { return _malloc_calls; }
	std::size_t free_calls() const { return _free_calls; }
	std::size_t realloc_calls() const { return _realloc_calls; }
	std::size_t malloc_bytes() const { return _malloc_bytes; }
	std::size_t realloc_bytes() const { return _realloc_bytes; }

	void post_malloc(size_t size, const void*)
	{
		++_malloc_calls;
		_malloc_bytes += size;
	}

	void post_free(const void* mem)
	{
		++_free_calls;
	}

	void post_realloc(const void*, size_t size, const void*)
	{
		++_realloc_calls;
		_realloc_bytes += size;
	}

private:
	std::size_t _malloc_calls = {};
	std::size_t _free_calls = {};
	std::size_t _realloc_calls = {};
	std::size_t _malloc_bytes = {};
	std::size_t _realloc_bytes = {};
};

struct mtrace
{
	mtrace()
	{
		_counters = {};

		save_hooks();
		load_custom_hooks();
	}

	~mtrace()
	{
		restore_hooks();
	}

	static const malloc_counter& counters() { return _counters; }

	static void* malloc(size_t size, const void* caller)
	{
		restore_hooks();

		void* p = ::malloc(size);
		_counters.post_malloc(size, caller);

		save_hooks();
		load_custom_hooks();
		return p;
	}

	static void free(void* mem, const void* caller)
	{
		restore_hooks();

		::free(mem);
		_counters.post_free(mem);

		save_hooks();
		load_custom_hooks();
	}

	static void* realloc(void* mem, size_t size, const void*)
	{
		restore_hooks();

		void* p = ::realloc(mem, size);
		_counters.post_realloc(mem, size, p);

		save_hooks();
		load_custom_hooks();
		return p;
	}

private:
	static void load_custom_hooks()
	{
		__malloc_hook = malloc;
		__free_hook = free;
		__realloc_hook = realloc;
	}

	static void save_hooks()
	{
		_old_malloc = __malloc_hook;
		_old_free = __free_hook;
		_old_realloc = __realloc_hook;
	}

	static void restore_hooks()
	{
		__malloc_hook = _old_malloc;
		__free_hook = _old_free;
		__realloc_hook = _old_realloc;
	}

	static malloc_hook  _old_malloc;
	static free_hook    _old_free;
	static realloc_hook _old_realloc;

	static malloc_counter _counters;
};

malloc_hook mtrace::_old_malloc;
free_hook mtrace::_old_free;
realloc_hook mtrace::_old_realloc;
malloc_counter mtrace::_counters;
