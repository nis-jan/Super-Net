#include "sendrcv.h"
#include <stdlib.h>
#include <iostream>
#include <userdata.h>
#include <Display.h>
#include <Log.h>

#define SENDRCV_DEBUG 1
#define forwardtest 1



int sendrcv::send_msg(char* msg, int msgsize, int recipient, int packref){
	//create msg packet based on the given parameters and send it:
	if (packref == 0)packref = Packet::create_reference();
	Packet * p = new Packet();
	vector<int> route = {userdata::Instance()->MYADRESS(), recipient};
	
	Packet::create_msg(p, recipient, packref, route, msgsize, msg);

	sendpacket(p, true);

	return packref;
}


void sendrcv::rcv_packet(int packetsize){
	Packet *p = new Packet();
	p->raw_data = (char*)malloc(packetsize);
	// receive packet
	for(int i = 0; i < packetsize; i++){
		if(!LoRa.available())throw "transmission incomplete";
		p->raw_data[i] = LoRa.read();
	}
	// parse packetinformation:

	int recipient = bytes::bytes2int(p->raw_data, 0);
	
	// cope only with packets that are destined for us
	if(!(recipient == userdata::Instance()->MYADRESS() || recipient == 0)){
		delete p;
		return;
	}

	p->packetsize = packetsize;
	p->reference = bytes::bytes2int(p->raw_data, sizeof(int));
	p->type = Packet::byte2type[(p->raw_data[2*sizeof(int)])];
	p->print();
	
	interpretpacket(p, LoRa.packetRssi());
	Log::Instance()->Log_msg("[SEND/RECEIVE]: done interpreting");
	delete p;
}

void sendrcv::sendpacket(Packet* p, bool isInitialMessage) {
	LoRa.beginPacket();
	for(int i = 0; i < p->packetsize; i++)
		LoRa.write(p->raw_data[i]);
	LoRa.endPacket();

	// #ifdef SENDRCV_DEBUG
	// Log::Instance()->Log_msg("[*] Packet was sent ref: " + String(p->reference));
	// #endif

	if(!isInitialMessage) return;
	forwarding_tasks.insert(pair<int, TaskHandle_t*>{p->reference, new TaskHandle_t()});
	ack_received_flag.insert(pair<int, bool>{p->reference, false});
	xTaskCreatePinnedToCore(
		forwarddaemon,
		(const char*)String(p->reference).c_str(),
		2500,
		p,
		configMAX_PRIORITIES - 1,
		forwarding_tasks[p->reference],
		0
	);
}

void sendrcv::fetch_data_from_msg(Packet* p, vector<int> * route, char* &msg, int &msg_size, int &destination) {
	//if msg packet is encapsulated in forwarding packets, unwrap it first:
	Packet * _p = p;
	while(true){
		if(_p->type == Packet::packettype::forward){
			_p = fetch_packet_from_forward(_p);
		}
		else break;
	}


	int routesize = bytes::bytes2int(_p->raw_data, (2 * sizeof(int)) + 1);
	
	//fill route vector
	route->clear();
	int current_ind = (3 * sizeof(int)) + 1;
	int addr;
	for (int i = 0; i < routesize / sizeof(int); i++) {
		addr = bytes::bytes2int(_p->raw_data, current_ind);
		route->push_back(addr);
		current_ind += sizeof(int);
	}

	msg = _p->raw_data + 4*sizeof(int) + 1 + routesize;

	msg_size = _p->packetsize - (4 * sizeof(int) + 1 + routesize);
	destination = bytes::bytes2int(_p->raw_data);
}

void sendrcv::fetch_data_from_ihave(Packet* p, ihave_grid &ihavegrid, int &max_hops) {
	max_hops = bytes::bytes2int(p->raw_data, 2 * sizeof(int) + 1);
	int gridsize = bytes::bytes2int(p->raw_data, 3*sizeof(int)+1);
	int curr_ind = 4 * sizeof(int) + 1;
	int id, rssi;
	for (int i = 0; i < gridsize / (2*sizeof(int)); i++) {
		id = bytes::bytes2int(p->raw_data, curr_ind + 2*i*sizeof(int));
		rssi = bytes::bytes2int(p->raw_data, curr_ind + 2*i*sizeof(int) + sizeof(int));
		ihavegrid.add_hop(hop(id, rssi));
	}
}

Packet* sendrcv::fetch_packet_from_forward(Packet* fwd) {
	Packet* ret = new Packet();
	ret->reference = fwd->reference;
	ret->type = Packet::byte2type[fwd->raw_data[(5 * sizeof(int) + 1)]];
	ret->packetsize = bytes::bytes2int(fwd->raw_data, 2 * sizeof(int) + 1);
	
	ret->raw_data = (char*)malloc(ret->packetsize);
	bytes::bytecopy(fwd->raw_data, ret->raw_data, 3*sizeof(int)+1, 0, ret->packetsize);

	return ret;
}

void sendrcv::fetch_data_from_whohas(Packet* p, int& max_hops, int& wanted) {
	max_hops = bytes::bytes2int(p->raw_data, 2*sizeof(int)+1);
	wanted = bytes::bytes2int(p->raw_data, 3*sizeof(int)+1);
}


void sendrcv::kill_current_daemon(int reference, void* packetpointer){
	Log::Instance()->Log_msg("[-]: Daemon with ref " + String(reference) + "will be killed");
	Packet * pp = (Packet*)packetpointer;
	delete pp;
	ack_received_flag.erase(ack_received_flag.find(reference));
	forwarding_tasks.erase(forwarding_tasks.find(reference));
	vTaskDelete(NULL);
	return;
}


void sendrcv::forwarddaemon(void * params){
	int reference = bytes::bytes2int(((Packet*)params)->raw_data, sizeof(int));
	Log::Instance()->Log_msg("[+] forwarddaemon gestartet für Paket mit Referenz " + String(reference));
	//give time for ack to arrive
	vTaskDelay(WAIT4ACK_DELAY);
	//check if ack has arrived:
	if (ack_received_flag[reference])
		kill_current_daemon(reference, params);
	
	Log::Instance()->Log_msg("[DAEMON]: Trying to resend! (ref: " + String(reference) + ")");
	sendpacket((Packet*)params);
	
	vTaskDelay(WAIT4ACK_DELAY);
	//check if ack has arrived:
	if (ack_received_flag[reference])
		kill_current_daemon(reference, params);
	
	Log::Instance()->Log_msg("[Daemon]: Looking for alternative routes (ref: " + String(reference) + ")");
	//extract data from the packet, we want to send:
	vector<int> * dummyroute = new vector<int>(); // buffer, so we can extract everything from the original msg
	char * msg;
	int msgsize;
	int recipient;
	fetch_data_from_msg((Packet*)params, dummyroute, msg, msgsize, recipient);
	delete dummyroute;

	
	//send whohaspacket
	//possible_routes[reference].clear();
	
	Packet * p = new Packet();
	Packet::create_whohas(p, reference, MAX_IHAVE_HOPS, recipient);
	sendpacket(p);
	delete p;


	//wait for ihaves
	vTaskDelay(WAIT4IHAVE_DELAY);
	//check if ack has arrived:
	if (ack_received_flag[reference])
		kill_current_daemon(reference, params);
	// provide error message, incase no ihaves have arrived and therefore no possible routes have been found
	if(possible_routes.find(reference) == possible_routes.end()){
		On_Send_Error(reference, "Ziel nicht erreichbar!");
		kill_current_daemon(reference, params);
	}
	//create new msg packet with the most reliable route
	vector<int> *route;
	find_best_route(reference, route);
	p = new Packet();
	Packet::create_msg(p, recipient, reference, *route, msgsize, msg);
	sendpacket(p);
	Log::Instance()->Log_msg("[Daemon]: resent packet (ref: " + String(reference) +") looks like: ");
	p->print();
	Log::Instance()->Log_msg("[Daemon]: Packet was resent\nusing route:\n");
	for(int i = 0; i < route->size(); i++){
		Serial.print("element nr." + String(i));
		Log::Instance()->Log_msg(": " + String((*route)[i]));
	}
	
	delete route;
	
	
	//waitforack
	vTaskDelay(WAIT4ACK_DELAY);

	//check if ack has arrived:
	if (ack_received_flag[reference])
		kill_current_daemon(reference, params);

	Log::Instance()->Log_msg("[DAEMON]: Trying to resend via alt. route! (ref: " + String(reference) + ")");
	sendpacket(p);

	delete p;

	//waitforack
	vTaskDelay(WAIT4ACK_DELAY);

	//check if ack has arrived:
	if (ack_received_flag[reference])
		kill_current_daemon(reference, params);

	On_Send_Error(reference, "Ziel nicht erreichbar!");
	kill_current_daemon(reference, params);
}

void sendrcv::find_best_route(int reference, vector<int> * &route){
	#ifdef SENDRCV_DEBUG
	Log::Instance()->Log_msg("trying to find best route");
	#endif
	ihave_grid * choice = &(possible_routes[reference][0]);
	// find route with minimal max_rssi:
	for (size_t i = 1; i < possible_routes[reference].size(); i++){
		if(possible_routes[reference][i].max_rssi() > choice->max_rssi()) choice = &(possible_routes[reference][i]);
	}
	route = choice->create_route();
}

void(*sendrcv::On_MSG_Receive)(char* msg, int msg_size, int rssi, int sender);
void(*sendrcv::On_Send_Error)(int packref, String error_msg);
void(*sendrcv::On_Send_Success)(int packref);
id_list* sendrcv::current_ids;
std::map<int, vector<ihave_grid>> sendrcv::possible_routes;
std::map<int, bool> sendrcv::ack_received_flag;
std::map<int, TaskHandle_t*> sendrcv::forwarding_tasks;

void sendrcv::interpretpacket(Packet* p, int rssi)
{
	digitalWrite(25, !digitalRead(25));
	Packet* outgoing;
	vector<int>* route;
	ihave_grid ihg;
	int max_hops;
	switch(p->type)
	{
	case Packet::packettype::whohas:
		if (current_ids == NULL)current_ids = new id_list(100);
		int wanted;
		fetch_data_from_whohas(p, max_hops, wanted);
		if (wanted == userdata::Instance()->MYADRESS()) {	//if we are the wanted client, we send an ihave packet with our id and rssi of 0
			outgoing = new Packet();
			ihave_grid ig;
			ig.add_hop(hop(userdata::Instance()->MYADRESS(), 0));
			Packet::create_ihave(outgoing, max_hops, p->reference, &ig);
			sendpacket(outgoing);
			delete outgoing;
		}
		else {
			// reproduce the whohas packet only if we are not the ones waiting for the corresponding "ihaves" and it hasn't already passed us
			if (forwarding_tasks.find(p->reference) == forwarding_tasks.end() &&!current_ids->contains(p->reference))
			{
				current_ids->push(p->reference);
				outgoing = new Packet();
				Packet::create_whohas(outgoing, p->reference, max_hops, wanted);
			}
		}
		break;
	case Packet::packettype::ihave:
		fetch_data_from_ihave(p, ihg, max_hops);
		#ifdef PACKETDEBUG
		Log::Instance()->Log_msg("[INTERPRETER]: ihgreceived:");
		ihg.print();
		#endif
		if (ack_received_flag.find(p->reference) == ack_received_flag.end()) {
			//if the ihave packet doesnt have passed this node and the max hops count isnt exceeded,
			//this nodes id is added and the ihavepacket forwarded.
			if (ihg.bytecount() / (2*sizeof(int)) < max_hops && !ihg.contains_id(userdata::Instance()->MYADRESS())) {
				ihg.add_hop(hop(userdata::Instance()->MYADRESS(), rssi));
				outgoing = new Packet();
				Packet::create_ihave(outgoing, max_hops, p->reference, &ihg);
				sendpacket(outgoing);
				delete outgoing;
			}
		}
		else {
			//if we are the original sender of the corresponding whohas packet,
			//we add the received possible route to the destination to our list of possible routes
			if (possible_routes.find(p->reference) == possible_routes.end()) {
				vector<ihave_grid> routes;
				routes.push_back(ihg);
				possible_routes.insert(pair<int, vector<ihave_grid>>(p->reference, routes));
			}
			else {
				possible_routes[p->reference].push_back(ihg);
			}
		}
		break;

	case Packet::packettype::ACK:
		Log::Instance()->Log_msg("[Interpreter]: Got ACK with reference:\n\t" + String(p->reference) + "\nwith rssi:\n    " + String(rssi));
		// set flag, that the daemon waiting for this ack can be killed:
		ack_received_flag[p->reference] = true;
		On_Send_Success(p->reference);
		break;

	case Packet::packettype::msg:
		
		route = new vector<int>();
		char* msg;
		int msg_size;
		int dest;
		fetch_data_from_msg(p, route, msg, msg_size, dest);	//extract data from packet
		On_MSG_Receive(msg, msg_size, rssi, (*route)[0]);
		outgoing = new Packet();							//create ack packet
		Packet::create_ACK(outgoing, route, p->reference);	
		Log::Instance()->Log_msg("[INTERPRETER]: MSG successfull interpreted");
		delay(1000);
		sendpacket(outgoing);								//send ack
		Log::Instance()->Log_msg("[Interpreter]: Ack sent");
		
		delete outgoing;

		break;

	case Packet::packettype::forward:
		Log::Instance()->Log_msg("[INTERPRETER]: forwarding Packet [##]--->");
		outgoing = fetch_packet_from_forward(p);	//get payloadpacket
		sendpacket(outgoing);	//send it
		delete outgoing;
		break;

	default:
		break;
	}
	
}
