import 'dart:async';
import 'dart:math';
import 'dart:typed_data';
import 'package:app/Design.dart';
import 'package:usb_serial/transaction.dart';
import 'package:usb_serial/usb_serial.dart';
import 'package:flutter/material.dart';
import 'main.dart';
import 'Konvo.dart';




Map<String, Message> pending_msgs= {"33": Message("i bims 1 pending msg!", 33, false, 3)};


class Serial extends StatefulWidget {
  Serial(this.msg_Callback, this.On_Packet_Sent, this.On_Send_Error, this.On_Send_Success, this.input_controller, this._recipient, this._OwnAdress, this.clickable, { Key? key }) : super(key: key);
  
  Function(Message msg) msg_Callback;
  Function(Message m) On_Packet_Sent;
  Function(int ref) On_Send_Error;
  Function(int ref) On_Send_Success;
  final TextEditingController? input_controller;
  final String _recipient;
  final String _OwnAdress;
  final bool clickable;
  @override
  _SerialState createState() => _SerialState();
}

class _SerialState extends State<Serial> {
  bool _connected = false;

  UsbPort? _port;
  String _status = "Idle";
  List<Widget> _ports = [];
  List<Widget> _serialData = [];

  StreamSubscription<String>? _subscription;
  Transaction<String>? _transaction;
  UsbDevice? _device;

  TextEditingController _textController = TextEditingController();

  void send_string(String str) async{
    await _port!.write(Uint8List.fromList(str.codeUnits));
  }

  List<int> getNumberfromString(String str, int startindex){
    
    String tmp = "";
    int index = 0;
    for(int i = startindex; i < str.length; i++){
      if(str[i] == '\n')break;
      tmp+=str[i];
      index = i;
    }
    List<int> ret = [];
    ret.add(int.parse(tmp));
    ret.add(index+2);
    return ret;
  }

  void str_callback(String line){
    if(line.contains("PSENT")){
      var tempref = getNumberfromString(line, line.indexOf("PSENT")+6);
      var actualref = getNumberfromString(line, tempref[1]);
      pending_msgs[tempref[0].toString()]?.set_ref(actualref[0]);
      widget.On_Packet_Sent(pending_msgs[tempref[0].toString()]!);
      pending_msgs.remove(tempref[0].toString());
    }
    if(line.contains("RCVMS")){
      var sender = getNumberfromString(line, line.indexOf("RCVMS")+6);
      var size = getNumberfromString(line, sender[1]);
      String msg = line.substring(size[1], size[1]+size[0]);
      widget.msg_Callback(Message(msg, 0, true, sender[0]));
    }
    if(line.contains("SENDE")){
      var ref = getNumberfromString(line, line.indexOf("SENDE")+6);
      widget.On_Send_Error(ref[0]);
    }
    if(line.contains("SENDS")){
      var ref = getNumberfromString(line, line.indexOf("SENDS")+6);
      widget.On_Send_Success(ref[0]);
    }
  }

  void _connectTo(bool connectdisconnect) async {
    _serialData.clear();
    if (!connectdisconnect) {
      _device = null;
      setState(() {
        _status = "Disconnected";
        _connected = false;
      });
      return;
    }

    if (_subscription != null) {
      _subscription!.cancel();
      _subscription = null;
    }

    if (_transaction != null) {
      _transaction!.dispose();
      _transaction = null;
    }

    if (_port != null) {
      _port!.close();
      _port = null;
    }

    List<UsbDevice> devices = await UsbSerial.listDevices();
    if(devices.length == 0)return;

    _port = await devices[0].create();

    if (await (_port!.open()) != true) {
      setState(() {
        _status = "Failed to open port";
        _connected = false;
      });
      return;
    }
    _device = devices[0];

    await _port!.setDTR(true);
    await _port!.setRTS(true);
    await _port!.setPortParameters(9600, UsbPort.DATABITS_8, UsbPort.STOPBITS_1, UsbPort.PARITY_NONE);

    _transaction = Transaction.stringTerminated(_port!.inputStream as Stream<Uint8List>, Uint8List.fromList([13, 10]));
    // MSG empfangen vorher war hier mit setstate:
    _subscription = _transaction!.stream.listen((String line) {
        str_callback(line);
    });
    
    setState(() {
      _connected = true;
    });
  }
  
  void set_addr(String Adress){
    send_string("SETAD\n$Adress\n");
  }

  String send_msg(String msg, String recipient){
    
    set_addr(widget._OwnAdress);
    
    String tempref = "55";
    Message outgoing = Message(msg, int.parse(tempref), false, int.parse(recipient));
    outgoing.set_status(Message_Status.ACK_PENDING);
    pending_msgs.addAll({recipient: outgoing});
    send_string("SENDM\n$recipient\n$tempref\n$msg");

    return tempref;
  }

  @override
  void initState() {
    super.initState();

    UsbSerial.usbEventStream!.listen((UsbEvent event) {
      _connectTo(false);
    });
  }

  @override
  void dispose() {
    super.dispose();
    //_connectTo(false);
  }

  @override
  Widget build(BuildContext context) {
    if(_connected == false)_connectTo(true);
    
    final Icon ic;
    //_connectTo(true);
    if(!_connected) ic = Icon(Icons.connect_without_contact);
    else ic = Icon(Icons.device_hub);

    if(widget.clickable){
    return InkWell(
      child: IconButton(
        onPressed: (){
          String tosend;

          if(widget.input_controller == null)tosend = "emptymsg";
          else tosend = widget.input_controller!.text;

          send_msg(tosend, widget._recipient);
          widget.input_controller?.clear();
        }, 
        icon: Icon(Icons.send, color: accentColor,)
      ),
      onLongPress: (){
        String tmpref = send_msg("was geht ab?", "500");
        str_callback("PSENT\n" + tmpref + "\n" + (22).toString() + "\n");
      },

    );}
    else{
      if(_connected)return Text("(connected)", style: infoText,);
      else return Text("(Disconnected)", style: infoText,);
    }


  }
}