#include <iostream>
#include <fstream>

#include "mjson.h"

void TraversePrint(MJson::ElementPtr root, int depth)
{
	switch(root->type){
		case MJson::ETDict:
			std::cout << "{" << std::endl;
			for(auto& kv : root->AsDict()){
				std::cout << std::string(depth + 1, ' ') << "\"" << kv.first << "\": ";
				TraversePrint(kv.second, depth + 1);
				std::cout << "," << std::endl;
			}
			std::cout << std::string(depth, ' ') << "}";
			break;

		case MJson::ETList:
			std::cout << "[" << std::endl;
			for(auto& val : root->AsList()){
				std::cout << std::string(depth + 1, ' ');
				TraversePrint(val, depth + 1);
				std::cout << "," << std::endl;
			}
			std::cout << std::string(depth, ' ') << "]";
			break;

		case MJson::ETStr:
			std::cout << "\"" << root->AsStr() << "\"";
			break;

		case MJson::ETInt:
			std::cout << root->AsInt();
			break;

		case MJson::ETFloat:
			std::cout << root->AsFloat();
			break;

		case MJson::ETBool:
			std::cout << (root->AsBool() ? "true" : "false");
			break;

		default:
			break;
	}
}

int main(int argc, char** argv)
{
	if(argc != 2){
		std::cerr << "usage: " << argv[0] << " [json file]" << std::endl;
		exit(1);
	}

	std::ifstream file(argv[1]);

	auto parser = MJson::Parser::Create();

	try {
		MJson::ElementPtr root = parser->Parse([&]() -> int {
			char c;
			if(file.get(c))
				return c;
			return -1;		
		});

		//std::cout << root->AsDict()["somestuff"]->AsStr() << std::endl;
		TraversePrint(root, 0);
		std::cout << std::endl;
	}

	catch(MJson::MJsonException ex)
	{
		std::cerr << ex.what() << std::endl;
	}

	file.close();

	return 0;
}
