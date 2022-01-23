#pragma once

namespace fast_io
{

class global_initializer
{
public:
	explicit global_initializer() noexcept
	{
		SSL_library_init();
	}
};


}