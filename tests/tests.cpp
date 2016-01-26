#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <functional>
#include <sstream>

#include "mjson.h"

#define T_ASSERT(_a) if(!(_a)){\
	std::cerr << "'" << #_a << "' " << "failed @ " << __FILE__ << ":" << __LINE__ << std::endl; return false; }

bool TestBasic()
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

		T_ASSERT(root->AsDict()["dimensions"]->AsDict()["width"]->AsInt() == 320);
		T_ASSERT(root->AsDict()["dimensions"]->AsDict()["height"]->AsInt() == 240);
		
		T_ASSERT(root->AsDict()["tags"]->AsList()[0]->AsStr() == "first");
		T_ASSERT(root->AsDict()["tags"]->AsList()[1]->AsStr() == "second");
		T_ASSERT(root->AsDict()["tags"]->AsList()[2]->AsStr() == "third");
	}

	catch(MJson::MJsonException ex)
	{
		std::cerr << ex.what() << std::endl;
		return false;
	}

	return true;
}

bool TestPaths()
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

		T_ASSERT(root->Get("dimensions/width")->AsInt() == 320);
		T_ASSERT(root->Get("dimensions/height")->AsInt() == 240);

		int nTags = root->Get("tags")->AsList().size();

		T_ASSERT(nTags == 3);

		std::vector<std::string> expected = {"first", "second", "third"};

		for(int i = 0; i < nTags; i++){
			T_ASSERT(root->Get("tags/%v", {i})->AsStr() == expected[i]); 
		}
	}

	catch(MJson::MJsonException ex)
	{
		std::cerr << ex.what() << std::endl;
		return false;
	}

	return true;
}

bool TestPathsEscaping()
{
	std::stringstream stream; 
	stream <<
		"{\n"
			"\"/\": {"
				"\"%v\": 123,\n"
				"\"\\\": 456,\n"
			"},"
		"}\n";

	auto reader = MJson::Reader::Create();

	try {
		MJson::ElementPtr root = reader->Read(stream);

		T_ASSERT(root->Get("\\//%%v")->AsInt() == 123);
		T_ASSERT(root->Get("\\//\\\\")->AsInt() == 456);
	}

	catch(MJson::MJsonException ex)
	{
		std::cerr << ex.what() << std::endl;
		return false;
	}

	return true;
}

int main(int argc, char** argv)
{
	std::map<std::string, std::function<bool()>> tests;

	tests["TestBasic"] = std::bind(TestBasic);
	tests["TestPaths"] = std::bind(TestPaths);
	tests["TestPathsEscaping"] = std::bind(TestPathsEscaping);

	std::vector<std::string> runTests;

	if(argc > 1){
		for(int i = 1; i < argc; i++)
			runTests.push_back(argv[i]);
	}else{
		for(auto t : tests)
			runTests.push_back(t.first);
	}

	int ret = 0;

	for(auto t : runTests){
		std::cout << t << ": ";
		std::cout.flush();

		bool success = tests[t]();

		if(!success)
			ret = 1;

		std::cout << (success ? "ok" : "failed") << std::endl;
	}

	return ret;
}
