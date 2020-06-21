#ifndef SEQ_PACKER_COMMONS_H
#define SEQ_PACKER_COMMONS_H
#define SEQ_LEN_FOR_CODE 255

unsigned char getSeqlenMin(uint64_t best_offset);


uint64_t getLastByte(uint64_t longRange);

uint64_t getLowestSpecial(uint64_t pages, uint64_t longRange);

uint64_t calcPageMax(uint64_t pages, uint64_t longRange);

#endif
