#include <fast_io_hosted/stack_pointer.h>

int main() noexcept {
#if defined (__GNUC__) || defined (__clang__)
    [[maybe_unused]] void* _{::fast_io::get_stack_pointer()};
#endif

    return 0;
}
