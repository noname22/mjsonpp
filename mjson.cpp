#include <iostream>
#include <sstream>
#include <cctype>
#include <queue>

#include "mjson.h"

#define MSTR(_what) [&]() -> std::string { std::stringstream _s; _s << _what; return _s.str(); }()

namespace MJson
{
	typedef std::shared_ptr<class CElement> CElementPtr;

	class CElement : public Element
	{
		public:
		std::map<std::string, ElementPtr> dict;
		std::vector<ElementPtr> list;

		std::string strVal;
		int64_t intVal;
		double floatVal;
		bool boolVal;
		
		virtual std::map<std::string, ElementPtr>& AsDict()
		{
			if(type != ETDict)
				throw MJsonException("not a dictionary");

			return dict;
		}

		virtual std::vector<ElementPtr>& AsList()
		{
			if(type != ETList)
				throw MJsonException("not a list");

			return list;
		}

		virtual std::string& AsStr()
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
			if(type != ETFloat)
				throw MJsonException("not a floating point value");

			return floatVal;
		}

		virtual bool AsBool()
		{
			if(type != ETBool)
				throw MJsonException("not a boolean");

			return boolVal;
		}
	};

	class CParser : public Parser
	{
		public:
		int line;
		ReadCharFn readChar;
		int peek = 0;

		CParser()
		{
		}

		int ReadChar()
		{
			int c = peek;
			peek = readChar();
			
			if(c == '\n'){
				line++;
			}

			if(c < 0){
				throw MJsonException(MSTR("unexpected end of file at line: " << line));
			}

			return c;
		}

		void ParseString(std::string& str)
		{
			int c = 0;

			while((c = ReadChar()) != '"'){
				str.push_back((char)c);
			}
		}

		void ParseKey(std::string& key)
		{
			int c = 0;

			ParseString(key);

			while((c = ReadChar()) != ':'){
				if(c > 32)
					throw MJsonException(MSTR("expected '\"key\":' at line: " << line));
			}
		}
		
		bool ParseBool()
		{
			int c = ReadChar();

			if(c == 't'){
				if(ReadChar() == 'r' && ReadChar() == 'u' && ReadChar() == 'e')
					return true;
				else
					throw MJsonException(MSTR("error parsing boolean at line: " << line));
			}

			else if(c == 'f'){
				if(ReadChar() == 'a' && ReadChar() == 'l' && ReadChar() == 's' && ReadChar() == 'e')
					return false;
				else
					throw MJsonException(MSTR("error parsing boolean at line: " << line));
			}

			throw MJsonException(MSTR("error parsing boolean at line: " << line));
		}

		ElemType ParseNumber(int64_t& i, double& f)
		{
			std::string num;
			bool wasDot = false;

			while(true)
			{
				int c = peek;

				if(!(c == '.' || isdigit(c)))
					break;

				if(c == '.')
					wasDot = true;

				num.push_back(c);
				ReadChar();
			}

			try
			{
				if(wasDot){
					f = std::stod(num);
					return ETFloat;
				}

				else {
					i = std::stol(num);
					return ETInt;
				}
			}
			catch (std::invalid_argument)
			{
				throw MJsonException(MSTR("error parsing number at line: " << line));
			}
		}

		CElementPtr ParseValue()
		{
			CElementPtr elem = std::make_shared<CElement>();

			while(true){
				int c = peek;

				if(c <= 32){
					ReadChar();
					continue;
				}

				if(c == '"'){
					elem->type = ETStr;
					ReadChar();
					ParseString(elem->strVal);
					break;
				}

				else if(c == 't' || c == 'f'){
					elem->type = ETBool;
					elem->boolVal = ParseBool();
					break;
				}

				else if(c == '.' || isdigit(c)){
					elem->type = ParseNumber(elem->intVal, elem->floatVal);
					break;
				}

				else if(c == '{'){
					ReadChar();
					return ParseDict();
				}

				else if(c == '['){
					ReadChar();
					return ParseList();
				}

				throw MJsonException(MSTR("expected value at line: " << line));
			}

			return elem;
		}

		CElementPtr ParseDict()
		{
			CElementPtr elem = std::make_shared<CElement>();
			elem->type = ETDict;
			bool acceptComma = false;

			while(true){
				int c = peek;

				if(c <= 32){
					ReadChar();
					continue;
				}

				else if(c == '}'){
					ReadChar();
					return elem;
				}
				
				else if(c == '"'){
					ReadChar();
					std::string key;
					ParseKey(key); 

					elem->dict[key] = ParseValue();
					acceptComma = true;
					continue;
				}

				else if(acceptComma && c == ','){
					ReadChar();
					continue;
				}

				throw MJsonException(MSTR("expected key for a value ('\"key\":') or end of dictionary ('}') at line: " << line));
			}
		}

		CElementPtr ParseList()
		{
			CElementPtr elem = std::make_shared<CElement>();
			elem->type = ETList;
			bool acceptComma = false;

			while(true){
				int c = peek;

				if(c <= 32){
					ReadChar();
					continue;
				}
				
				else if(acceptComma && c == ','){
					ReadChar();
					continue;
				}

				else if(c == ']'){
					ReadChar();
					return elem;
				}
				
				else {
					elem->list.push_back(ParseValue());
					acceptComma = true;
					continue;
				}

				throw MJsonException(MSTR("expected a value or end of list (']') at line: " << line));
			}
		}

		ElementPtr Parse(ReadCharFn readChar)
		{
			line = 1;
			this->readChar = readChar;
			ReadChar(); // set up peek

			int c = ReadChar();

			if(c == '{')
				return ParseDict();

			else if(c == '[')
				return ParseList();

			throw MJsonException("Expected root element to be either a dictionary ({}) or a list ([]).");
		}
	};
		
	ParserPtr Parser::Create()
	{
		return std::make_shared<CParser>();
	}
}
