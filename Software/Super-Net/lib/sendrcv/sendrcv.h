#ifndef SENDRCV_H
#define SENDRCV_H
#include "id_list.h"
#include "Paket.h"
#include <map>
#include <LoRa.h>
#include <userdata.h>

/**amount of time the node waits for an ack to arrive*/
#define WAIT4ACK_DELAY 8000

/**amount of time the node waits for the ihave packets to arrive*/
#define WAIT4IHAVE_DELAY 8000

/**maximum route length to destination*/
#define MAX_IHAVE_HOPS 5

/// <summary>
/// provides functions to send, receive and interpret packets<br>
/// event-driven, allowing the user to implement the handling<br>
/// of events like incoming messages or sending errors 
/// </summary>
class sendrcv
{
public:
	/// <summary>
	/// interprets an incoming packet and reacts to it
	/// </summary>
	/// <param name="p">the packet thats supossed to be interpreted</param>
	/// <param name="rssi">the signalstrength with wich the packet arrived</param>
	static void interpretpacket(Packet * p, int rssi = 0);

	/// <summary>
	/// sends a message to a specified recipient
	/// </summary>
	/// <param name="msg">pointer to the bytearray containing the message</param>
	/// <param name="msgsize">the size in bytes of the message</param>
	/// <param name="recipient">the recipient of the message</param>
	/// <param name="packref">if 0, this method automatically generates a reference</param>
	/// <returns>the packet referenceid</returns>
	static int send_msg(char* msg, int msgsize, int recipient, int packref);

	/// <summary>
	/// receive/read an incoming LoRa packet
	/// </summary>
	/// <param name="packetsize">the size of the packet</param>
	static void rcv_packet(int packetsize);

	/// <summary>
	/// sends a packet via LoRa
	/// </summary>
	/// <param name="p">the packet to be sent</param>
	/// <param name="isInitialMessage"></param>
	static void sendpacket(Packet* p, bool isInitialMessage=false);
	
	
	/// <summary>
	/// is invoked once a message is received
	/// </summary>
	static void(*On_MSG_Receive)(char* msg, int msg_size, int rssi, int sender);

	/// <summary>
	/// is invoked once a message couldn't be sent
	/// </summary>
	static void(*On_Send_Error)(int pack_ref, String error_msg);

	/// <summary>
	/// is invoked once a message was sent successfully
	/// </summary>
	static void(*On_Send_Success)(int packref);

private:
	/// <summary>
	/// unpacks a forward packet, retrieving the contained packet
	/// </summary>
	/// <param name="fwd">the forward packet to be unpacked</param>
	/// <returns>the contained packet</returns>
	static Packet* fetch_packet_from_forward(Packet * fwd);

	/// <summary>
	/// parses important information from the raw data of a msg packet
	/// </summary>
	/// <param name="p">the msg packet</param>
	/// <param name="route">out: the route the packet took</param>
	/// <param name="msg">out: the bytearray representing the message that was sent</param>
	/// <param name="msg_size">out: the size of the bytearray</param>
	/// <param name="dest">out: the destination of the Message</param>
	static void fetch_data_from_msg(Packet* p, vector<int> *route, char* &msg, int &msg_size, int &dest);

	/// <summary>
	/// parses important information from the raw data of a whohas packet
	/// </summary>
	/// <param name="p">the whohas packet</param>
	/// <param name="sender">out: the sender of the packet</param>
	/// <param name="wanted">out: the wanted Node</param>
	static void fetch_data_from_whohas(Packet* p, int &sender, int &wanted);

	/// <summary>
	/// parses important information from the raw data of an ihave packet
	/// </summary>
	/// <param name="p">the ihave packet</param>
	/// <param name="ihavegrid">out: the sent ihavegrid</param>
	/// <param name="max_hops">out: the maximum route length</param>
	static void fetch_data_from_ihave(Packet* p, ihave_grid &ihavegrid, int &max_hops);

	/// <summary>
	/// finds the most reliable route out of the possible routes
	/// </summary>
	/// <param name="reference">the reference of the packet the route is for</param>
	/// <param name="route">out: the found route</param>
	static void find_best_route(int reference, vector<int>* &route);

	/// <summary>
	/// id list containing the referenceid of the last few whohas packets, that passed this node
	/// </summary>
	static id_list* current_ids;
	
	/// <summary>
	/// map to save the possible routes for outgoin packets, retrieved from ihave packets
	/// </summary>
	static std::map<int, vector<ihave_grid>> possible_routes;

	/// <summary>
	/// map to save the taskhandles for the forwarding tasks
	/// </summary>
	static std::map<int, TaskHandle_t*> forwarding_tasks; //wenn msg gesendet wird, wird task gestartet, welcher nach WAIT4IHAVE_DELAY Sekunden automatisch das routen startet. wenn vorher das ack kommt, wird der prozess gekillt 
	
	/// <summary>
	/// map to save flags, which tell a forwarding task if the ack for its packet was received
	/// </summary>
	static std::map<int, bool> ack_received_flag; //wenn flag gesetzt, wird der Task sicher gekillt

	/// <summary>
	/// method executed by the forwarding tasks,<br>
	/// it automatically resends the packet after WAIT4IHAVE_DELAY s<br>
	/// if no ack is received, if then still no ack is received, it looks for alternative routes<br>
	/// by broadcasting whohas packets<br>
	/// </summary>
	/// <param name="params">pointer of the packet whose deliverance is supposed to be assured</param>
	static void forwarddaemon(void * params);
	
	/// <summary>
	/// safely kills a running forwarding daemon and deletes all entries about it
	/// </summary>
	/// <param name="reference">referenceid of the packet the daemon was working on</param>
	/// <param name="packetpointer">pointer to the packet the daemon was working on</param>
	static void kill_current_daemon(int reference, void* packetpointer);
};

#endif