#include "Paket.h"
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <Arduino.h>
#include <Log.h>
#include <Display.h>


//hop:
hop::hop(unsigned int id, int rssi) {
	this->id = id;
	this->rssi = rssi;
}


//ihave_grid
#if true 
char* ihave_grid::tobytes() {
	char* ret = (char*)malloc(_bytecount);
	char* intptr;
	for (int i = 0; i < this->hops.size(); i++) {
		intptr = bytes::int2bytes(hops[i].id);
		bytes::bytecopy(intptr, ret, 0, i*2*sizeof(int), sizeof(int)); //id
		free(intptr);

		intptr = bytes::int2bytes(hops[i].rssi);
		bytes::bytecopy(intptr, ret, 0, i * 2 * sizeof(int) + sizeof(int), sizeof(int)); //rssi
		free(intptr);
	}
	return ret;
}

void ihave_grid::add_hop(hop h) {
	hops.push_back(h);
	_bytecount += 2 * sizeof(int);
}
//getter fuer die anzahl an bytes die anfallen, wenn zu bytes konvertiert wird.
int ihave_grid::bytecount() {
	return _bytecount;
}
int ihave_grid::max_rssi() {
	int max = 0;
	for (int i = 0; i < hops.size(); i++) {
		if (abs(hops[0].rssi) > max) max = abs(hops[0].rssi);
	}
	return max;
}

bool ihave_grid::contains_id(int id){
	for (int i = 0; i < hops.size(); i++){
		if(hops[i].id == id) return true;
	}
	return false;
}

vector<int> * ihave_grid::create_route(){
	vector<int> * ret = new vector<int>();
	ret->push_back(userdata::Instance()->MYADRESS());
	for(int i = this->hops.size()-1; i >= 0; i--){ //can't be size_t because it gets negative eventually
		ret->push_back(this->hops[i].id);
	}
	return ret;
}

#ifdef PACKETDEBUG
	void ihave_grid::print(){
		Log::Instance()->Log_msg("ihavegrid-structure:");
		for(int i = 0; i < hops.size(); i++){
			Log::Instance()->Log_msg("Hop nr." + String(i));
			Log::Instance()->Log_msg("\tID: " + String(hops[i].id));
			Log::Instance()->Log_msg("\tRSSI: " + String(hops[i].rssi));
		}
	}
#endif
#endif

//byteoperations
#if true
//converts int to bytearray
char * bytes::int2bytes(int in) {
	char* ret = (char*)malloc(sizeof(int));
	copy(static_cast<char*>(static_cast<void*>(&in)), static_cast<char*>(static_cast<void*>(&in)) + sizeof(in), ret);
	return ret;
}
//converts a bytearray to an integer
int bytes::bytes2int(char* arr, int start_index) {
	int* ret = (int*)malloc(sizeof(int));
	for (char i = 0; i < sizeof(int); i++) {
		((char*)ret)[i] = arr[i+start_index];
	}
	return *ret;
}
//copies bytes from one array to another
void bytes::bytecopy(char* src, char* dest, int startsrc, int startdest, int count) {
	for (int i = 0; i < count; i++) {
		dest[startdest + i] = src[startsrc + i];
	}
}
#endif




//packet:
std::map<Packet::packettype, char> Packet::type2byte = { {packettype::whohas, 0x0},{packettype::ihave, 0x1},{packettype::msg, 0x2},{packettype::ACK, 0x3}, {packettype::forward, 0x4 } };
std::map<char, Packet::packettype> Packet::byte2type = { {0x0, packettype::whohas},{0x1, packettype::ihave},{0x2, packettype::msg},{0x3, packettype::ACK}, {0x4, packettype::forward} };


int Packet::create_reference() {
	srand(time(NULL));
	int r = rand()%1000000+1;
	return r;
}


Packet::Packet() {
	this->packetsize = 0;
}

Packet::~Packet() {
	//free memory
	if(this->raw_data != NULL){
		free(this->raw_data);
		this->raw_data = NULL;
	}
}


void Packet::create_whohas(Packet* p, int reference, int max_hops, int wanted_client)
{
	// set attribute fields of packet
	p->packetsize = (4 * sizeof(int) + 1);
	p->raw_data = (char*)malloc(p->packetsize);
	p->type = Packet::packettype::whohas;
	int recipient = 0;

	char* intptr;

	//building packet:
	intptr = bytes::int2bytes(recipient);
	bytes::bytecopy(intptr, p->raw_data, 0, 0, sizeof(int)); //address = 0 -> broadcast
	free(intptr);

	intptr = bytes::int2bytes(reference);
	bytes::bytecopy(intptr, p->raw_data, 0, sizeof(int), sizeof(int)); //reference
	free(intptr);

	p->raw_data[2 * sizeof(int)] = Packet::type2byte[Packet::packettype::whohas];   //bytecode

	intptr = bytes::int2bytes(max_hops);
	bytes::bytecopy(intptr, p->raw_data, 0, 2*sizeof(int)+1, sizeof(int));   //max_hops 
	free(intptr);

	intptr = bytes::int2bytes(wanted_client);
	bytes::bytecopy(intptr, p->raw_data, 0, 3*sizeof(int) + 1, sizeof(int));   //wanted_client id
	free(intptr);
	return;
}


void Packet::create_ihave(Packet* p, int max_hops, int reference, ihave_grid * ihavegrid)
{
	#ifdef PACKETDEBUG
	Log::Instance()->Log_msg("create ihave ihavestruct:");
	ihavegrid->print();
	#endif
	// set attribute fields of packet
	p->packetsize = 4 * sizeof(int) + 1 + ihavegrid->bytecount();
	p->raw_data = (char*)malloc(p->packetsize);
	p->reference = reference;
	p->type = packettype::ihave;

	char* intptr;
	//filling the packet:
	intptr = bytes::int2bytes(0);
	bytes::bytecopy(intptr, p->raw_data, 0, 0, sizeof(int)); //recipient
	free(intptr);

	intptr = bytes::int2bytes(reference);
	bytes::bytecopy(intptr, p->raw_data, 0, sizeof(int), sizeof(int)); //reference
	free(intptr);

	p->raw_data[2 * sizeof(int)] = Packet::type2byte[Packet::packettype::ihave]; //bytecode

	intptr = bytes::int2bytes(max_hops);
	bytes::bytecopy(intptr, p->raw_data, 0, 2*sizeof(int)+1, sizeof(int));
	free(intptr);

	intptr = bytes::int2bytes(ihavegrid->bytecount());
	bytes::bytecopy(intptr, p->raw_data, 0, 3*sizeof(int) + 1, sizeof(int)); //ihave_grid_size
	free(intptr);
	
	char* buf = ihavegrid->tobytes();
	bytes::bytecopy(buf, p->raw_data, 0, 4*sizeof(int) + 1, ihavegrid->bytecount());  //ihavegrid
	free(buf);

	return;
}


void Packet::create_msg(Packet* p, int recipient, int reference, vector<int> route, int msgsize, char* msg)
{
	// set attribute fields of packet
	p->type = Packet::packettype::msg;
	p->reference = reference;
	p->packetsize = 4 * sizeof(int) + 1 + route.size() * sizeof(int) + msgsize;
	p->raw_data = (char*)malloc(p->packetsize);

	// filling in the raw data structured like defined in Paket.h
	char* intptr;
	intptr = bytes::int2bytes(recipient);
	bytes::bytecopy(intptr, p->raw_data, 0, 0, sizeof(int)); //recipient
	free(intptr);

	intptr = bytes::int2bytes(reference);
	bytes::bytecopy(intptr, p->raw_data, 0, sizeof(int), sizeof(int)); //reference
	free(intptr);

	p->raw_data[2 * sizeof(int)] = Packet::type2byte[Packet::packettype::msg]; //msg-bytecode

	int routesize = route.size() * sizeof(int);
	intptr = bytes::int2bytes(routesize);
	bytes::bytecopy(intptr, p->raw_data, 0, 2 * sizeof(int) + 1, sizeof(int)); //routesize
	free(intptr);

	for (int i = 0; i < route.size(); i++) {
		intptr = bytes::int2bytes(route[i]);
		bytes::bytecopy(intptr, p->raw_data, 0, 3 * sizeof(int) + 1 + i * sizeof(int), sizeof(int)); //routeelements
		free(intptr);
	}

	int curr_ind = (3 + route.size()) * sizeof(int) + 1; //for easier codereading

	intptr = bytes::int2bytes(msgsize);
	bytes::bytecopy(intptr, p->raw_data, 0, curr_ind, sizeof(int)); //msgsize
	free(intptr);

	bytes::bytecopy(msg, p->raw_data, 0, curr_ind + sizeof(int), msgsize);  //msgdata

	//if there is an actual route, create the forward packet stack
	if(route.size() > 2)
		p->routepacket(route);
	return;
}


void Packet::create_ACK(Packet* p, vector<int> *orig_route, int reference)
{
	// reverse route, since we want to send the ACK back to the sender of the msg packet
	// along the same route it came, but in the opposite direction
	p->type = Packet::packettype::ACK;
	p->reference = reference;
	vector<int> reversed_route;
	for (int i = (*orig_route).size() - 1; i >= 0; i--) {
		reversed_route.push_back((*orig_route)[i]);
	}

	//generate ack
	p->packetsize = 2 * sizeof(int) + 1;
	p->raw_data = (char*)malloc(p->packetsize);
	
	char* intptr = bytes::int2bytes(reversed_route[reversed_route.size()-1]); //get recipient for this packet
	bytes::bytecopy(intptr, p->raw_data, 0, 0, sizeof(int));
	free(intptr);
	intptr = bytes::int2bytes(reference); //filling in reference
	bytes::bytecopy(intptr, p->raw_data, 0, sizeof(int), sizeof(int));
	
	free(intptr);
	p->raw_data[2 * sizeof(int)] = Packet::type2byte[Packet::packettype::ACK];

	p->routepacket(reversed_route);	//route the packet

	return;

}

void Packet::routepacket(vector<int> route) {
	int routingsize = ((route.size() - 2) * (3 * sizeof(int) + 1));	// all the headerbytes of the forwarding packets
	int fwdsize = this->packetsize + routingsize;	// size of the entire packet
	
	char* ret = (char*)malloc(fwdsize);


	int curr_ind = 0; //the index where currently is written in the raw data of the packet

	// stacking this packet in several forwarding packets so it can be sent along its route
	char* intptr;
	for (int i = 1; i < route.size() - 1; i++) { //starts at one since the sender adress is included.
		intptr = bytes::int2bytes(route[i]);
		bytes::bytecopy(intptr, ret, 0, curr_ind, sizeof(int)); //recipient
		free(intptr);

		intptr = bytes::int2bytes(reference);
		bytes::bytecopy(intptr, ret, 0, curr_ind + sizeof(int), sizeof(int)); //reference
		free(intptr);

		ret[curr_ind + (2 * sizeof(int))] = Packet::type2byte[Packet::packettype::forward]; // forward-bytecode

		intptr = bytes::int2bytes(fwdsize - (3 * sizeof(int) + 1));
		bytes::bytecopy(intptr, ret, 0, curr_ind + 2 * sizeof(int) + 1, sizeof(int)); //size of the forwarded packet
		free(intptr);
		curr_ind = i * (3 * sizeof(int) + 1);

	}

	bytes::bytecopy(raw_data, ret, 0, routingsize, packetsize); //append packetdata
	
	
	free(raw_data);
	
	
	//now put all the data in this packet
	// !no return the rawdata of THIS packet is overwritten.
	packetsize = fwdsize;
	raw_data = (char*)calloc(fwdsize, sizeof(char));
	bytes::bytecopy(ret, raw_data, 0,0, packetsize);

	free(ret);

	//packettype auf forward �ndern:
	this->type = packettype::forward;
}



void Packet::print() {
	Log::Instance()->Log_msg("Type: " + String((int)type) + "\nReference: " + String(reference) + "\nPacketsize: " + String(this->packetsize) + "Bytes");
	for (int i = 0; i < this->packetsize; i++) {
		Serial.print(String((int)(this->raw_data[i])) + " | ");
	}
	Log::Instance()->Log_msg("");
	return;
}
