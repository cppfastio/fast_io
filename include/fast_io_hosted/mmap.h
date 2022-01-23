#pragma once

namespace fast_io
{
/*
enum class mapping
{
indeterminate,private,shared,copy_on_write,clean
};
*/

enum class file_map_attribute
{
execute_read=0x20,execute_read_write=0x40,execute_write_copy=0x80,
read_only=0x02,read_write=0x04,write_copy=0x08
};


}