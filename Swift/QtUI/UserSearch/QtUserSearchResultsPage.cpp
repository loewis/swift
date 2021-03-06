/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swift/QtUI/UserSearch/QtUserSearchResultsPage.h"

namespace Swift {

QtUserSearchResultsPage::QtUserSearchResultsPage() {
	setupUi(this);
	connect(results_, SIGNAL(activated(const QModelIndex&)), this, SLOT(emitCompletenessCheck()));
	connect(results_, SIGNAL(activated(const QModelIndex&)), this, SIGNAL(onUserTriggersContinue()));
	connect(results_, SIGNAL(clicked(const QModelIndex&)), this, SLOT(emitCompletenessCheck()));
	connect(results_, SIGNAL(entered(const QModelIndex&)), this, SLOT(emitCompletenessCheck()));
	results_->setExpandsOnDoubleClick(false);
	setNoResults(false);
}

bool QtUserSearchResultsPage::isComplete() const {
	return results_->currentIndex().isValid();
}

void QtUserSearchResultsPage::setNoResults(bool noResults) {
	if (noResults) {
		results_->setEnabled(false);
		noResults_->show();
	} else {
		results_->setEnabled(true);
		noResults_->hide();
	}
}

void QtUserSearchResultsPage::emitCompletenessCheck() {
	emit completeChanged();
}

}
