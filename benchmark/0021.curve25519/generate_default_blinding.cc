#include<fast_io.h>
#include<fast_io_crypto.h>

void print_fn(char const* name,::fast_io::curve25519::field_number const& f)
{
	::fast_io::print(::fast_io::mnp::os_c_str(name),": {");
	for(std::size_t i{};i!=::fast_io::curve25519::field_number::array_size;++i)
	{
		::fast_io::print(::fast_io::mnp::hex0x(f.index_unchecked(i)),"ULL,");
	}
	::fast_io::print("}\n");
}

int main()
{
	/* deterministic internal seed */
	::fast_io::containers::array<std::byte,64> seed;
	for(std::size_t i{};i!=seed.size();++i)
	{
		seed.index_unchecked(i)=std::byte{static_cast<char unsigned>(0xa5^i)};
	}

	::fast_io::sha512_context H;
	::fast_io::containers::array<std::byte,::fast_io::sha512_context::digest_size> digest;
	H.update(seed.data(),seed.data()+seed.size());
	H.do_final();
	H.digest_to_byte_ptr(digest.data());

	::fast_io::curve25519::field_number zr,t,bl;
	for(std::size_t i{};i!=::fast_io::curve25519::field_number::array_size;++i)
	{
		t.index_unchecked(i)=::fast_io::curve25519::bytes_to_u64_little_endian(digest.data()+i*8);
		zr.index_unchecked(i)=::fast_io::curve25519::bytes_to_u64_little_endian(digest.data()+32+i*8);
	}
	::fast_io::curve25519::eco_mod(t);
	::fast_io::curve25519::subtraction_u256(bl,::fast_io::curve25519::NxBPOraw.index_unchecked(1),t);   /* bl = BPO - t */

	::fast_io::curve25519::extended_point T;
	::fast_io::curve25519::base_point_mult(T,t,zr);
	::fast_io::curve25519::precomputed_extended_point bp;
	::fast_io::curve25519::edp_ext_point_2e(bp,T);

	/* self-check: (s+bl)*B + bp == s*B for a fixed scalar s */
	::fast_io::curve25519::field_number s{{0x9d61b19deffd5a60ULL,0xba844af492ec2cc4ULL,0x4449c5697b326919ULL,0x103bac031cae7f60ULL}};
	::fast_io::curve25519::affine_point a1,a2;
	::fast_io::curve25519::base_point_multiply(a1,s);                              /* plain */
	::fast_io::curve25519::field_number g;
	::fast_io::curve25519::eco_add_reduce(g,s,bl);
	::fast_io::curve25519::extended_point S2;
	::fast_io::curve25519::base_point_mult(S2,g,zr);
	::fast_io::curve25519::add_point(S2,S2,bp);
	::fast_io::curve25519::field_number_inverse(S2.z,S2.z);
	::fast_io::curve25519::field_multiplication_mod(a2.x,S2.x,S2.z);
	::fast_io::curve25519::field_multiplication_mod(a2.y,S2.y,S2.z);
	bool ok{true};
	for(std::size_t i{};i!=::fast_io::curve25519::field_number::array_size;++i)
	{
		ok&=a1.x.index_unchecked(i)==a2.x.index_unchecked(i);
		ok&=a1.y.index_unchecked(i)==a2.y.index_unchecked(i);
	}
	::fast_io::print("self-check: ",ok,"\n");
	if(!ok)
	{
		return 1;
	}
	print_fn("bl",bl);
	print_fn("zr",zr);
	print_fn("bp.ypx",bp.ypx);
	print_fn("bp.ymx",bp.ymx);
	print_fn("bp.t2d",bp.t2d);
	print_fn("bp.z2 ",bp.z2);
}
