public class Rooms {
  
  private boolean object;
  private String corridorID;
  private String roomID;  

  public Rooms() {
    object = false;
    corridorID = "unavailable";
    roomID = "unavailable";
  }
  
  public boolean getObject() {
    return object;
  }
  
  public String getCorridorID() {
    return corridorID;
  }
  
  public String getRoomID() {
    return roomID;
  }
  
  public void setObject(boolean newObject) {
    object = newObject;
  }
  
  public void setCorridorID(String newCorridorID) {
    corridorID = newCorridorID;
  }
  
  public void setRoomID(String newRoomID) {
    roomID = newRoomID;
  }

  public String toString() {
    return "Room ID: " + roomID +  "\nCorridor ID: " + corridorID + "\nObject Found: " + object;
  }
}