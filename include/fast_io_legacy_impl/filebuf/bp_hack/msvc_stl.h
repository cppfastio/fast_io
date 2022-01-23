#pragma once
//Let's do great brutal hack to msvc_stl with ::fast_io::details::my_memcpy
namespace fast_io
{

/*
https://github.com/microsoft/STL/blob/master/stl/inc/streambuf

private:
    _Elem* _Gfirst; // beginning of read buffer
    _Elem* _Pfirst; // beginning of write buffer
    _Elem** _IGfirst; // pointer to beginning of read buffer
    _Elem** _IPfirst; // pointer to beginning of write buffer
    _Elem* _Gnext; // current position in read buffer
    _Elem* _Pnext; // current position in write buffer
    _Elem** _IGnext; // pointer to current position in read buffer
    _Elem** _IPnext; // pointer to current position in write buffer

    int _Gcount; // length of read buffer
    int _Pcount; // length of write buffer
    int* _IGcount; // pointer to length of read buffer
    int* _IPcount; // pointer to length of write buffer

protected:
    locale* _Plocale; // pointer to imbued locale object
*/

namespace details::streambuf_hack
{

template<std::size_t pos,typename T>
inline typename T::char_type*& hack_first_next(T* fb)
{
	constexpr std::size_t offset(sizeof(std::uintptr_t)	//vptr
	+sizeof(std::uintptr_t)*2		//_Gfirst,_Pfirst. Why do they set these useless fields??? OOP just encourages bad code tbh
	+sizeof(std::uintptr_t)*pos);	//Yes to C with concepts. No to C with classes.
	typename T::char_type** value;
	::fast_io::details::my_memcpy(__builtin_addressof(value),reinterpret_cast<std::byte*>(fb)+offset,sizeof(typename T::char_type**));
	return *value;
}

template<std::size_t pos,typename T>
inline int& hack_last(T* fb)
{
	constexpr std::size_t offset(sizeof(std::uintptr_t)	//vptr
	+sizeof(std::uintptr_t)*8		//pass those 8 pointers
	+2*sizeof(int)					//pass 2 integers
	+sizeof(std::uintptr_t)*pos);
//	printf("offset : %zu\n",offset);
	int* value;
	::fast_io::details::my_memcpy(__builtin_addressof(value),reinterpret_cast<std::byte*>(fb)+offset,sizeof(int*));
	return *value;
}

template<std::size_t position,typename T>
requires (position<6)
inline typename T::char_type* hack_buffer_ptr(T* fb) noexcept
{
	if constexpr(position==0)
		return hack_first_next<0>(fb);
	else if constexpr(position==1)
		return hack_first_next<4>(fb);
	else if constexpr(position==2)
		return hack_first_next<4>(fb)+hack_last<0>(fb);
	else if constexpr(position==3)
		return hack_first_next<1>(fb);
	else if constexpr(position==4)
		return hack_first_next<5>(fb);
	else if constexpr(position==5)
		return hack_first_next<5>(fb)+hack_last<1>(fb);
}
template<std::size_t position,typename T>
requires (position==1||position==4)
inline void hack_set_buffer_curr(T* fb,typename T::char_type* ptr) noexcept
{
	auto& pptr{hack_first_next<4+static_cast<std::size_t>(position==4)>(fb)};
	hack_last<static_cast<std::size_t>(position==4)>(fb)-=static_cast<int>(ptr-pptr);
	pptr=ptr;
}
}


}