#pragma once

namespace fast_io::curve25519
{

struct affine_point
{
	field_number x, y;
};

struct precomputed_affine_point
{
	field_number ypx;
	field_number ymx;
	field_number t2d;
};

} // namespace fast_io::curve25519
