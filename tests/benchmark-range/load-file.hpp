#ifndef LOAD_FILE_HPP
#define LOAD_FILE_HPP

#include <memory>
#include <stdexcept>
#include <cstdio>
#include <string>

struct FileDescriptorDeleter {
	inline void operator()(FILE * f) const noexcept {
		fclose(f);
	}
};

[[nodiscard]] inline std::string load_file(const char * fname) {
	std::unique_ptr<FILE, FileDescriptorDeleter> fptr{fopen(fname, "rb")};
	if (!fptr) {
		throw std::runtime_error{"can't open file"};
	}
	fseek(fptr.get(),0,SEEK_END);
	auto size = ftell(fptr.get());
	fseek(fptr.get(),0,SEEK_SET);
	std::string output;
	output.resize(size);
	fread(output.data(),1,size,fptr.get());
	return output;
}

#endif // LOAD_FILE_HPP
