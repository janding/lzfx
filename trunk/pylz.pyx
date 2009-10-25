
cdef extern from "stdlib.h":
    ctypedef unsigned int size_t
    void* malloc(size_t amt)
    void free(void* ptr)

cdef extern from "string.h":
    void memset(void* dest, unsigned char value, size_t n)

cdef extern from "lzfx.h":

    int lzfx_compress(void* ibuf, unsigned int ilen, void* obuf, unsigned int *olen)
    int lzfx_decompress(void* ibuf, unsigned int ilen, void* obuf, unsigned int *olen)
    int lzfx_guess(void* ibuf, unsigned int ilen, unsigned int *olen)

cimport python_string

def raw_op(int compress, object istring, unsigned int bufsize, int claimed_size=-1):
    """ Compress or decompress a byte string into an output buffer
    of size bufsize.

    Returns a 2-tuple (output buffer, compressed length, return code).
    """

    cdef int rc
    cdef unsigned int olen = bufsize
    cdef unsigned char* obuf = <unsigned char*>malloc(bufsize)
    cdef unsigned int ilen = len(istring)
    cdef char* ibuf = istring

    memset(obuf, 0x00, bufsize)

    olen = claimed_size if claimed_size >= 0 else bufsize

    if compress:
        rc = lzfx_compress(ibuf, ilen, obuf, &olen)
    else:
        rc = lzfx_decompress(ibuf, ilen, obuf, &olen)

    ostring = python_string.PyString_FromStringAndSize(<char*>obuf, bufsize)

    free(obuf)

    return (ostring, olen, rc)

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


