import 'package:app/main.dart';
import 'package:flutter/material.dart';
import 'Design.dart';

enum Message_Status{
  ACK_PENDING,
  SENT_SUCCESFULL,
  Error,
  INCOMING,
}

class Message{
  int commpartner;
  String Content;
  int _reference;
  final bool incoming;
  double _left_padding = 0;
  double _right_padding = 0;
  DateTime time = DateTime.now();
  Message_Status _status = Message_Status.ACK_PENDING;
  Message(this.Content, this._reference, this.incoming, this.commpartner){
    time = DateTime.now();
  }
  
  bool get_incoming(){
    return incoming;
  }
  int get_ref(){
    return _reference;
  }
  void set_ref(int ref){
    this._reference = ref;
  }
  void set_status(Message_Status stat){
    _status = stat;
  }
}



class Konversation{
  String Alias;
  int KonvoID;
  List<Message> Messages = <Message>[];
  
  Konversation(this.Alias, this.KonvoID);

  String getTitle(){
    return Alias + " (Addr.: " + KonvoID.toString() + ")";
  }

  void AddMessage(Message msg){
    this.Messages.add(msg);
  }

  List<Widget> build_Messages(){
    List<Widget> ret = <Widget>[];
    String datetime;
    for(int i = 0; i < Messages.length; i+=1){
      datetime = Messages[i].time.hour.toString() + ":" + Messages[i].time.minute.toString() + ":" + Messages[i].time.second.toString() + "\n" + Messages[i].time.day.toString() + "." + Messages[i].time.month.toString() + "." + Messages[i].time.year.toString();
      ret.add(
        Row(
          children: [
            
            Messages[i].incoming?SizedBox():Expanded(child: SizedBox()),
            Container(
                decoration: BoxDecoration(
                  border: Border.all(
                    color: accentColor
                    ),
                  borderRadius: BorderRadius.all(Radius.circular(20))
                  ),
                child: Row(
                  children: [
                    SizedBox(width: 15,),
                    Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: [
                        SizedBox(height: 5,),
                        Align(alignment: Alignment.topLeft, child: Text(Messages[i].Content, style: Messages[i].incoming?Text_receivecolor:Text_sendcolor,)),
                        SizedBox(height: 20,),
                        Text(datetime, style: Text_Time_Style,),
                        SizedBox(height: 5,),
                      ],
                    ),                    
                    SizedBox(width: 15,),
                    VerticalDivider(color: Colors.grey, thickness: 1,),
                    SizedBox(width: 15),
                    Messages[i].incoming?SizedBox(width: 10,):(Messages[i]._status==Message_Status.ACK_PENDING?pendingIcon:Messages[i]._status==Message_Status.Error?errorIcon:successIcon),
                    SizedBox(width: 15),
                  ],
                )
              ), 
            
            
            SizedBox(width: Messages[i].incoming?80:10,)
          ],
        )
      );
      ret.add(
        SizedBox(height: 10,)
      );
    }
    return ret;
  }
}


class cKonversationen{

  cKonversationen(){}

  List<Konversation> Konversationen = [];


  String get_title_of(int konvoid){
    String ret="not existing";
    Konversationen.forEach((element){
      if(element.KonvoID == konvoid)ret = element.getTitle();
    });
    return ret;
  }

  void add_msg(Message msg){
    Konversationen.forEach((element) {
      if(element.KonvoID == msg.commpartner)element.AddMessage(msg);
    });
  }

  void update_status_of_msg(int reference, Message_Status stat){
    Konversationen.forEach((i) {
      i.Messages.forEach((j) {
        if(j._reference == reference)j.set_status(stat);
      });
    });
  }

  List<Widget> get_messages_of(int Konvoid){
    Konversation ret = Konversation("not found", 0);
    Konversationen.forEach((element) {
      if(element.KonvoID == Konvoid){
        ret = element;
      }
    });

    return ret.build_Messages();
  }

  void add_Konvo(Konversation konvo){
    this.Konversationen.add(konvo);
  }

  List<Widget> createe_Convo_list(){
    var ret = <Widget>[];
    for(int i = 0; i < Konversationen.length; i++){
      ret.add(
        listitem(Konversationen[i].KonvoID),
      );
    }
    return ret;
  }

}

class listitem extends StatelessWidget {
  listitem(this._Konvoid, { Key? key }) : super(key: key);
  int _Konvoid;
  @override
  Widget build(BuildContext context) {
    return InkWell(
      onTap: ()=>{
        Navigator.of(context).push(
          MaterialPageRoute<void>(
            builder: (context) {
                return Konversation_Page(_Konvoid);
              },
            ),
        )
      },
      child: Container(
        height: 70,
        color: bgColor,
        child: Row(
          children: <Widget>[
            Icon(Icons.message, color: Colors.white,),
            SizedBox(width: 15,),

            Expanded(
              child: Text(konversationen.get_title_of(_Konvoid), style: colorText,),
            ),
            Icon(Icons.arrow_forward, color: Colors.red,)
          ] ,
        ),
      )
    );
  }
}