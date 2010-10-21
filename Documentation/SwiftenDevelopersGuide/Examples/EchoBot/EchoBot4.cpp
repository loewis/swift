/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

//...
#include <iostream>
#include <boost/bind.hpp>

#include "Swiften/Swiften.h"

using namespace Swift;
using namespace boost;
//...
class EchoBot {
	public:
		EchoBot() {
			//...
			client = new Client(JID("echobot@wonderland.lit"), "mypass");
			client->onConnected.connect(bind(&EchoBot::handleConnected, this));
			client->onMessageReceived.connect(
					bind(&EchoBot::handleMessageReceived, this, _1));
			//...
			client->onPresenceReceived.connect(
					bind(&EchoBot::handlePresenceReceived, this, _1));
			//...
			tracer = new ClientXMLTracer(client);
			client->connect();
			//...
		}

		//...
		~EchoBot() {
			delete tracer;
			delete client;
		}
	
	private:
		//...
		void handlePresenceReceived(Presence::ref presence) {
			// Automatically approve subscription requests
			if (presence->getType() == Presence::Subscribe) {
				Presence::ref response = Presence::create();
				response->setTo(presence->getFrom());
				response->setType(Presence::Subscribed);
				client->sendPresence(response);
			}
		}

		void handleConnected() {
			// Request the roster
			GetRosterRequest::ref rosterRequest = 
					GetRosterRequest::create(client->getIQRouter());
			rosterRequest->onResponse.connect(
					bind(&EchoBot::handleRosterReceived, this, _2));
			rosterRequest->send();
		}

		void handleRosterReceived(const optional<ErrorPayload>& error) {
			if (error) {
				std::cerr << "Error receiving roster. Continuing anyway.";
			}
			// Send initial available presence
			client->sendPresence(Presence::create("Send me a message"));
		}
		//...
		
		void handleMessageReceived(Message::ref message) {
			// Echo back the incoming message
			message->setTo(message->getFrom());
			message->setFrom(JID());
			client->sendMessage(message);
		}

	private:
		Client* client;
		ClientXMLTracer* tracer;
		//...
};
//...

int main(int, char**) {
	SimpleEventLoop eventLoop;
	EchoBot bot;
	eventLoop.run();
	return 0;
}
//...