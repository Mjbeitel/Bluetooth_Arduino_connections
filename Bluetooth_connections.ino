


// This program is used to create a BLE peripheral so far this is example code.
// Currently in V1.2 there is no sensors connected so p,f,c variables are used for testing. 
// very beta version as there are statments to make sure printing occurs
// create peripheral instance
    
#include <ArduinoBLE.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <SPI.h>
#include <uFire_EC.h>
uFire_EC ec;

// specify services and characteristics ////////////////////////////////////////////

BLEService batteryService("180F"); // service uuid
BLEUnsignedCharCharacteristic batteryLevelChar("2A19", BLERead | BLENotify); // characteristic uuid

// assign random ones for now change later
/////////////////////////////////////////////////////// for each individual service not necesary 
//BLEService flowrate("F34A"); // service uuid
//BLEService pH("A3F1"); // service uuid
//BLEService Conduct("CB48");// service uuid
///////////////////////////////////////////////////////


BLEService info("F2A4");// service uuid with all readings


BLEUnsignedCharCharacteristic flowrateChar("F34B", BLERead | BLENotify); // characteristic uuid
BLEUnsignedCharCharacteristic pHChar("A3F2", BLERead | BLENotify); // characteristic uuid
BLEUnsignedCharCharacteristic ConductChar("CB49", BLERead | BLENotify); // characteristic uuid

// read phones to read value
// notify allows to see if value changed


// must check this before i implement

#define conn_LED 13 // define pin as 13
#define dis_LED  7 // define as pin 14
#define pH_pin A6 // Analog pin for pH
byte PWM_pin = 2;


volatile float waterflow;
volatile float freq;
float tmp = 0.00;
float Q;

// pH sensor
float pH;
float voltage = 0.00; // voltage used in pH

// Conductivity sensor
float cond;

// testing values can only read last two bits
int p = 0x00EF; 
int f = 0x0001;
int c = 0x00A1;
// For testing purposes end

///////////////////////////////////////////////////////////////////////////////////

void serial_setup(){
Serial.begin(9600);       // initialize UART with baud rate of 9600 bps

// no need to wait for serial monitor

// while (!Serial) { // while no signal   
//   
//}

Serial.println("serial setup complete");  // TESTING
}

///////////////////////////////////////////////////////////////////////////////////

void BL_setup(){

BLE.setLocalName("WMC Analyser"); // set local name
BLE.setDeviceName("WMC Analyser"); // set name to WMC analyser

// advertising parameters
// this is example for battery level service
BLE.setAdvertisedService(batteryService);
batteryService.addCharacteristic(batteryLevelChar);
BLE.addService(batteryService);

///////////////////////////////////////////

// set all services to value
////////////////////////////////////////// not necessary to have service for each
//BLE.setAdvertisedService(flowrate);
//BLE.setAdvertisedService(pH);
//BLE.setAdvertisedService(Conduct);
//////////////////////////////////////////


BLE.setAdvertisedService(info); // create one service for all readings

// assign characteristics /////////////////////////////////
////////////////////////////////////////////////////////// not necessary to have service for each
//flowrate.addCharacteristic(flowrateChar);
//pH.addCharacteristic(pHChar);
//Conduct.addCharacteristic(ConductChar);
//////////////////////////////////////////////////////////

info.addCharacteristic(flowrateChar);
info.addCharacteristic(pHChar);
info.addCharacteristic(ConductChar);


// create services////////////////////////////////
///////////////////////////////////////////////// not necessary to have service for each
//BLE.addService(flowrate);
//BLE.addService(pH);
//BLE.addService(Conduct);
/////////////////////////////////////////////////


BLE.addService(info);

Serial.println("BL_setup = success");  // TESTING


// need this for later need to change

}

///////////////////////////////////////////////////////////////////////////////////

void Pin_setup(){ // set all pins that we are using
  
pinMode(conn_LED,OUTPUT);
pinMode(dis_LED,OUTPUT);
 
}


///////////////////////////////////////////////////////////////////////////////////

void setup() {
  // put your setup code here, to run once:

serial_setup(); // run serial setup
startBLE(); // start BLEconnection

BL_setup(); // set up characteristics
Pin_setup(); // intialize pins 

BLE.setEventHandler(BLEConnected, onBLEConnected); // set connected event
BLE.setEventHandler(BLEDisconnected, onBLEDisconnected); // set disconnected event


  Serial.println("hello"); // TESTING



  BLE.advertise(); // start advertising
  
  Serial.println("Bluetooth device active, waiting for connections..."); // TESTING
  
}

///////////////////////////////////////////////////////////////////////////////////


void loop() {
  // put your main code here, to run repeatedly:

BLEDevice central = BLE.central();

if (central) // when connected to central
{
 
// for testing
String address = BLE.address();

  Serial.print("Local address is: ");
  Serial.println(address);
  Serial.print("Your are now connected\n");
// for testing end
  

while (central.connected()) {  // while a central is connected
BLE_connect_Led();


// For testing purposes
/*
flowrateChar.writeValue(f);
pHChar.writeValue(p);
ConductChar.writeValue(cond);

*/

pH_read();
flow_read();
cond_read();
delay(1000);


// For testing purposes end
  
}

}else { // if disconnected

BLE_Disconnect_Led(); 
 
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

void onBLEConnected(BLEDevice central) { // connected event
  Serial.print("Connected event, central: "); // TESTING
  Serial.println(central.address()); // TESTING
  BLE_connect_Led();
}


void onBLEDisconnected(BLEDevice central) { // disconnected event
  Serial.print("Disconnected event, central: "); // TESTING
  Serial.println(central.address()); // TESTING
  BLE_Disconnect_Led();
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

float pH_read(){

 analogReadResolution(12); // change resolution
    // pH calculation
    voltage = analogRead(pH_pin)*(3.3/4095);
    pH = ((-5.6548*(voltage))+ 15.511);

    
    if (isnan(pH)) {
      
      Serial.println("Error reading pH!");
      Serial.print("\n");
    }
    else {
      Serial.print("pH: ");
      Serial.print(pH);
      
  
    pHChar.writeValue(pH);
    }
    
  return pH;
  
}



float flow_read(){

   // Get Flow Rate event and print its value.
    waterflow = pulseIn(PWM_pin, HIGH);
    
    if (waterflow != 0.0) {
      freq = (1 / (waterflow * 0.000001));
      Q = (freq / 11); // believe this is correct
      
      Serial.print("\n");
      Serial.print("waterFlow:");
      Serial.print(Q);
      Serial.println("   L/min");

      //publish to tingg.io
      flowrateChar.writeValue(Q);
     } 
     else {
      Serial.print("\n");
      Serial.println("Error reading Flow Rate!");

      waterflow = 0.00;
      freq =0.00;
      Q =0.00;
      flowrateChar.writeValue(Q);
     }

  return Q;
  
}


float cond_read(){

  // Get EC event and print its value.
    tmp = ec.measureEC(21.0);

    if((tmp >= -0.5) && (tmp <= 100.0)){
      Serial.print("\n");
      Serial.println((String) "EC (mS/cm): " + ec.measureEC(21.0));
      cond = tmp;
      
  ConductChar.writeValue(cond);
    }
return cond;
  
}
