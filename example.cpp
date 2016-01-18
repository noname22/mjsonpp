#include "mjson.h"

#include <iostream>

int main(int argc, char** argv)
{
	if(argc != 2){
		std::cerr << "usage: " << argv[0] << " [json file]" << std::endl;
		exit(1);
	}

	return 0;
}
