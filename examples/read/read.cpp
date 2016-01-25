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
			"\"dimensions\": {"
				"\"width\": 320,\n"
				"\"height\": 240\n"
			"},"
			"\"properties\": [32, {\"greeting\":\"hello\"}, [1, 2, 3]]\n"
		"}\n";

	auto reader = MJson::Reader::Create();

	try {
		MJson::ElementPtr root = reader->Read(stream);

		std::cout << "width: " << root->Get("dimensions/width")->AsInt() << std::endl;
		std::cout << "height: " << root->Get("dimensions/height")->AsInt() << std::endl;
		
		std::cout << "greeting: " << root->Get("properties/%v/%v", {1, "greeting"})->AsStr() << std::endl;
	}

	catch(MJson::MJsonException ex)
	{
		std::cerr << ex.what() << std::endl;
	}

	return 0;
}
