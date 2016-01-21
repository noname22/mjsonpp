#ifndef MJSON_H
#define MJSON_H

#include <cstdint>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <stdexcept>

namespace MJson
{
	typedef std::function<int()> ReadCharFn;
	typedef std::shared_ptr<class Element> ElementPtr;
	typedef std::shared_ptr<class Parser> ParserPtr;
	
	struct MJsonException : public std::runtime_error
	{
		MJsonException(const std::string& str) : std::runtime_error(str) {}
	};

	enum ElemType
	{
		ETDict,
		ETList,
		ETStr,
		ETInt,
		ETFloat,
		ETBool,
	};

	class Element
	{
		public:
		ElemType type;

		virtual std::map<std::string, ElementPtr>& AsDict() = 0;
		virtual std::vector<ElementPtr>& AsList() = 0;
		virtual std::string& AsStr() = 0;
		virtual int64_t AsInt() = 0;
		virtual double AsFloat() = 0;
		virtual bool AsBool() = 0;
	};

	class Parser
	{
		public:
		virtual ElementPtr Parse(ReadCharFn) = 0;

		static ParserPtr Create();
	};
}

#endif
