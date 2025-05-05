#ifndef FIN_INITIALIZE_PROCESS_DATA
#define FIN_INITIALIZE_PROCESS_DATA

#include "Nanolog.hpp"
#include "SnapshotProcess.h"
#include "ConfigReader.h"
#include "CurrentPath.h"
namespace FIN
{

	class Initialize_process
	{
	public:
		Initialize_process();
		Initialize_process(UNSIGNED_INT_16 exchID);
		void init();

	private:
		NFCAST_SNAPSHOT nfcast_snap;
		void start_processing();
		Config_Details conf_Det;


	};
}

#endif
