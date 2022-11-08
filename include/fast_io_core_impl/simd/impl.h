#pragma once

#include"cpu_flags.h"
#if __has_cpp_attribute(__gnu__::__vector_size__)
#include"gcc_clang.h"
#else
#include"generic_operations.h"
#include"generic.h"
#endif
#include"x86_64.h"

#include"prrsv.h"
#include"is_all_zeros.h"
#include"mask_countr.h"
