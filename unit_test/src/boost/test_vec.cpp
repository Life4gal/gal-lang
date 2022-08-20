#include <gtest/gtest.h>

#include <eve/wide.hpp>
#include <eve/module/core.hpp>
#include <eve/module/math.hpp>
#include <gsl/boost/vec.hpp>

using namespace gal::gsl::boost;

TEST(TestVec, TestVec4f)
{
	constexpr vec4f v1{};

	static_assert(r(v1) == 0);
	static_assert(g(v1) == 0);
	static_assert(b(v1) == 0);
	static_assert(a(v1) == 0);

	ASSERT_FLOAT_EQ(r(v1), 0);
	ASSERT_FLOAT_EQ(g(v1), 0);
	ASSERT_FLOAT_EQ(b(v1), 0);
	ASSERT_FLOAT_EQ(a(v1), 0);

	constexpr vec4f v2{1};

	static_assert(r(v2) == 1);
	static_assert(g(v2) == 0);
	static_assert(b(v2) == 0);
	static_assert(a(v2) == 0);

	ASSERT_FLOAT_EQ(r(v2), 1);
	ASSERT_FLOAT_EQ(g(v2), 0);
	ASSERT_FLOAT_EQ(b(v2), 0);
	ASSERT_FLOAT_EQ(a(v2), 0);

	constexpr vec4f v3{1, vec_fill_tag::duplicate};

	static_assert(r(v3) == 1);
	static_assert(g(v3) == 1);
	static_assert(b(v3) == 1);
	static_assert(a(v3) == 1);

	ASSERT_FLOAT_EQ(r(v3), 1);
	ASSERT_FLOAT_EQ(g(v3), 1);
	ASSERT_FLOAT_EQ(b(v3), 1);
	ASSERT_FLOAT_EQ(a(v3), 1);

	constexpr vec4f v4{1, 2, 3, 4};

	static_assert(r(v4) == 1);
	static_assert(g(v4) == 2);
	static_assert(b(v4) == 3);
	static_assert(a(v4) == 4);

	ASSERT_FLOAT_EQ(r(v4), 1);
	ASSERT_FLOAT_EQ(g(v4), 2);
	ASSERT_FLOAT_EQ(b(v4), 3);
	ASSERT_FLOAT_EQ(a(v4), 4);

	constexpr vec3f v5{v4};

	static_assert(r(v5) == 1);
	static_assert(g(v5) == 2);
	static_assert(b(v5) == 3);

	ASSERT_FLOAT_EQ(r(v5), 1);
	ASSERT_FLOAT_EQ(g(v5), 2);
	ASSERT_FLOAT_EQ(b(v5), 3);
}

TEST(TestVec, TestVec3f)
{
	constexpr vec3f v1{};

	static_assert(r(v1) == 0);
	static_assert(g(v1) == 0);
	static_assert(b(v1) == 0);

	ASSERT_FLOAT_EQ(r(v1), 0);
	ASSERT_FLOAT_EQ(g(v1), 0);
	ASSERT_FLOAT_EQ(b(v1), 0);

	constexpr vec3f v2{1};

	static_assert(r(v2) == 1);
	static_assert(g(v2) == 0);
	static_assert(b(v2) == 0);

	ASSERT_FLOAT_EQ(r(v2), 1);
	ASSERT_FLOAT_EQ(g(v2), 0);
	ASSERT_FLOAT_EQ(b(v2), 0);

	constexpr vec3f v3{1, vec_fill_tag::duplicate};

	static_assert(r(v3) == 1);
	static_assert(g(v3) == 1);
	static_assert(b(v3) == 1);

	ASSERT_FLOAT_EQ(r(v3), 1);
	ASSERT_FLOAT_EQ(g(v3), 1);
	ASSERT_FLOAT_EQ(b(v3), 1);

	constexpr vec3f v4{1, 2, 3};

	static_assert(r(v4) == 1);
	static_assert(g(v4) == 2);
	static_assert(b(v4) == 3);

	ASSERT_FLOAT_EQ(r(v4), 1);
	ASSERT_FLOAT_EQ(g(v4), 2);
	ASSERT_FLOAT_EQ(b(v4), 3);

	constexpr vec4f v5{v4};

	static_assert(r(v5) == 1);
	static_assert(g(v5) == 2);
	static_assert(b(v5) == 3);
	static_assert(a(v5) == 0);

	ASSERT_FLOAT_EQ(r(v5), 1);
	ASSERT_FLOAT_EQ(g(v5), 2);
	ASSERT_FLOAT_EQ(b(v5), 3);
	ASSERT_FLOAT_EQ(a(v5), 0);
}

TEST(TestVec, TestVec4i)
{
	constexpr vec4i v1{};

	static_assert(i1(v1) == 0);
	static_assert(i2(v1) == 0);
	static_assert(i3(v1) == 0);
	static_assert(i4(v1) == 0);

	ASSERT_EQ(i1(v1), 0);
	ASSERT_EQ(i2(v1), 0);
	ASSERT_EQ(i3(v1), 0);
	ASSERT_EQ(i4(v1), 0);

	constexpr vec4i v2{1};

	static_assert(i1(v2) == 1);
	static_assert(i2(v2) == 0);
	static_assert(i3(v2) == 0);
	static_assert(i4(v2) == 0);

	ASSERT_EQ(i1(v2), 1);
	ASSERT_EQ(i2(v2), 0);
	ASSERT_EQ(i3(v2), 0);
	ASSERT_EQ(i4(v2), 0);

	constexpr vec4i v3{1, vec_fill_tag::duplicate};

	static_assert(i1(v3) == 1);
	static_assert(i2(v3) == 1);
	static_assert(i3(v3) == 1);
	static_assert(i4(v3) == 1);

	ASSERT_EQ(i1(v3), 1);
	ASSERT_EQ(i2(v3), 1);
	ASSERT_EQ(i3(v3), 1);
	ASSERT_EQ(i4(v3), 1);

	constexpr vec4i v4{1, 2, 3, 4};

	static_assert(i1(v4) == 1);
	static_assert(i2(v4) == 2);
	static_assert(i3(v4) == 3);
	static_assert(i4(v4) == 4);

	ASSERT_EQ(i1(v4), 1);
	ASSERT_EQ(i2(v4), 2);
	ASSERT_EQ(i3(v4), 3);
	ASSERT_EQ(i4(v4), 4);
}
