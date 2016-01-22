#ifndef MJSON_H
#define MJSON_H

#include <cstdint>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <stdexcept>
#include <ostream>

namespace MJson
{
	typedef std::function<int()> ReadCharFn;
	typedef std::function<void(char)> WriteCharFn;
	typedef std::shared_ptr<class Element> ElementPtr;
	typedef std::shared_ptr<class Reader> ReaderPtr;
	typedef std::shared_ptr<class Writer> WriterPtr;
	
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

	class PathArg
	{
		friend class CElement;

		protected:
		bool isString;
		std::string strVal;
		int64_t intVal;

		public:
		PathArg(const char* strVal) : isString(true), strVal(strVal) {}
		PathArg(const std::string& strVal) : isString(true), strVal(strVal) {}
		PathArg(int64_t intVal) : isString(false), intVal(intVal) {}
		PathArg(int intVal) : isString(false), intVal(intVal) {}
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

		virtual ElementPtr Get(const std::string& path, const std::vector<PathArg> args = {}) = 0;
	};

	class Reader
	{
		public:
		virtual ElementPtr Read(std::istream& stream) = 0;
		// readCharFn must return 0-255 on success and < 0 on error (eg. -1)
		virtual ElementPtr Read(ReadCharFn readCharFn) = 0;

		static ReaderPtr Create();
	};

	class Writer
	{
		public:
		virtual void SetTab(const std::string& tab) = 0;
		virtual void SetEndl(const std::string& endl) = 0;

		virtual void Write(ElementPtr root, std::ostream& stream) = 0;
		virtual void Write(ElementPtr root, WriteCharFn writeCharFn) = 0;
		
		static WriterPtr Create();
	};
}

#endif
