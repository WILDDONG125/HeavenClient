/////////////////////////////////////////////////////////////////////////////
// This file is part of the Journey MMORPG client                           //
// Copyright � 2015 Daniel Allendorf                                        //
//                                                                          //
// This program is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU Affero General Public License as           //
// published by the Free Software Foundation, either version 3 of the       //
// License, or (at your option) any later version.                          //
//                                                                          //
// This program is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of           //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
// GNU Affero General Public License for more details.                      //
//                                                                          //
// You should have received a copy of the GNU Affero General Public License //
// along with this program.  If not, see <http://www.gnu.org/licenses/>.    //
//////////////////////////////////////////////////////////////////////////////
#pragma once
#include <functional>
#include <vector>
#include <map>

using std::vector;
using std::map;
using std::function;

template <typename T>
class Optional
{
public:
	Optional(const T* p)
	{
		ptr = p;
	}

	Optional(const T& p)
	{
		ptr = &p;
	}

	Optional()
	{
		ptr = nullptr;
	}

	explicit operator bool() const
	{
		return ispresent();
	}

	bool ispresent() const
	{
		return ptr != nullptr;
	}

	bool isempty() const
	{
		return ptr == nullptr;
	}

	template<typename T, typename ...Args>
	void ifpresent(void (T::*action)(Args...) const, Args... args) const
	{
		if (ptr)
			(ptr->*action)(std::forward<Args>(args)...);
	}

	template<typename T, typename R>
	R mapordefault(R (T::*mapper)() const, R def) const
	{
		if (ptr)
			return (ptr->*mapper)();
		else
			return def;
	}

	template<typename T, typename R, typename ...Args>
	R map(R (T::*mapper)(Args...) const, Args... args) const
	{
		if (ptr)
			return (ptr->*mapper)(std::forward<Args>(args)...);
		else
			return R();
	}

	template<typename T, typename ...Args>
	bool maporfalse(bool (T::*mapper)(Args...) const, Args... args) const
	{
		if (ptr)
			return (ptr->*mapper)(std::forward<Args>(args)...);
		else
			return false;
	}

	template<typename E, typename R>
	Optional<R> transform(E& ext, Optional<R> (E::*mapper)(const T&)) const
	{
		if (ptr)
			return (ext.*mapper)(*ptr);
		else
			return nullptr;
	}

	template<typename T, typename R, typename ...Args>
	Optional<R> transform(const R& (T::*mapper)(Args...) const, Args &&... args) const
	{
		if (ptr)
			return &(ptr->*mapper)(std::forward<Args>(args)...);
		else
			return nullptr;
	}

	template<typename T, typename E, typename V, typename R>
	Optional<R> transform(E& ext, const R& (E::*mapper)(V), V (T::*mapper2)() const) const
	{
		if (ptr)
			return (ext.*mapper)((ptr->*mapper2)());
		else
			return nullptr;
	}

	template<typename T, typename E, typename V, typename R>
	Optional<R> transform(E& ext, Optional<R> (E::*mapper)(V), V (T::*mapper2)() const) const
	{
		if (ptr)
			return (ext.*mapper)((ptr->*mapper2)());
		else
			return nullptr;
	}

	template<typename R>
	Optional<R> askey(const std::map<T, R>& container) const
	{
		if (ptr)
		{
			const T& key = *ptr;
			if (container.count(key))
			{
				return container.at(key);
			}
		}
		return nullptr;
	}

	template<typename R>
	Optional<R> reinterpret() const
	{
		if (ptr)
			return reinterpret_cast<const R*>(ptr);
		else
			return nullptr;
	}

	T getordefault(T def) const
	{
		if (ptr)
			return *ptr;
		else
			return def;
	}

	const T& operator *() const
	{
		return *ptr;
	}

	const T* get() const
	{
		return ptr;
	}

	const T* operator ->() const
	{
		return ptr;
	}

	template <typename T, typename V>
	static Optional<T> comparevalues(const std::map<T, V>& container, function<bool(const V&, const V&)> predicate)
	{
		Optional<T> result;
		V current;
		for (auto& it : container)
		{
			if (!result || predicate(it.second, current))
			{
				result = &it.first;
				current = it.second;
			}
		}
		return result;
	}

	template <typename T, typename V>
	static Optional<T> minvalue(const std::map<T, V>& container)
	{
		return Optional<T>::comparevalues<T, V>(container, [](const V& v1, const V& v2) {
			return v1 < v2;
		});
	}

	template <typename T, typename V>
	static Optional<T> maxvalue(const std::map<T, V>& container)
	{
		return Optional<T>::comparevalues<T, V>(container, [](const V& v1, const V& v2) {
			return v1 > v2;
		});
	}

private:
	const T* ptr;
};