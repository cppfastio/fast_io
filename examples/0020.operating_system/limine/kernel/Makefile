KERNEL := kernel.elf

CXX ?= g++
LD ?= ld

CXXFLAGS  ?= -Wall -Wextra -O3 -pipe -fno-exceptions -fno-rtti -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-ident -fwhole-program
LDFLAGS ?=

INTERNALLDFLAGS :=         \
	-Tlinker.ld            \
	-nostdlib              \
	-zmax-page-size=0x1000 \
	-static                \
	-pie                   \
	--no-dynamic-linker    \
	-ztext

INTERNALCXXFLAGS :=        \
	-I.                  \
	-std=c++20           \
	-ffreestanding       \
	-fno-stack-protector \
	-fpie                \
	-mno-80387           \
	-mno-mmx             \
	-mno-3dnow           \
	-mno-sse             \
	-mno-sse2            \
	-mno-red-zone        \
	-MMD

CXXFILES      := $(shell find ./ -type f -name '*.cc')
OBJ         := $(CXXFILES:.cc=.o)
HEADER_DEPS := $(CFILES:.cc=.d)

.PHONY: all
all: $(KERNEL)

stivale2.h:
	curl https://raw.githubusercontent.com/stivale/stivale/master/stivale2.h -o $@

$(KERNEL): $(OBJ)
	$(LD) $(OBJ) $(LDFLAGS) $(INTERNALLDFLAGS) -o $@

-include $(HEADER_DEPS)

%.o: %.cc stivale2.h
	$(CXX) $(CXXFLAGS) $(INTERNALCXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(KERNEL) $(OBJ) $(HEADER_DEPS)

.PHONY: distclean
distclean: clean
	rm -f stivale2.h
