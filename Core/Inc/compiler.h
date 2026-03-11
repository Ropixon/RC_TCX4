/*
 * compiler.h
 *
 *  Created on: 7. 11. 2018
 *      Author: jirik
 */

#ifndef COMPILER_H_
#define COMPILER_H_


/************************************************************************/
/* Area #DEFINE															*/
/************************************************************************/

/************************************************************************/
/* MAKRO with parameters                                                */
/************************************************************************/

/************************************************************************/
/* Area TYPEDEF                                                         */
/************************************************************************/

/*! \name Usual Types
 */
//! @{
typedef int8_t                  S8 ;  //!< 8-bit signed integer.
typedef uint8_t                 U8 ;  //!< 8-bit unsigned integer.
typedef int16_t                 S16;  //!< 16-bit signed integer.
typedef uint16_t                U16;  //!< 16-bit unsigned integer.
typedef uint16_t                le16_t;
typedef uint16_t                be16_t;
typedef int32_t                 S32;  //!< 32-bit signed integer.
typedef uint32_t                U32;  //!< 32-bit unsigned integer.
typedef uint32_t                le32_t;
typedef uint32_t                be32_t;
typedef int64_t                 S64;  //!< 64-bit signed integer.
typedef uint64_t                U64;  //!< 64-bit unsigned integer.
typedef float                   F32;  //!< 32-bit floating-point number.
typedef double                  F64;  //!< 64-bit floating-point number.
//! @}

/*! \name Status Types
 */
//! @{
typedef bool                Status_bool_t;  //!< Boolean status.
typedef U8                  Status_t;       //!< 8-bit-coded status.
//! @}

/*! \name Aliasing Aggregate Types
 */
//! @{

//! 16-bit union.
typedef union
{
  S16 s16   ;
  U16 u16   ;
  S8  s8 [2];
  U8  u8 [2];
} Union16;

//! 32-bit union.
typedef union
{
  S32 s32   ;
  U32 u32   ;
  S16 s16[2];
  U16 u16[2];
  S8  s8 [4];
  U8  u8 [4];
} Union32;

//! 64-bit union.
typedef union
{
  S64 s64   ;
  U64 u64   ;
  S32 s32[2];
  U32 u32[2];
  S16 s16[4];
  U16 u16[4];
  S8  s8 [8];
  U8  u8 [8];
} Union64;
//! @}


/************************************************************************/
/* Declaration global variables - EXTERN                                */
/************************************************************************/

/************************************************************************/
/* Declaration global function											*/
/************************************************************************/
/*! \name MCU Endianism Handling
 * ARM is MCU little endianism.
 */
//! @{
#define  MSB(u16)       (((U8  *)&(u16))[1]) //!< Most significant byte of \a u16.
#define  LSB(u16)       (((U8  *)&(u16))[0]) //!< Least significant byte of \a u16.

#define  MSH(u32)       (((U16 *)&(u32))[1]) //!< Most significant half-word of \a u32.
#define  LSH(u32)       (((U16 *)&(u32))[0]) //!< Least significant half-word of \a u32.
#define  MSB0W(u32)     (((U8  *)&(u32))[3]) //!< Most significant byte of 1st rank of \a u32.
#define  MSB1W(u32)     (((U8  *)&(u32))[2]) //!< Most significant byte of 2nd rank of \a u32.
#define  MSB2W(u32)     (((U8  *)&(u32))[1]) //!< Most significant byte of 3rd rank of \a u32.
#define  MSB3W(u32)     (((U8  *)&(u32))[0]) //!< Most significant byte of 4th rank of \a u32.
#define  LSB3W(u32)     MSB0W(u32)           //!< Least significant byte of 4th rank of \a u32.
#define  LSB2W(u32)     MSB1W(u32)           //!< Least significant byte of 3rd rank of \a u32.
#define  LSB1W(u32)     MSB2W(u32)           //!< Least significant byte of 2nd rank of \a u32.
#define  LSB0W(u32)     MSB3W(u32)           //!< Least significant byte of 1st rank of \a u32.

//! @}
#endif /* COMPILER_H_ */
