#include "Cryptography.h"

std::string generateRandomToken(){
	static CryptoPP::AutoSeededRandomPool generator;
	static std::mutex randomLock;
	
	CryptoPP::SecByteBlock byteBlock(Config::getUniqueTokenLength() / 2);
	randomLock.lock();//make sure that this is called in a thread safe way
	generator.GenerateBlock(byteBlock, byteBlock.size());
	randomLock.unlock();
	
	std::string randomToken;
	CryptoPP::HexEncoder hex(new CryptoPP::StringSink(randomToken));
	
	hex.Put(byteBlock, byteBlock.size());
	hex.MessageEnd();
	
	return randomToken;
}

std::string generateSecureHash(std::string password, std::string salt){
    uint8_t derived[Config::getUniqueTokenLength() / 2];
    
    crypto_scrypt((const uint8_t*)password.c_str(), password.size(), (const uint8_t*)salt.c_str(), salt.size(), pow(2, 15), 8, 1, derived, sizeof(derived));
    
    std::string result;
    CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(result));

    encoder.Put(derived, sizeof(derived));
    encoder.MessageEnd();

    return result;
}