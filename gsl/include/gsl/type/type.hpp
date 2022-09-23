#pragma once

namespace gal::gsl::type
{
	enum Type
	{
		UNKNOWN,

		AUTO_INFER,
		ALIAS,

		VOID,
		BOOLEAN,			  // boolean.hpp
		SIGNED_INTEGER,		  // integer.hpp
		UNSIGNED_INTEGER,	  // integer.hpp
		SIGNED_LONG_INTEGER,  // integer.hpp
		UNSIGNED_LONG_INTEGER,// integer.hpp
		SINGLE_PRECISION,	  // floating_point.hpp
		DOUBLE_PRECISION,	  // floating_point.hpp
		VEC4_FLOAT,				// wide.hpp
		VEC4_SIGNED,			// wide.hpp
		VEC4_UNSIGNED,			// wide.hpp

		// todo
	};
}
