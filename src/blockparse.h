#ifndef BLOCKPARSE_H
#define BLOCKPARSE_H

bool isblockmagic(char* bytes);
void printblock(char* bytes, unsigned int len);
int blockfile_exists(char* path, int filenum, char* filepathout);
bool blockfile_scanmagic(char* filepathin, int filesizein);
void blockfile_process();

#endif // BLOCKPARSE_H
