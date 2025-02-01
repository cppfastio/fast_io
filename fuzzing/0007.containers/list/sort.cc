#include <fast_io.h>
#include <fast_io_dsal/list.h>
#include <fast_io_dsal/span.h>
#include <algorithm>

extern "C" int LLVMFuzzerTestOneInput(::std::uint8_t *data, ::std::size_t size)
{
    auto first{reinterpret_cast<::std::size_t*>(data)};
    auto last{first+size/sizeof(::std::size_t)};
    ::fast_io::span<::std::size_t> sp(first,last);
    ::fast_io::list<::std::size_t> lst(::std::from_range,sp);
    lst.sort();
    if(sp.size()!=static_cast<::std::size_t>(::std::ranges::distance(lst))||
        !::std::ranges::is_sorted(lst)) [[unlikely]]
    {
        ::fast_io::panic("lst not sorted:\n", ::fast_io::mnp::rgvw(lst,"\n"));
    }
    return 0;
}