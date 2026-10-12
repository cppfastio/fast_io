#pragma once

#include "api/impl.h"
#include "create.h"
#include "io_observer.h"
#include "readwrite.h"
#include "seek.h"
#if !defined(_KERNEL_MODE)
#include "scatters.h"
#endif
#include "dir_9xa.h"
#include "file.h"
#include "status.h"
#include "console.h"
#include "pipe.h"
#include "aliases.h"
#include "stdhandle.h"
