#include <base58.h>
#include <crypto/sha256.h>
#include <hash.h>
#include <indexman.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <random.h>
#include <streams.h>
#include <tinyformat.h>
#include <uint256.h>
#include <util/strencodings.h>
#include <util/system.h>

#include <blockparse.h>

#include <secp256k1/include/secp256k1.h>

#include <iostream>
#include <vector>

std::vector<std::string> blocks;

int main()
{
    char path[] = "/root/.phore/blocks";
    int fileno = 0;
    char *fpathout = (char*)malloc(1024);
    int fsizeout = 0;

    //! stop at given height
    bool halt = false;
    int stop_height = 100000;

    printf("\nstopping after height %d\n", stop_height);

    while (fsizeout = blockfile_exists(path, fileno, fpathout)) {
        printf("%s %d %s %d\n", path, fileno, fpathout, fsizeout);
        blockfile_scanmagic(fpathout, fsizeout);
        blockfile_process(halt, stop_height);
        if (halt) {
            return 1;
        }
        blocks.clear();
        fileno += 1;
        printf("\n");
    }

    return 1;
}
