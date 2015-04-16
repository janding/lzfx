## Buffer compression format ##

The format is identical to LZF:

```
/*
    There are two kinds of structures in LZF/LZFX: literal runs and back
    references. The length of a literal run is encoded as L - 1, as it must
    contain at least one byte.  Literals are encoded as follows:

    000LLLLL <L+1 bytes>

    Back references are encoded as follows.  The smallest possible encoded
    length value is 1, as otherwise the control byte would be recognized as
    a literal run.  Since at least three bytes must match for a back reference
    to be inserted, the length is encoded as L - 2 instead of L - 1.  The
    offset (distance to the desired data in the output buffer) is encoded as
    o - 1, as all offsets are at least 1.  The binary format is:

    LLLooooo oooooooo           for backrefs of real length < 9   (1 <= L < 7)
    111ooooo LLLLLLLL oooooooo  for backrefs of real length >= 9  (L > 7)
*/
```

## LZFX utility file format ##

An LZFX file (as opposed to a compressed buffer) is made up of a sequence of blocks, each consisting of a header and a data payload.

### Block header format ###

Each block starts with a 10-byte header in the following format:

| 'L' | 'Z' | 'F' | 'X' | K1 | K2 | L1 | L2 | L3 | L4 |
|:----|:----|:----|:----|:---|:---|:---|:---|:---|:---|

**K1** and **K2** are a two-byte code identifying the contents of the block.  The values are stored in big-endian format; K1 is the high byte and K2 is the low byte.  Currently registered kinds are:

| 0 | Reserved |
|:--|:---------|
| 1 | Compresssed data block |
| 2 | Uncompressed data block |

**L1** through **L4** contain the length of the data payload.  L1 is the most significant byte and L4 is the least.  Decompression programs which cannot understand a given type code are required to skip the block.

### Uncompressed blocks ###

The block contains raw data which should be copied to the output file.

### Compressed blocks ###

The first four bytes contain the uncompressed size of the data, in big-endian format.  The rest of the data payload contains compressed data.
