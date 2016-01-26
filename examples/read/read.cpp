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
			"\"tags\": [\"first\", \"second\", \"third\"]\n"
		"}\n";

	auto reader = MJson::Reader::Create();

	try {
		MJson::ElementPtr root = reader->Read(stream);

		std::cout << "width: " << root->Get("dimensions/width")->AsInt() << std::endl;
		std::cout << "height: " << root->Get("dimensions/height")->AsInt() << std::endl;
		
		// read tags using paths
		for(int i = 0; i < (int)root->Get("tags")->AsList().size(); i++){
			std::cout << "tag (path): " << root->Get("tags/%v", {i})->AsStr() << std::endl;
		}

		// read tags without path
		for(auto tag : root->AsDict()["tags"]->AsList())
		{
			std::cout << "tag: " << tag->AsStr() << std::endl;
		}
	}

	catch(MJson::MJsonException ex)
	{
		std::cerr << ex.what() << std::endl;
	}

	return 0;
}
