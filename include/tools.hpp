#pragma once

#include <memory>
#include <type_traits>
#include <tuple>
#include <exception>
#include <string>
#include <random>

#define toolsTypeName(Val) std::string{typeid(Val).name()}
#define throwInvalidMemento(memento) \
	throw std::runtime_error{std::string{FUNC_SIGN} + std::string{" | Received invalid memento type: "} + toolsTypeName(memento.get())};
#define TODO(msg) \
    throw std::runtime_error{std::string{FUNC_SIGN} + " " + msg};

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


inline int generate_random(int beg, int end = std::numeric_limits<int>::max())
{
	std::random_device dev;
	std::mt19937 eng{dev()};
	std::uniform_int_distribution dstr{beg, end};
	return dstr(dev);
}

//TODO fix it
template<size_t Ind = 0, typename... TItems, typename Func>
typename std::enable_if<Ind == sizeof...(TItems), void>::type tuple_for_each(std::tuple<TItems...>& items, Func func)
{

}

template<size_t Ind = 0, typename... TItems, typename Func>
typename std::enable_if<Ind < sizeof...(TItems), void>::type tuple_for_each(std::tuple<TItems...>& items, Func func)
{
	func(std::get<Ind>(items));
	tuple_for_each<Ind + 1, TItems..., Func>(items, func);
}

//TODO fix it
template<size_t Ind = 0, typename... TItems, typename Func>
typename std::enable_if<Ind == sizeof...(TItems), void>::type tuple_for_each(std::tuple<TItems...> const& items, Func func)
{

}

template<size_t Ind = 0, typename... TItems, typename Func>
typename std::enable_if < Ind < sizeof...(TItems), void>::type tuple_for_each(std::tuple<TItems...> const& items, Func func)
{
	func(std::get<Ind>(items));
	tuple_for_each<Ind + 1, TItems..., Func>(items, func);
}

}
