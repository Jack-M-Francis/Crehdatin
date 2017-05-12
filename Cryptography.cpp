#include "Cryptography.h"

std::string generateRandomToken(){
	static CryptoPP::AutoSeededRandomPool generator;
	static std::mutex randomLock;
	
	CryptoPP::SecByteBlock byteBlock(256);
	randomLock.lock();//make sure that this is called in a thread safe way
	generator.GenerateBlock(byteBlock, byteBlock.size());
	randomLock.unlock();
	
	std::string randomToken;
	CryptoPP::HexEncoder hex(new CryptoPP::StringSink(randomToken));
	
	hex.Put(byteBlock, byteBlock.size());
	hex.MessageEnd();
	
	return randomToken;
}

std::string generateSecureHash(std::string data){//for now we will be using the whirlpool hash provided by crypto++, I can always change this later
	byte digest[CryptoPP::Whirlpool::DIGESTSIZE];
	CryptoPP::Whirlpool().CalculateDigest(digest, (unsigned char*)data.c_str(), data.size());
	
	std::string output;
	
	CryptoPP::HexEncoder hex(new CryptoPP::StringSink(output));
	hex.Put(digest, CryptoPP::Whirlpool::DIGESTSIZE);
	hex.MessageEnd();
	
	return output;
}