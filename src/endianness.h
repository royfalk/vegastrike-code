#ifndef _ENDIANNESS_H
#define _ENDIANNESS_H

inline float VSSwapHostFloatToLittle( float x ) { return x; }
inline double VSSwapHostDoubleToLittle( double x ) { return x; }
inline unsigned int VSSwapHostIntToLittle( unsigned int x ) { return x; }
inline unsigned short VSSwapHostShortToLittle( unsigned short x ) { return x; }
inline int le32_to_cpu(int x) { return x;}
inline short le16_to_cpu(short x) { return x;}



#endif  //_ENDIANNESS_H

