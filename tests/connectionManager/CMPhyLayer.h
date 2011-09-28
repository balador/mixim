/* -*- mode:c++ -*- ********************************************************
 * file:        BasePhyLayer.h
 *
 * author:      Marc Loebbers
 *
 * copyright:   (C) 2004 Telecommunication Networks Group (TKN) at
 *              Technische Universitaet Berlin, Germany.
 *
 *              This program is free software; you can redistribute it
 *              and/or modify it under the terms of the GNU General Public
 *              License as published by the Free Software Foundation; either
 *              version 2 of the License, or (at your option) any later
 *              version.
 *              For further information see file COPYING
 *              in the top level directory
 ***************************************************************************
 * part of:     framework implementation developed by tkn
 ***************************************************************************/


#ifndef TEST_PHY_LAYER_H
#define TEST_PHY_LAYER_H

#include "ChannelAccess.h"
#include <MACAddress.h>
#include <MacPkt_m.h>
#include <iostream>

/**
 * @brief
 */
class CMPhyLayer : public ChannelAccess
{

protected:

	int myAddr(){
		return findHost()->getIndex();
	};

	void sendDown(MACAddress destAddr) {
		MacPkt* m = new MacPkt;
		m->setDestAddr(destAddr);
		m->setSrcAddr(myAddr());
		sendToChannel(m);
	}

	void assertTrue(std::string msg, bool value) {
		if (!value) {
			ev << "FAILED: ";
		} else {
			ev << "Passed: ";
		}

		ev << "Test Node " << myAddr() << ": " << msg << std::endl;
	}

	void assertFalse(std::string msg, bool value) { assertTrue(msg, !value); }

public:
    //Module_Class_Members( CMPhyLayer, ChannelAccess, 0 );

    /** @brief Called every time a message arrives*/
    virtual void handleMessage( cMessage* );

	virtual void handleSelfMsg() { assertFalse("This phy layer expects no self-msg!", true); }
	virtual void handleLowerMsg(MACAddress srcAddr) { assertFalse("This phy layer expects no msg!", true); }

protected:

};

#endif
