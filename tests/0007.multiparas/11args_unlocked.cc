#include<fast_io.h>
#include<climits>

int main()
{
	auto cst{fast_io::c_stdout()};
	fast_io::io_lock_guard guard{cst};
	auto cstd_unlocked{cst.unlocked_handle()};
	println(cstd_unlocked,2,3,4,5,6,7,8,9,"a",10,11,12);
	println(cstd_unlocked,2,3,4,5,6,7,8,9,"a",10,11,12);
	println(cstd_unlocked,2,3,4,5,6,7,8,9,"a",10,11,12);
	println(cstd_unlocked,2,3,4,5,6,7,8,9,"a",10,11,12);
	println(cstd_unlocked,2,3,4,5,6,7,8,9,"a",10,11,12);
	println(cstd_unlocked,2,3,4,5,6,7,8,9,"a",10,11,12);
	println(cstd_unlocked,2,3,4,5,6,7,8,9,"a",10,11,12);
	println(cstd_unlocked,2,3,4,5,6,7,8,9,"a",10,11,12);
	println(cstd_unlocked,2,3,4,5,6,7,8,9,"a",10,11,12);
	println(cstd_unlocked,2,3,4,5,6,7,8,9,"a",10,11,12);
	println(cstd_unlocked,2,3,4,5,6,7,8,9,"a",10,11,12);
	println(cstd_unlocked,2,3,4,5,6,7,8,9,"a",10,11,12);
	println(cstd_unlocked,2,3,4,5,6,7,8,9,"a",10,11,12);
	println(cstd_unlocked,2,3,4,5,6,7,8,9,"a",10,11,12);
	println(cstd_unlocked,2,3,4,5,6,7,8,9,"a",10,11,12);
	println(cstd_unlocked,2,3,4,5,6,7,8,9,"a",10,11,12);
	println(cstd_unlocked,INT_MAX," ",INT_MAX," ",INT_MAX," ",INT_MAX," ",INT_MAX," ",INT_MAX," ",INT_MAX," ",INT_MAX,"a",INT_MAX," ",INT_MAX," ",INT_MAX);
	println(cstd_unlocked,INT_MAX," ",INT_MAX," ",INT_MAX," ",INT_MAX," ",INT_MAX," ",INT_MAX," ",INT_MAX," ",INT_MAX,"a",INT_MAX," ",INT_MAX," ",INT_MAX);
	println(cstd_unlocked,INT_MAX," ",INT_MAX," ",INT_MAX," ",INT_MAX," ",INT_MAX," ",INT_MAX," ",INT_MAX," ",INT_MAX,"a",INT_MAX," ",INT_MAX," ",INT_MAX);
	println(cstd_unlocked,INT_MAX," ",INT_MAX," ",INT_MAX," ",INT_MAX," ",INT_MAX," ",INT_MAX," ",INT_MAX," ",INT_MAX,"a",INT_MAX," ",INT_MAX," ",INT_MAX);
//	partition_impl<3>(3,4,5,6,7);
}