#ifndef SWIFTEN_ChatController_H
#define SWIFTEN_ChatController_H

#include "Swift/Controllers/Chat/ChatControllerBase.h"

namespace Swift {
	class AvatarManager;
	class ChatStateNotifier;
	class ChatStateMessageSender;
	class ChatStateTracker;
	class NickResolver;
	class ChatController : public ChatControllerBase {
		public:
			ChatController(const JID& self, StanzaChannel* stanzaChannel, IQRouter* iqRouter, ChatWindowFactory* chatWindowFactory, const JID &contact, NickResolver* nickResolver, PresenceOracle* presenceOracle, AvatarManager*);
			virtual void setToJID(const JID& jid);

		private:
			void handlePresenceChange(boost::shared_ptr<Presence> newPresence, boost::shared_ptr<Presence> previousPresence);
			String getStatusChangeString(boost::shared_ptr<Presence> presence);
			bool isIncomingMessageFromMe(boost::shared_ptr<Message> message);
			void postSendMessage(const String &body);
			void preHandleIncomingMessage(boost::shared_ptr<Message> message);
			void preSendMessageRequest(boost::shared_ptr<Message>);
			String senderDisplayNameFromMessage(const JID& from);

		private:
			NickResolver* nickResolver_;
			JID contact_;
			ChatStateNotifier* chatStateNotifier_;
			ChatStateMessageSender* chatStateMessageSender_;
			ChatStateTracker* chatStateTracker_;
	};
}
#endif
