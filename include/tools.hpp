#pragma once

#include <memory>

#define typeName(Val) std::string{typeid(Val).name()}

#if defined(_MSC_VER)
#define FUNC_SIGN __FUNCSIG__
#elif defined(__GNUC__) || defined(__clang__)
#define FUNC_SIGN __PRETTY_FUNCTION__
#endif

namespace tools
{

template<typename To, typename From, typename Deleter>
std::unique_ptr<To, Deleter> unique_dyn_cast(std::unique_ptr<From, Deleter>&& ptr)
{
	if (auto to = dynamic_cast<To*>(ptr.get()))
	{
		auto casted = std::unique_ptr<To, Deleter>(to, ptr.get_deleter());
		ptr.release();
		return std::move(casted);
	}

	return std::unique_ptr<To>(nullptr);
}

}