#include <iostream>
#include <sstream>
#include <cctype>
#include <queue>
#include <sstream>

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

	class CReader : public Reader
	{
		public:
		int line;
		ReadCharFn readChar;
		int peek = 0;

		CReader()
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

		ElementPtr Read(ReadCharFn readChar)
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
		
		ElementPtr Read(std::istream& stream)
		{
			return Read([&]() -> int {
				char c;

				if(stream.get(c))
					return c;

				return -1;		
			});
		}
	};

	class StringBufWriter : public std::stringbuf
	{
		public:
		WriteCharFn writeFn;

		StringBufWriter(WriteCharFn writeFn) : writeFn(writeFn)
		{
		}

		virtual int sync()
		{
			for(char c : str())
				writeFn(c);
			
			str("");

			return 0;
		}
	};

	class CWriter: public Writer
	{
		public:
		std::string endl;
		std::string tab = "  ";

		CWriter()
		{
			std::stringstream ss;
			ss << std::endl;
			endl = ss.str();
		}

		virtual void SetTab(const std::string& tab)
		{
			this->tab = tab;
		}

		virtual void SetEndl(const std::string& endl)
		{
			this->endl = endl;
		}

		std::string GenTabs(int num)
		{
			std::stringstream ss;

			for(int i = 0; i < num; i++){
				ss << tab;
			}

			return ss.str();
		}

		void TraverseWrite(MJson::ElementPtr root, int depth, std::ostream& stream)
		{
			unsigned i = 0;

			switch(root->type){
				case MJson::ETDict:
					i = 0;
					stream << "{" << endl;

					for(auto& kv : root->AsDict()){
						stream << GenTabs(depth + 1) << "\"" << kv.first << "\": ";
						TraverseWrite(kv.second, depth + 1, stream);
						stream << (i++ < root->AsDict().size() - 1 ? "," : "") << endl;
					}
					stream << GenTabs(depth) << "}";
					break;

				case MJson::ETList:
					i = 0;
					stream << "[" << endl;
					for(auto& val : root->AsList()){
						stream << GenTabs(depth + 1);
						TraverseWrite(val, depth + 1, stream);
						stream << (i++ < root->AsList().size() - 1 ? "," : "") << endl;
					}
					stream << GenTabs(depth) << "]";
					break;

				case MJson::ETStr:
					stream << "\"" << root->AsStr() << "\"";
					break;

				case MJson::ETInt:
					stream << root->AsInt();
					break;

				case MJson::ETFloat:
					stream << root->AsFloat();
					break;

				case MJson::ETBool:
					stream << (root->AsBool() ? "true" : "false");
					break;

				default:
					break;
			}

			stream.flush();
		}

		void Write(ElementPtr root, WriteCharFn writeFn)
		{
			StringBufWriter buf(writeFn);
			std::ostream stream(&buf);
			Write(root, stream);
		}
		
		void Write(ElementPtr root, std::ostream& stream)
		{
			TraverseWrite(root, 0, stream);
			stream << endl;
			stream.flush();
		}
	};
		
	ReaderPtr Reader::Create()
	{
		return std::make_shared<CReader>();
	}
	
	WriterPtr Writer::Create()
	{
		return std::make_shared<CWriter>();
	}
}