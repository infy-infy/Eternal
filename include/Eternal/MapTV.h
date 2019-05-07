#pragma once
#include "MapTT.h"
#include <memory>
#include <functional>

namespace Eternal
{
	namespace
	{
		template<typename T>
		class TypeValue_Value
		{
		protected:
			Storage<>::get<1, T> value;
		public:
			constexpr TypeValue_Value() = default;

			template<typename U>
			constexpr TypeValue_Value(U&& val) : value(std::forward<U>(val)) {}
		};
	}
	

	template<typename Map = MapTT<>, typename ...Types>
	class MapTV : public TypeValue_Value<Types>...
	{
	public:

		constexpr MapTV() = default;

		template<typename ...Ts, typename = std::enable_if_t<sizeof...(Ts) != 0>, typename = exclude_class_default_t<MapTV, Ts...>>
		constexpr MapTV(Ts&&... args)
			: TypeValue_Value<Types>(std::forward<Ts>(args))...
		{ }

		template<typename Key>
		constexpr auto& get() {
			return TypeValue_Value<typename Map:: template getKeyValueByKey<Key>>::value;
		}

		template<typename Key>
		constexpr const auto& get() const {
			return TypeValue_Value<typename Map:: template getKeyValueByKey<Key>>::value;
		}

		template<typename Key, typename Value, typename Arg, typename = std::enable_if_t<!std::is_same_v<Value, Arg>>>
		constexpr auto add(Arg&& val) {
			using decayed = std::decay_t<Value>;
			static_assert(!Map:: template hasKey<Key>(), "Key is already in map");
			return MapTV<typename Map:: template add<Key, decayed>, Types..., Storage<Key, decayed>>(std::move(TypeValue_Value<Types>::value)..., std::forward<Arg>(val));
		}

		template<typename Key, typename Value>
		constexpr auto add(Value&& val) {
			using decayed = std::decay_t<Value>;
			static_assert(!Map:: template hasKey<Key>(), "Key is already in map");
			return MapTV<typename Map:: template add<Key, decayed>, Types..., Storage<Key, decayed>>(std::move(TypeValue_Value<Types>::value)..., std::forward<Value>(val));
		}

		template<typename Key, typename Value>
		constexpr auto add() {
			using decayed = std::decay_t<Value>;
			static_assert(!Map:: template hasKey<Key>(), "Key is already in map");
			return MapTV<typename Map:: template add<Key, decayed>, Types..., Storage<Key, decayed>>(std::move(TypeValue_Value<Types>::value)..., decayed());
		}

		////////////////////////////////////////////////////

		template<typename Key>
		constexpr bool hasKey() {
			return Map:: template hasKey<Key>();
		}

	};
}

