//This code uses G4P which was created by Peter Lager

import g4p_controls.*;
import processing.serial.*;

Serial myPort;  // Create object from Serial class

Corridors[] corridors = new Corridors[10];    //Object array for corridors
Rooms[] rooms = new Rooms[20];                //Object array for rooms

String[] arrRooms;
String[] arrCorridors;
String[] data;

String val;              //Temp data received from the serial port
String newVal;           //Data received from the serial port
String trimmedVal;       //trims the received value to easily compare
String currentCorridor;  //keep track of current corridor
String currentRoom;      //keep track of current room

String corridorName;    //Keeps the latest corridorName in a string
String roomName;        //keeps the latest roomNamae in a string

public void setup()
{
  size(480, 320, JAVA2D);
  createGUI();
  customGUI();
  
  
  String portName = "COM9";                     //comms for xbee
  myPort = new Serial(this, portName, 9600);    //setup for serial port
  
  arrRooms = new String[100];                   //initialize arrRooms array
  arrCorridors = new String[100];               //initialize arrCorrs array
  data = new String[100];                       //initialize data array
  
  for (int i = 0; i < corridors.length; i++)
  {
    corridors[i] = new Corridors();
  }
  
  for (int i = 0; i < rooms.length; i++)
  {
    rooms[i] = new Rooms();
  }
}

public void draw(){
  background(230);
  
  for (int i = 0; i < 3; i++)
  { //sets the first 3 values of data with different values to prevent null pointers
    data[i] = "nothing" + i;
  }
  
  if (myPort.available() > 0)
  { //if data is available.
  
  val = myPort.readStringUntil('\n');         // read it and store it in val
  }

  
  if (val != null && val != newVal)
  {  //if value is different to previous value
    newVal = val;
    trimmedVal = trim(val);
    
    if ("Moving forwards.".equals(trimmedVal)) 
    {//if forward received and no room objects have been created, create one
      if (corridors[0].getCorridorID() == "unavailable")
      {
        corridors[0].setCorridorID("Corridor"+ 0);
      }
    }
    
    if ("Turned left.".equals(trimmedVal)) 
    {//if leftTurn received, add to the data array
      for (int i = 0; i < data.length; i++)
      {
        if (data[i] == null) {
          data[i] = "LeftTurn";
          btnRight.setEnabled(true);
          break;
        }
      }
    }
    if ("Turned right.".equals(trimmedVal)) 
    {//if rightTurn received, all rightTurn to data array
      for (int i = 0; i < data.length; i++)
      {
        if (data[i] == null) {
          data[i] = "RightTurn";
          btnLeft.setEnabled(true);
          break;
        }
      }
    }
    if ("objectFound".equals(trimmedVal)) 
    {//if object has been found, set room object to true and show room information in GUI
      for (int i = 0; i < corridors.length; i++)
      {
        if (currentCorridor == corridors[i].getCorridorID())
        {
          for (int k = 0; k < rooms.length; k++)
          {
            if (currentRoom == rooms[k].getRoomID())
            {
              rooms[k].setObject(true);
              
              txtArea.appendText("Room info");
              txtArea.appendText(rooms[k].toString());
            }
          }
        }
      }
    }
    if ("rRo".equals(trimmedVal)) 
    {//if right room button is pressed, set roomID and corridorID in rooms object and set number of rooms in corridors object
      for (int i = 0; i < corridors.length; i++)
      {
        if (currentCorridor == corridors[i].getCorridorID())
        {
          for (int k = 0; k < rooms.length; k++)
          {
            if (rooms[k].getRoomID() == "unavailable")
            { 
              rooms[k].setRoomID("R_Room" + k);
              rooms[k].setCorridorID(currentCorridor);
              corridors[i].setRoomNum();     
              currentRoom = rooms[k].getRoomID();

              break;
            }
          }
        }
      }
    }
    else if ("lRo".equals(trimmedVal)) 
    {//if left room button is pressed, set roomID and corridorID in rooms object and set number of rooms in corridors object
      for (int i = 0; i < corridors.length; i++)
      {
        if (currentCorridor == corridors[i].getCorridorID())
        {
          for (int k = 0; k < rooms.length; k++)
          {
            if (rooms[k].getRoomID() == "unavailable")
            {              
              rooms[k].setRoomID("L_Room" + k);
              rooms[k].setCorridorID(currentCorridor);
              corridors[i].setRoomNum();     
              currentRoom = rooms[k].getRoomID();
              
              break;
             }
          }
        }
      }
    }
    else if ("lCo".equals(trimmedVal)) 
    {//if left corridor is selected, set corridorID in corridor object
      for (int i = 0; i < corridors.length; i++)
      {
        if (corridors[i].getCorridorID() == "unavailable")
        {
        for (int n = 0; n < data.length; n++)
        {
          if (data[n] == null)
          {
            if (data[n-1] == data[n - 2])
            {//if previous two data entries are the same (i.e. two left turns), knows robot is returning to previous corridor
            
              for (int j = 0; j < arrCorridors.length; j++)
              {
                if (arrCorridors[j] == null)
                {
                  currentCorridor = arrCorridors[j-2];
                  arrCorridors[j] = currentCorridor; 
                  break;
                }
              }
            
              txtArea.appendText("Returning to " + currentCorridor);
              break;
            }
            else
            {//if previous two data entries not the same it must be a new corridor, 
              corridors[i].setCorridorID("L_Corridor" + i);
              currentCorridor = corridors[i].getCorridorID();
              txtArea.appendText("Reached: " + corridors[i].getCorridorID());
              data[n] = currentCorridor;
              for (int j = 0; j < arrCorridors.length; j++)
              {
                if (arrCorridors[j] == null)
                {
                  arrCorridors[j] = currentCorridor; 
                  break;
                }
              }
              break;
            }
            }
          }
          break;
        }
      }
    }
    else if ("rCo".equals(trimmedVal)) 
    {//if right corridor is selected, set corridorID in corridor object
      for (int i = 0; i < corridors.length; i++)
      {
        if (corridors[i].getCorridorID() == "unavailable")
        {
        for (int n = 0; n < data.length; n++)
        {
          if (data[n] == null)
          {
            if (data[n-1] == data[n - 2])
            {//if previous two data entries are the same (i.e. two left turns), knows robot is returning to previous corridor
              
              for (int j = 0; j < arrCorridors.length; j++)
              {
                if (arrCorridors[j] == null)
                {
                  data[n] = currentCorridor;
                  currentCorridor = arrCorridors[j-2];
                  arrCorridors[j] = currentCorridor; 
                  break;
                }
              }
            
              txtArea.appendText("Returning to " + currentCorridor);
              
              break;
            }
            else
            {//if previous two data entries not the same it must be a new corridor, 
              corridors[i].setCorridorID("R_Corridor" + i);
              currentCorridor = corridors[i].getCorridorID();
              txtArea.appendText("Reached: " + corridors[i].getCorridorID());
              data[n] = currentCorridor;
              for (int j = 0; j < arrCorridors.length; j++)
              {
                if (arrCorridors[j] == null)
                {
                  arrCorridors[j] = currentCorridor; 
                  break;
                }
              }
              break;
            }
            }
          }
          break;
        }
      }
    }

    else if ("turnC".equals(trimmedVal)) 
    {//if turn C received do this
      for (int i = 0; i < corridors.length; i++)
      {
        if (currentCorridor == corridors[i].getCorridorID())
        {
          txtArea.appendText(corridors[i].getCorridorID());
        }
      }
    }
    else if ("Wall detected, stopping.".equals(trimmedVal))
    {
      txtArea.appendText(trimmedVal);
      
      if (corridors[1].getCorridorID() != "unavailable")
      {
        for (int i = 0; i < corridors.length; i++)
        {
          if (corridors[i].getCorridorID() == "unavailable")
          {
            for (int n = 0; n < data.length; n++)
            {
              if (data[n] == null)
              {
                if (data[n-1] == data[n-2])
                {
                  for (int j = 0; j < arrCorridors.length; j++)
                  {
                    if (arrCorridors[j] == null)
                    {
                      currentCorridor = arrCorridors[j-2];
                      
                      if (currentCorridor.charAt(0) == 'R')
                      {
                        btnLeft.setEnabled(false);
                      }
                      else if (currentCorridor.charAt(0) == 'L')
                      {
                        btnRight.setEnabled(false); 
                      }
                      break;
                    }
                  }
                  
                  txtArea.appendText("Returning to " + currentCorridor);
                  break;
                }
                break;
              }
            }
            break;
          }
        }
      }
    }
    else
    {
      txtArea.appendText(newVal); //print it out in the console
    }
  }
}
// Use this method to add additional statements
// to customise the GUI controls
public void customGUI()
{
}