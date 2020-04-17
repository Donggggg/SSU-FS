#ifndef	_FLASH_H_
#define	_FLASH_H_

#define	PAGE_NUM	4
#define	SECTOR_SIZE	512			
#define	SPARE_SIZE	16			
#define	PAGE_SIZE	(SECTOR_SIZE+SPARE_SIZE)
#define	BLOCK_SIZE	(PAGE_SIZE*PAGE_NUM)

#endif

