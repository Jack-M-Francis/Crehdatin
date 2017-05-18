#pragma once

#include "StringHelper.h"
#include <stack>
#include <vector>

class FormatPrimitive{
public:
	virtual ~FormatPrimitive() = default;
	virtual bool isValidBufferBegin(const std::string& buffer) = 0;
	virtual bool isExactBufferBegin(const std::string& buffer) = 0;
	virtual std::string startTag(const std::string& buffer){throw std::invalid_argument("Can not call startTag on this format");};
	virtual bool isValidBufferEnd(const std::string& buffer) = 0;
	virtual bool isExactBufferEnd(const std::string& buffer) = 0;
	virtual std::string endTag(const std::string& buffer){throw std::invalid_argument("Can not call endTag on this format");};
	virtual std::string finishExclusiveFormat(const std::string& internal){throw std::invalid_argument("cannot call finishExclusiveFormat on this format");};
	virtual void gotBeginTag(const std::string& buffer){}
	virtual void gotEndTag(const std::string& buffer){}
	bool exclusive;
};

class GreenTextFormat: public FormatPrimitive{
public:
	GreenTextFormat();
	virtual bool isValidBufferBegin(const std::string& buffer);
	virtual bool isExactBufferBegin(const std::string& buffer);
	virtual std::string startTag(const std::string& buffer);
	virtual bool isValidBufferEnd(const std::string& buffer);
	virtual bool isExactBufferEnd(const std::string& buffer);
	virtual std::string endTag(const std::string& buffer);
};

class ItalicFormat: public FormatPrimitive{
public:
	ItalicFormat();
	virtual bool isValidBufferBegin(const std::string& buffer);
	virtual bool isExactBufferBegin(const std::string& buffer);
	virtual std::string startTag(const std::string& buffer);
	virtual bool isValidBufferEnd(const std::string& buffer);
	virtual bool isExactBufferEnd(const std::string& buffer);
	virtual std::string endTag(const std::string& buffer);
};

class BoldFormat: public FormatPrimitive{
public:
	BoldFormat();
	virtual bool isValidBufferBegin(const std::string& buffer);
	virtual bool isExactBufferBegin(const std::string& buffer);
	virtual std::string startTag(const std::string& buffer);
	virtual bool isValidBufferEnd(const std::string& buffer);
	virtual bool isExactBufferEnd(const std::string& buffer);
	virtual std::string endTag(const std::string& buffer);
};

class HyperLinkFormat: public FormatPrimitive{
public:
	HyperLinkFormat();
	virtual bool isValidBufferBegin(const std::string& buffer);
	virtual bool isExactBufferBegin(const std::string& buffer);
	virtual bool isValidBufferEnd(const std::string& buffer);
	virtual bool isExactBufferEnd(const std::string& buffer);
	virtual std::string finishExclusiveFormat(const std::string& internal);
	virtual void gotBeginTag(const std::string& buffer);
	virtual void gotEndTag(const std::string& buffer);
	std::string startTag;
	std::string endTag;
};

std::string formatUserPostBody(std::string body);

