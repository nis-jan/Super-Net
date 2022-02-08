#ifndef SENDRCV_H
#define SENDRCV_H
#include "id_list.h"
#include "Paket.h"
#include <map>
#include <LoRa.h>
#include <userdata.h>


#define WAIT4ACK_DELAY 8000
#define WAIT4IHAVE_DELAY 8000

#define MAX_IHAVE_HOPS 5

class sendrcv
{
public:
	static void interpretpacket(Packet * p, int rssi = 0);
	// hier noch implementieren, dass auf ack gewartet wird und wenn nichts kommt, dass dann
	// ein whohas raus geht wonach die richtige route verwendet wird.
	// kommt wieder kein ack oder keine route wurde gefunden, wird die fehlercallback gerufen
	static int send_msg(char* msg, int msgsize, int recipient, int packref);

	static void rcv_packet(int packetsize);
	static void sendpacket(Packet* p, bool isInitialMessage=false);
	
	//callbacks:
	static void(*Msg_Callback)(char* msg, int msg_size, int rssi, int sender);
	static void(*Send_Error)(int pack_ref, String error_msg);
	static void(*On_Send_Success)(int packref);

private:
	
	static char* read_bytes_from_stream(int count);
	static Packet* fetch_packet_from_forward(Packet * fwd);
	static void fetch_data_from_msg(Packet* p, vector<int> *route, char* &msg, int &msg_size, int &dest);
	static void fetch_data_from_whohas(Packet* p, int &sender, int &wanted);
	static void fetch_data_from_ihave(Packet* p, ihave_grid &ihavegrid, int &max_hops);
	static void find_best_route(int reference, vector<int>* &route);
	static id_list* current_ids;
	
	static std::map<int, vector<ihave_grid>> possible_routes;

	static std::map<int, TaskHandle_t*> forwarding_tasks; //wenn msg gesendet wird, wird task gestartet, welcher nach 15s automatisch das routen startet. wenn vorher das ack kommt, wird der prozess gekillt 
	static std::map<int, bool> ack_received_flag; //wenn flag gesetzt, wird der Task sicher gekillt

	static void forwarddaemon(void * params);
	static void kill_current_daemon(int reference, void* packetpointer);
};

#endif