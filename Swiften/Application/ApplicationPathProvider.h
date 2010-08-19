/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/filesystem.hpp>

#include "Swiften/Base/String.h"

namespace Swift {
	class ApplicationPathProvider {
		public:
			ApplicationPathProvider(const String& applicationName);
			virtual ~ApplicationPathProvider();

			boost::filesystem::path getSettingsFileName() const;
			boost::filesystem::path getAvatarDir() const;
			virtual boost::filesystem::path getHomeDir() const = 0;
			virtual boost::filesystem::path getSettingsDir() const = 0;
			boost::filesystem::path getProfileDir(const String& profile) const;

		protected:
			const String& getApplicationName() const {
				return applicationName;
			}

		private:
			String applicationName;
	};
}