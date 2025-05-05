#include "MulticastReceiver.h"

FIN::MulticastReceiver::MulticastReceiver(boost::asio::io_service& io_service, Config_Details* conf_det) : socket(io_service)
{
	local_conf = conf_det;
}

void FIN::MulticastReceiver::start_Reciver()
{

	try {
		int count = 0;
		for (;;)
		{
			size_t rcved_data = socket.receive_from(boost::asio::buffer(data_, max_length), sender_endpoint);
			MessagePacket packet;
			packet.msg = new char[rcved_data];
			memcpy(packet.msg, data_, rcved_data);
			packet.len = rcved_data;
			
			multicast_q.push(packet);
			//std::memset(data_, 0, max_length);
			//if (consumer_waiting)
			//{
			//	{
			//		std::lock_guard<std::mutex> lock(cv_mutex);
			//		consumer_waiting = false;  // Reset the flag
			//	}
			//	cv.notify_one();  // Notify the waiting consumer
			//}
		}

	}
	catch (std::exception& e)
	{
		LOG_INFO << e.what();
		std::cout << e.what() << std::endl;
	}


}

void FIN::MulticastReceiver::init()
{
	boost::asio::ip::udp::endpoint listen_endpoint(boost::asio::ip::address::from_string(local_conf->interfacea_Addr), local_conf->incremental_multicast_port);
	socket.open(listen_endpoint.protocol());
	socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
	socket.bind(listen_endpoint);

	// Join the multicast group.
	socket.set_option(
		boost::asio::ip::multicast::join_group(boost::asio::ip::address::from_string(local_conf->incremental_multicast_addr)));


	std::cout << "INCREMENTAL MULTICAST DETAILS " << local_conf->incremental_multicast_addr << " " << local_conf->incremental_multicast_port << std::endl;
	LOG_INFO << "INCREMENTAL MULTICAST DETAILS " << local_conf->incremental_multicast_addr << " " << local_conf->incremental_multicast_port;



}
