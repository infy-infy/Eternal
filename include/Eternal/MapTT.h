#pragma once
#include "Helpers.h"

namespace Eternal
{

	template<typename ...Types>
	struct MapTT
	{
		enum Kind
		{
			KEY,
			VALUE
		};

		template<Kind kind, typename Elem>
		constexpr static inline int getIndex() noexcept
		{
			int counter = 0, result = -1;
			if constexpr (kind == Kind::KEY) {
				(..., (result = (std::is_same_v<Elem, Storage<>::get<0, Types>> && result == -1)
								? counter 
								: result,
						++counter));
			} else {
				(..., (result = (std::is_same_v<Elem, Storage<>::get<1, Types>> && result == -1)
					? counter
					: result,
					++counter));
			}
			return result;
		}

		////////////////////////////////////////////////

		template<typename Key>
		using getValue = Storage<>::get<0, get_type_at_t<getIndex<Kind::KEY, Key>(), Types...>>;

		template<typename Value>
		using getKey = Storage<>::get<0, get_type_at_t<getIndex<Kind::VALUE, Value>(), Types...>>;

		////////////////////////////////////////////////

		template<int index>
		using getKeyByIndex = Storage<>::get<0, get_type_at_t<index, Types...>>;

		template<int index>
		using getValueByIndex = Storage<>::get<1, get_type_at_t<index, Types...>>;

		template<int index>
		using getKeyValueByIndex = get_type_at_t<index, Types...>;

		template<typename Key>
		using getKeyValueByKey = get_type_at_t<getIndex<Kind::KEY, Key>(), Types...>;

		template<typename Value>
		using getKeyValueByValue = get_type_at_t<getIndex<Kind::VALUE, Value>(), Types...>;

		////////////////////////////////////////////////

		using getAllKeys = run_with_index_sequence_t<sizeof...(Types), getKeyByIndex>;

		using getAllValues = run_with_index_sequence_t<sizeof...(Types), getValueByIndex>;

		////////////////////////////////////////////////

		template<typename Key>
		constexpr static inline bool hasKey() noexcept {
			int counter = -1;
			return ( ... || ( (++counter), (std::is_same_v<Key, Storage<>::get<0, Types>>) ) );
		}

		template<typename Value>
		constexpr static inline bool hasValue() noexcept {
			int counter = -1;
			return ( ... || ( (++counter), (std::is_same_v<Value, Storage<>::get<1, Types>>) ) );
		}

		////////////////////////////////////////////////

		template<typename Key, typename Value>
		using add = std::enable_if_t<!MapTT::hasKey<Key>(), MapTT<Types..., Storage<Key, Value>> >;
	};
}
