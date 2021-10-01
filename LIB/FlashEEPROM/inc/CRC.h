#ifndef __CRC_H
#define __CRC_H

uint32_t crc32_32block(uint32_t *data, uint32_t length);
uint32_t crc32_8block(uint8_t *data, uint32_t length);
void FlashPageCRCcalc(uint8_t* PageData);
void CRC_init(void);

#endif /*__CRC_H */