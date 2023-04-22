#include<fast_io.h>
#include<fast_io_driver/boost/uuid.h>
#include<boost/uuid/uuid_generators.hpp>
#include<boost/uuid/uuid_io.hpp>

using namespace fast_io::io;

int main()
{
	boost::uuids::uuid uuid = boost::uuids::random_generator()();
	println(uuid,"\nboost:",to_string(uuid));
}
