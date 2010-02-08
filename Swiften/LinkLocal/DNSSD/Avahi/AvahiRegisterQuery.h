#pragma once

#include <avahi-client/publish.h>

#include "Swiften/LinkLocal/DNSSD/Avahi/AvahiQuery.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDRegisterQuery.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/EventLoop/MainEventLoop.h"

namespace Swift {
	class AvahiQuerier;

	class AvahiRegisterQuery : public DNSSDRegisterQuery, public AvahiQuery {
		public:	
			AvahiRegisterQuery(const String& name, int port, const ByteArray& txtRecord, boost::shared_ptr<AvahiQuerier> querier) : AvahiQuery(querier), name(name), port(port), txtRecord(txtRecord), group(0) {
			}

			void registerService() {
				std::cout << "Registering service " << name << ":" << port << std::endl;
				avahi_threaded_poll_lock(querier->getThreadedPoll());
				if (!group) {
					std::cout << "Creating entry group" << std::endl;
					group = avahi_entry_group_new(querier->getClient(), handleEntryGroupChange, this);
					if (!group) {
						std::cout << "Error ceating entry group" << std::endl;
						MainEventLoop::postEvent(boost::bind(boost::ref(onRegisterFinished), boost::optional<DNSSDServiceID>()), shared_from_this());
					}
				}

				doRegisterService();
				avahi_threaded_poll_unlock(querier->getThreadedPoll());
			}

			void unregisterService() {
			}

			void updateServiceInfo(const ByteArray& txtRecord) {
				this->txtRecord = txtRecord;
				avahi_threaded_poll_lock(querier->getThreadedPoll());
				assert(group);
				avahi_entry_group_reset(group);
				doRegisterService();
				avahi_threaded_poll_unlock(querier->getThreadedPoll());
			}

		private:
			void doRegisterService() {
				AvahiStringList* txtList;
				avahi_string_list_parse(txtRecord.getData(), txtRecord.getSize(), &txtList);

				int result = avahi_entry_group_add_service_strlst(group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, (AvahiPublishFlags) 0, name.getUTF8Data(), "_presence._tcp", NULL, NULL, port, txtList);
				if (result < 0) {
					std::cout << "Error registering service: " << avahi_strerror(result) << std::endl;
					MainEventLoop::postEvent(boost::bind(boost::ref(onRegisterFinished), boost::optional<DNSSDServiceID>()), shared_from_this());
				}
				result = avahi_entry_group_commit(group);
				if (result < 0) {
					std::cout << "Error registering service: " << avahi_strerror(result) << std::endl;
				}
			}

			static void handleEntryGroupChange(AvahiEntryGroup *g, AvahiEntryGroupState state, void *userdata) {
				static_cast<AvahiRegisterQuery*>(userdata)->handleEntryGroupChange(g, state);
			}

			void handleEntryGroupChange(AvahiEntryGroup* g, AvahiEntryGroupState state) {
				std::cout << "ENtry group callback: " << state << std::endl;
				switch (state) {
					case AVAHI_ENTRY_GROUP_ESTABLISHED :
						// Domain is a hack!
						MainEventLoop::postEvent(boost::bind(boost::ref(onRegisterFinished), boost::optional<DNSSDServiceID>(DNSSDServiceID(name, "local", "_presence._tcp", 0))), shared_from_this());
						std::cout << "Entry group established" << std::endl;
            break;
        case AVAHI_ENTRY_GROUP_COLLISION : {
						std::cout << "Entry group collision" << std::endl;
            /*char *n;
            n = avahi_alternative_service_name(name);
            avahi_free(name);
            name = n;*/
            break;
        }

        case AVAHI_ENTRY_GROUP_FAILURE :
						std::cout << "Entry group failure " << avahi_strerror(avahi_client_errno(avahi_entry_group_get_client(g))) << std::endl;
            break;

        case AVAHI_ENTRY_GROUP_UNCOMMITED:
        case AVAHI_ENTRY_GROUP_REGISTERING:
            ;

				/*
				DNSServiceErrorType result = DNSServiceRegister(
						&sdRef, 0, 0, name.getUTF8Data(), "_presence._tcp", NULL, NULL, port, 
						txtRecord.getSize(), txtRecord.getData(), 
						&AvahiRegisterQuery::handleServiceRegisteredStatic, this);
				if (result != kDNSServiceErr_NoError) {
					sdRef = NULL;
				}*/
				//MainEventLoop::postEvent(boost::bind(boost::ref(onRegisterFinished), boost::optional<DNSSDServiceID>()), shared_from_this());
			}
		}

/*
			static void handleServiceRegisteredStatic(DNSServiceRef, DNSServiceFlags, DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain, void *context) {
				static_cast<AvahiRegisterQuery*>(context)->handleServiceRegistered(errorCode, name, regtype, domain);
			}

			void handleServiceRegistered(DNSServiceErrorType errorCode, const char *name, const char *regtype, const char *domain) {
				if (errorCode != kDNSServiceErr_NoError) {
					MainEventLoop::postEvent(boost::bind(boost::ref(onRegisterFinished), boost::optional<DNSSDServiceID>()), shared_from_this());
				}
				else {
				}
			}
			*/

		private:
			String name;
			int port;
			ByteArray txtRecord;
			AvahiEntryGroup* group;
	};
}