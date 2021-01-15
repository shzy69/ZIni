/*  ZIni: A Real Crude Cross-Platform Ini-File-Reader Written by C++
*
*   version  :  v0.0.1
*   Author   :  SHZY
*   Email    :  188813950@qq.com
*   now-time :  2021-1-13
*
***************************************
*
*   Happy Using!
*
*/

#ifndef __INI_H__
#define __INI_H__

#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include "stdafx.h"

class ZIni
{
public:
	// 构造函数
	explicit ZIni(const char *filePath);

	// 析够函数
	virtual ~ZIni() {};
public:
	template<typename T>
	T get(const char *mainKey, const char *subKey);

private:
	std::map<std::string, std::map<std::string, std::string>> mainMap;

private:
	std::string::size_type removeSpace(std::string &str)
	{
		str.erase(0, str.find_first_not_of(" "));
		str.erase(str.find_last_not_of(" \r") + 1); //防止有换行符\r\n遗漏的\r
		return str.length();
	}
};

inline ZIni::ZIni(const char *filePath)
{
	std::ifstream fs(filePath);
	std::stringstream ss;
	ss << fs.rdbuf();
	std::string filestring(ss.str()); //直接从文件读出来的string

	//开始遍历该文件所有的字符串，争取只遍历一次
	std::string mainKey(""), subKey(""), subValue("");
	std::map<std::string, std::string> subMap;
	std::string::size_type lastLineBreakIndex;  //记录上一个换行符的位置，为了保证复杂度O(n)，所以遍历到=符号的时候，需要记住该位置，直接取出前面的key值
	std::string::size_type firstValidIndex = filestring.find_first_of('['); //第一个'['的位置
	auto i = firstValidIndex;
	while (i < filestring.length())
	{
		switch (filestring[i])
		{
		case '[':
			//先把上一个mainKey的东西insert进去保存
			if (removeSpace(mainKey) != 0 && subMap.size() != 0)
			{
				mainMap.insert({ mainKey, subMap });
			}
			mainKey.clear();
			subMap.clear();
			for (auto j = i + 1; j < filestring.length(); ++j)
			{
				if (filestring[j] == ']')
				{
					std::string buf(filestring, i + 1, j - i - 1);
					mainKey = std::move(buf);
					i = j; //直接让索引i跳到该闭方括号的位置，之后i++接着遍历']'后面的字符
					break;
				}
			}
			break;
		case ';':
			for (auto j = i + 1; j < filestring.length(); ++j)
			{
				if (filestring[j] == '\n')
				{
					i = j;
					lastLineBreakIndex = i;
					break;
				}
			}
			break;
		case '\n':
			lastLineBreakIndex = i;
			break;
		case '=':
			std::string buf(filestring, lastLineBreakIndex + 1, i - lastLineBreakIndex - 1);
			subKey = std::move(buf);
			bool isLastLine = true; //是否为最后一行。实际上就算此变量为false，也不能保证这一定不是最后一行（可能文件末尾有个\n），他只作为一个flag，看下面的for循环是否进入了其中的if语句
			for (auto j = i + 1; j < filestring.length(); ++j)
			{
				if (filestring[j] == '\n')
				{
					isLastLine = false;
					std::string buffer(filestring, i + 1, j - i - 1);
					subValue = std::move(buffer);
					i = j;
					lastLineBreakIndex = i;
					break;
				}
			}
			if (isLastLine)
			{
				//如果该for循环完了之后，都没有找到'\n'，那么这行就是文件末尾了，那就把'='号之后的全部当成subValue
				std::string buffer(filestring, i + 1, std::string::npos);
				subValue = std::move(buffer);
			}
			if ((removeSpace(subKey) != 0) && (removeSpace(subValue) != 0))
			{
				subMap.insert({ subKey, subValue });
			}
			subKey.clear();
			subValue.clear();
			break;
		}
		i++;
	}
	if (removeSpace(mainKey) != 0 && subMap.size() != 0)
	{
		mainMap.insert({ mainKey, subMap });
	}
}



#endif