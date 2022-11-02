#pragma once

#if (defined(__GNUC__) || defined(__clang__))
#include"gcc_clang.h"
#else
#include"generic_operations.h"
#include"generic.h"
#endif

#include"prrsv.h"
#include"is_all_zeros.h"
