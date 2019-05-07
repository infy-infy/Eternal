#pragma once
#include <type_traits>
#include <utility>

namespace Eternal {
	//////////////////////////////////////////////

	template<int N, typename T, typename ...Types>
	struct get_type_at {
		using type = typename get_type_at<N - 1, Types...>::type;
	};

	template<typename T, typename ...Types>
	struct get_type_at<0, T, Types...> {
		using type = T;
	};

	template<int N, typename ...Types>
	struct get_type_at_proxy {
		using type = typename get_type_at<N, Types...>::type;
	};

	template<int N>
	struct get_type_at_proxy<N> {
		using type = void;
	};


	template<int N, typename ...Types>
	using get_type_at_t = typename get_type_at_proxy<N, Types...>::type;

	/////////////////////////////////////////////////////

	template<typename T, typename ...Types>
	inline constexpr bool match_any_v = std::disjunction_v<std::is_same<Types, T>...>;

	template<typename T>
	using remove_cv_ref_t = std::remove_reference_t<std::remove_cv_t<T>>;

	namespace {
		template<typename ClassType, typename ...ArgTypes>
		using _exclude_class_default_t = typename std::enable_if_t<
			(sizeof...(ArgTypes) > 1) ||
			(sizeof...(ArgTypes) == 1 &&
			!std::is_same_v<ClassType, get_type_at_t<0, ArgTypes...>> /*&&
			!std::is_convertible_v<get_type_at_t<0, ArgTypes...>, ClassType>*/)
			//too complex for MSVC, ha.
		>;
	}

	template<typename ClassType, typename ...ArgTypes>
	using exclude_class_default_t = _exclude_class_default_t<remove_cv_ref_t<ClassType>, remove_cv_ref_t<ArgTypes>...>;

	/////////////////////////////////////////////////////

	namespace
	{
		template<int index, template<typename...> typename Arg, typename ...Types>
		constexpr inline get_type_at_t<index, Types...> _get_type_in_template_pack_at(Arg<Types...> arg) noexcept {
			return get_type_at_t<index, Types...>();
		}
	}
	
	

	template<typename ...Types>
	struct Storage {
		constexpr static inline int size = sizeof...(Types);
	};

	template<>
	struct Storage<> {
		constexpr static inline int size = 0;

		template<int index, typename T>
		using get = decltype(_get_type_in_template_pack_at<index>(std::declval<T>()));

	};

	////////////////////////////////////////////////////

	namespace {
		template<int lastIndex, template <int> typename MetaFunc, typename...Tail>
		struct _run_with_index_sequence {
			using type = typename _run_with_index_sequence<lastIndex - 1, MetaFunc, MetaFunc<lastIndex - 1>, Tail...>::type;
		};

		template<template <int> typename MetaFunc, typename...Tail>
		struct _run_with_index_sequence<0, MetaFunc, Tail...> {
			using type = Storage<Tail...>;
		};
	}
	

	template<int lastIndex, template <int> typename MetaFunc>
	struct run_with_index_sequence {
		using type = typename _run_with_index_sequence<lastIndex, MetaFunc>::type;
	};

	template<int lastIndex, template <int> typename MetaFunc>
	using run_with_index_sequence_t = typename run_with_index_sequence<lastIndex, MetaFunc>::type;

	/////////////////////////////////////////////////////
	//https://en.cppreference.com/w/cpp/experimental/is_detected
	
	struct nonesuch {
		~nonesuch() = delete;
		nonesuch(nonesuch const&) = delete;
		void operator=(nonesuch const&) = delete;
	};

	namespace detail {
		template <class Default, class AlwaysVoid, template<class...> class Op, class... Args>
		struct detector {
			using value_t = std::false_type;
			using type = Default;
		};

		template <class Default, template<class...> class Op, class... Args>
		struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
			using value_t = std::true_type;
			using type = Op<Args...>;
		};

	}

	template <template<class...> class Op, class... Args>
	constexpr inline bool is_detected_v = detail::detector<nonesuch, void, Op, Args...>::value_t::value;

	template <template<class...> class Op, class... Args>
	using detected_t = typename detail::detector<nonesuch, void, Op, Args...>::type;

	template <class Default, template<class...> class Op, class... Args>
	using detected_or = detail::detector<Default, void, Op, Args...>;
}