#include <gtest/gtest.h>
#include <gsl/core/macro.hpp>

GSL_DISABLE_WARNING_PUSH
GSL_DISABLE_WARNING(-Wclass-memaccess)

#include <eve/wide.hpp>
#include <eve/module/core.hpp>
#include <eve/module/math.hpp>

GSL_DISABLE_WARNING_POP

#include <gsl/boost/vec.hpp>
#include <gsl/boost/vec_operation.hpp>


using namespace gal::gsl::boost;

TEST(TestVecOperation, TestMakeZero)
{
	const auto v1{vec::make_zero<vec4f>()};
	ASSERT_FLOAT_EQ(v1.get(0), 0);
	ASSERT_FLOAT_EQ(v1.get(1), 0);
	ASSERT_FLOAT_EQ(v1.get(2), 0);
	ASSERT_FLOAT_EQ(v1.get(3), 0);

	const auto v2{vec::make_zero<vec4i>()};
	ASSERT_EQ(v2.get(0), 0);
	ASSERT_EQ(v2.get(1), 0);
	ASSERT_EQ(v2.get(2), 0);
	ASSERT_EQ(v2.get(3), 0);
}

TEST(TestVecOperation, TestMakeOne)
{
	const auto v1{vec::make_one<vec4f>()};
	ASSERT_FLOAT_EQ(v1.get(0), 1);
	ASSERT_FLOAT_EQ(v1.get(1), 1);
	ASSERT_FLOAT_EQ(v1.get(2), 1);
	ASSERT_FLOAT_EQ(v1.get(3), 1);

	const auto v2{vec::make_one<vec4i>()};
	ASSERT_EQ(v2.get(0), 1);
	ASSERT_EQ(v2.get(1), 1);
	ASSERT_EQ(v2.get(2), 1);
	ASSERT_EQ(v2.get(3), 1);
}

TEST(TestVecOperation, TestMakeMax)
{
	const auto v1{vec::make_max<vec4f>()};
	ASSERT_FLOAT_EQ(v1.get(0), std::numeric_limits<vec4f::value_type>::max());
	ASSERT_FLOAT_EQ(v1.get(1), std::numeric_limits<vec4f::value_type>::max());
	ASSERT_FLOAT_EQ(v1.get(2), std::numeric_limits<vec4f::value_type>::max());
	ASSERT_FLOAT_EQ(v1.get(3), std::numeric_limits<vec4f::value_type>::max());

	const auto v2{vec::make_max<vec4i>()};
	ASSERT_EQ(v2.get(0), std::numeric_limits<vec4i::value_type>::max());
	ASSERT_EQ(v2.get(1), std::numeric_limits<vec4i::value_type>::max());
	ASSERT_EQ(v2.get(2), std::numeric_limits<vec4i::value_type>::max());
	ASSERT_EQ(v2.get(3), std::numeric_limits<vec4i::value_type>::max());
}

TEST(TestVecOperation, TestMakeMin)
{
	const auto v1{vec::make_min<vec4f>()};
	ASSERT_FLOAT_EQ(v1.get(0), std::numeric_limits<vec4f::value_type>::lowest());
	ASSERT_FLOAT_EQ(v1.get(1), std::numeric_limits<vec4f::value_type>::lowest());
	ASSERT_FLOAT_EQ(v1.get(2), std::numeric_limits<vec4f::value_type>::lowest());
	ASSERT_FLOAT_EQ(v1.get(3), std::numeric_limits<vec4f::value_type>::lowest());

	const auto v2{vec::make_min<vec4i>()};
	ASSERT_EQ(v2.get(0), std::numeric_limits<vec4i::value_type>::lowest());
	ASSERT_EQ(v2.get(1), std::numeric_limits<vec4i::value_type>::lowest());
	ASSERT_EQ(v2.get(2), std::numeric_limits<vec4i::value_type>::lowest());
	ASSERT_EQ(v2.get(3), std::numeric_limits<vec4i::value_type>::lowest());
}

TEST(TestVecOperation, TestMakeSignMask)
{
	const auto v{vec::make_sign_mask<vec4i>()};
	ASSERT_EQ(v.get(0), 0x80000000);
	ASSERT_EQ(v.get(1), 0x80000000);
	ASSERT_EQ(v.get(2), 0x80000000);
	ASSERT_EQ(v.get(3), 0x80000000);
}

TEST(TestVecOperation, TestMakeSplat)
{
	constexpr vec4f::value_type float_value = 3.14f;
	constexpr vec4i::value_type int_value = 42;

	const auto v1{vec::make_splat<vec4f>(float_value)};
	ASSERT_FLOAT_EQ(v1.get(0), float_value);
	ASSERT_FLOAT_EQ(v1.get(1), float_value);
	ASSERT_FLOAT_EQ(v1.get(2), float_value);
	ASSERT_FLOAT_EQ(v1.get(3), float_value);

	const auto v2{vec::make_splat<vec4i>(int_value)};
	ASSERT_EQ(v2.get(0), int_value);
	ASSERT_EQ(v2.get(1), int_value);
	ASSERT_EQ(v2.get(2), int_value);
	ASSERT_EQ(v2.get(3), int_value);
}

TEST(TestVecOperation, TestMakeLoad)
{
	constexpr vec4f::value_type float_value[]{1.2f, 2.3f, 3.4f, 4.5f, 5.6f, 6.7f, 7.8f, 8.9f};
	constexpr vec4i::value_type int_value[]{12, 23, 34, 45, 56, 67, 78, 89};

	const auto vf1{vec::make_load<vec4f>(float_value)};
	ASSERT_FLOAT_EQ(vf1.get(0), float_value[0]);
	ASSERT_FLOAT_EQ(vf1.get(1), float_value[1]);
	ASSERT_FLOAT_EQ(vf1.get(2), float_value[2]);
	ASSERT_FLOAT_EQ(vf1.get(3), float_value[3]);

	constexpr auto vf2_offset = 2;
	const auto vf2{vec::make_load<vec4f>(float_value + vf2_offset)};
	ASSERT_FLOAT_EQ(vf2.get(0), float_value[0 + vf2_offset]);
	ASSERT_FLOAT_EQ(vf2.get(1), float_value[1 + vf2_offset]);
	ASSERT_FLOAT_EQ(vf2.get(2), float_value[2 + vf2_offset]);
	ASSERT_FLOAT_EQ(vf2.get(3), float_value[3 + vf2_offset]);

	const auto vi1{vec::make_load<vec4i>(int_value)};
	ASSERT_EQ(vi1.get(0), int_value[0]);
	ASSERT_EQ(vi1.get(1), int_value[1]);
	ASSERT_EQ(vi1.get(2), int_value[2]);
	ASSERT_EQ(vi1.get(3), int_value[3]);

	constexpr auto vfi_offset = 2;
	const auto vi2{vec::make_load<vec4i>(int_value + vfi_offset)};
	ASSERT_EQ(vi2.get(0), int_value[0 + vfi_offset]);
	ASSERT_EQ(vi2.get(1), int_value[1 + vfi_offset]);
	ASSERT_EQ(vi2.get(2), int_value[2 + vfi_offset]);
	ASSERT_EQ(vi2.get(3), int_value[3 + vfi_offset]);
}

TEST(TestVecOperation, TestMakeGenerator)
{
	auto float_generator = [](const auto index, const auto) { return index * 3.14f; };

	auto int_generator = [](const auto index, const auto) { return index * 42; };

	const auto v1{vec::make_load<vec4f>(float_generator)};
	ASSERT_FLOAT_EQ(v1.get(0), float_generator(0, 0));
	ASSERT_FLOAT_EQ(v1.get(1), float_generator(1, 0));
	ASSERT_FLOAT_EQ(v1.get(2), float_generator(2, 0));
	ASSERT_FLOAT_EQ(v1.get(3), float_generator(3, 0));

	const auto v2{vec::make_load<vec4i>(int_generator)};
	ASSERT_EQ(v2.get(0), int_generator(0, 0));
	ASSERT_EQ(v2.get(1), int_generator(1, 0));
	ASSERT_EQ(v2.get(2), int_generator(2, 0));
	ASSERT_EQ(v2.get(3), int_generator(3, 0));
}


TEST(TestVecOperation, TestMakeEqual)
{
	const auto vf1{vec::make<vec4f>(0.0f, 1.0f, -1.0f, -2.0f)};
	const auto vf2{vec::make<vec4f>(1.0f, -1.0f, -1.0f, -2.0f)};
	const auto vf_result = vec::equal(vf1, vf2);
	ASSERT_FLOAT_EQ(vf_result.get(0), 0.f);
	ASSERT_FLOAT_EQ(vf_result.get(1), 0.f);
	ASSERT_FLOAT_EQ(vf_result.get(2), 1.f);
	ASSERT_FLOAT_EQ(vf_result.get(3), 1.f);

	const auto vi1{vec::make<vec4i>(0, 1, -1, -2)};
	const auto vi2{vec::make<vec4i>(1, -1, -1, -2)};
	const auto vi_result = vec::equal(vi1, vi2);
	ASSERT_EQ(vi_result.get(0), 0);
	ASSERT_EQ(vi_result.get(1), 0);
	ASSERT_EQ(vi_result.get(2), 1);
	ASSERT_EQ(vi_result.get(3), 1);
}
