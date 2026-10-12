#pragma once

#include "nt_definitions.h"
#include "nt_api.h"
#include "nt_zw.h"
#include "rtl_unicode_string_unique_ptr.h"
#include "nt_path.h"
#include "create.h"
#include "io_observer.h"
#include "readwrite.h"
#include "seek.h"
#include "file_lock.h"
#include "status.h"
#include "file.h"
#include "pipe.h"
#include "aliases.h"
#include "stdhandle.h"
#if !defined(_KERNEL_MODE)
#include "scatters.h"
#endif
