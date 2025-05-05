
#pragma once
#ifndef FIN_COMMON_REQ_STRUCT_H
#define FIN_COMMON_REQ_STRUCT_H

#include "ETI.h"
#include <iostream>
using namespace Derivatives;
namespace FIN
{

	class Common_Req_Structure
	{
	public:
		Common_Req_Structure()
		{

		}

		MessageHeaderInCompT SetMessageHeaderIN(uint32_t bodyLen, uint32_t templateId)
		{
			MessageHeaderInCompT _mHeader;

			_mHeader.BodyLen = bodyLen;
			_mHeader.TemplateID = templateId;
			std::memset(_mHeader.NetworkMsgID, NULL, 8);
			std::memset(_mHeader.Pad2, NULL, 2);

			return _mHeader;
		}

		RequestHeaderCompT setRequestHeader(uint32_t seq, uint32_t senderSubID)
		{
			RequestHeaderCompT _reqHeader;
			_reqHeader.MsgSeqNum = seq;
			_reqHeader.SenderSubID = senderSubID;

			return _reqHeader;

		}




	private:

	};







}





#endif