#include "Swiften/Network/PlatformDomainNameServiceQuery.h"

#include "Swiften/Base/Platform.h"
#include <stdlib.h>
#ifdef SWIFTEN_PLATFORM_WINDOWS
#undef UNICODE
#include <windows.h>
#include <windns.h>
#ifndef DNS_TYPE_SRV
#define DNS_TYPE_SRV 33
#endif
#else
#include <arpa/nameser.h>
#include <arpa/nameser_compat.h>
#include <resolv.h>
#endif
#include <boost/bind.hpp>

#include "Swiften/Base/ByteArray.h"
#include "Swiften/EventLoop/MainEventLoop.h"
#include "Swiften/Base/foreach.h"

using namespace Swift;

namespace {
	struct SRVRecordPriorityComparator {
		bool operator()(const DomainNameServiceQuery::Result& a, const DomainNameServiceQuery::Result& b) const {
			return a.priority < b.priority;
		}
	};
}

namespace Swift {

PlatformDomainNameServiceQuery::PlatformDomainNameServiceQuery(const String& service) : thread(NULL), service(service), safeToJoin(true) {
}

PlatformDomainNameServiceQuery::~PlatformDomainNameServiceQuery() {
	if (safeToJoin) {
		thread->join();
	}
	else {
		// FIXME: UGLYYYYY
	}
	delete thread;
}

void PlatformDomainNameServiceQuery::run() {
	safeToJoin = false;
	thread = new boost::thread(boost::bind(&PlatformDomainNameServiceQuery::doRun, shared_from_this()));
}

void PlatformDomainNameServiceQuery::doRun() {
	std::vector<DomainNameServiceQuery::Result> records;

#if defined(SWIFTEN_PLATFORM_WINDOWS)
	DNS_RECORD* responses;
	// FIXME: This conversion doesn't work if unicode is deffed above
	if (DnsQuery(service.getUTF8Data(), DNS_TYPE_SRV, DNS_QUERY_STANDARD, NULL, &responses, NULL) != ERROR_SUCCESS) {
		emitError();
		return;
	}

	DNS_RECORD* currentEntry = responses;
	while (currentEntry) {
		if (currentEntry->wType == DNS_TYPE_SRV) {
			DomainNameServiceQuery::Result record;
			record.priority = currentEntry->Data.SRV.wPriority;
			record.weight = currentEntry->Data.SRV.wWeight;
			record.port = currentEntry->Data.SRV.wPort;
				
			// The pNameTarget is actually a PCWSTR, so I would have expected this 
			// conversion to not work at all, but it does.
			// Actually, it doesn't. Fix this and remove explicit cast
			// Remove unicode undef above as well
			record.hostname = String((const char*) currentEntry->Data.SRV.pNameTarget);
			records.push_back(record);
		}
		currentEntry = currentEntry->pNext;
	}
	DnsRecordListFree(responses, DnsFreeRecordList);

#else

	std::cout << "SRV: Querying " << service << std::endl;
	ByteArray response;
	response.resize(NS_PACKETSZ);
	int responseLength = res_query(const_cast<char*>(service.getUTF8Data()), ns_c_in, ns_t_srv, reinterpret_cast<u_char*>(response.getData()), response.getSize());
	std::cout << "res_query done " << (responseLength != -1) << std::endl;
	if (responseLength == -1) {
		emitError();
		return;
	}

	// Parse header
	HEADER* header = reinterpret_cast<HEADER*>(response.getData());
	unsigned char* messageStart = reinterpret_cast<unsigned char*>(response.getData());
	unsigned char* messageEnd = messageStart + responseLength;
	unsigned char* currentEntry = messageStart + NS_HFIXEDSZ;

	// Skip over the queries
	int queriesCount = ntohs(header->qdcount);
	while (queriesCount > 0) {
		int entryLength = dn_skipname(currentEntry, messageEnd);
		if (entryLength < 0) {
			emitError();
			return;
		}
		currentEntry += entryLength + NS_QFIXEDSZ;
		queriesCount--;
	}

	// Process the SRV answers
	int answersCount = ntohs(header->ancount);
	while (answersCount > 0) {
		DomainNameServiceQuery::Result record;

		int entryLength = dn_skipname(currentEntry, messageEnd);
		currentEntry += entryLength;
		currentEntry += NS_RRFIXEDSZ;

		// Priority
		if (currentEntry + 2 >= messageEnd) {
			emitError();
			return;
		}
		record.priority = ns_get16(currentEntry);
		currentEntry += 2;

		// Weight
		if (currentEntry + 2 >= messageEnd) {
			emitError();
			return;
		}
		record.weight = ns_get16(currentEntry);
		currentEntry += 2;

		// Port
		if (currentEntry + 2 >= messageEnd) {
			emitError();
			return;
		}
		record.port = ns_get16(currentEntry);
		currentEntry += 2; 

		// Hostname
		if (currentEntry >= messageEnd) {
			emitError();
			return;
		}
		ByteArray entry;
		entry.resize(NS_MAXDNAME);
		entryLength = dn_expand(messageStart, messageEnd, currentEntry, entry.getData(), entry.getSize());
		if (entryLength < 0) {
			emitError();
			return;
		}
		record.hostname = String(entry.getData());
		records.push_back(record);
		currentEntry += entryLength;
		answersCount--;
	}
#endif

	safeToJoin = true;
	std::sort(records.begin(), records.end(), SRVRecordPriorityComparator());
	std::cout << "Sending out " << records.size() << " SRV results " << std::endl;
	MainEventLoop::postEvent(boost::bind(boost::ref(onResult), records)); 
}

void PlatformDomainNameServiceQuery::emitError() {
	safeToJoin = true;
	MainEventLoop::postEvent(boost::bind(boost::ref(onResult), std::vector<DomainNameServiceQuery::Result>()), shared_from_this());
}

}