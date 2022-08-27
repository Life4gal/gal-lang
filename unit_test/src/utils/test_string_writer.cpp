#include <gtest/gtest.h>

#include <gsl/utils/string_writer.hpp>
#include <gsl/boost/format.hpp>

#include <gsl/core/runtime_info.hpp>
#include <gsl/type/number_range.hpp>

#include <iostream>
#include <ranges>

using namespace gal::gsl::utils;
using namespace gal::gsl::boost;

TEST(TestStringWriter, TestFixedStringWriter)
{
	fixed_string_writer fsw{};

	ASSERT_EQ(fsw.view(), "");
	ASSERT_EQ(fsw.size(), 0);

	// -1 ==> '\0'
	constexpr auto func_size = sizeof(__func__) - 1;
	fsw.append(__func__);
	ASSERT_EQ(fsw.view(), __func__);
	ASSERT_EQ(fsw.size(), func_size);

	constexpr char test_string[] = "TestFixedStringWriter";
	// -1 ==> '\0'
	constexpr auto test_string_size = sizeof(test_string) - (sizeof("Writer") - 1) - 1;
	fsw.append(test_string, test_string_size);
	ASSERT_EQ(fsw.view() | std::views::drop(func_size) | std::views::take(test_string_size), "TestFixedString");
	ASSERT_EQ(fsw.size(), func_size + test_string_size);

	fsw.push_back('a');
	ASSERT_EQ(fsw.view() | std::views::drop(func_size + test_string_size), "a");
	ASSERT_EQ(fsw.size(), func_size + test_string_size + 1);
}

TEST(TestStringWriter, TestFixedStringWriterBackInsert)
{
	fixed_string_writer fsw{};

	ASSERT_EQ(fsw.view(), "");
	ASSERT_EQ(fsw.size(), 0);

	format::format_to(
			back_insert(fsw),
			"This string is for testing {}.",
			__func__);

	ASSERT_EQ(fsw.view(), format::format("This string is for testing {}.", __func__));
	// -3 ==> '{' + '}' + '\0'
	// -1 ==> '\0'
	ASSERT_EQ(fsw.size(), sizeof("This string is for testing {}.") - 3 + sizeof(__func__) - 1);
}

TEST(TestStringWriter, TestTextStringWriter)
{
	text_string_writer tsw{};

	ASSERT_EQ(tsw.view(), "");
	ASSERT_EQ(tsw.size(), 0);

	// -1 ==> '\0'
	constexpr auto func_size = sizeof(__func__) - 1;
	tsw.append(__func__);
	ASSERT_EQ(tsw.view(), __func__);
	ASSERT_EQ(tsw.size(), func_size);

	constexpr char test_string[]	= "TestTextStringWriter";
	// -1 ==> '\0'
	constexpr auto test_string_size = sizeof(test_string) - (sizeof("Writer") - 1) - 1;
	tsw.append(test_string, test_string_size);
	ASSERT_EQ(tsw.view() | std::views::drop(func_size) | std::views::take(test_string_size), "TestTextString");
	ASSERT_EQ(tsw.size(), func_size + test_string_size);

	tsw.push_back('a');
	ASSERT_EQ(tsw.view() | std::views::drop(func_size + test_string_size), "a");
	ASSERT_EQ(tsw.size(), func_size + test_string_size + 1);
}

TEST(TestStringWriter, TestTextStringWriterBackInsert)
{
	text_string_writer tsw{};

	ASSERT_EQ(tsw.view(), "");
	ASSERT_EQ(tsw.size(), 0);

	format::format_to(
			back_insert(tsw),
			"This string is for testing {}.",
			__func__);

	ASSERT_EQ(tsw.view(), format::format("This string is for testing {}.", __func__));
	// -3 ==> '{' + '}' + '\0'
	// -1 ==> '\0'
	ASSERT_EQ(tsw.size(), sizeof("This string is for testing {}.") - 3 + sizeof(__func__) - 1);
}
