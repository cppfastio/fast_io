CXX=g++
CXXFLAGS=-O3 -s -D_WIN32_WINNT=0x0502
FAST_IO_PATH=-I../../../include

mydriver.sys:kernel_entry.cc
	$(CXX) -o mydriver.sys kernel_entry.cc -shared -fno-exceptions -fno-rtti -fno-unwind-tables -fno-asynchronous-unwind-tables -ffreestanding -Wl,--subsystem,native -Wl,--image-base,0x10000 -Wl,--file-alignment,0x1000 -Wl,--section-alignment,0x1000 -Wl,--entry,DriverEntry -Wl,--stack,0x40000 -Wl,--dynamicbase -Wl,--nxcompat -nostartfiles -nostdlib -std=c++20 -lntoskrnl -lhal -Wall -Wextra -Wpedantic -Wmisleading-indentation -Wunused -Wuninitialized -Wshadow -Wconversion $(CXXFLAGS) $(FAST_IO_PATH)
clean:
	rm mydriver.sys
