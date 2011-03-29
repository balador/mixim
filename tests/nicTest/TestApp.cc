//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "TestApp.h"
#include "NetwToMacControlInfo.h"
#include "SimpleAddress.h"

Define_Module(TestApp);

void TestApp::initialize(int stage)
{
	BaseModule::initialize(stage);

	if(stage == 0)
	{
		pingsSent = 0;
		myIndex = findHost()->getIndex();
		init("app" + toString(myIndex));

		mac = FindModule<Mac80211MultiChannel*>::findSubModule(findHost());
		if(!mac) {
			opp_error("Could not find MultiChannelMac80211.");
		}

		out = gate("out");
	}
}

void TestApp::handleMessage(cMessage *msg)
{
    announceMessage(msg);

    delete msg;
}



void TestApp::finish()
{
	finalize();
}

void TestApp::ping(int nr){
	Enter_Method_Silent();
	cPacket* p = new cPacket(("Ping Traffic #" + toString(nr)).c_str(), PING+nr, 5000);
	NetwToMacControlInfo* cInfo = new NetwToMacControlInfo(myIndex - 1);
	p->setControlInfo(cInfo);
	send(p, out);
	assertMessage("Ping", PING+nr, in(0), in(5), "app" + toString(myIndex - 1));
	pingsSent++;
}

void TestApp::pong(){
	Enter_Method_Silent();
	cPacket* p = new cPacket("Ping Traffic", PONG, 5000);
	NetwToMacControlInfo* cInfo = new NetwToMacControlInfo(myIndex + 1);
	p->setControlInfo(cInfo);
	send(p, out);
	assertMessage("Pong", PONG, in(0), in(5), "app" + toString(myIndex + 1));
}

void TestApp::startTraffic() {
	Enter_Method_Silent();

	ping(0);
}

void TestApp::testRun1(int stage)
{
	Enter_Method_Silent();
	const double min = 0.0025;
	const double max = 0.0037;

	if(stage == 0) {
//planTest("1.1", "A same channel but different time as B");
		sendPacket();
		assertMessage("Com A at receiver B", TESTPACKET+myIndex,
					  min, max, "app2");
		assertMessage("Com A at sender B", TESTPACKET+myIndex,
					  min, max, "app3");
		testAndWaitForMessage("1.1", TESTPACKET+myIndex, min, max, "app0");
	} else if(stage == 1){
//planTest("1.2", "B same channel but different time as A");
		sendPacket();
		assertMessage("Com B at receiver A", TESTPACKET+myIndex,
					  in(min), in(max), "app0");
		assertMessage("Com B at sender A", TESTPACKET+myIndex,
					  in(min), in(max), "app1");
		testAndWaitForMessage("1.2", TESTPACKET+myIndex,
							  in(min), in(max), "app2");
	} else if(stage == 2) {
//planTest("2.1", "A same channel same time as B");
		sendPacket();
		continueIn(0.0015);
	} else if(stage == 3){
		assertMessage("Com A at receiver B", TESTPACKET+1,
				  in(min-0.0015), in(max-0.0015), "app2");
		assertMessage("Com A at sender B", TESTPACKET+1,
				  in(min-0.0015), in(max-0.0015), "app3");
		testForMessage("2.1", TESTPACKET+1,
				  in(min-0.0015), in(max-0.0015), "app0");
//planTest("2.2", "B same channel same time as A");
		sendPacket();
		assertMessage("Com B at receiver A", TESTPACKET+myIndex,
				  in(min-0.0015+min), in(max-0.0015+max), "app0");
		assertMessage("Com B at sender A", TESTPACKET+myIndex,
				  in(min-0.0015+min), in(max-0.0015+max), "app1");
		testAndWaitForMessage("2.2", TESTPACKET+myIndex,
				  in(min-0.0015+min), in(max-0.0015+max), "app2");
	} else if(stage == 4) {
//planTest("3", "Sender of B switches to different channel");
		switchChannel(14);
		testForEqual("3", 14, getCurrentChannel());
//planTest("4", "B different channel between sender and receiver");
		sendPacket();
		continueIn(5);
	} else if(stage == 5) {
		if(myIndex == 2) {
			testPassed("4");
//planTest("5", "Receiver of B switches to different channel");
			switchChannel(14);
			testForEqual("5", 14, getCurrentChannel());
		} else if(myIndex == 3) {
//planTest("6", "B sender and receiver back to same channel");
			sendPacket();
			testAndWaitForMessage("6", TESTPACKET+myIndex,
								  in(min), in(max), "app2");
		} else {
			assertFalse("Not expected host index.", true);
		}
	}	else if(stage == 6) {
//planTest("7.1", "A different channel same time as B");
		sendPacket();
		continueIn(0.0015);
	} else if(stage == 7){
		testForMessage("7.1", TESTPACKET+1,
				  in(min-0.0015), in(max-0.0015), "app0");
//planTest("7.2", "B different channel same time as A");
		sendPacket();
		testAndWaitForMessage("7.2", TESTPACKET+myIndex,
				  in(min), in(max), "app2");
	}
}

simtime_t TestApp::in(simtime_t delta) {
	return simTime() + delta;
}

void TestApp::switchChannel(int channel)
{
	mac->switchChannel(channel);
}

int TestApp::getCurrentChannel(){
	return mac->getChannel();
}

void TestApp::continueIn(simtime_t time){
	scheduleAt(simTime() + time, new cMessage(0, WAITING));
	waitForMessage(	"Waiting for " + toString(time) + "s.",
					WAITING,
					simTime() + time);
}

void TestApp::sendPacket(int dest)
{
	cPacket* p = new cPacket("Test packet", TESTPACKET+myIndex, 5000);
	NetwToMacControlInfo* cInfo = new NetwToMacControlInfo(dest);
	p->setControlInfo(cInfo);
	send(p, out);
}