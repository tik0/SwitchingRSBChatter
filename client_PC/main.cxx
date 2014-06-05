#include <string>

#include <boost/shared_ptr.hpp>

#include <rsb/Event.h>
#include <rsb/Factory.h>
#include <rsb/Handler.h>
#include <rsb/filter/OriginFilter.h>

using namespace std;

using namespace boost;

void printMessage(rsb::EventPtr event) {
    shared_ptr<string> message
        = static_pointer_cast<string>(event->getData());

    string sender = event->getScope().getComponents().back();

    cout << "\r" << sender << ": " << *message  << endl
         << "> ";
    cout.flush();
}

int main(int argc, char *argv[]) {
    string nick = "client_PC";

    rsb::Factory &factory = rsb::Factory::getInstance();

    // Get the global participant config as a template
    rsb::ParticipantConfig tmpPartConf = factory.getDefaultParticipantConfig();

    // Let us view the default config set by "rsb.conf"
//    std::cout << tmpPartConf <<  std::endl;

    // Get the options for socket transport, because we want to change them
    rsc::runtime::Properties tmpProp  = tmpPartConf.mutableTransport("socket").getOptions();

    // Let us view the default properties for socket transport set by "rsb.conf"
//    std::cout << tmpProp <<  std::endl;

    // Get the different options
//    std::string enabled = boost::any_cast<std::string>(tmpProp["enabled"]);
//    std::string host    = boost::any_cast<std::string>(tmpProp["host"]);
//    std::string port    = boost::any_cast<std::string>(tmpProp["port"]);
//    std::string server  = boost::any_cast<std::string>(tmpProp["server"]);

    // Change the config
    std::string host = "129.70.144.5";
    tmpProp["host"] = boost::any(host);

    // Write the socket tranport properties back to the participant config
    tmpPartConf.mutableTransport("socket").setOptions(tmpProp);

    // Set the default config (We dont want to change this, because it is set by "rsb.conf")
//    factory.setDefaultParticipantConfig()

    // Set the local chatter (it uses the "rsb.conf")
    rsb::Informer<string>::Ptr informer
        = factory.createInformer<string>("/chat/text/" + nick);
    rsb::ListenerPtr listener = factory.createListener("/chat/text");
    listener->addFilter(rsb::filter::FilterPtr(new rsb::filter::OriginFilter(informer->getId(), true)));
    listener->addHandler(rsb::HandlerPtr(new rsb::EventFunctionHandler(&printMessage)));

    // Set the remote chatter (it uses the programatical solution)
    rsb::Informer<string>::Ptr informerRemote
		= factory.createInformer<string>("/chat/text/" + nick, tmpPartConf);
    rsb::ListenerPtr listenerRemote = factory.createListener("/chat/text", tmpPartConf);
    listenerRemote->addFilter(rsb::filter::FilterPtr(new rsb::filter::OriginFilter(informer->getId(), true)));
    listenerRemote->addHandler(rsb::HandlerPtr(new rsb::EventFunctionHandler(&printMessage)));


    while (true) {
        cout << "> ";
        cout.flush();
        shared_ptr<string> message(new string());
        getline(cin, *message);
        if (*message == "/quit") {
            break;
        }
        informer->publish(message);
        informerRemote->publish(message);
    }

    return EXIT_SUCCESS;
}
