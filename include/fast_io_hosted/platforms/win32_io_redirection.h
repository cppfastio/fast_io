#pragma once
namespace fast_io
{
struct win32_io_redirection
{
	void *win32_pipe_in_handle{};
	void *win32_pipe_out_handle{};
	void *win32_handle{};
	bool is_dev_null{};
};
} // namespace fast_io
