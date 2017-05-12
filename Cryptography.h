#pragma once

#include <mutex>

#include <cryptopp/osrng.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/whrlpool.h>
#include <string>

//no bloods

std::string generateRandomToken();
std::string generateSecureHash(std::string data);