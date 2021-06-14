

// This program is used to create a BLE central device, to connect to the secondary sensor module
// Currently in V1.1 there is no sensors connected so p,f,c variables are pre set and read from the secondary sensor. 
// very beta version as there are statments to make sure printing occurs
// THIS PROGRAM ONLY WORKS WHEN SERIAL MONITOR IS OPEN. OPEN SERIAL MONITOR


#include <ArduinoBLE.h>
#include <SPI.h>
#include <WiFiNINA.h>

#define conn_LED 13 // define pin as 13
#define dis_LED  7 // define as pin 14

String device = "WMC Analyser"; // This is string that is compared for local name for connection

///////////////////////////////////////////////////////////////////////////////////

void serial_setup(){
Serial.begin(9600);       // initialize UART with baud rate of 9600 bps

// no need to wait for serial monitor
// leave for now

 while (!Serial) { // while no signal   
   
}

Serial.println("serial setup complete");  // TESTING
}

///////////////////////////////////////////////////////////////////////////////////

void Pin_setup(){ // set all pins that we are using
  
pinMode(conn_LED,OUTPUT);
pinMode(dis_LED,OUTPUT);
 
}


///////////////////////////////////////////////////////////////////////////////////
void BLE_cha_setup(BLEDevice peripheral){

 BLECharacteristic flowrateChar = peripheral.characteristic("F34B");
 BLECharacteristic pHChar = peripheral.characteristic("A3F2");
 BLECharacteristic  ConductChar = peripheral.characteristic("CB49");
}

///////////////////////////////////////////////////////////////////////////////////

void setup() {
  // put your setup code here, to run once:

  serial_setup(); // run serial setup
  Pin_setup(); // intialize pins 

  
  startBLE(); 
  

  
BLE.setEventHandler(BLEConnected, onBLEConnected); // set connected event
BLE.setEventHandler(BLEDisconnected, onBLEDisconnected); // set disconnected event

Serial.println("BLE Central scan");
  BLE_Disconnect_Led();
  // start scanning for peripheral
  BLE.scan();

    Serial.println("Discovered a peripheral"); // print in format TESTING only
    Serial.println("-----------------------");
  
}

void loop() {

  BLEDevice peripheral = BLE.available();

 if (peripheral) { // if peripherals are detected
  discover(peripheral); // prints all local devices found
  rd_info(peripheral); // reads attributes and characteristics
  
}else{

 
}

}


///////////////////////////////////////////////////////////////////////////////////

void startBLE() {
  if (!BLE.begin())
  {
    Serial.println("starting BLE failed!");
    while (1);
  }
Serial.println("startBLE = success");
  
}


///////////////////////////////////////////////////////////////////////////////////

void discover (BLEDevice peripheral){ // discovers and prints found devices

// can be applicable to bluetooth connections
  
    // print the local name, if present
    if (peripheral.hasLocalName()) {       
      Serial.print("Local Name: ");
      Serial.println(peripheral.localName()); // obtain local names of devices
      Serial.print("MAC Address: "); // only print values with name
      Serial.println(peripheral.address()); // obtain MAC addresses
      Serial.print("UUID: "); // only print values with name
      Serial.println(peripheral.advertisedServiceUuid());
      Serial.print("RSSI: ");
      Serial.println(peripheral.rssi());

    String Add = peripheral.localName(); // if WMC is found connect to it

     if (device.equals(Add)){ // if device wanted is found establish connection

   Serial.print("Found WMC Device\n");
    BLE.stopScan(); // stop scanning
    est_connect(peripheral); // connect to device
     BLE_connect_Led();  
    }
   Serial.println();
    }
    
 //BLE_Disconnect_Led();
  
}

///////////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////////

void est_connect(BLEDevice peripheral){

Serial.println("Connecting ...");
  if (peripheral.connect()) {
    Serial.println("Connected");
    BLE_connect_Led();
  } else {
    Serial.println("Failed to connect!");
    BLE_Disconnect_Led();
    BLE.scan();
    return;
  }

  
}


///////////////////////////////////////////////////////////////////////////////////

void onBLEConnected(BLEDevice peripheral) { // connected event
  Serial.print("Connected event, peripheral: "); // TESTING
  Serial.println(peripheral.address()); // TESTING
  BLE_connect_Led(); 
}


void onBLEDisconnected(BLEDevice peripheral) { // disconnected event
  Serial.print("Disconnected event, peripheral: "); // TESTING
  Serial.println(peripheral.address()); // TESTING
  BLE_Disconnect_Led();
  BLE.scan();
}

///////////////////////////////////////////////////////////////////////////////////

// currently does not work
void rd_info(BLEDevice peripheral){ // this function reads discovers attributes and reads characteristics
  
if (peripheral.connected()) { // if we are connected
   BLE_connect_Led();  
  BLE_cha_setup(peripheral); // initialize characteristics apparently this does nothing

Serial.println("Discovering service 0xf2a4 ...");
  if (peripheral.discoverService("f2a4")) {
    Serial.println("Service discovered");
     BLE_connect_Led();
  } else {
    Serial.println("Attribute discovery failed.");
    //peripheral.disconnect(); // this causes issues
    BLE_Disconnect_Led();
}

 BLECharacteristic flowrateChar = peripheral.characteristic("F34B");
 BLECharacteristic pHChar = peripheral.characteristic("A3F2");
 BLECharacteristic  ConductChar = peripheral.characteristic("CB49");
 
     byte f = 0;
     byte p = 0;
     byte c = 0;

 while (peripheral.connected()) { //uncomment if you want to look continuously so no need for value updates
     
     flowrateChar.readValue(f);
     pHChar.readValue(p);
     ConductChar.readValue(c);


     Serial.print("flowrate(HEX) = ");
     Serial.println(f,HEX);
     Serial.print("pH(HEX) = ");
     Serial.println(p,HEX);
     Serial.print("Conductivity(HEX)= ");
     Serial.println(c,HEX);

     

if (flowrateChar.valueUpdated()) { // if value is updated

      flowrateChar.readValue(f);   
      Serial.println(f);
}


 } //uncomment if you want to look continuously so no need for value updates
 
}else{

 return; 
}

}

///////////////////////////////////////////////////////////////////////////////////

void BLE_connect_Led(){ //conn led high

digitalWrite(conn_LED, HIGH);  // high led high
digitalWrite(dis_LED, LOW); // other lED low

  
}

void BLE_Disconnect_Led(){ // blink dis_LED

digitalWrite(conn_LED, LOW);  // high led high
digitalWrite(dis_LED, HIGH); // other lED low
delay(500);
digitalWrite(dis_LED, LOW); // other lED low
delay(500);
  
}

///////////////////////////////////////////////////////////////////////////////////
