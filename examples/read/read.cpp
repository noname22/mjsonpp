/*
	Parses a json string and prints out the parsed data.
*/

#include <iostream>
#include <fstream>
#include <sstream>

#include "mjson.h"

int main(int argc, char** argv)
{
	std::stringstream stream; 
	stream <<
		"{\n"
			"\"width\": 320,\n"
			"\"height\": 240,\n"
			"\"properties\": [32, \"hello\", [1, 2, 3]]\n"
		"}\n";

	auto reader = MJson::Reader::Create();

	try {
		MJson::ElementPtr root = reader->Read(stream);
		int i = 0;
		root->Get("width %v %v %v %v", {1, 2, "lel", i});
		//std::cout << "width is " << root->Get("width")->AsInt() << std::endl;
	}

	catch(MJson::MJsonException ex)
	{
		std::cerr << ex.what() << std::endl;
	}

	return 0;
}
