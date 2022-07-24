#pragma once

namespace fast_io
{

/*
Referenced from man on Linux
*/

struct posix_file_status
{
	std::uintmax_t dev;     // ID of device containing file
	std::uintmax_t ino;     // Inode number
	perms perm;
	file_type type;
	std::uintmax_t nlink;   // Number of hard links
	std::uintmax_t uid;     // User ID of owner
	std::uintmax_t gid;     // Group ID of owner
	std::uintmax_t rdev;    // Device ID (if special file)
	std::uintmax_t size;    // Total size, in bytes 
	std::uintmax_t blksize; // Block size for filesystem I/O
	std::uintmax_t blocks;  // Number of 512B blocks allocated

	/* Since Linux 2.6, the kernel supports nanosecond
		precision for the following timestamp fields.
		For the details before Linux 2.6, see NOTES. */
	unix_timestamp atim;					// Time of last access
	unix_timestamp mtim;					// Time of last modification
	unix_timestamp ctim;					// Time of last status change
	unix_timestamp btim;					// Time of file creation
	std::uintmax_t flags;	// user defined flags for file
	std::uintmax_t gen;	// file generation number
};

template<std::integral char_type>
inline constexpr std::size_t print_reserve_size(io_reserve_type_t<char_type,fast_io::posix_file_status>)
{
	constexpr std::size_t res{
	print_reserve_size(io_reserve_type<char_type,std::uintmax_t>)*11+
	sizeof(u8"dev:")+sizeof(u8"\nino:")+sizeof(u8"\nperm:")+sizeof(u8"\ntype:")+
	sizeof(u8"\nnlink:")+sizeof(u8"\nuid:")+sizeof(u8"\ngid:")+sizeof(u8"\nrdev:")+
	sizeof(u8"\nsize:")+sizeof(u8"\nblksize:")+sizeof(u8"\nblocks:")+sizeof(u8"\natim:")+
	sizeof(u8"\nmtim:")+sizeof(u8"\nctim:")+sizeof(u8"\nbtim:")+sizeof(u8"\nflags:")+sizeof(u8"\ngen:")-16+
	print_reserve_size(io_reserve_type<char_type,perms>)+
	print_reserve_size(io_reserve_type<char_type,file_type>)+
	print_reserve_size(io_reserve_type<char_type,unix_timestamp>)*4};
	return res;
}

namespace details
{
template<std::integral char_type>
inline constexpr char_type* print_file_status_impl(char_type* iter,fast_io::posix_file_status const& status)
{
	if constexpr(std::same_as<char_type,char>)
	{
		iter=details::copy_string_literal("dev:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.dev);
		iter=details::copy_string_literal("\nino:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.ino);
		iter=details::copy_string_literal("\nperm:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,perms>,iter,status.perm);
		iter=details::copy_string_literal("\ntype:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,file_type>,iter,status.type);
		iter=details::copy_string_literal("\nnlink:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.nlink);
		iter=details::copy_string_literal("\nuid:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.uid);
		iter=details::copy_string_literal("\ngid:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.gid);
		iter=details::copy_string_literal("\nrdev:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.rdev);
		iter=details::copy_string_literal("\nsize:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.size);
		iter=details::copy_string_literal("\nblksize:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.blksize);
		iter=details::copy_string_literal("\nblocks:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.blocks);


		iter=details::copy_string_literal("\natim:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,unix_timestamp>,iter,status.atim);
		iter=details::copy_string_literal("\nmtim:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,unix_timestamp>,iter,status.mtim);
		iter=details::copy_string_literal("\nctim:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,unix_timestamp>,iter,status.ctim);
		iter=details::copy_string_literal("\nbtim:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,unix_timestamp>,iter,status.btim);

		iter=details::copy_string_literal("\nflags:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.flags);
		iter=details::copy_string_literal("\ngen:",iter);
		return print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.gen);
	}
	else if constexpr(std::same_as<char_type,wchar_t>)
	{
		iter=details::copy_string_literal(L"dev:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.dev);
		iter=details::copy_string_literal(L"\nino:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.ino);
		iter=details::copy_string_literal(L"\nperm:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,perms>,iter,status.perm);
		iter=details::copy_string_literal(L"\ntype:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,file_type>,iter,status.type);
		iter=details::copy_string_literal(L"\nnlink:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.nlink);
		iter=details::copy_string_literal(L"\nuid:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.uid);
		iter=details::copy_string_literal(L"\ngid:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.gid);
		iter=details::copy_string_literal(L"\nrdev:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.rdev);
		iter=details::copy_string_literal(L"\nsize:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.size);
		iter=details::copy_string_literal(L"\nblksize:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.blksize);
		iter=details::copy_string_literal(L"\nblocks:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.blocks);


		iter=details::copy_string_literal(L"\natim:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,unix_timestamp>,iter,status.atim);
		iter=details::copy_string_literal(L"\nmtim:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,unix_timestamp>,iter,status.mtim);
		iter=details::copy_string_literal(L"\nctim:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,unix_timestamp>,iter,status.ctim);
		iter=details::copy_string_literal(L"\nbtim:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,unix_timestamp>,iter,status.btim);

		iter=details::copy_string_literal(L"\nflags:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.flags);
		iter=details::copy_string_literal(L"\ngen:",iter);
		return print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.gen);
	}
	else if constexpr(std::same_as<char_type,char16_t>)
	{
		iter=details::copy_string_literal(u"dev:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.dev);
		iter=details::copy_string_literal(u"\nino:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.ino);
		iter=details::copy_string_literal(u"\nperm:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,perms>,iter,status.perm);
		iter=details::copy_string_literal(u"\ntype:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,file_type>,iter,status.type);
		iter=details::copy_string_literal(u"\nnlink:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.nlink);
		iter=details::copy_string_literal(u"\nuid:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.uid);
		iter=details::copy_string_literal(u"\ngid:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.gid);
		iter=details::copy_string_literal(u"\nrdev:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.rdev);
		iter=details::copy_string_literal(u"\nsize:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.size);
		iter=details::copy_string_literal(u"\nblksize:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.blksize);
		iter=details::copy_string_literal(u"\nblocks:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.blocks);


		iter=details::copy_string_literal(u"\natim:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,unix_timestamp>,iter,status.atim);
		iter=details::copy_string_literal(u"\nmtim:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,unix_timestamp>,iter,status.mtim);
		iter=details::copy_string_literal(u"\nctim:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,unix_timestamp>,iter,status.ctim);
		iter=details::copy_string_literal(u"\nbtim:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,unix_timestamp>,iter,status.btim);

		iter=details::copy_string_literal(u"\nflags:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.flags);
		iter=details::copy_string_literal(u"\ngen:",iter);
		return print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.gen);
	}
	else if constexpr(std::same_as<char_type,char32_t>)
	{
		iter=details::copy_string_literal(U"dev:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.dev);
		iter=details::copy_string_literal(U"\nino:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.ino);
		iter=details::copy_string_literal(U"\nperm:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,perms>,iter,status.perm);
		iter=details::copy_string_literal(U"\ntype:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,file_type>,iter,status.type);
		iter=details::copy_string_literal(U"\nnlink:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.nlink);
		iter=details::copy_string_literal(U"\nuid:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.uid);
		iter=details::copy_string_literal(U"\ngid:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.gid);
		iter=details::copy_string_literal(U"\nrdev:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.rdev);
		iter=details::copy_string_literal(U"\nsize:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.size);
		iter=details::copy_string_literal(U"\nblksize:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.blksize);
		iter=details::copy_string_literal(U"\nblocks:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.blocks);


		iter=details::copy_string_literal(U"\natim:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,unix_timestamp>,iter,status.atim);
		iter=details::copy_string_literal(U"\nmtim:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,unix_timestamp>,iter,status.mtim);
		iter=details::copy_string_literal(U"\nctim:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,unix_timestamp>,iter,status.ctim);
		iter=details::copy_string_literal(U"\nbtim:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,unix_timestamp>,iter,status.btim);

		iter=details::copy_string_literal(U"\nflags:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.flags);
		iter=details::copy_string_literal(U"\ngen:",iter);
		return print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.gen);
	}
	else
	{
		iter=details::copy_string_literal(u8"dev:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.dev);
		iter=details::copy_string_literal(u8"\nino:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.ino);
		iter=details::copy_string_literal(u8"\nperm:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,perms>,iter,status.perm);
		iter=details::copy_string_literal(u8"\ntype:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,file_type>,iter,status.type);
		iter=details::copy_string_literal(u8"\nnlink:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.nlink);
		iter=details::copy_string_literal(u8"\nuid:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.uid);
		iter=details::copy_string_literal(u8"\ngid:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.gid);
		iter=details::copy_string_literal(u8"\nrdev:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.rdev);
		iter=details::copy_string_literal(u8"\nsize:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.size);
		iter=details::copy_string_literal(u8"\nblksize:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.blksize);
		iter=details::copy_string_literal(u8"\nblocks:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.blocks);


		iter=details::copy_string_literal(u8"\natim:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,unix_timestamp>,iter,status.atim);
		iter=details::copy_string_literal(u8"\nmtim:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,unix_timestamp>,iter,status.mtim);
		iter=details::copy_string_literal(u8"\nctim:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,unix_timestamp>,iter,status.ctim);
		iter=details::copy_string_literal(u8"\nbtim:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,unix_timestamp>,iter,status.btim);

		iter=details::copy_string_literal(u8"\nflags:",iter);
		iter=print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.flags);
		iter=details::copy_string_literal(u8"\ngen:",iter);
		return print_reserve_define(io_reserve_type<char_type,std::uintmax_t>,iter,status.gen);
	}
}
}

template<std::integral char_type>
inline constexpr char_type* print_reserve_define(io_reserve_type_t<char_type,fast_io::posix_file_status>, char_type* iter,fast_io::posix_file_status const& status)
{
	return details::print_file_status_impl<char_type>(iter,status);
}

}
