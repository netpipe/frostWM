#ifndef _ZLA_TYPES_H
#define _ZLA_TYPES_H

#include <float.h>

#define ZLA_DOUBLE_MODE

#ifdef ZLA_DOUBLE_MODE
	typedef double ZLA_FLOAT;
	#define ZLAFL_EPSILON (DBL_EPSILON*2.0)
	#define ZLAFL_MAX 999999999.0
	#define ZLAFL_EPSILON2 1e-12
#else
	typedef float ZLA_FLOAT;
	#define ZLAFL_EPSILON (FLT_EPSILON*2.0f)
	#define ZLAFL_MAX 999999999.0f
	#define ZLAFL_EPSILON2 1e-3f
#endif

#endif