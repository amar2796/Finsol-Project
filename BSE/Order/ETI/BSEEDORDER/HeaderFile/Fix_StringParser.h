#ifndef FIN_ORDER_GATEWAY_STRING_PARSER
#define FIN_ORDER_GATEWAY_STRING_PARSER

#include <iostream>
#include <sstream>
#include <unordered_map>
#include "Common.h"

class FixStringParser
{

public:

	inline void loadString(const std::string& inputString)
	{

		reset();

		std::istringstream f(inputString.c_str());
		std::string s;
		int pos = 0;
		while (getline(f, s, keyValPairSep)) {
			pos = s.find(keyValSep);
			_mapKeyValues[s.substr(0, pos)] = s.substr(pos + 1);
		}

		_itr = _mapKeyValues.begin();
		_itrEnd = _mapKeyValues.end();
	}

	inline void reset()
	{
		_mapKeyValues.clear();
	}

	inline bool getIfSet(const std::string& key, std::string& output)
	{
		_itr = _mapKeyValues.find(key);
		if (_itr == _itrEnd)
		{
			//std::cout <<"[INFO] " << "Unable to find field: " << key << std::endl ;
			return false;
		}
		output = _itr->second;
		return true;
	}
	inline const std::string get_multi(const std::string& key)
	{
		return _mapKeyValues[key];
	}
	inline std::string get(const std::string& key)
	{

		//std::cout <<"[INFO] " << "Key: " << key << std::endl ;
		return _mapKeyValues[key];

	}
	inline char getChar(const std::string& key)
	{
		//std::cout <<"[INFO] " << "Key: " << key << std::endl ;
		return _mapKeyValues[key].at(0);
	}

	inline void print()
	{
		_itr = _mapKeyValues.begin();

		while (_itr != _itrEnd)
		{
			std::cout <<"[INFO] " << _itr->first << " = [" << _itr->second << "]" << std::endl;
			++_itr;
		}
	}
private:

	char keyValSep = '=';
	char keyValPairSep = 1; // SOH

	FIN::FixKeyValue _mapKeyValues;
	FIN::FixKeyValue::iterator _itr;
	FIN::FixKeyValue::iterator _itrEnd;
};

#endif