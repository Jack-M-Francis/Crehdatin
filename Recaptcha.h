#pragma once

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <string>

#include <sstream>

bool validateRecaptcha(std::string response, std::string ip);