/*
 * Copyright (c) 2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */


#include <SwifTools/CrashReporter.h>
#include <Swiften/Base/Platform.h>

#if defined(HAVE_BREAKPAD)

#pragma GCC diagnostic ignored "-Wold-style-cast"

#include <boost/smart_ptr/make_shared.hpp>

#ifdef SWIFTEN_PLATFORM_MACOSX
#include "client/mac/handler/exception_handler.h"
#endif
#ifdef SWIFTEN_PLATFORM_WINDOWS
#include "client/windows/handler/exception_handler.h"
#endif

#if defined(SWIFTEN_PLATFORM_WINDOWS)
static bool handleDump(const wchar_t* /* dir */, const wchar_t* /* id*/, void* /* context */, EXCEPTION_POINTERS*, MDRawAssertionInfo*, bool /* succeeded */) {
	return false;
}
#else
static bool handleDump(const char* /* dir */, const char* /* id*/, void* /* context */, bool /* succeeded */) {
	return false;
}
#endif

namespace Swift {

struct CrashReporter::Private {
	boost::shared_ptr<google_breakpad::ExceptionHandler> handler;
};

CrashReporter::CrashReporter(const boost::filesystem::path& path) {
	// Create the path that will contain the crash dumps
	if (!boost::filesystem::exists(path)) {
		try {
			boost::filesystem::create_directories(path);
		}
		catch (const boost::filesystem::filesystem_error& e) {
			std::cerr << "ERROR: " << e.what() << std::endl;
		}
	}

	p = boost::make_shared<Private>();
#if defined(SWIFTEN_PLATFORM_WINDOWS)
	// FIXME: Need UTF8 conversion from string to wstring
	std::string pathString = path.string();
	p->handler = boost::make_shared<google_breakpad::ExceptionHandler>(
			std::wstring(pathString.begin(), pathString.end()), 
			(google_breakpad::ExceptionHandler::FilterCallback) 0, 
			handleDump, 
			(void*) 0, 
			google_breakpad::ExceptionHandler::HANDLER_ALL);
// Turning it off for Mac, because it doesn't really help us
//#elif defined(SWIFTEN_PLATFORM_MACOSX)
//	p->handler = boost::make_shared<google_breakpad::ExceptionHandler>(path.string(), (google_breakpad::ExceptionHandler::FilterCallback) 0, handleDump, (void*) 0, true, (const char*) 0);
#endif
}

};

#else

// Dummy implementation
namespace Swift {
	CrashReporter::CrashReporter(const boost::filesystem::path&) {}
};

#endif
