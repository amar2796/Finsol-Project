#ifndef FIN_GLOBAL_DEFINES_H
#define FIN_GLOBAL_DEFINES_H

#undef _WINSOCKAPI_   // Force winsock2.h to be included instead of winsock.h
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "Ws2_32.lib")

#include <thread>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"

namespace FIN {
	namespace NSE {
		namespace TRIMMED {

			typedef char    SIGNED_NNF_CHAR;
			typedef int16_t SIGNED_NNF_SHORT;
			typedef int32_t SIGNED_NNF_LONG;
			typedef int64_t SIGNED_NNF_LONG_LONG;
			typedef char    SIGNED_CHAR;
			typedef uint_least8_t  SIGNED_SUB_SHORT;
			typedef int16_t SIGNED_SHORT;
			typedef int32_t SIGNED_LONG;
			typedef int64_t SIGNED_LONG_LONG;
			typedef uint64_t UNSIGNED_LONG_LONG;
			typedef uint16_t UNSIGNED_SHORT;
			typedef uint32_t UNSIGNED_LONG;
			typedef unsigned char UNSIGNED_CHAR;

			

			/***
			 * Making this platform independent.
			 * No matter if host system in little or big endian.
			 * We are always storing in Big Endian ( Expected by NSE )
			 * And Getting in Host Endianness ( Be it Little or Big )
			 ***/

#define Swap8Bytes(val) \
	((((val) >> 56) & 0x00000000000000FF) | (((val) >> 40) & 0x000000000000FF00) | \
	(((val) >> 24) & 0x0000000000FF0000) | (((val) >> 8) & 0x00000000FF000000) | \
	(((val) << 8) & 0x000000FF00000000) | (((val) << 24) & 0x0000FF0000000000) | \
	(((val) << 40) & 0x00FF000000000000) | (((val) << 56) & 0xFF00000000000000))
#define Swap4Bytes(val) \
	((((val) >> 24) & 0x000000FF) | (((val) >> 8) & 0x0000FF00) | \
	(((val) << 8) & 0x00FF0000) | (((val) << 24) & 0xFF000000))

			union DOUBLE
			{
				double value;
				int64_t longValue;
				DOUBLE() { value = 0; }
				DOUBLE(double d) : value(d) {}
			};

#define NNF_WITH_TWIDDLE( NAME, SIZE, TYPE)\
	private : TYPE _##NAME ;\
	public :\
		void set##NAME( TYPE value ) {\
			_##NAME = htobe##SIZE( value ) ;\
		}\
	TYPE get##NAME() const {\
		return be##SIZE##toh( _##NAME ) ;\
	}

#define CREATE_NNF_SHORT( NAME ) \
		private: short _##NAME; \
			public :\
				void set##NAME( short value ) {\
					_##NAME = htons( value ) ;\
				}\
			short get##NAME() const {\
				return (short)ntohs( _##NAME ) ;\
			}

#define CREATE_NNF_LONG( NAME ) \
	private: long _##NAME; \
			public :\
				void set##NAME( long value ) {\
					_##NAME = htonl( value ) ;\
				}\
			long get##NAME() const {\
				return (long)ntohl( _##NAME ) ;\
			}   

#define CREATE_NNF_LONG_LONG( NAME ) \
	private: SIGNED_LONG_LONG _##NAME; \
			public :\
				void set##NAME( long long value ) {\
					_##NAME = _byteswap_uint64( value ) ;\
				}\
			long long get##NAME() const {\
				return (long long)_byteswap_uint64( _##NAME ) ;\
			}   

#define CREATE_NNF_DOUBLE( NAME )\
	private : DOUBLE _##NAME ;\
		public :\
			void set##NAME( double value ) {\
				_##NAME.value = value  ;\
				_##NAME.longValue =( _byteswap_uint64( _##NAME.longValue ) );\
			}\
			double get##NAME() const {\
				DOUBLE d ;\
				d.longValue = _byteswap_uint64( _##NAME.longValue ) ;\
				return d.value ;\
			}

#define CREATE_NNF_STRING( NAME, LENGTH )\
	private: char _##NAME[LENGTH];\
	public:\
	       void set##NAME ( const std::string &value ){\
		       if (value.size() <= LENGTH )\
		       value.copy ( _##NAME, value.size() );else value.copy ( _##NAME, LENGTH ) ;\
	       }\
	const std::string get##NAME() const{\
		return std::string( _##NAME, LENGTH );\
	}\
	const char * get##NAME(){\
		return _##NAME;\
	}

#define CREATE_NNF_BIT( NAME )\
	private: unsigned char _##NAME : 1;\
	public:\
	       void set##NAME ( bool value ){\
		       _##NAME = value;\
	       }\
	bool get##NAME()const{\
		return _##NAME ;\
	}

#define CREATE_NNF_CHAR( NAME )\
	private: char _##NAME;\
	public:\
	       void set##NAME ( char value ){\
		       _##NAME = value;\
	       }\
	char get##NAME()const{\
		return _##NAME;\
	}

#define INCLUDE_STRUCT( STRUCT, NAME ) \
	public: STRUCT _##NAME; \
	public: \
	       void set##NAME ( const STRUCT &v ){ \
		       _##NAME = v; \
	       } \
	STRUCT &get##NAME (){ \
		return _##NAME ; \
	} \
	const STRUCT &get##NAME()const{ \
		return _##NAME; \
	}


#define CREATE_NNF_TIME( NAME )                                             \
    private : SIGNED_LONG _##NAME ;                                     \
    public :                                                                \
    void set##NAME##FromLocal( long value ) {                               \
        _##NAME = htonl( value - 315532800 ) ; }                          \
    SIGNED_LONG get##NAME##AsLocal() const {                            \
        return ntohl( _##NAME ) + 315532800  ; }                          \
    void set##NAME##FromUtc( long value ) {                                 \
        _##NAME = htonl( value - 315532800 - 19800) ; }                   \
    SIGNED_LONG get##NAME##AsUtc() const {                              \
        return ntohl( _##NAME ) + 315532800 - 19800 ; }                   \
    SIGNED_LONG get##NAME##AsNseTimeT() const {                         \
        return htonl( _##NAME )                   ; }

#define PRINT_NNF_FIELD( NAME )\
	DEVLOG( #NAME << ": "<< get##NAME() ) ;

#define PRINT_BODY_START( NAME )\
	DEVLOG( "------ " << #NAME << " START ------" );

#define PRINT_BODY_END( NAME )\
	DEVLOG ("---------- " << #NAME <<" END ----------" );
		}
	}
}

#endif
