#include "mjson.h"

namespace MJson
{
	class CElement : public Element
	{
		public:
		std::map<std::string, ElementPtr> dict;
		std::vector<ElementPtr> list;

		std::string strVal;
		int64_t intVal;
		double floatVal;
		bool boolVal;
		
		virtual const std::map<std::string, ElementPtr>& AsDict()
		{
			if(type != ETDict)
				throw MJsonException("not a dictionary");

			return dict;
		}

		virtual const std::vector<ElementPtr>& AsList()
		{
			if(type != ETList)
				throw MJsonException("not a list");

			return list;
		}

		virtual const std::string& AsStr()
		{
			if(type != ETStr)
				throw MJsonException("not a string");

			return strVal;
		}
		
		virtual int64_t AsInt()
		{
			if(type != ETInt)
				throw MJsonException("not an int");

			return intVal;
		}

		virtual double AsFloat()
		{
			if(type != ETInt)
				throw MJsonException("not a floating point value");

			return floatVal;
		}

		virtual bool AsBool()
		{
			if(type != ETInt)
				throw MJsonException("not a boolean");

			return boolVal;
		}
	};

	class CParser : public Parser
	{
		public:
		CParser()
		{
		}

		void Parse(ReadCharFn readChar)
		{
			int c = 0;
			while((c = readChar()) >= 0){
				if(c < 32)
					continue;


			}
		}
	};
		
	ParserPtr Parser::Create()
	{
		return std::make_shared<CParser>();
	}
}
