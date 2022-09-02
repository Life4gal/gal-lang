#pragma once

namespace gal::gsl::core
{
	enum runtime_type
	{
		none,
		any,
		auto_infer,
		alias,

		void_type,
		bool_type,

		int8_type,
		uint8_type,
		int16_type,
		uint16_type,
		int32_type,
		uint32_type,
		int64_type,
		uint64_type,
		float_type,
		double_type,

		range_type,
		string_type,
		array_type,
		tuple_type,
		table_type,
		enumeration,
		variant_type,

		function_type,
		lambda_type,
	};
}
