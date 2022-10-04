#pragma once

/*
Create python's PyBytesObject by using fast_io's api
*/
#include"../../fast_io_core.h"
#include<Python.h>
#include<bytesobject.h>

namespace fast_io
{

class pyobject_io_observer
{
public:
	using native_handle_type = PyObject*;
	native_handle_type p{};
	constexpr native_handle_type native_handle() const noexcept
	{
		return p;
	}
	constexpr native_handle_type release() noexcept
	{
		auto temp{p};
		this->p = nullptr;
		return temp;
	}
};

namespace details
{

template<bool dec>
inline void py_inc_dec_ref_model_impl(void *p)
{
	if constexpr(dec)
	{
		Py_DECREF(p);
	}
	else
	{
		Py_INCREF(p);
	}
}

template<bool dec>
inline void py_inc_dec_ref_impl(void * p) noexcept
{
	if(p==nullptr)
		return;
	::fast_io::noexcept_call(py_inc_dec_ref_model_impl<dec>,p);
}

inline PyObject* pybytes_fromstringandsize_model_impl(char const *v, Py_ssize_t len)
{
	return PyBytes_FromStringAndSize(v,len);
}

inline PyObject* pybytes_fromstringandsize_impl(char const *v, Py_ssize_t len) noexcept
{
	auto p{::fast_io::noexcept_call(pybytes_fromstringandsize_model_impl,v,len)};
	if(p==nullptr)
	{
		::fast_io::fast_terminate();
	}
	return p;
}

inline PyObject* pyobject_repr_model_impl(PyObject* p)
{
	return PyObject_Repr(p);
}

inline PyObject* pyunicode_asencodedstring_model_impl(PyObject *self, char const *name, char const *value)
{
	return PyUnicode_AsEncodedString(self,name,value);
}

inline PyObject* pyunicode_asencodedstring_impl(PyObject *self, char const *name, char const *value) noexcept
{
	if(self==nullptr)
	{
		return nullptr;
	}
	return ::fast_io::noexcept_call(pyunicode_asencodedstring_model_impl,self,name,value);
}


inline int pybytes_asstringandsize_model_impl(PyObject *self, char** p,Py_ssize_t* len)
{
	return PyBytes_AsStringAndSize(self,p,len);
}

inline basic_io_scatter_t<char> pybytes_asstringandsize_impl(PyObject *self) noexcept
{
	char* s{};
	Py_ssize_t len{};
	if(self!=nullptr)
#if __has_cpp_attribute(likely)
	[[likely]]
#endif
	{
		if(::fast_io::noexcept_call(pybytes_asstringandsize_model_impl,self,__builtin_addressof(s),__builtin_addressof(len))<0)
		{
			s=nullptr;
			len=0;
		}
	}
	return {s,static_cast<::std::size_t>(len)};
}

}

class pyobject_file:public pyobject_io_observer
{
public:
	using typename pyobject_io_observer::native_handle_type;
	
	constexpr pyobject_file() noexcept = default;
	explicit constexpr pyobject_file(native_handle_type pp) noexcept:pyobject_io_observer{pp}
	{
	}
	pyobject_file(pyobject_io_observer) noexcept = delete;
	pyobject_file(pyobject_file const& pob) noexcept:pyobject_io_observer{pob.p}
	{
		::fast_io::details::py_inc_dec_ref_impl<false>(this->p);
	}
	pyobject_file& operator=(pyobject_file const & pob) noexcept
	{
		::fast_io::details::py_inc_dec_ref_impl<true>(this->p);
		::fast_io::details::py_inc_dec_ref_impl<false>(this->p=pob.p);
		return *this;
	}
	constexpr pyobject_file(pyobject_file&& pob) noexcept:pyobject_io_observer{pob.p}
	{
		pob.p = nullptr;
	}
	pyobject_file& operator=(pyobject_file&& pob) noexcept
	{
		::fast_io::details::py_inc_dec_ref_impl<true>(this->p);
		this->p=pob.p;
		pob.p=nullptr;
		return *this;
	}
	~pyobject_file()
	{
		::fast_io::details::py_inc_dec_ref_impl<true>(this->p);
	}
};

inline constexpr pyobject_io_observer io_strlike_ref(io_alias_t,pyobject_io_observer pyiob) noexcept
{
	return pyiob;
}

inline pyobject_file strlike_construct_define(io_strlike_type_t<char,pyobject_file>,char const* first,char const* last) noexcept
{
	std::size_t diff{static_cast<std::size_t>(last-first)};
	if constexpr(::std::numeric_limits<Py_ssize_t>::max()<::std::numeric_limits<::std::size_t>::max())
	{
		constexpr Py_ssize_t max_sz{static_cast<Py_ssize_t>(std::numeric_limits<Py_ssize_t>::max()/static_cast<Py_ssize_t>(sizeof(char)))};
		if(max_sz<diff)
			::fast_io::fast_terminate();
	}
	auto p{::fast_io::details::pybytes_fromstringandsize_impl(first,static_cast<Py_ssize_t>(diff))};
	::fast_io::details::py_inc_dec_ref_impl<false>(p);
	return pyobject_file(p);
}

template<typename... Args>
inline pyobject_file concat_pyobject_file(Args&& ...args)
{
	return ::fast_io::basic_general_concat<false,char,pyobject_file>(::std::forward<Args>(args)...);
}

template<typename... Args>
inline pyobject_file concatln_pyobject_file(Args&& ...args)
{
	return ::fast_io::basic_general_concat<true,char,pyobject_file>(::std::forward<Args>(args)...);
}

inline pyobject_file print_alias_define(io_alias_t,pyobject_io_observer pyiob) noexcept
{
	pyobject_file pyob_repr{::fast_io::noexcept_call(::fast_io::details::pyobject_repr_model_impl,pyiob.p)};
	return pyobject_file(::fast_io::details::pyunicode_asencodedstring_impl(
		pyob_repr.p,reinterpret_cast<char const*>(u8"utf-8"),reinterpret_cast<char const*>(u8"~E~")));
}

template<std::integral char_type>
inline std::conditional_t<sizeof(char_type)==sizeof(char),basic_io_scatter_t<char_type>,manipulators::code_cvt_t<encoding_scheme::execution_charset,encoding_scheme::execution_charset,char>> status_io_print_forward(io_alias_type_t<char_type>,pyobject_io_observer piob) noexcept
{
	using char_type_may_alias_const_ptr
#if __has_cpp_attribute(gnu::may_alias)
	[[gnu::may_alias]]
#endif
	= char_type const*;
	if constexpr(::std::same_as<char_type,char>)
	{
		return ::fast_io::details::pybytes_asstringandsize_impl(piob.p);
	}
	else if constexpr(sizeof(char)==sizeof(char_type))
	{
		auto ret{::fast_io::details::pybytes_asstringandsize_impl(piob.p)};
		return {reinterpret_cast<char_type_may_alias_const_ptr>(ret.base),ret.len};
	}
	else
	{
		auto ret{::fast_io::details::pybytes_asstringandsize_impl(piob.p)};
		return {{reinterpret_cast<char_type_may_alias_const_ptr>(ret.base),ret.len}};
	}
}

namespace manipulators
{

inline constexpr pyobject_io_observer pyobj(PyObject* p) noexcept
{
	return {p};
}

}

}

