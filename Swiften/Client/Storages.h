/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>

namespace Swift {
	class VCardStorage;
	class AvatarStorage;
	class CapsStorage;
	class RosterStorage;
	class HistoryStorage;

	/**
	 * An interface to hold storage classes for different
	 * controllers.
	 */
	class SWIFTEN_API Storages {
		public:
			virtual ~Storages();

			virtual VCardStorage* getVCardStorage() const = 0;
			virtual AvatarStorage* getAvatarStorage() const = 0;
			virtual CapsStorage* getCapsStorage() const = 0;
			virtual RosterStorage* getRosterStorage() const = 0;
			virtual HistoryStorage* getHistoryStorage() const = 0;
	};
}
