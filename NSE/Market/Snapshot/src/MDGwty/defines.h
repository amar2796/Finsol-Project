#ifndef FIN_NSE_TRIMMED_DEFINES_H
#define FIN_NSE_TRIMMED_DEFINES_H

#include <thread>
#include <stdint.h>
#include <stdlib.h>
#include <boost\predef\other\endian.h>
#include <string.h>
#include<string>
#include<WinSock2.h>

namespace FIN {
	namespace NSE {
		namespace COM {

			typedef char    SIGNED_NNF_CHAR;
			typedef int16_t SIGNED_NNF_SHORT;
			typedef int32_t SIGNED_NNF_LONG;
			typedef int64_t SIGNED_NNF_LONG_LONG;
			typedef uint16_t SIGNED_NNF_USHORT;

			union DOUBLE
			{
				double value;
				int64_t longValue;
				DOUBLE() { value = 0; }
				DOUBLE(double d) : value(d) {}
			};

#define NNF_WITH_TWIDDLE_SHORT( NAME, SIZE, TYPE)\
	private : TYPE _##NAME ;\
	public :\
		void set##NAME( TYPE value ) {\
			_##NAME =  value ;\
		}\
	TYPE get##NAME() const {\
		return  ntohs(_##NAME)  ;\
	}


#define NNF_WITH_TWIDDLE_LONG( NAME, SIZE, TYPE)\
	private : TYPE _##NAME ;\
	public :\
		void set##NAME( TYPE value ) {\
			_##NAME =  value  ;\
		}\
	TYPE get##NAME() const {\
		return  ntohl(_##NAME)  ;\
	}
#define NNF_WITH_TWIDDLE_LONG_LONG( NAME, SIZE, TYPE)\
	private : TYPE _##NAME ;\
	public :\
		void set##NAME( TYPE value ) {\
			_##NAME = value ;\
		}\
	TYPE get##NAME() const {\
		return  _##NAME  ;\
	}



#define CREATE_NNF_SHORT( NAME ) \
					NNF_WITH_TWIDDLE_SHORT( NAME, 16, SIGNED_NNF_SHORT ) 

#define CREATE_NNF_LONG( NAME ) \
				NNF_WITH_TWIDDLE_LONG( NAME, 32, SIGNED_NNF_LONG )   
#define CREATE_NNF_USHORT( NAME ) \
				NNF_WITH_TWIDDLE_SHORT( NAME,16 , SIGNED_NNF_USHORT )   

#define CREATE_NNF_LONG_LONG( NAME ) \
				NNF_WITH_TWIDDLE_LONG_LONG( NAME, 64, SIGNED_NNF_LONG_LONG )


#define CREATE_NNF_DOUBLE( NAME )\
				private : DOUBLE _##NAME ;\
				public :\
					void set##NAME( double value ) {\
						_##NAME.value = value  ;\
						_##NAME.longValue = _##NAME.longValue ;\
					}\
				double get##NAME() const {\
					DOUBLE d ;\
					d.longValue = _##NAME.longValue;\
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
				private : SIGNED_NNF_LONG _##NAME ;                                     \
				public :                                                                \
				void set##NAME##FromLocal( long value ) {                               \
					_##NAME = htobe32( value - 315532800 ) ; }                          \
				SIGNED_NNF_LONG get##NAME##AsLocal() const {                                       \
					return be32toh( _##NAME ) + 315532800  ; }                          \
				void set##NAME##FromUtc( long value ) {                                 \
					_##NAME = htobe32( value - 315532800 - 19800) ; }                   \
				SIGNED_NNF_LONG get##NAME##AsUtc() const {                                         \
					return be32toh( _##NAME ) + 315532800 - 19800 ; }                   \
				SIGNED_NNF_LONG get##NAME##AsNseTimeT() const {                                    \
					return be32toh( _##NAME )\
                   }

#define INCLUDE_STRUCT_ARRAY(STRUCT,NAME,SIZE) \
					public: STRUCT _##NAME[SIZE]; \
					public:\
							void set##NAME(const STRUCT v[],int POSN){_##NAME[POSN] = v[POSN]; \
										} \
					STRUCT get##NAME(){ \
						return _##NAME[0];  \
					}\
					 STRUCT get##NAME(int POSN) {\
							return _##NAME[POSN];\
					}\
					const STRUCT get##NAME()const {\
						return _##NAME[0]; \
					}

#define CREATE_NNF_SHORT_ARRAY(NAME,LENGTH) \
				private: SIGNED_NNF_SHORT _##NAME[LENGTH]; \
				public: \
                void set##NAME(SIGNED_NNF_SHORT V[],int POSN){ \
					/*##NAME[POSN] = htobe16(V[POSN]);*/ \
                    _##NAME[POSN] = V[POSN]; \
				} \
				SIGNED_NNF_SHORT get##NAME(int POSN) const { \
					/*return b16toh(_##NAME[POSN]);*/ \
					return _##NAME[POSN];\
				} \
				const SIGNED_NNF_SHORT get##NAME() const { \
					/*return b16toh(_##NAME[0]);*/ \
					return _##NAME[0]; \
				} 

#define PRINT_NNF_FIELD( NAME )\
					std::cout << "[" <<std::this_thread::get_id() << "]" << "(" << __FILE__ << ":" << __LINE__ << "): "<< #NAME ": "<< get##NAME() << std::endl ;

#define PRINT_NNF_FIELD_ARRAY(NAME,LENGTH)\
					for(int i=0;i<LENGTH;i++){ \
						std::cout<<"("<<__FILE__<<":"<<__LINE__<<"):"<< #NAME[i]": "<<get##NAME(i)<<"\n"; \
					} 
		}
	}
}

#endif