#pragma once
#include "Helpers.h"

namespace Eternal
{

	template<typename ...Types>
	struct Vector
	{
	private:
		template<size_t index, typename T, size_t ...I>
		static inline auto vector_setter(std::index_sequence<I...> = std::index_sequence_for<Types...>)
		{
			return Vector<std::conditional_t<I == index, T, Types>...>;
		}

	public:
		template<size_t index>
		using get = get_type_at_t<index, Types...>;

		template<size_t index, typename T, typename = std::enable_if_t<(index < sizeof...(Types))>>
		using set = decltype(Vector::vector_setter<index, T>());

		template<typename T>
		using push_back = typename Vector<Types..., T>;

		template<template <typename> typename Fn, typename ...Args>
		void for_each(Args&&... args) {
			Fn<int>()(args...);
		}
	};
}
