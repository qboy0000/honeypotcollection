#include <gloox/client.h>
#include <gloox/connectionlistener.h>
#include <gloox/mucroomhandler.h>
#include <gloox/mucroom.h>
#include <gloox/disco.h>
#include <gloox/presence.h>
#include <gloox/message.h>
#include <gloox/dataform.h>
#include <gloox/gloox.h>
#include <gloox/lastactivity.h>
#include <gloox/loghandler.h>
#include <gloox/logsink.h>
using namespace gloox;

#include <iostream>
#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <pthread.h>
#include <cstdio> // [s]print[f]
#include <sys/stat.h> //mkfifo

//GLIB
#include <glib.h>

//LibXML
#include <libxml++/libxml++.h>

#include <fstream>
using namespace std;

#ifdef WIN32
#include <windows.h>
#endif

class XMPP_listener : public ConnectionListener, LogHandler, MUCRoomHandler
{
  public:

    const char *server, *nick, *password, *channel, *domain, *fifo;

    XMPP_listener(const char *server, const char *nick, const char *password, const char *channel, const char *domain, const char *fifo) {
	this->server=server;
	this->nick=nick;
	this->password=password;
	this->channel=channel;
	this->domain=domain;
	this->fifo=fifo;
	mkfifo(this->fifo,0777);

	cout << "XMPP listener created!\n";
    }
    virtual ~XMPP_listener() {}

    void start()
    {

      cout << "Starting XMPP listener\n";
      string fullnick=this->nick;
	fullnick.append("@");
	fullnick.append(this->server);
      string join=this->channel;
	join.append("@");
	join.append(this->domain);
	join.append("/");
	join.append(this->nick);
      string pw=this->password;

	cout << "Connecting to " << fullnick.c_str() << " and channel " << join.c_str() << "\n";

      JID jid( fullnick );
      j = new Client( jid, pw );
      j->registerConnectionListener( this );
      j->setPresence( Presence::Available, -1 );
      j->disco()->setVersion( "gloox muc_example", GLOOX_VERSION, "Linux" );
      j->disco()->setIdentity( "client", "bot" );
      j->setCompression( false );
      StringList ca;
      ca.push_back( "/path/to/cacert.crt" );
      j->setCACerts( ca );

      j->logInstance().registerLogHandler( LogLevelDebug, LogAreaAll, this );

      JID nick( join );
      m_room = new MUCRoom( j, nick, this, 0 );

      if( j->connect( false ) )
      {
        ConnectionError ce = ConnNoError;
        while( ce == ConnNoError )
        {
          ce = j->recv();
        }
        printf( "ce: %d\n", ce );
      }

      // cleanup
      delete m_room;
      delete j;
      cout << "XMPP listener started!\n";
    }

    virtual void onConnect()
    {
      printf( "connected!!!\n" );
      m_room->join();
      m_room->getRoomInfo();
      m_room->getRoomItems();
    }

    virtual void onDisconnect( ConnectionError e )
    {
      printf( "message_test: disconnected: %d\n", e );
      if( e == ConnAuthenticationFailed )
        printf( "auth failed. reason: %d\n", j->authError() );
    }

    virtual bool onTLSConnect( const CertInfo& info )
    {
      printf( "status: %d\nissuer: %s\npeer: %s\nprotocol: %s\nmac: %s\ncipher: %s\ncompression: %s\n",
              info.status, info.issuer.c_str(), info.server.c_str(),
              info.protocol.c_str(), info.mac.c_str(), info.cipher.c_str(),
              info.compression.c_str() );
      return true;
    }

/*    virtual bool parseXML(const xmlpp::Node* pNode, bool dionaea, SplayTree::honeypotEvent * event) {
		const Glib::ustring nodename = pNode->get_name();

		if(nodename == "dionaea") {
			//std::cout << "Found dionaea node!\n";
			dionaea=true;
		}

		const xmlpp::ContentNode* nodeContent = dynamic_cast<const xmlpp::ContentNode*>(pNode);
  		const xmlpp::TextNode* nodeText = dynamic_cast<const xmlpp::TextNode*>(pNode);
  		const xmlpp::CommentNode* nodeComment = dynamic_cast<const xmlpp::CommentNode*>(pNode);

		if(nodeContent || nodeText || nodeComment) {
		} else {
			if(dionaea && event != NULL) {
				//std::cout << "Event is " << event << "\n";
                		const xmlpp::Element* nodeElement = dynamic_cast<const xmlpp::Element*>(pNode);
                		const xmlpp::Element::AttributeList& attributes = nodeElement->get_attributes();
    				std::cout << " --\n";
	            		for(xmlpp::Element::AttributeList::const_iterator iter = attributes.begin(); iter != attributes.end(); ++iter)
                        	{
                                	const xmlpp::Attribute* attribute = *iter;
                                	std::cout << "  Attribute " << attribute->get_name() << " = " << attribute->get_value() << std::endl;

					if(attribute->get_name() == "incident")
					 event->incident=attribute->get_value();
					else if(attribute->get_name() == "ref" || attribute->get_name() == "child")
					 event->reference=atol(attribute->get_value().c_str());
					else if(attribute->get_name() == "transport")
					 event->transport+=attribute->get_value();
				 	else if(attribute->get_name() == "local_port")
					 event->localPort=atol(attribute->get_value().c_str());
					else if(attribute->get_name() == "local_host")
					 event->localIP+=(attribute->get_value());
					else if(attribute->get_name() == "remote_port")
					 event->remotePort=atol(attribute->get_value().c_str());
					else if(attribute->get_name() == "remote_host")
					 event->remoteIP+=(attribute->get_value());
					else if(attribute->get_name() == "protocol")
					 event->protocol+=(attribute->get_value());
					else if(attribute->get_name() == "type")
					 event->type+=(attribute->get_value());
                        	}
			}

                	xmlpp::Node::NodeList list = pNode->get_children();
                	for(xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
                        	{
                              	 dionaea = parseXML(*iter, dionaea, event); //recursive
                       		}
		}

		return dionaea;
    }
*/
    virtual void handleLog( LogLevel level, LogArea area, const std::string& message )
    {
	// !!!! ADD STUFF HERE !!!! //

      //printf("----------\ngot info loglevel: %d, area: %d, %s\n", level, area, message.c_str() );

	try {
		xmlpp::DomParser parser;
		parser.set_substitute_entities();
      		parser.parse_memory_raw((unsigned char*)message.c_str(), message.size());
		if(parser) {
			//std::cout << "------------\n";
			//SplayTree::honeypotEvent event;

			//bool dionaea = parseXML( parser.get_document()->get_root_node(), false, &event );

			//if(dionaea) {
			//	std::cout << "------ PARSED \n " << event.reference << ": " << event.incident << "\n";
			//}

			//Properly formed XML, write to buffer
			ofstream myfile (this->fifo);
  			if (myfile.is_open())
  			{
    				myfile << message.c_str() << endl;
    				myfile.close();
  			} else
			cout << "Failed opening FIFO!\n";
		}

	} catch(const std::exception& ex)
  	{
  	  //std::cout << "Exception caught: " << ex.what() << std::endl;
  	}
    }

    virtual void handleMUCParticipantPresence( MUCRoom * /*room*/, const MUCRoomParticipant participant,
                                            const Presence& presence )
    {
      if( presence.presence() == Presence::Available )
        printf( "!!!!!!!!!!!!!!!! %s is in the room too\n", participant.nick->resource().c_str() );
      else if( presence.presence() == Presence::Unavailable )
        printf( "!!!!!!!!!!!!!!!! %s left the room\n", participant.nick->resource().c_str() );
      //else
      //  printf( "Presence is %d of %s\n", presence.presence(), participant.nick->resource().c_str() );
    }

    virtual void handleMUCMessage( MUCRoom* /*room*/, const Message& msg, bool priv )
    {
      //printf( "%s said: '%s' (history: %s, private: %s)\n", msg.from().resource().c_str(), msg.body().c_str(),
      //        msg.when() ? "yes" : "no", priv ? "yes" : "no" );
    }

    virtual void handleMUCSubject( MUCRoom * /*room*/, const std::string& nick, const std::string& subject )
    {
      //if( nick.empty() )
      //  printf( "Subject: %s\n", subject.c_str() );
      //else
      //  printf( "%s has set the subject to: '%s'\n", nick.c_str(), subject.c_str() );
    }

    virtual void handleMUCError( MUCRoom * /*room*/, StanzaError error )
    {
      printf( "!!!!!!!!got an error: %d", error );
    }

    virtual void handleMUCInfo( MUCRoom * /*room*/, int features, const std::string& name,
                                    const DataForm* infoForm )
    {
      printf( "features: %d, name: %s, form xml: %s\n",
              features, name.c_str(), infoForm->tag()->xml().c_str() );
    }

    virtual void handleMUCItems( MUCRoom * /*room*/, const Disco::ItemList& items )
    {
      Disco::ItemList::const_iterator it = items.begin();
      for( ; it != items.end(); ++it )
      {
        printf( "%s -- %s is an item here\n", (*it)->jid().full().c_str(), (*it)->name().c_str() );
      }
    }

    virtual void handleMUCInviteDecline( MUCRoom * /*room*/, const JID& invitee, const std::string& reason )
    {
      //printf( "Invitee %s declined invitation. reason given: %s\n", invitee.full().c_str(), reason.c_str() );
    }

    virtual bool handleMUCRoomCreation( MUCRoom *room )
    {
      //printf( "room %s didn't exist, beeing created.\n", room->name().c_str() );
      return true;
    }

  private:
    Client *j;
    MUCRoom *m_room;
};

//void xmpp_start() {
//	std::cout << "BOOSH!\n";
//}


int main( int argc, const char* argv[])
{

	XMPP_listener *r;

	if(argc==6) {
		//Honeybrid execl call doesn't have executable name as argv[0]
		r = new XMPP_listener(argv[0],argv[1],argv[2],argv[3],argv[4],argv[5]);
	} else if(argc==7) {
  		r = new XMPP_listener(argv[1],argv[2],argv[3],argv[4],argv[5],argv[6]);
	} else {
		 cout << "Insufficient number of arguments!\n";
                        for(int i=0;i<argc;i++) {
                                cout << "Got " << i << ": " << argv[i] << "\n";
                        }
                cout << "Usage: xmpp_listener nick@server password channel@domain/nick fifo_file\n";
		return 1;

	}
  	r->start();
  	delete( r );
  	return 0;
}
