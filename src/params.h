#ifndef PARAMS_H
#define PARAMS_H

const int WITNESS_V0_SCRIPTHASH_SIZE = 32;
const int WITNESS_V0_KEYHASH_SIZE = 20;

const std::vector<unsigned char> pubkey_prefix = { 0x37 };
const std::vector<unsigned char> script_prefix = { 0xd };
const std::vector<unsigned char> secretkey = { 0xd4 };
const std::vector<unsigned char> extpubkey = { 0x02, 0x2d, 0x25, 0x33 };
const std::vector<unsigned char> extseckey = { 0x02, 0x21, 0x31, 0x2b };

const std::string bech32_hrp = "ph";

#endif // PARAMS_H
