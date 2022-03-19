#ifndef INDEXMAN_H
#define INDEXMAN_H

#include <uint256.h>
#include <primitives/transaction.h>

#include <map>

extern std::map<CScript, CAmount> transaction_ledger[256][256];
extern std::map<uint256, CTransaction> transaction_index[256][256];

void store_transaction(uint256& hash, CTransaction& tx);
CTransaction retrieve_transaction(uint256& hash);
void process_transaction(CTransaction&);
void dump_ledger();

#endif // INDEXMAN_H
