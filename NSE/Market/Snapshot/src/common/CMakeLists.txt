add_library( Common STATIC
		marketDataMessage.cpp
		multicastSender.cpp
		multicastRcvr.cpp
		tcpSender.cpp
		templaticConnection.cpp
		connectionStatus.cpp
)

target_link_libraries( Common
-lpthread
-lboost_thread
-lboost_system
)
