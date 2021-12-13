#include <primitives/block.h>
#include <primitives/transaction.h>
#include <streams.h>
#include <util/system.h>
#include <util/strencodings.h>

#include <iostream>
#include <memory>
#include <map>
#include <vector>

//! barrystyle 04032022

bool ledger_debug = false;
std::map<std::string, CAmount> transaction_ledger;
std::map<uint256, CTransaction> transaction_index;

void store_transaction(uint256& hash, CTransaction& tx) {
    transaction_index.emplace(std::pair<uint256, CTransaction>(hash, tx));
}

CTransaction retrieve_transaction(uint256& hash) {
    std::map<uint256, CTransaction>::iterator it = transaction_index.begin();
    while (it != transaction_index.end()) {
        uint256 search_hash = it->first;
        if (search_hash == hash) {
            return it->second;
        }
        it++;
    }
    return CMutableTransaction();
}

bool fetch_prevtx_addramt(uint256& hash, unsigned int& n, CScript& prevdest, CAmount& prevamount) {
    try {
        CTransaction txprev = retrieve_transaction(hash);
        if (txprev == CMutableTransaction()) {
            return false;
        }
        prevdest = txprev.vout[n].scriptPubKey;
        prevamount = txprev.vout[n].nValue;
    } catch (...) {
        return false;
    }
    return true;
}

void subtract_from_address(std::string& address, CAmount& amount) {
    if (amount <= 0) return;
    try {
        transaction_ledger.find(address)->second -= amount;
        if (ledger_debug) {
            printf("debited %llu from address %s\n", amount, address.c_str());
        }
    } catch (...) {
        if (ledger_debug) {
            printf("tried to subtract from address with no balance\n");
        }
    }
}

void credit_to_address(std::string& address, CAmount& amount) {
    if (amount <= 0) return;
    try {
        transaction_ledger.find(address)->second += amount;
        if (ledger_debug) {
            printf("credited %llu to address %s\n", amount, address.c_str());
        }
    } catch (...) {
        transaction_ledger.emplace(std::pair<std::string, CAmount>(address, amount));
        if (ledger_debug) {
            printf("credited %llu to new address %s\n", amount, address.c_str());
        }
    }
}

void process_transaction(CTransaction& tx_ref) {
    for (unsigned int i=0; i<tx_ref.vin.size(); i++) {
        CTxIn txin = tx_ref.vin[i];
        if (tx_ref.IsCoinBase()) {
            continue;
        }
        uint256 txprev_hash = tx_ref.vin[i].prevout.hash;
        unsigned int n = tx_ref.vin[i].prevout.n;
        CScript prevdest;
        CAmount prevamount;
        if (!fetch_prevtx_addramt(txprev_hash, n, prevdest, prevamount)) {
            continue;
        }
        std::string address = HexStr(prevdest);
        subtract_from_address(address, prevamount);
    }

    for (unsigned int i=0; i<tx_ref.vout.size(); i++) {
        CTxOut txout = tx_ref.vout[i];
        CScript dest = txout.scriptPubKey;
        CAmount amount = txout.nValue;
        std::string address = HexStr(dest);
        credit_to_address(address, amount);
    }
}