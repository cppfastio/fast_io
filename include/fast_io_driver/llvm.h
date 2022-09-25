#pragma once

#include"../fast_io.h"
#include<llvm/Support/raw_ostream.h>
#include"llvm_impl/hack_buffer_ptr.h"
#include"llvm_impl/raw_ostream_io_observer.h"
#if !defined(__AVR__)
#include"llvm_impl/raw_fd_ostream_file.h"
#endif
