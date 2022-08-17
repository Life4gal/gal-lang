#include <gtest/gtest.h>

#include <eve/wide.hpp>
#include <eve/module/core.hpp>
#include <eve/module/math.hpp>
#include <gsl/math/vec.hpp>

using namespace gal::gsl::math;

TEST(TestVec, TestVec4f)
{
	constexpr vec4f v1{};

	static_assert(v1.r() == 0);
	static_assert(v1.g() == 0);
	static_assert(v1.b() == 0);
	static_assert(v1.a() == 0);

	ASSERT_FLOAT_EQ(v1.r(), 0);
	ASSERT_FLOAT_EQ(v1.g(), 0);
	ASSERT_FLOAT_EQ(v1.b(), 0);
	ASSERT_FLOAT_EQ(v1.a(), 0);

	constexpr vec4f v2{1};

	static_assert(v2.r() == 1);
	static_assert(v2.g() == 0);
	static_assert(v2.b() == 0);
	static_assert(v2.a() == 0);

	ASSERT_FLOAT_EQ(v2.r(), 1);
	ASSERT_FLOAT_EQ(v2.g(), 0);
	ASSERT_FLOAT_EQ(v2.b(), 0);
	ASSERT_FLOAT_EQ(v2.a(), 0);

	constexpr vec4f v3{1, vec_fill_tag::duplicate};

	static_assert(v3.r() == 1);
	static_assert(v3.g() == 1);
	static_assert(v3.b() == 1);
	static_assert(v3.a() == 1);

	ASSERT_FLOAT_EQ(v3.r(), 1);
	ASSERT_FLOAT_EQ(v3.g(), 1);
	ASSERT_FLOAT_EQ(v3.b(), 1);
	ASSERT_FLOAT_EQ(v3.a(), 1);

	constexpr vec4f v4{1, 2, 3, 4};
	
	static_assert(v4.r() == 1);
	static_assert(v4.g() == 2);
	static_assert(v4.b() == 3);
	static_assert(v4.a() == 4);
	
	ASSERT_FLOAT_EQ(v4.r(), 1);
	ASSERT_FLOAT_EQ(v4.g(), 2);
	ASSERT_FLOAT_EQ(v4.b(), 3);
	ASSERT_FLOAT_EQ(v4.a(), 4);

	constexpr vec3f v5{v4};

	static_assert(v5.r() == 1);
	static_assert(v5.g() == 2);
	static_assert(v5.b() == 3);

	ASSERT_FLOAT_EQ(v5.r(), 1);
	ASSERT_FLOAT_EQ(v5.g(), 2);
	ASSERT_FLOAT_EQ(v5.b(), 3);
}

TEST(TestVec, TestVec3f)
{
	constexpr vec3f v1{};

	static_assert(v1.r() == 0);
	static_assert(v1.g() == 0);
	static_assert(v1.b() == 0);

	ASSERT_FLOAT_EQ(v1.r(), 0);
	ASSERT_FLOAT_EQ(v1.g(), 0);
	ASSERT_FLOAT_EQ(v1.b(), 0);

	constexpr vec3f v2{1};

	static_assert(v2.r() == 1);
	static_assert(v2.g() == 0);
	static_assert(v2.b() == 0);

	ASSERT_FLOAT_EQ(v2.r(), 1);
	ASSERT_FLOAT_EQ(v2.g(), 0);
	ASSERT_FLOAT_EQ(v2.b(), 0);

	constexpr vec3f v3{1, vec_fill_tag::duplicate};

	static_assert(v3.r() == 1);
	static_assert(v3.g() == 1);
	static_assert(v3.b() == 1);

	ASSERT_FLOAT_EQ(v3.r(), 1);
	ASSERT_FLOAT_EQ(v3.g(), 1);
	ASSERT_FLOAT_EQ(v3.b(), 1);

	constexpr vec3f v4{1, 2, 3};
	
	static_assert(v4.r() == 1);
	static_assert(v4.g() == 2);
	static_assert(v4.b() == 3);
	
	ASSERT_FLOAT_EQ(v4.r(), 1);
	ASSERT_FLOAT_EQ(v4.g(), 2);
	ASSERT_FLOAT_EQ(v4.b(), 3);

	constexpr vec4f v5{v4};

	static_assert(v5.r() == 1);
	static_assert(v5.g() == 2);
	static_assert(v5.b() == 3);
	static_assert(v5.a() == 0);

	ASSERT_FLOAT_EQ(v5.r(), 1);
	ASSERT_FLOAT_EQ(v5.g(), 2);
	ASSERT_FLOAT_EQ(v5.b(), 3);
	ASSERT_FLOAT_EQ(v5.a(), 0);
}

TEST(TestVec, TestVec4i)
{
	constexpr vec4i v1{};

	static_assert(v1.i1() == 0);
	static_assert(v1.i2() == 0);
	static_assert(v1.i3() == 0);
	static_assert(v1.i4() == 0);

	ASSERT_EQ(v1.i1(), 0);
	ASSERT_EQ(v1.i2(), 0);
	ASSERT_EQ(v1.i3(), 0);
	ASSERT_EQ(v1.i4(), 0);

	constexpr vec4i v2{1};

	static_assert(v2.i1() == 1);
	static_assert(v2.i2() == 0);
	static_assert(v2.i3() == 0);
	static_assert(v2.i4() == 0);

	ASSERT_EQ(v2.i1(), 1);
	ASSERT_EQ(v2.i2(), 0);
	ASSERT_EQ(v2.i3(), 0);
	ASSERT_EQ(v2.i4(), 0);

	constexpr vec4i v3{1, vec_fill_tag::duplicate};

	static_assert(v3.i1() == 1);
	static_assert(v3.i2() == 1);
	static_assert(v3.i3() == 1);
	static_assert(v3.i4() == 1);

	ASSERT_EQ(v3.i1(), 1);
	ASSERT_EQ(v3.i2(), 1);
	ASSERT_EQ(v3.i3(), 1);
	ASSERT_EQ(v3.i4(), 1);

	constexpr vec4i v4{1, 2, 3, 4};

	static_assert(v4.i1() == 1);
	static_assert(v4.i2() == 2);
	static_assert(v4.i3() == 3);
	static_assert(v4.i4() == 4);

	ASSERT_EQ(v4.i1(), 1);
	ASSERT_EQ(v4.i2(), 2);
	ASSERT_EQ(v4.i3(), 3);
	ASSERT_EQ(v4.i4(), 4);
}
