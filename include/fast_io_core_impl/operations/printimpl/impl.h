#pragma once

#if defined(__HERBCEPTIONS__) || defined(__cpp_expansion_statements)
#include "print_freestanding.h"
#include "common_post.h"
#include "concat_buffer.h"
#include "concat.h"
#else
#include "print_freestanding_cxx20.h"
#endif
