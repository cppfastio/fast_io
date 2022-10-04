#pragma once

namespace fast_io
{

namespace details
{

template<std::integral char_type,typename T>
inline constexpr std::size_t lc_print_reserve_size_width_impl(basic_lc_all<char_type> const* all,T w,std::size_t width) noexcept
{
	using value_type = std::remove_cvref_t<T>;
	if constexpr(lc_dynamic_reserve_printable<char_type,value_type>)
	{
		std::size_t const sz{print_reserve_size(all,w)};
		if(width<sz)
			return sz;
		return width;
	}
	else if constexpr(lc_scatter_printable<char_type,value_type>)
	{
		basic_io_scatter_t<char_type> const scatter{print_scatter_define(all,w)};
		if(width<scatter.len)
			return scatter.len;
		return width;
	}
}

template<::fast_io::manipulators::scalar_placement placement,::std::integral char_type,typename T>
requires (std::is_trivially_copyable_v<T>)
inline constexpr char_type* lc_print_reserve_define_width_ch_impl(basic_lc_all<char_type> const* __restrict all,char_type* iter,T t,std::size_t wdt,char_type fillch) noexcept
{
	using value_type = std::remove_cvref_t<T>;
	if constexpr(placement==::fast_io::manipulators::scalar_placement::internal)
	{
		static_assert(lc_printable_internal_shift<char_type,value_type>);
		if constexpr(lc_printable_internal_shift<char_type,value_type>)
		{
			if constexpr(lc_scatter_printable<char_type,value_type>)
			{
				auto sc{print_scatter_define(all,t)};
				auto it{copy_scatter(sc,iter)};
				return handle_common_internal_ch(iter,it,wdt,fillch,print_define_internal_shift(all,t));
			}
			else
			{
				char_type* it{print_reserve_define(all,iter,t)};
				return handle_common_internal_ch(iter,it,wdt,fillch,print_define_internal_shift(all,t));
			}
		}
		else
		{
			return print_reserve_define_width_ch_impl<::fast_io::manipulators::scalar_placement::right>(iter,t,wdt,fillch);
		}
	}	
	else
	{
		if constexpr(lc_scatter_printable<char_type,value_type>)
		{
			auto sc{print_scatter_define(all,t)};
			auto it{copy_scatter(sc,iter)};
			return handle_common_ch<placement>(iter,it,wdt,fillch);
		}
		else
		{
			char_type* it{print_reserve_define(all,iter,t)};
			return handle_common_ch<placement>(iter,it,wdt,fillch);
		}
	}
}

template<::fast_io::manipulators::scalar_placement wm,::std::integral char_type,typename T>
inline constexpr char_type* lc_print_reserve_define_width_impl(basic_lc_all<char_type> const* __restrict all, char_type* iter,T t,std::size_t width) noexcept
{
	return lc_print_reserve_define_width_ch_impl<wm>(all,iter,t,width,char_literal_v<u8' ',char_type>);
}

}

template<std::integral char_type,manipulators::scalar_placement wm,typename T>
requires ((lc_dynamic_reserve_printable<char_type,std::remove_cvref_t<T>>||
	lc_scatter_printable<char_type,std::remove_cvref_t<T>>))
inline constexpr std::size_t print_reserve_size(basic_lc_all<char_type> const* __restrict all,manipulators::width_t<wm,T> w) noexcept
{
	if constexpr(std::is_reference_v<T>)
		return details::lc_print_reserve_size_width_impl<char_type>(all,parameter<T>{w.reference},w.width);
	else
		return details::lc_print_reserve_size_width_impl<char_type>(all,w.reference,w.width);
}

template<::std::integral char_type,manipulators::scalar_placement wm,typename T>
requires ((lc_dynamic_reserve_printable<char_type,std::remove_cvref_t<T>>||
	lc_scatter_printable<char_type,std::remove_cvref_t<T>>))
inline constexpr char_type* print_reserve_define(basic_lc_all<char_type> const* __restrict all, char_type* iter,manipulators::width_t<wm,T> w) noexcept
{
	if constexpr(std::is_reference_v<T>)
		return details::lc_print_reserve_define_width_impl<wm>(all,iter,parameter<T>{w.reference},w.width);
	else
		return details::lc_print_reserve_define_width_impl<wm>(all,iter,w.reference,w.width);
}


template<std::integral char_type,manipulators::scalar_placement wm,typename T>
requires ((lc_dynamic_reserve_printable<char_type,std::remove_cvref_t<T>>||
	lc_scatter_printable<char_type,std::remove_cvref_t<T>>))
inline constexpr std::size_t print_reserve_size(basic_lc_all<char_type> const* __restrict all,manipulators::width_ch_t<wm,T,char_type> w) noexcept
{
	if constexpr(std::is_reference_v<T>)
		return details::lc_print_reserve_size_width_impl<char_type>(all,parameter<T>{w.reference},w.width);
	else
		return details::lc_print_reserve_size_width_impl<char_type>(all,w.reference,w.width);
}

template<::std::integral char_type,manipulators::scalar_placement wm,typename T>
requires ((lc_dynamic_reserve_printable<char_type,std::remove_cvref_t<T>>||
	lc_scatter_printable<char_type,std::remove_cvref_t<T>>))
inline constexpr char_type* print_reserve_define(basic_lc_all<char_type> const* __restrict all, char_type* iter,manipulators::width_ch_t<wm,T, char_type> w) noexcept
{
	if constexpr(std::is_reference_v<T>)
		return details::lc_print_reserve_define_width_ch_impl<wm>(all,iter,parameter<T>{w.reference},w.width,w.ch);
	else
		return details::lc_print_reserve_define_width_ch_impl<wm>(all,iter,w.reference,w.width,w.ch);
}

}
