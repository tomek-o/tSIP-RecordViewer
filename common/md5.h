#ifndef md5H
#define md5H

/**
 * @file md5.h
 * @brief MD5 Functions
 */


/** Signed 32bit integer. */
typedef int		pj_int32_t;

/** Unsigned 32bit integer. */
typedef unsigned int	pj_uint32_t;

/** Signed 16bit integer. */
typedef short		pj_int16_t;

/** Unsigned 16bit integer. */
typedef unsigned short	pj_uint16_t;

/** Signed 8bit integer. */
typedef signed char	pj_int8_t;

/** Unsigned 8bit integer. */
typedef unsigned char	pj_uint8_t;

/** Large unsigned integer. Changed from size_t */
typedef unsigned int		pj_size_t;

/** Large signed integer. Changed from long since TI's long is 5/8 B */
typedef int		pj_ssize_t;

/** Status code. */
typedef int		pj_status_t;

/** Boolean. */
typedef int		pj_bool_t;


/**
 * @defgroup PJLIB_UTIL_MD5 MD5
 * @ingroup PJLIB_UTIL_ENCRYPTION
 * @{
 */


/** MD5 context. */
typedef struct pj_md5_context
{
	pj_uint32_t buf[4];
	pj_uint32_t bits[2];
	pj_uint8_t  in[64];
} pj_md5_context;

/** Initialize the algorithm. 
 *  @param pms		MD5 context.
 */
void pj_md5_init(pj_md5_context *pms);

/** Append a string to the message. 
 *  @param pms		MD5 context.
 *  @param data		Data.
 *  @param nbytes	Length of data.
 */
void pj_md5_update( pj_md5_context *pms, 
			     const pj_uint8_t *data, unsigned nbytes);

/** Finish the message and return the digest. 
 *  @param pms		MD5 context.
 *  @param digest	16 byte digest.
 */
void pj_md5_final(pj_md5_context *pms, pj_uint8_t digest[16]);



#endif	/* __PJLIB_UTIL_MD5_H__ */
