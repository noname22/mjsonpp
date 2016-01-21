#include <iostream>
#include <fstream>

#include "mjson.h"

int main(int argc, char** argv)
{
	if(argc != 2){
		std::cerr << "usage: " << argv[0] << " [json file]" << std::endl;
		exit(1);
	}

	// Read a json file and write it to stdout

	std::ifstream file(argv[1]);

	auto reader = MJson::Reader::Create();
	auto writer = MJson::Writer::Create();

	try {
		MJson::ElementPtr root = reader->Read(file);
		writer->Write(root, std::cout);
	}

	catch(MJson::MJsonException ex)
	{
		std::cerr << ex.what() << std::endl;
	}

	file.close();

	return 0;
}
