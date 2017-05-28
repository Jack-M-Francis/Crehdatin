#pragma once

#include "scrypt/crypto_scrypt.h"

#include <mutex>

#include <cryptopp/osrng.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <string>
#include <tgmath.h>

#include "Config.h"

//no bloods

std::string generateRandomToken();

std::string generateSecureHash(std::string password, std::string salt);