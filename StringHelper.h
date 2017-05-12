#pragma once

#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <functional> 
#include <cctype>
#include <locale>
#include <string>
#include <mutex>

// trim from start
static inline std::string &trimLeft(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
static inline std::string &trimRight(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
static inline std::string &trimString(std::string &s) {
	return trimLeft(trimRight(s));
}

static void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    std::size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

static void safePrint(std::string data){
	static std::mutex printLock;
	printLock.lock();
	std::cout << data;
	std::cout.flush();
	printLock.unlock();
}