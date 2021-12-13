#include <base58.h>
#include <crypto/sha256.h>
#include <hash.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <random.h>
#include <streams.h>
#include <tinyformat.h>
#include <uint256.h>
#include <util/strencodings.h>
#include <util/system.h>

#include <blockparse.h>

#include <iostream>
#include <vector>

std::vector<std::string> blocks;

int main()
{
    char path[] = "/root/.phore/blocks";
    int fileno = 0;
    char *fpathout = (char*)malloc(1024);
    int fsizeout = 0;

    while (fsizeout = blockfile_exists(path, fileno, fpathout)) {
        printf("%s %d %s %d\n", path, fileno, fpathout, fsizeout);
        blockfile_scanmagic(fpathout, fsizeout);
        blockfile_process();
        blocks.clear();
        fileno += 1;
        printf("\n");
    }

    return 1;
}
