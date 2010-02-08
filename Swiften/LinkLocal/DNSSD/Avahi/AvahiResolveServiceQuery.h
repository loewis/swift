#pragma once

#include "Swiften/LinkLocal/DNSSD/Avahi/AvahiQuery.h"
#include "Swiften/LinkLocal/DNSSD/DNSSDResolveServiceQuery.h"
#include "Swiften/LinkLocal/LinkLocalServiceInfo.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/EventLoop/MainEventLoop.h"

namespace Swift {
	class AvahiQuerier;

	class AvahiResolveServiceQuery : public DNSSDResolveServiceQuery, public AvahiQuery {
		public:	
			AvahiResolveServiceQuery(const DNSSDServiceID& service, boost::shared_ptr<AvahiQuerier> querier) : AvahiQuery(querier), service(service), resolver(NULL) {
			}

			void start() {
				std::cout << "Resolving " << service.getName() << std::endl;
				avahi_threaded_poll_lock(querier->getThreadedPoll());
				assert(!resolver);
				resolver = avahi_service_resolver_new(querier->getClient(), service.getNetworkInterfaceID(), AVAHI_PROTO_UNSPEC, service.getName().getUTF8Data(), service.getType().getUTF8Data(), service.getDomain().getUTF8Data(), AVAHI_PROTO_UNSPEC, (AvahiLookupFlags) 0, handleServiceResolvedStatic, this);
				if (!resolver) {
					std::cout << "Error starting resolver" << std::endl;
					MainEventLoop::postEvent(boost::bind(boost::ref(onServiceResolved), boost::optional<Result>()), shared_from_this());
				}
				avahi_threaded_poll_unlock(querier->getThreadedPoll());
			}

			void stop() {
				avahi_threaded_poll_lock(querier->getThreadedPoll());
				avahi_service_resolver_free(resolver);
				resolver = NULL;
				avahi_threaded_poll_unlock(querier->getThreadedPoll());
			}

		private:
			static void handleServiceResolvedStatic(AvahiServiceResolver* resolver, AvahiIfIndex interfaceIndex, AvahiProtocol protocol, AvahiResolverEvent event, const char *name, const char *type, const char *domain, const char *host_name, const AvahiAddress *address, uint16_t port, AvahiStringList *txt, AvahiLookupResultFlags flags, void* context) {
				static_cast<AvahiResolveServiceQuery*>(context)->handleServiceResolved(resolver, interfaceIndex, protocol, event, name, type, domain, host_name, address, port, txt, flags);
			}

			void handleServiceResolved(AvahiServiceResolver* resolver, AvahiIfIndex, AvahiProtocol, AvahiResolverEvent event, const char *name, const char * type, const char* domain, const char * /*host_name*/, const AvahiAddress *address, uint16_t port, AvahiStringList *txt, AvahiLookupResultFlags) {
				std::cout << "Resolve finished" << std::endl;
				switch(event) {
					case AVAHI_RESOLVER_FAILURE:
						std::cout << "Resolve error " << avahi_strerror(avahi_client_errno(avahi_service_resolver_get_client(resolver))) << std::endl;
						MainEventLoop::postEvent(boost::bind(boost::ref(onServiceResolved), boost::optional<Result>()), shared_from_this());
						break;
					case AVAHI_RESOLVER_FOUND: {
						char a[AVAHI_ADDRESS_STR_MAX];
						avahi_address_snprint(a, sizeof(a), address);

						ByteArray txtRecord;
						txtRecord.resize(1024);
						avahi_string_list_serialize(txt, txtRecord.getData(), txtRecord.getSize());

						// FIXME: Probably not accurate
						String fullname = String(name) + "." + String(type) + "." + String(domain) + ".";
						std::cout << "Result: " << fullname << "->" << String(a) << ":" << port << std::endl;
						MainEventLoop::postEvent(
								boost::bind(
									boost::ref(onServiceResolved), 
									Result(fullname, String(a), port, txtRecord)),
								shared_from_this());
						break;
					}
				}
			}

		private:
			DNSSDServiceID service;
			AvahiServiceResolver* resolver;
	};
}