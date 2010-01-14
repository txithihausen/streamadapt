/*
 * policyengine.cpp
 *
 *  Created on: 01/01/2010
 *      Author: ivocalado
 */

#include <policyengine.h>
#include <logger/logger.h>
#include <sstream>
namespace infrastream {

PolicyEngine::PolicyEngine() {
}

PolicyEngine::~PolicyEngine() {
}

//void PolicyEngine::addListener(Session* listener) {
//	log_info("Adding listener to map");
//	set<EventType> deps = listener->getDependencies();
//	for (set<EventType>::iterator it = deps.begin(); it != deps.end(); it++) {
//		map<EventType, set<Session*> >::iterator it2 = this->listeners.find(
//				*it);
//		if (it2 == listeners.end()) {
//			set<Session*> l;
//			l.insert(listener);
//			listeners[*it] = l;
//		} else
//			it2->second.insert(listener);
//	}
//}

void PolicyEngine::registerProvider(EventType type, PluginBase* plugin,
		Event default_event) throw(InvalidPolicyException) {
	map<EventType, PluginBase*>::iterator it = providers.find(type);
	if (it != providers.end())
		throw InvalidPolicyException(
				"Only one provider per information is allowed."
					" Reconfigure the police");
	else {
		log_info("Registering new provider: ");
		log_info(string("Provider Name: ") + plugin->getName());
		log_info("Info name: "+ type.getName());
		log_info("Registering default: ");
		log_info(default_event.getType().getName());
		providers[type] = plugin;
		currentValues.insert(default_event);
	}
}

bool PolicyEngine::unregisterProvider(EventType type) {
	map<EventType, PluginBase*>::iterator it = providers.find(type);
	if (it == providers.end())
		return false;
	providers.erase(it);
	return true;
}

// This function MUST be called into a thread to updates
void PolicyEngine::updateData() {
	for (map<EventType, PluginBase*>::iterator it = providers.begin(); it
			!= providers.end(); it++) {
		PluginBase* plugin = it->second;
		string s;

		try {
			s = plugin->retrievePluginInformation(it->first.getName());
			istringstream str(s); //OBJETIVO DESTA FUNCAO EH ATUALIZAR VALORES DE EVENT E DISPARAR EVENTOS CASO O VALOR CORRENTE SEJA DIFERENTE DO NOVO
			double eventValue;
			str >> eventValue;
			if (str.fail()) {
				log_error("Fail to update" + it->first.getName());
				continue;
			}

			Event newEvent(it->first, eventValue);

			set<Event>::iterator itEv = currentValues.find(newEvent);
			newEvent.setPayload(eventValue);
			if (itEv == currentValues.end() || itEv->getPayload()
					!= newEvent.getPayload()) {
				currentValues.insert(*itEv);
				fireEvent(*itEv);
			} else
				log_info("Event ignored");

		} catch (OperationNotSupportedException) {
			log_error("Problem launched in retrieve plugin information");
			s = __default_value(it->first.getName());
			for (set<Event>::iterator i = currentValues.begin(); i
					!= currentValues.end(); i++)
				if (i->getType().getName() == s) {
					fireEvent(Event(it->first, i->getPayload()));
					break;
				}
		}

	}
}

int PolicyEngine::fireEvent(Event event) {
	map<EventType, set<Session*> >::iterator it = listeners.find(
			event.getType());
	if (it == listeners.end())
		return 0;
	for (set<Session*>::iterator it2 = it->second.begin(); it2
			!= it->second.end(); it2++)
		(*it2)->newEvent(event);
	return it->second.size();
}

Event PolicyEngine::getLastEvent(EventType type) {
	Event default_event;
	EventType defaultEventType = EventType(__default_value(type.getName()));
	for (set<Event>::iterator it = currentValues.begin(); it
			!= currentValues.end(); it++) {
		if (it->getType() == type)
			return *it;
		else if (it->getType() == defaultEventType)
			default_event = *it;
	}
	return default_event;
}

}