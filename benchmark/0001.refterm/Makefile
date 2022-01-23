CXX = clang++
FASTIOINCLUDEPATH = -I../../include
CXXFLAGS = -Ofast -march=native -std=c++20 $(FASTIOINCLUDEPATH)
CXXEXTRAFLAGS = -flto
LDFLAGS = -fuse-ld=lld -s

all: out_buf.exe fpipe.exe stdio.exe stdio_hack.exe iostream.exe fstream.exe filebuf_file.exe

out_buf.exe:pch.hpp.gch out_buf.cc
	$(CXX) -o out_buf.exe out_buf.cc $(CXXFLAGS) $(LDFLAGS) $(CXXEXTRAFLAGS) -include pch.hpp
fpipe.exe:pch.hpp.gch fpipe.cc
	$(CXX) -o fpipe.exe fpipe.cc $(CXXFLAGS) $(LDFLAGS) $(CXXEXTRAFLAGS) -include pch.hpp
stdio.exe:pch.hpp.gch stdio.cc
	$(CXX) -o stdio.exe stdio.cc $(CXXFLAGS) $(LDFLAGS) $(CXXEXTRAFLAGS) -include pch.hpp
stdio_hack.exe:pch.hpp.gch stdio_hack.cc
	$(CXX) -o stdio_hack.exe stdio_hack.cc $(CXXFLAGS) $(LDFLAGS) $(CXXEXTRAFLAGS) -include pch.hpp
iostream.exe:iostreampch.hpp.gch iostream.cc
	$(CXX) -o iostream.exe iostream.cc $(CXXFLAGS) $(LDFLAGS) $(CXXEXTRAFLAGS) -include iostreampch.hpp
fstream.exe:filebufpch.hpp.gch fstream.cc
	$(CXX) -o fstream.exe fstream.cc $(CXXFLAGS) $(LDFLAGS) $(CXXEXTRAFLAGS) -include filebufpch.hpp -lntdll
filebuf_file.exe:filebufpch.hpp.gch filebuf_file.cc
	$(CXX) -o filebuf_file.exe filebuf_file.cc $(CXXFLAGS) $(LDFLAGS) $(CXXEXTRAFLAGS) -include filebufpch.hpp -lntdll
filebufpch.hpp.gch:filebufpch.hpp pch.hpp.gch
	$(CXX) -c filebufpch.hpp $(CXXFLAGS) $(FASTIOINCLUDEPATH) -include pch.hpp
iostreampch.hpp.gch:iostreampch.hpp pch.hpp.gch
	$(CXX) -c iostreampch.hpp $(CXXFLAGS) $(FASTIOINCLUDEPATH) -include pch.hpp
pch.hpp.gch:pch.hpp
	$(CXX) -c pch.hpp $(CXXFLAGS) $(FASTIOINCLUDEPATH)
clean:
	rm *.gch *.tmp *.txt
distclean:
	rm filebuf_file.exe fpipe.exe fstream.exe iostream.exe out_buf.exe stdio.exe stdio_hack.exe *.gch *.tmp *.txt
