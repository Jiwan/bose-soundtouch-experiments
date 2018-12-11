// An utility to extract files from a SoundTouch Update file (Update.stu).

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <iterator>
#include <iostream>
#include <filesystem>
#include <fstream>

template <class T>
T read_little_endian(std::ifstream& ifs)
{
	T t;
	auto* p = reinterpret_cast<char*>(&t);
	ifs.read(p, sizeof(t)); 
	std::reverse(p, p + sizeof(T));
	return t;
}

int main(int argc, char* argv[])
{
	if (argc != 3) {
		std::cout << "Need 3 arguments plox!" << std::endl;
		return EXIT_FAILURE;
	}

	std::ifstream ifs(argv[1], std::ios::binary);

	std::uint32_t header_size = read_little_endian<std::uint32_t>(ifs);

	assert(header_size == 0x14);
	ifs.seekg(header_size);

	std::filesystem::path dir(argv[2]);

	if (std::filesystem::exists(dir) && !std::filesystem::is_directory(dir)) {
		std::cout << "Ain't a dir my dear!" << std::endl;
		return EXIT_FAILURE;	
	}

	if (std::filesystem::exists(dir)) {
		if (!std::filesystem::is_directory(dir)) {
			std::cout << "Ain't a dir my dear!" << std::endl;
			return EXIT_FAILURE;
		} else {
			std::filesystem::remove_all(dir);	
		}
	}

	std::filesystem::create_directory(dir);

	while (ifs.peek() != EOF) {
		ifs.seekg(0x04, std::ios::cur);
		std::array<char, 0x80> buff;
		ifs.read(buff.data(), 0x80);

		std::string file_name(buff.begin(), buff.end());
		std::cout << "file name: " << file_name << "\n";

		std::uint32_t file_size = read_little_endian<std::uint32_t>(ifs);		
		std::cout << "file size: " << file_size << std::endl;

		std::uint32_t second_file_size = read_little_endian<std::uint32_t>(ifs);		
		std::cout << "second file size: " << second_file_size << std::endl;
		
		ifs.seekg(0x98, std::ios_base::cur);

		std::istreambuf_iterator<char> iit(ifs); 
		std::ofstream ofs((dir / file_name), std::ios::binary);
		std::ostreambuf_iterator<char> oit(ofs); 

		ifs.seekg(0x1, std::ios_base::cur);
		std::copy_n(iit, file_size, oit);
	}


	return 0;
}
