#include <key_io.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <script/standard.h>
#include <streams.h>
#include <util/system.h>
#include <util/strencodings.h>

#include <iostream>
#include <memory>
#include <map>
#include <vector>

//! barrystyle 04032022

bool ledger_debug = false;
std::map<CScript, CAmount> transaction_ledger[256];
std::map<uint256, CTransaction> transaction_index[256];

inline void which_map(uint256& hash, uint8_t& map_num) {
    map_num = *(uint8_t*)&hash;
}

inline void which_map(CScript& scr, uint8_t& map_num) {
    map_num = *(uint8_t*)&scr[0];
}

void store_transaction(uint256& hash, CTransaction& tx) {
    uint8_t b;
    which_map(hash, b);
    transaction_index[b].insert(std::pair<uint256, CTransaction>(hash, tx));
}

CTransaction* retrieve_transaction(uint256& hash) {
    uint8_t b;
    which_map(hash, b);
    std::map<uint256, CTransaction>::iterator it = transaction_index[b].begin();
    while (it != transaction_index[b].end()) {
        uint256 search_hash = it->first;
        if (search_hash == hash) {
            return &it->second;
        }
        it++;
    }
    return nullptr;
}

bool fetch_prevtx_addramt(uint256& hash, unsigned int& n, CScript& prevdest, CAmount& prevamount) {
    CTransaction *txprev = retrieve_transaction(hash);
    if (!txprev) {
        return false;
    }
    prevdest = txprev->vout[n].scriptPubKey;
    prevamount = txprev->vout[n].nValue;
    return true;
}

void subtract_from_address(CScript& dest, CAmount& amount) {
    uint8_t b;
    which_map(dest, b);
    std::map<CScript, CAmount>::iterator it = transaction_ledger[b].begin();
    while (it != transaction_ledger[b].end()) {
       CScript search_addr = it->first;
       if (search_addr == dest) {
           if (ledger_debug) {
               CTxDestination outputaddr;
               ExtractDestination(dest, outputaddr);
               printf("  debited %llu from address %s\n", amount, EncodeDestination(outputaddr).c_str());
           }
           it->second -= amount;
           return;
       }
       it++;
    }
    if (ledger_debug) {
       printf("  tried to debit from address with no balance\n");
    }
}

void credit_to_address(CScript& dest, CAmount& amount) {
    uint8_t b;
    which_map(dest, b);
    std::map<CScript, CAmount>::iterator it = transaction_ledger[b].begin();
    while (it != transaction_ledger[b].end()) {
       CScript search_addr = it->first;
       if (search_addr == dest) {
           if (ledger_debug) {
               CTxDestination outputaddr;
               ExtractDestination(dest, outputaddr);
               printf("  credited %llu to address %s\n", amount, EncodeDestination(outputaddr).c_str());
           }
           it->second += amount;
           return;
       }
       it++;
    }
    if (ledger_debug) {
        CTxDestination outputaddr;
        ExtractDestination(dest, outputaddr);
        printf("  credited %llu to new address %s\n", amount, EncodeDestination(outputaddr).c_str());
    }
    transaction_ledger[b].insert(std::pair<CScript, CAmount>(dest, amount));
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
        if (prevamount > 0) {
            subtract_from_address(prevdest, prevamount);
        }
    }

    for (unsigned int i=0; i<tx_ref.vout.size(); i++) {
        CTxOut txout = tx_ref.vout[i];
        CScript dest = txout.scriptPubKey;
        CAmount amount = txout.nValue;
        if (amount > 0) {
            credit_to_address(dest, amount);
        }
    }
}

void dump_ledger() {
    for (unsigned int b=0; b<256; b++) {
        for (const auto &l : transaction_ledger[b]) {
            CTxDestination outputaddr;
            ExtractDestination(l.first, outputaddr);
            printf("%s, %.8f\n", EncodeDestination(outputaddr).c_str(), (double) l.second / COIN);
        }
    }
}
