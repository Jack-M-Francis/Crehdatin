#include "FormatPost.h"

GreenTextFormat::GreenTextFormat(){exclusive = false;}

bool GreenTextFormat::isValidBufferBegin(const std::string& buffer){
	if(buffer.size() > 4){
		return false;
	}
	return buffer == std::string("&gt;").substr(0, buffer.size());
}

bool GreenTextFormat::isExactBufferBegin(const std::string& buffer){
	return buffer == "&gt;";
}

std::string GreenTextFormat::startTag(const std::string& buffer){
	return "<div class='greenText'>" + buffer;
}

bool GreenTextFormat::isValidBufferEnd(const std::string& buffer){
	return buffer == "\n";
}

bool GreenTextFormat::isExactBufferEnd(const std::string& buffer){
	return buffer == "\n";
}

std::string GreenTextFormat::endTag(const std::string& buffer){
	return "</div>" + buffer;
}

//====

RainbowTextFormat::RainbowTextFormat(){exclusive = false;}

bool RainbowTextFormat::isValidBufferBegin(const std::string& buffer){
	if(buffer.size() > 11){
		return false;
	}
	return buffer == std::string("%%").substr(0, buffer.size());
}

bool RainbowTextFormat::isExactBufferBegin(const std::string& buffer){
	return buffer == "%%";
}

std::string RainbowTextFormat::startTag(const std::string& buffer){
	return "<div class='rainbowText'>";
}

bool RainbowTextFormat::isValidBufferEnd(const std::string& buffer){
	if(buffer.size() > 11){
		return false;
	}
	return buffer == std::string("%%").substr(0, buffer.size());
}

bool RainbowTextFormat::isExactBufferEnd(const std::string& buffer){
	return buffer == "%%";
}

std::string RainbowTextFormat::endTag(const std::string& buffer){
	return "</div>";
}

//====

ItalicFormat::ItalicFormat(){exclusive = false;}

bool ItalicFormat::isValidBufferBegin(const std::string& buffer){
	if(buffer.size() > 2){
		return false;
	}
	return buffer == std::string("**").substr(0, buffer.size());
}

bool ItalicFormat::isExactBufferBegin(const std::string& buffer){
	return buffer == "**";
}

std::string ItalicFormat::startTag(const std::string& buffer){
	return "<i>";
}

bool ItalicFormat::isValidBufferEnd(const std::string& buffer){
	if(buffer.size() > 2){
		return false;
	}
	return buffer == std::string("**").substr(0, buffer.size());
}

bool ItalicFormat::isExactBufferEnd(const std::string& buffer){
	return buffer == "**";
}

std::string ItalicFormat::endTag(const std::string& buffer){
	return "</i>";
}

//====

BoldFormat::BoldFormat(){exclusive = false;}

bool BoldFormat::isValidBufferBegin(const std::string& buffer){
	if(buffer.size() > 2){
		return false;
	}
	return buffer == std::string("^^").substr(0, buffer.size());
}

bool BoldFormat::isExactBufferBegin(const std::string& buffer){
	return buffer == "^^";
}

std::string BoldFormat::startTag(const std::string& buffer){
	return "<b>";
}

bool BoldFormat::isValidBufferEnd(const std::string& buffer){
	if(buffer.size() > 2){
		return false;
	}
	return buffer == std::string("^^").substr(0, buffer.size());
}

bool BoldFormat::isExactBufferEnd(const std::string& buffer){
	return buffer == "^^";
}

std::string BoldFormat::endTag(const std::string& buffer){
	return "</b>";
}

//===

HyperLinkFormat::HyperLinkFormat(){exclusive = true;}

bool HyperLinkFormat::isValidBufferBegin(const std::string& buffer){
	if(buffer.size() <= 8 && buffer == std::string("https://").substr(0, buffer.size())){
		return true;
	}
	if(buffer.size() <= 7 && buffer == std::string("http://").substr(0, buffer.size())){
		return true;
	}
	return false;
}

bool HyperLinkFormat::isExactBufferBegin(const std::string& buffer){
	return buffer == "https://" || buffer == "http://";
}

bool HyperLinkFormat::isValidBufferEnd(const std::string& buffer){
	return buffer == " " || buffer == "\n";
}

bool HyperLinkFormat::isExactBufferEnd(const std::string& buffer){
	return buffer == " " || buffer == "\n";
}

std::string HyperLinkFormat::finishExclusiveFormat(const std::string& internal){
	return "<a href='" + startTag + internal + "'>" + startTag + internal + "</a>" + endTag;
}

void HyperLinkFormat::gotBeginTag(const std::string& buffer){
	startTag = buffer;
}

void HyperLinkFormat::gotEndTag(const std::string& buffer){
	endTag = buffer;
}

//===

std::string formatUserPostBody(std::string body, std::string userPosition){
	replaceAll(body, "\r", "");
	
	GreenTextFormat greenTextFormat;
	ItalicFormat italicFormat;
	BoldFormat boldFormat;
	HyperLinkFormat hyperLinkFormat;
	RainbowTextFormat rainbowTextFormat;
	
	std::vector<FormatPrimitive*> unusedFormats{&greenTextFormat, &italicFormat, &boldFormat, &hyperLinkFormat};
	if(hasRainbowTextPermissions(userPosition)){
		unusedFormats.push_back(&rainbowTextFormat);
	}
	
	std::stack<FormatPrimitive*> formatStack;
	
	std::string output;
	std::string buffer;
	std::string exclusiveContents;
	
	for(int i = 0; i < body.size(); i++){
		buffer += body[i];
		
		if(!formatStack.empty() && formatStack.top()->exclusive){
			while(buffer.size() > 0 && !formatStack.top()->isValidBufferEnd(buffer)){
				exclusiveContents += buffer[0];
				buffer.erase(0, 1);
			}
			if(formatStack.top()->isExactBufferEnd(buffer)){
				formatStack.top()->gotEndTag(buffer);
				buffer.clear();
				output += formatStack.top()->finishExclusiveFormat(exclusiveContents);
				exclusiveContents.clear();
				unusedFormats.push_back(formatStack.top());
				formatStack.pop();
			}
		}
		else{
			bool anythingValid;
			do{
				anythingValid = false;
				for(auto y = unusedFormats.begin(); y != unusedFormats.end(); y++){
					if((*y)->isValidBufferBegin(buffer)){
						anythingValid = true;
					}
				}
				
				if(!formatStack.empty() && formatStack.top()->isValidBufferEnd(buffer)){
					anythingValid = true;
				}
				
				if(anythingValid == false){
					output += buffer[0];
					buffer.erase(0, 1);
				}
				
			}while(buffer.size() > 0 && anythingValid == false);
			
			if(!formatStack.empty() && formatStack.top()->isExactBufferEnd(buffer)){
				output += formatStack.top()->endTag(buffer);
				unusedFormats.push_back(formatStack.top());
				formatStack.pop();
				buffer.clear();
			}
			else{
				for(auto y = unusedFormats.begin(); y != unusedFormats.end(); y++){
					if((*y)->isExactBufferBegin(buffer)){
						formatStack.push(*y);
						y = unusedFormats.erase(y);
						if(!formatStack.top()->exclusive){
							output += formatStack.top()->startTag(buffer);
						}
						else{
							formatStack.top()->gotBeginTag(buffer);
						}
						buffer.clear();
						if(y == unusedFormats.end()){
							break;
						}
						y--;
					}
				}
			}
		}
	}
	
	if(formatStack.empty()){
		output += buffer;
		buffer.clear();
	}
	else{
		if(formatStack.top()->exclusive){
			exclusiveContents += buffer;
			buffer.clear();
			formatStack.top()->gotEndTag(buffer);
			output += formatStack.top()->finishExclusiveFormat(exclusiveContents);
			exclusiveContents.clear();
			formatStack.pop();
		}
		output += buffer;
		buffer.clear();
		while(!formatStack.empty()){
			output += formatStack.top()->endTag(buffer);
			formatStack.pop();
		}
	}
	
	replaceAll(output, "\n", "<br>");
	trimString(output);
	
	return output;
}




