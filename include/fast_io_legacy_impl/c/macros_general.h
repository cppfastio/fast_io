#pragma once

namespace fast_io
{

inline c_io_observer c_stdin() noexcept
{
	return {stdin};
}

inline c_io_observer c_stdout() noexcept
{
	return {stdout};
}

inline c_io_observer c_stderr() noexcept
{
	return {stderr};
}
inline wc_io_observer wc_stdin() noexcept
{
	return {stdin};
}

inline wc_io_observer wc_stdout() noexcept
{
	return {stdout};
}

inline wc_io_observer wc_stderr() noexcept
{
	return {stderr};
}

inline u8c_io_observer u8c_stdin() noexcept
{
	return {stdin};
}

inline u8c_io_observer u8c_stdout() noexcept
{
	return {stdout};
}

inline u8c_io_observer u8c_stderr() noexcept
{
	return {stderr};
}

}
