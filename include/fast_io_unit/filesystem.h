#pragma once

namespace fast_io
{

inline cross_code_cvt_t<typename std::filesystem::path::value_type> print_alias_define(io_alias_t,std::filesystem::path const& pth) noexcept
{
	auto const& native{pth.native()};
	return {{native.c_str(),native.size()}};
}

}
