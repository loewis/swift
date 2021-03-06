/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFT_QtChatView_H
#define SWIFT_QtChatView_H

#include <QString>
#include <QWidget>
#include <QList>
#include <QWebElement>

#include <boost/shared_ptr.hpp>

#include "ChatSnippet.h"

#include <Swift/Controllers/UIInterfaces/ChatWindow.h>

class QWebPage;
class QUrl;
class QDate;

namespace Swift {
	class QtWebView;
	class QtChatTheme;
	class QtChatView : public QWidget {
			Q_OBJECT
		public:
			QtChatView(QtChatTheme* theme, QWidget* parent, bool disableAutoScroll = false);
			void addMessageTop(boost::shared_ptr<ChatSnippet> snippet);
			void addMessageBottom(boost::shared_ptr<ChatSnippet> snippet);
			void addLastSeenLine();
			void replaceLastMessage(const QString& newMessage);
			void replaceLastMessage(const QString& newMessage, const QString& note);
			void replaceMessage(const QString& newMessage, const QString& id, const QDateTime& time);
			void rememberScrolledToBottom();
			void setAckXML(const QString& id, const QString& xml);
			void setReceiptXML(const QString& id, const QString& xml);
			void displayReceiptInfo(const QString& id, bool showIt);

			QString getLastSentMessage();
			void addToJSEnvironment(const QString&, QObject*);
			void setFileTransferProgress(QString id, const int percentageDone);
			void setFileTransferStatus(QString id, const ChatWindow::FileTransferState state, const QString& msg);
			void setWhiteboardSessionStatus(QString id, const ChatWindow::WhiteboardSessionState state);
			void setMUCInvitationJoined(QString id);
			void showEmoticons(bool show);
			int getSnippetPositionByDate(const QDate& date);

		signals:
			void gotFocus();
			void fontResized(int);
			void logCleared();
			void scrollRequested(int pos);
			void scrollReachedTop();
			void scrollReachedBottom();

		public slots:
			void copySelectionToClipboard();
			void scrollToBottom();
			void handleLinkClicked(const QUrl&);
			void handleKeyPressEvent(QKeyEvent* event);
			void resetView();
			void resetTopInsertPoint();
			void increaseFontSize(int numSteps = 1);
			void decreaseFontSize();
			void resizeFont(int fontSizeSteps);

		private slots:
			void handleViewLoadFinished(bool);
			void handleFrameSizeChanged();
			void handleClearRequested();
			void handleScrollRequested(int dx, int dy, const QRect& rectToScroll);

		private:
			void headerEncode();
			void messageEncode();
			void addToDOM(boost::shared_ptr<ChatSnippet> snippet);
			QWebElement snippetToDOM(boost::shared_ptr<ChatSnippet> snippet);

			bool viewReady_;
			bool isAtBottom_;
			bool topMessageAdded_;
			int scrollBarMaximum_;
			QtWebView* webView_;
			QWebPage* webPage_;
			int fontSizeSteps_;
			QtChatTheme* theme_;
			QWebElement newInsertPoint_;
			QWebElement topInsertPoint_;
			QWebElement lineSeparator_;
			QWebElement lastElement_;
			QWebElement firstElement_;
			QWebElement document_;
			bool disableAutoScroll_;
	};
}

#endif
