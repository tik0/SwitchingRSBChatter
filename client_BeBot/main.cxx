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
    string nick = "client_BeBot";

    rsb::Factory &factory = rsb::Factory::getInstance();

    rsb::Informer<string>::Ptr informer
        = factory.createInformer<string>("/chat/text/" + nick);
    rsb::ListenerPtr listener = factory.createListener("/chat/text");
    listener->addFilter(rsb::filter::FilterPtr(new rsb::filter::OriginFilter(informer->getId(), true)));
    listener->addHandler(rsb::HandlerPtr(new rsb::EventFunctionHandler(&printMessage)));

    while (true) {
        cout << "> ";
        cout.flush();
        shared_ptr<string> message(new string());
        getline(cin, *message);
        if (*message == "/quit") {
            break;
        }
        informer->publish(message);
    }

    return EXIT_SUCCESS;
}
