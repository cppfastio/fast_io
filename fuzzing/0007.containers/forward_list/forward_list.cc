#include <fast_io.h>
#include <fast_io_dsal/forward_list.h>
#include <fast_io_dsal/span.h>
#include <algorithm>

extern "C" int LLVMFuzzerTestOneInput(::std::uint8_t *data, ::std::size_t size)
{
    auto first{reinterpret_cast<::std::size_t*>(data)};
    auto last{first+size/sizeof(::std::size_t)};
    ::fast_io::span<::std::size_t> sp(first,last);
    ::fast_io::forward_list<::std::size_t> flist(::std::from_range,sp);
    flist.sort();
    if(sp.size()!=static_cast<::std::size_t>(::std::ranges::distance(flist))||
        !::std::ranges::is_sorted(flist)) [[unlikely]]
    {
        ::fast_io::panic("flist not sorted:\n", ::fast_io::mnp::rgvw(flist,"\n"));
    }
    return 0;
}