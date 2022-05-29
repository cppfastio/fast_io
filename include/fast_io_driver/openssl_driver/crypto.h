#pragma once

namespace fast_io
{

class crypto
{
public:
	crypto(std::uint_least64_t opts,settings_observer sob)
	{
		if(!OPENSSL_init_crypto(opt,sob.native_handle()))
			throw_openssl_error();
	}
	crypto(crypto const&)=delete;
	crypto const& operator=(crypto const&)=delete;
	~crypto()
	{
		OPENSSL_cleanup();
	}
};

};