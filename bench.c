#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "lzfxs.h"

typedef unsigned char u8;

#define BLOCKSIZE (1024*1024)
#define NITER 100

int main(int argc, char* argv[]){

    int ifd, rc, i;
    u8* data = NULL;
    u8* obuf = NULL;
    unsigned int olen;
    unsigned int count=0;

    ifd = open(argv[1], O_RDONLY);
    if(ifd<0){
        return 1;
    }

    do {
        data = realloc(data, count+BLOCKSIZE);
        rc = read(ifd, data, BLOCKSIZE);
        if(rc<0){
            return -1;
        }
        count += rc;
    } while(rc > 0);

    fprintf(stderr, "read %u bytes\n", count);

    olen = count*2;
    obuf = (u8*)malloc(olen);

    for(i=0;i<NITER;i++){
        rc = lzfx_compress(data, count, obuf, &olen);
        if(rc<0){
            fprintf(stderr, "fail\n");
            return -rc;
        }
        olen = count*2;
    }

    return 0;
}
