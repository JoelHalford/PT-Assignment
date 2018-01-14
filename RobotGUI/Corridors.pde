public class Corridors {
  
  private String corridorID;
  private int numOfRooms;
  private int numOfObjects;
   
  public Corridors() {
    this.corridorID = "unavailable";
    this.numOfRooms = 0;
    this.numOfObjects = 0;
  }
  
  public String getCorridorID() {
    return corridorID;
  }
  
  public int getRoomNum() {
    return numOfRooms;
    
  }
  
  public int getObjectNum() {
    return numOfObjects;
  }
  
  public void setCorridorID(String newCorridorID) {
    corridorID = newCorridorID;
  }
  
  public void setRoomNum() {
    numOfRooms++;
  }
  
  public void setObjectNum(int newNumOfObjects) {
    numOfObjects = newNumOfObjects;
  }
}