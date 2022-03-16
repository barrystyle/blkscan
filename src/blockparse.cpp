#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>

#include <indexman.h>
#include <primitives/block.h>
#include <streams.h>
#include <util/system.h>
#include <util/strencodings.h>

#include <iostream>
#include <memory>
#include <vector>

//! barrystyle 04032022

int current_height{0};
extern std::vector<std::string> blocks;
uint8_t blk_magic[4] = { 0x91, 0xc4, 0xfd, 0xe9 };

bool isblockmagic(char* bytes)
{
    if (memcmp(bytes, blk_magic, 4) == 0)
        return true;
    return false;
}

void pushblock(char* bytes, unsigned int len)
{
    std::stringstream blkconstruct;
    for (unsigned int i = 4; i < len; i++) {
        blkconstruct << strprintf("%02hhx", (uint8_t) bytes[i]);
    }
    blocks.push_back(blkconstruct.str());
}

int blockfile_exists(char* path, int filenum, char* filepathout)
{
    //! construct path
    char filepath[256];
    memset(filepath, 0, sizeof(filepath));
    sprintf(filepath, "%s/blk%05d.dat", path, filenum);

    //! test if exists
    struct stat st;
    stat(filepath, &st);
    if (st.st_size > 0) {
        strcpy(filepathout, filepath);
        return st.st_size;
    }
    return 0;
}

bool blockfile_scanmagic(char* filepathin, int filesizein)
{
    //! open once we know its safe to do so
    FILE* blkfile;
    if ((blkfile = fopen(filepathin, "r+")) == nullptr) {
        return false;
    }

    //! read the thing into memory
    char* memfile = (char*)malloc(167772160);
    memset(memfile, 0, sizeof(167772160));
    fseek(blkfile, 0, SEEK_SET);
    fread(memfile, filesizein, 1, blkfile);
    fclose(blkfile);

    //! counters
    int blocks = 0;

    //! alloc storage for block
    char blockbuf[1572864];
    memset(blockbuf, 0, sizeof(blockbuf));

    //! scan for starting bytes
    char *memptr;
    int filepos = 0, blockpos = 0;
    while (filepos < filesizein - 4) {
        memptr = &memfile[filepos];
        if (isblockmagic(memptr)) {
            if (filepos > 80) {
                pushblock(blockbuf, blockpos);
                blocks++;
                printf("\rblock %d (%d addr)", blocks, transaction_ledger.size());
                memset(blockbuf, 0, sizeof(blockbuf));
                blockpos = 0;
            }
            filepos += 4;
        } else {
            blockbuf[blockpos] = memptr[0];
            filepos += 1;
            blockpos += 1;
        }
    }

    free(memfile);

    return true;
}

#define PROTOCOL_VERSION 70002

bool DecodeHexBlk(CBlock& block, const std::string& strHexBlk)
{
    if (!IsHex(strHexBlk))
        return false;

    std::vector<unsigned char> blockData(ParseHex(strHexBlk));
    CDataStream ssBlock(blockData, SER_NETWORK, PROTOCOL_VERSION);
    ssBlock >> block;

    return true;
}

int height_from_coinbasescript(std::string& coinbase_script)
{
    std::string height_flip;
    int length = stoi(coinbase_script.substr(0, 2));
    std::string cb_height = coinbase_script.substr(2, 2+length);

    if (length == 1) {
        height_flip = coinbase_script.substr(2, 2);
    } else if (length == 2) {
        height_flip = coinbase_script.substr(4, 2) + coinbase_script.substr(2, 2);
    } else {
        height_flip = coinbase_script.substr(6, 2) + coinbase_script.substr(4, 2) + coinbase_script.substr(2, 2);
    }

    return stoi(height_flip, 0, 16);
}

void blockfile_process()
{
    printf("\n");

    for (const auto& l : blocks)
    {
        std::shared_ptr<CBlock> blockptr = std::make_shared<CBlock>();
        CBlock& block = *blockptr;
        if (!DecodeHexBlk(block, l)) {
            printf("error decoding block\n");
            continue;
        }

        std::string coinbase_script = HexStr(block.vtx[0]->vin[0].scriptSig);
        int height = height_from_coinbasescript(coinbase_script);

        if (current_height == height) {
            printf("\nskipping orphan block height %d\n", height);
            continue;
        }

        for (unsigned int i=0; i<block.vtx.size(); i++) {
             uint256 tx_hash = block.vtx[i]->GetHash();
             CTransaction tx_ref = *block.vtx[i];
             store_transaction(tx_hash, tx_ref);
             if (tx_ref != CMutableTransaction()) {
                 process_transaction(tx_ref);
             }
        }

        current_height = height;
        printf("\rreplaying transactions in block %d", height);
    }
}
