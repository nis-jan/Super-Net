// Copyright 2018 The Flutter team. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:math';
import 'package:app/Serial.dart';

import 'Konvo.dart';
import 'Design.dart';
import 'package:flutter/material.dart';

void main() {
  runApp(MyApp());
}

cKonversationen konversationen = cKonversationen();
int _OwnAdress = 2000;


class MyApp extends StatefulWidget {
  const MyApp({ Key? key }) : super(key: key);

  @override
  _MyAppState createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Super-App',
      home: Scaffold(
        appBar: AppBar(
          title: Row(children: [Center(child: Text('Konversationen', style: colorText,)), Expanded(child: SizedBox()),Serial((msg) => null, (m) => null, (ref) => null, (ref) => null, null, "0", "0", false)]),
          backgroundColor: bgColor,
        ),
        body: const Center(
          child: ConvoList(),
        ),
        backgroundColor: bgColor,
        ),
    );
  }
}




class ConvoList extends StatefulWidget {
  const ConvoList({ Key? key }) : super(key: key);

  @override
  _ConvoListState createState() => _ConvoListState();
}

class _ConvoListState extends State<ConvoList> {
  @override
  Widget build(BuildContext context) {
    
    return Stack(
      
    children: [
      ListView(
        padding: const EdgeInsets.all(0),
        children: konversationen.createe_Convo_list(),
      ),
      Align(
        alignment: Alignment.bottomCenter,
        child: FloatingActionButton(onPressed: (){
          Navigator.of(context).push(
            MaterialPageRoute<void>(builder: (context ){
              return addKonvPage(this);
            })
          );
        }, child: Icon(Icons.add), backgroundColor: accentColor,),
      ),
      Align(
        alignment: Alignment.bottomLeft,
        child: FloatingActionButton(
          child: Icon(Icons.settings,),
          backgroundColor: accentColor,
          onPressed: (){
            Navigator.of(context).push(
              MaterialPageRoute<void>(builder: (context ){
                return Settingspage();
              })
            );
          },
        ),
      )
    ]
    );
  }
}

class Settingspage extends StatefulWidget {
  const Settingspage({ Key? key }) : super(key: key);
  @override
  _SettingspageState createState() => _SettingspageState();
}

class _SettingspageState extends State<Settingspage> {
  final input_controller = TextEditingController();
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text("Set own Address"),
        backgroundColor: bgColor,
      ),
      backgroundColor: bgColor,
      body: Column(children: [
        Text("The Adress on the Node\nwill be updated with\nthe next sent message!", style: colorText,),
        SizedBox(height: 10,),
        TextField(controller: input_controller,decoration: InputDecoration(hintText: "new Adress hiere", hintStyle: accentText, labelText: "old Adress: " + _OwnAdress.toString()),style: colorText,),
        SizedBox(height: 10,),
        TextButton(onPressed: (){
          if(input_controller.text != ""){
            _OwnAdress = int.parse(input_controller.text);
            Navigator.of(context).pop();
          }
        }, child: Text("update Adress"))
      ],),
    );
  }
}


class chatWindow extends StatefulWidget {
  chatWindow(this.Konvoid, { Key? key }) : super(key: key);
  int Konvoid;
  @override
  _chatWindowState createState() => _chatWindowState();
}

class _chatWindowState extends State<chatWindow> {
  final inputController = TextEditingController();
  final ScrollController scrollController = ScrollController();
  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        Expanded(child: ListView(
          children: konversationen.get_messages_of(widget.Konvoid),
          controller: scrollController,
        )),
        Container(
            color: bgColor,
            child:Container(
              decoration: BoxDecoration(
                color: bgColor,
                border: Border.all(
                      color: accentColor
                ),
                borderRadius: BorderRadius.all(Radius.circular(20))
              ),
              
              child: Container(
                    
                    child: Row(
                      children: [
                        SizedBox(width: 15,),
                        Expanded(
                          child: TextField(
                            style: colorText,
                            decoration: InputDecoration(
                              hintText: "Message...",
                              hintStyle: TextStyle(color: accentColor),
                              focusColor: accentColor
                            ),
                            controller: inputController,
                          ),
                        ),
                        Serial((Message msg){//msg callback
                          setState(() {
                            widget.Konvoid = widget.Konvoid;
                            konversationen.add_msg(msg);
                          });
                        },
                        (Message msg){//on packet sent
                          setState(() {
                            widget.Konvoid = widget.Konvoid;
                            konversationen.add_msg(msg);
                          });
                        },
                        (ref){//on send error
                          setState(() {
                            widget.Konvoid = widget.Konvoid;
                            konversationen.update_status_of_msg(ref, Message_Status.Error);
                          });
                        },
                        (ref){//on send success
                          setState(() {
                            widget.Konvoid = widget.Konvoid;
                            konversationen.update_status_of_msg(ref, Message_Status.SENT_SUCCESFULL);
                          });
                        },
                        inputController, widget.Konvoid.toString(), _OwnAdress.toString(), true),
                        SizedBox(width: 10,)
                      ],
                    ),
              )
          )
        )  
      
      ],
    );
  }
}

class Konversation_Page extends StatefulWidget {
  Konversation_Page(this.konvoid, { Key? key}) : super(key: key);
  int konvoid;
  @override
  _Konversation_PageState createState() => _Konversation_PageState();
}

class _Konversation_PageState extends State<Konversation_Page> {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(konversationen.get_title_of(widget.konvoid)),
        backgroundColor: bgColor,
      ),
      backgroundColor: bgColor,
      body: chatWindow(widget.konvoid),
    );
  }
}

class addKonvPage extends StatefulWidget {
  const addKonvPage(this.parent, { Key? key }) : super(key: key);
  final _ConvoListState parent;
  @override
  _addKonvPageState createState() => _addKonvPageState();
}

class _addKonvPageState extends State<addKonvPage> {
  @override
  final aliasController = TextEditingController(); 
  final idController = TextEditingController(); 
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text("Add Conversation", style: colorText,), backgroundColor: bgColor,),
      body: Column(
        children: [
          SizedBox(height: 15,),
          TextField(decoration: InputDecoration(hintStyle: accentText, hintText: "enter Alias here....",), style: colorText,controller: aliasController,),
          SizedBox(height: 15,),
          TextField(decoration: InputDecoration(hintStyle: accentText, hintText: "enter Adress here....",), style: colorText,controller: idController,),
          SizedBox(height: 50,),
          TextButton(onPressed: (){
            if(!(idController.text=='' || aliasController.text =='')){
              konversationen.add_Konvo(Konversation(aliasController.text, int.parse(idController.text)));
            }
            Navigator.of(context).pop();
            widget.parent.setState(() {
              
            });
          }, child: Text("Add Conversation", style: colorText,), style: ButtonStyle(backgroundColor: MaterialStateProperty.all(accentColor)),)
        ],
      ),
      backgroundColor: bgColor,
    );
  }
}