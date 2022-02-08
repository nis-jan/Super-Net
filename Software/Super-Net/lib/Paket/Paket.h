#ifndef PAKET_H
#define PAKET_H
#include <string>
#include <map>
#include <vector>
#include <userdata.h>

#define PACKETDEBUG


using namespace std;

class hop {
public:
	unsigned int id;
	int rssi;
	hop(unsigned int id, int rssi);
};

class bytes {
public:
	static char* int2bytes(int in);
	static int bytes2int(char* in, int startindex=0);
	static void bytecopy(char* src, char* dest, int startsrc, int startdest, int count);
};

//ihave data
class ihave_grid {
public:
	void add_hop(hop h);
	char* tobytes();
	int bytecount();
	int max_rssi();
	bool contains_id(int id);
	vector<int> * create_route();
	#ifdef PACKETDEBUG
	void print();
	#endif
private:
	vector<hop> hops;
	int _bytecount = 0;
};



/*
general packet-structure:
  ================================================================
|| recipient | referenceid | type      | size      | payload	  ||
  ================================================================

  recipient: INTEGER            |  0 = broadcast
  referenceid: INTEGER
  type: BYTE(CHAR)
  size: INTEGER [OPTIONAL]
  payload: CHAR* (bytearray)  [OPTIONAL]

  It's possible to stack a packet into another one.

  ==============================================================================================

  FORWARD-packet:

  <recipient> <referenceid> <forward-byte-code> <size of forwarded packet> <packet to forward>
	
	--> forwarded packet can also be another forward packet, so that way it is possible
		to send the packet over a predefined route.

		SIZE: 3*INTEGER + 1 + forward_packet_size

  ==============================================================================================

  ACK-packet:

  <recipient> <referenceid> <ACK-byte-code>
	--> through the reference id the recipient of this packet is assured, that his message 
		arrived at its destination

		SIZE: 2*INTEGER + 1

  ==============================================================================================

  WHOHAS-packet:

  <recipient = 0> <referenceid> <WHOHAS-byte-code> <maximum hops [INTEGER]> <address of the wanted client [INTEGER]>

	--> repeated by everyone who receives this. the referenceid assures that a packet isn't
		repeated twice by the same client.

	maximum hops: maximum count of hops to destination
	SIZE: 4*INTEGER + 1

  ==============================================================================================

  IHAVE-packet:

  <recipient> <referenceid> <IHAVE-byte-code> <maximum hops> <ihave_grid_size> <ihavegrid>

	--> this packet is the answer to the WHOHAS packet and contains an ihavegrid.

	ihavegrid:
		contains a possible network route (consisting of hops) to the desired client.
		a hop is defined by its address and reachability(signal strength).

	maximum hops:	maximum count of hops in this possible route, if a node gets an ihave packet
					it only add itself to the ihavegrid and forwards the packet if the ihave grid
					doesnt already have the maximum count of hops and also doesn't contain this
					node yet.. that garantees that ihave packets
					dont circle in the aether forever since they are broadcasted and reproduced
	
	if several IHAVE-packets are received the best route can be determined using the hop-data
	of every single hop.

	SIZE: 3*INTEGER + 1 + gridsize


  ==============================================================================================

  MSG-packet:

  <recipient> <referenceid> <MSG-byte-code> <routesize> <used route> <payloadsize> <payload>

  routesize: INTEGER               | number of bytes, not hops
  used route: INTEGER[]
	--> the route should also contain the sender adress so that the ack can be directed
		
		SIZE: 4*INTEGER + 1 + routesize + payloadsize
  
  ==============================================================================================

  typebytes:
	 0 - whohas
	 1 - ihave
	 2 - msg
	 3 - ACK 
	 4 - forward
*/

//packet klasse
class Packet
{
public:
	//typedef
	enum packettype {
		whohas,
		ihave,
		msg,
		ACK,
		forward
	};

	//conversion between types and their bytevalue:
	static std::map<packettype, char> type2byte;
	static std::map<char, packettype> byte2type;
	
	//packetgenerators:
	static void create_whohas(Packet* p, int reference, int max_hops, int wanted_client);
	static void create_ihave(Packet* p, int max_hops, int reference, ihave_grid * ihavegrid);
	static void create_msg(Packet* p, int recipient, int reference, vector<int> route, int msgsize, char* msg);
	static void create_ACK(Packet* p, vector<int> *route, int reference);
	static void create_forward(Packet * p, int recipient, int reference, int payloadsize, char* payload);

	static void printla(char* bytes, int count);
	static int create_reference();


	//constructors:
	Packet();

	~Packet();

	//object members:
	packettype type;
	int packetsize;
	int payloadsize;
	char* raw_data;
	int reference;

	//member-methods:
	void print();
	void reallocate(); //wichtig, um die ganzen pointer zu clearen!!!

private:
	void routepacket(vector<int> route);
};

#endif