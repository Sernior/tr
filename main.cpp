#include "./detail/ebo.h"

#include "folder.h"

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <tuple>
#include <utility>

// TODO:
// tr::tuple
// tr::compressed_tuple
// tr::span (with unpack capabilities)
// try and add a tag type to ebo and see if compressing capabilities increase.

namespace
{
	template <typename...>
	void foo();

	struct TestEbo
	{
		void value_categories()
		{
			using tr::detail::ebo;

			struct tag_t {};
			struct S {};
			ebo<S, tag_t, true> ec{};
			ebo<S, tag_t, false> e{};

			static_assert(
				std::is_same_v<decltype(get_ebo_val(ec)), decltype(get_ebo_val(e))>);

			static_assert(std::is_same_v<decltype(get_ebo_val(std::as_const(ec))),
				decltype(get_ebo_val(std::as_const(e)))>);

			static_assert(std::is_same_v<decltype(get_ebo_val(std::move(ec))),
				decltype(get_ebo_val(std::move(e)))>);

			static_assert(
				std::is_same_v<decltype(get_ebo_val(std::move(std::as_const(ec)))),
				decltype(get_ebo_val(std::move(std::as_const(e))))>);
		}

		void traits()
		{
			using tr::detail::ebo;
			using tr::detail::ebo_traits;

			struct tag_t {};
			struct S {};

			struct S_final final {};
			using func_ptr_t = void (*)();
			static_assert(ebo_traits<ebo<S, tag_t>>::is_compressed);
			static_assert(!ebo_traits<ebo<S_final, tag_t>>::is_compressed);
			static_assert(!ebo_traits<ebo<S&, tag_t>>::is_compressed);
			static_assert(!ebo_traits<ebo<int, tag_t>>::is_compressed);
			static_assert(!ebo_traits<ebo<func_ptr_t, tag_t>>::is_compressed);
		}
	};

    // Check that class tr::detail::ebo behaves like I expect. If this function
    // compiles, the tests pass.
	void test_ebo()
	{
		using tr::detail::ebo;
		using tr::detail::ebo_traits;

        struct tag_t {};

		struct S {};
		ebo<S, tag_t, true> ec{};
		ebo<S, tag_t, false> e{};

		static_assert(
			std::is_same_v<decltype(get_ebo_val(ec)), decltype(get_ebo_val(e))>);

		static_assert(std::is_same_v<decltype(get_ebo_val(std::as_const(ec))),
			decltype(get_ebo_val(std::as_const(e)))>);

		static_assert(std::is_same_v<decltype(get_ebo_val(std::move(ec))),
			decltype(get_ebo_val(std::move(e)))>);

		static_assert(
			std::is_same_v<decltype(get_ebo_val(std::move(std::as_const(ec)))),
			decltype(get_ebo_val(std::move(std::as_const(e))))>);

		struct S_final final {};
		using func_ptr_t = void (*)();
		static_assert(ebo_traits<ebo<S, tag_t>>::is_compressed);
		static_assert(!ebo_traits<ebo<S_final, tag_t>>::is_compressed);
		static_assert(!ebo_traits<ebo<S&, tag_t>>::is_compressed);
		static_assert(!ebo_traits<ebo<int, tag_t>>::is_compressed);
		static_assert(!ebo_traits<ebo<func_ptr_t, tag_t>>::is_compressed);
	}

    // Check that class tr::combinator behaves like I expect.
	struct TestCombinator
	{
		void apply()
		{
			using tr::combinator;
			constexpr std::tuple vals{ 0.1, 1, 2, 3, 4 };

			{
				constexpr combinator minComb{ [](auto i, auto j) { return i < j ? i : j; } };
				static_assert(sizeof(minComb) == 1);
				static_assert(std::is_aggregate_v<decltype(minComb)>);

				constexpr auto minVal = std::apply(
					[=](auto ... args) { return (minComb | ... | args)(); },
					vals);

				static_assert(minVal == std::get<0>(vals));
			}

			{
				constexpr combinator maxComb{ [](auto i, auto j) { return i > j ? i : j; }, 5 };
				static_assert(sizeof(maxComb) == sizeof(int));
				static_assert(std::is_aggregate_v<decltype(maxComb)>);

				constexpr auto max = std::apply(
					[=](auto ... args) { return (maxComb | ... | args)(); },
					vals);

				constexpr auto init = maxComb();
				static_assert(max == init);
			}
		}

		void value_categories()
		{
			using tr::combinator;

			{
				combinator comb{ [](int, int) {} };
				static_assert(
					std::is_rvalue_reference_v<decltype(std::move(comb).get_operator())> &&
					!std::is_const_v<std::remove_reference_t<decltype(std::move(comb).get_operator())>>);

				static_assert(
					std::is_rvalue_reference_v<decltype(std::move(std::as_const(comb)).get_operator())> &&
					std::is_const_v<std::remove_reference_t<decltype(std::move(std::as_const(comb)).get_operator())>>);

				static_assert(
					std::is_lvalue_reference_v<decltype(comb.get_operator())> &&
					!std::is_const_v<std::remove_reference_t<decltype(comb.get_operator())>>);

				static_assert(
					std::is_lvalue_reference_v<decltype(std::as_const(comb).get_operator())> &&
					std::is_const_v<std::remove_reference_t<decltype(std::as_const(comb).get_operator())>>);
			}
		}
	};
}

int main()
{
}
