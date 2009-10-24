
cdef extern from "stdlib.h":
    ctypedef unsigned int size_t
    void* malloc(size_t amt)
    void free(void* ptr)

cdef extern from "lzfx.h":

    int lzfx_compress(void* ibuf, unsigned int ilen, void* obuf, unsigned int *olen)
    int lzfx_decompress(void* ibuf, unsigned int ilen, void* obuf, unsigned int *olen)
    int lzfx_guess(void* ibuf, unsigned int ilen, unsigned int *olen)

def compress(object istring, size_t bufsize):

    cdef int retcode
    cdef unsigned int ilen = len(istring)
    cdef unsigned int olen = bufsize
    cdef char* data = istring
    cdef char* outdata = <char*>malloc(olen)

    retcode = lzfx_compress(data, ilen, outdata, &olen)
    if(retcode<0):
        free(outdata)
        return retcode

    ostring = outdata[0:olen]
    free(outdata)

    return ostring

def guess(object instring):

    cdef unsigned int ilen, olen
    cdef char* ibuf = instring
    cdef int retcode

    retcode = lzfx_guess(ibuf, len(instring), &olen)
    if retcode < 0:
        return retcode

    return olen

def decompress(object istring, size_t bufsize):


    cdef int retcode
    cdef unsigned int ilen = len(istring)
    cdef unsigned int olen = bufsize
    cdef char* data = istring
    cdef char* outdata = <char*>malloc(olen)

    retcode = lzfx_decompress(data, ilen, outdata, &olen)
    if(retcode<0):
        free(outdata)
        return retcode

    ostring = outdata[0:olen]
    free(outdata)

    return ostring


