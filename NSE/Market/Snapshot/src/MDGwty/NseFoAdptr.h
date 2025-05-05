#ifndef FIN_NSE_FO_ADAPTER
#define FIN_NSE_FO_ADAPTER

#include<iostream>
#include "NseFoMarketData.h"
#include "../common/configReader.h"
#include<string>

namespace FIN {
	namespace NSE {
		namespace TRIMMED {
			namespace FO {

				/*class NseFoAdapter
				{
				public:
					static NseFoAdapter * getInstance()
					{
						if (!_instance)
							_instance = new NseFoAdapter();
						std::cout << "getinstance function1\n";
						return _instance;
					}
					void init(const std::string &configFile);
					
				private:
					static NseFoAdapter * _instance;
					NseFoAdapter() {}
				};*/
			}
		}
	}
}
#endif