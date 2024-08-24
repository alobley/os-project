#include "fat32.h"
#include "ata.h"

// Definitions:

// Bad/Defective sector: a sector whose contents cannot be read or written

// File: a named stream of bytes representing a collection of information

// Sector: a unit of data that can be accessed independently of other units on the media

// Cluster: a unit of allocation containing a set of logically contiguous sectors
//      -Referred to by a cluster number "N"
//      - Allocation of a file must be an integral multiple of a cluster

// Partition: an extent of sectors in a volume

// Volume: logically contiguous sector address space as specifies in the relevant standard for recording

// Arithmetic notation:
//      - ip(x) means the integer part of x
//      - ciel(x) means the minimum integer that is greater than x (x + 1 most likely)
//      - rem(x, y) means the remainder of the integer division of x by y

// FAT regions:
//  0: reserved
//  1: FAT region
//  2: Root directory region (IMPORTANT: does not exist on FAT32 volumes)
//  3: File and directory data region

// All data structures on the disk are little-endian (thank god)

// The BPB (BIOS parameter block) is the first sector of the volume
