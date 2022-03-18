## blkscan

barrystyle 18032022

parses blockfile to give a complete address balance,
height detected by value encoded into coinbase transaction (block->vtx[0]->vin[0]).


# requirements

recommended g++ 8/9 series minimum
boost 1.6+ for boost-filesystem


# how to install

git clone http://github.com/barrystyle/blkscan
cd blkscan
cd src/secp256k1
./autogen.sh
./configure
make -j2 install

cd ../..
make -j2
./blkscan
