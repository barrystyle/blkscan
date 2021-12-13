#ifndef INDEXMAN_H
#define INDEXMAN_H

#include <uint256.h>
#include <primitives/transaction.h>

#include <map>

extern std::map<std::string, CAmount> transaction_ledger;
extern std::map<uint256, CTransaction> transaction_index;

void store_transaction(uint256& hash, CTransaction& tx);
CTransaction retrieve_transaction(uint256& hash);
void process_transaction(CTransaction&);

#endif // INDEXMAN_H
