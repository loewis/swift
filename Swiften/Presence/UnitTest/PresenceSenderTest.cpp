#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#include "Swiften/Client/DummyStanzaChannel.h"
#include "Swiften/Presence/PresenceSender.h"

using namespace Swift;

class PresenceSenderTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(PresenceSenderTest);
		CPPUNIT_TEST(testSendPresence);
		CPPUNIT_TEST(testSendPresence_UndirectedPresenceWithDirectedPresenceReceivers);
		CPPUNIT_TEST(testSendPresence_DirectedPresenceWithDirectedPresenceReceivers);
		CPPUNIT_TEST(testAddDirectedPresenceReceiver);
		CPPUNIT_TEST(testAddDirectedPresenceReceiver_AfterSendingDirectedPresence);
		CPPUNIT_TEST(testRemoveDirectedPresenceReceiver);
		CPPUNIT_TEST_SUITE_END();

	public:
		void setUp() {
			channel = new DummyStanzaChannel();
			testPresence = boost::shared_ptr<Presence>(new Presence());
			testPresence->setStatus("Foo");
			secondTestPresence = boost::shared_ptr<Presence>(new Presence());
			secondTestPresence->setStatus("Bar");
		}

		void tearDown() {
			delete channel;
		}

		void testSendPresence() {
			std::auto_ptr<PresenceSender> testling(createPresenceSender());
			testling->sendPresence(testPresence);

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel->sentStanzas.size()));
			boost::shared_ptr<Presence> presence = boost::dynamic_pointer_cast<Presence>(channel->sentStanzas[0]);
			CPPUNIT_ASSERT(testPresence == presence);
		}

		void testSendPresence_UndirectedPresenceWithDirectedPresenceReceivers() {
			std::auto_ptr<PresenceSender> testling(createPresenceSender());
			testling->addDirectedPresenceReceiver(JID("alice@wonderland.lit/teaparty"));

			testling->sendPresence(testPresence);

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(channel->sentStanzas.size()));
			boost::shared_ptr<Presence> presence = boost::dynamic_pointer_cast<Presence>(channel->sentStanzas[0]);
			CPPUNIT_ASSERT(testPresence == presence);
			presence = boost::dynamic_pointer_cast<Presence>(channel->sentStanzas[1]);
			CPPUNIT_ASSERT_EQUAL(testPresence->getStatus(), presence->getStatus());
			CPPUNIT_ASSERT_EQUAL(JID("alice@wonderland.lit/teaparty"), presence->getTo());
		}

		void testSendPresence_DirectedPresenceWithDirectedPresenceReceivers() {
			std::auto_ptr<PresenceSender> testling(createPresenceSender());
			testling->addDirectedPresenceReceiver(JID("alice@wonderland.lit/teaparty"));
			channel->sentStanzas.clear();

			testPresence->setTo(JID("foo@bar.com"));
			testling->sendPresence(testPresence);

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel->sentStanzas.size()));
			boost::shared_ptr<Presence> presence = boost::dynamic_pointer_cast<Presence>(channel->sentStanzas[0]);
			CPPUNIT_ASSERT(testPresence == presence);
		}

		void testAddDirectedPresenceReceiver() {
			std::auto_ptr<PresenceSender> testling(createPresenceSender());
			testling->sendPresence(testPresence);
			channel->sentStanzas.clear();

			testling->addDirectedPresenceReceiver(JID("alice@wonderland.lit/teaparty"));

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel->sentStanzas.size()));
			boost::shared_ptr<Presence> presence = boost::dynamic_pointer_cast<Presence>(channel->sentStanzas[0]);
			CPPUNIT_ASSERT_EQUAL(testPresence->getStatus(), presence->getStatus());
			CPPUNIT_ASSERT_EQUAL(JID("alice@wonderland.lit/teaparty"), presence->getTo());
		}

		void testAddDirectedPresenceReceiver_AfterSendingDirectedPresence() {
			std::auto_ptr<PresenceSender> testling(createPresenceSender());
			testling->sendPresence(testPresence);
			secondTestPresence->setTo(JID("foo@bar.com"));
			testling->sendPresence(secondTestPresence);
			channel->sentStanzas.clear();

			testling->addDirectedPresenceReceiver(JID("alice@wonderland.lit/teaparty"));

			CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(channel->sentStanzas.size()));
			boost::shared_ptr<Presence> presence = boost::dynamic_pointer_cast<Presence>(channel->sentStanzas[0]);
			CPPUNIT_ASSERT_EQUAL(testPresence->getStatus(), presence->getStatus());
			CPPUNIT_ASSERT_EQUAL(JID("alice@wonderland.lit/teaparty"), presence->getTo());
		}

		void testRemoveDirectedPresenceReceiver() {
			std::auto_ptr<PresenceSender> testling(createPresenceSender());
			testling->addDirectedPresenceReceiver(JID("alice@wonderland.lit/teaparty"));
			channel->sentStanzas.clear();

			testling->removeDirectedPresenceReceiver(JID("alice@wonderland.lit/teaparty"));
			testling->sendPresence(testPresence);

			CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(channel->sentStanzas.size()));
			CPPUNIT_ASSERT_EQUAL(boost::dynamic_pointer_cast<Presence>(channel->sentStanzas[0])->getType(), Presence::Unavailable);
			CPPUNIT_ASSERT(channel->sentStanzas[1] == testPresence);
		}

	private:
		PresenceSender* createPresenceSender() {
			return new PresenceSender(channel);
		}
	
	private:
		DummyStanzaChannel* channel;
		boost::shared_ptr<Presence> testPresence;
		boost::shared_ptr<Presence> secondTestPresence;
};

CPPUNIT_TEST_SUITE_REGISTRATION(PresenceSenderTest);