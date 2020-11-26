
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <math.h>


#define servoCount 6  // Define number of servo motors in your robot arm
//const int sclPin = D1;
//const int sdaPin = D2;

uint16_t servoMaxAngle[]= {270,270,270,270,270,180};
uint16_t SERVOMIN[]= {100,100,100,100,100,150}; // this is the 'minimum' pulse length count (out of 4096)
uint16_t SERVOMAX[]= {600,600,600,600,600,600};

uint16_t newAngleServo[]= {141,124,128,135,135,7}; // this is first move angles. On powerup robot will move to take this position.
uint16_t preAngleServo[]= {141,124,128,135,135,7};

double myXYZ[]= {186,0,264}; // I will save endpoint coordinates here.

const char* ssid = "Suresh";
const char* password = "18041974";

ESP8266WebServer server(80);

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();


double l1=137;
double l2=64;
double l3=64;
double l4=25;
double l5=160;
double l6;

double angle0;
double angle1;
double angle2;
double angle3;

double x;
double y;
double z;
double m;
double k;
double radian=57.2957;  //multiplier for radian to degree conversion.

double r;

String lastInput="app"; // flag to remember last update. app, serial or web for accelerometer-app, Serial port or webpage respectively

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.setTimeout(1000);
 WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("IP number assigned by DHCP is ");
  Serial.println(WiFi.localIP());
  
server.on("/", handleRoot);
server.onNotFound(handleNotFound);
server.begin();


    pwm.begin();
  
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  yield();

//Move robot to start position
firstMove(0); //Move Servo-0
firstMove(1); //Move Servo-1
firstMove(2); //Move Servo-2
firstMove(3); //Move Servo-3
firstMove(4); //Move Servo-4
firstMove(5); //Move Servo-5
  }

void loop() {
  // put your main code here, to run repeatedly:
  
getXYZ();
server.handleClient(); //receive accelerometer data from mobile and find new x y z

 //moveServo(0);
   // moveServo(1);
   // moveServo(2);
   // moveServo(3);
   // moveServo(4);
   // moveServo(5);
    //smoothMoveServo();


yield();



if (x != 0 or y != 0 or z != 0) {
findAngles(x,y,z);

//if (myXYZ[0] != 0 or myXYZ[1] != 0 or myXYZ[2] != 0) {
//findAngles(myXYZ[0],myXYZ[1],myXYZ[2]);


newAngleServo[0]= 141+angle0; //Our mean position is 135 degree. As per right hand rule we should use minus sign as per orientation and rotating direction of our motors.
newAngleServo[1]= 124+angle1; 
newAngleServo[2]= 128+angle2; 
newAngleServo[3]= 135+angle3; 
}

    if (!isnan(angle0) && !isnan(angle1)  && !isnan(angle2)  && !isnan(angle3) ) {  
    smoothMoveServo();
    
   
    }

}


//FUNCTIONS/////////////////////////////////

void getXYZ(){        //get xyz values from serial input
  x=0;
  y=0;
  z=0;
Serial.println("x,y,z from serial:");

//////////////////////////////////////////
//while(!Serial.available()) ; 
if (Serial.available() > 0) {
  x=Serial.parseInt();
  y=Serial.parseInt();
  z=Serial.parseInt();

   lastInput="serial";
//    if (Serial.read() == '\n') {
//      return;
//    }
}
/////////////////////////////////////////////////
  

/*

String serInput;
Serial.flush(); //flush all previous received and transmitted data
Serial.println("x:");
while(!Serial.available()) ; // hang program until a byte is received notice the ; after the while()
serInput=Serial.read();
x=serInput.toInt();

Serial.flush(); //flush all previous received and transmitted data
Serial.println("y:");
while(!Serial.available()) ; // hang program until a byte is received notice the ; after the while()
serInput=Serial.read();
y=serInput.toInt();


Serial.flush(); //flush all previous received and transmitted data
Serial.println("z:");
while(!Serial.available()) ; // hang program until a byte is received notice the ; after the while()
serInput=Serial.read();
z=serInput.toInt();
*/
Serial.print("x:");
Serial.println(x);
Serial.print("y:");
Serial.println(y);
Serial.print("z:");
Serial.println(z);

}

void findxyzFromAngle(){
  Serial.println("findxyzFromAngle Function start");
angle0=preAngleServo[0]-141;
angle1=preAngleServo[1]-124;
angle2=preAngleServo[2]-128;
angle3=preAngleServo[3]-135;

  myXYZ[2]=l2*sin((90-angle1)/radian)+l3*sin((90-angle1-angle2)/radian)+l1; //this is z coordinate

if (angle1+angle2+angle3 > 0) {                                     // x is positive if angle1+angle2+angle3=90
  Serial.println("angle1+angle2+angle3 is positive");
  r=l2*sin(angle1/radian)+l3*sin((angle1+angle2)/radian)+l4+l5;
}
else {
  Serial.println("angle1+angle2+angle3 is negative");
   r=l2*sin(angle1/radian)+l3*sin((angle1+angle2)/radian)-l4-l5;    // x is negative if angle1+angle2+angle3=-90
}

if (abs(r) < l4+l5) {   
  return;              // exit from function to avoid wrong values of x y z
  Serial.println("Exit from findxyzFromAngle function");
  } 
  
myXYZ[0]=r*sin((90-angle0)/radian); // this is my x coordinate
myXYZ[1]=r*sin(angle0/radian); //// this is my y coordinate


Serial.println(r);
Serial.println(myXYZ[0]);
Serial.println(myXYZ[1]);
Serial.println(myXYZ[2]);
Serial.println(angle0);
Serial.println(angle1);
Serial.println(angle2);
Serial.println(angle3);
Serial.println("findxyzFromAngle Function end");
  }

void findAngles(double x,double y,double z){
 angle0=atan(y/x)*radian;
 double r=sqrt(pow(x,2)+pow(y,2));
 l6=sqrt(sq(r-(l4+l5))+sq(z-l1));
 
// double k=acos((r-l4-l5)/l6)*radian;
double k=atan((z-l1)/(r-l4-l5))*radian;
 
 double m=acos((sq(l2)+sq(l6)-sq(l3))/(2*l2*l6))*radian;
 angle1=90-k-m;
 double b=acos((sq(l2)+sq(l3)-sq(l6))/(2*l2*l3))*radian;      // angle B im triangle ABC
 angle2=180-b;
 
 double n=180-k-m;
 angle3=180-angle2-n;


//if x is -ve then change sign of angle1,2,3

if (x<0){
  //angle0=angle0*(-1);
angle1=angle1*(-1);
angle2=angle2*(-1);
angle3=angle3*(-1);
}
 
Serial.print("angles from findAngles function:");
Serial.print(angle0);
Serial.print(",");
Serial.print(angle1);
Serial.print(",");
   Serial.print(angle2);
Serial.print(",");
Serial.println(angle3);

  }


void moveServo(uint8_t i) {
    uint16_t prePulselength = map(preAngleServo[i], 0, servoMaxAngle[i], SERVOMIN[i], SERVOMAX[i]);
    uint16_t newPulselength = map(newAngleServo[i], 0, servoMaxAngle[i], SERVOMIN[i], SERVOMAX[i]);
    Serial.println("moveServo Function Start on Servo: "+ String(i,DEC));
    Serial.println(prePulselength);
    Serial.println(newPulselength);
    

  
        if  (newPulselength > prePulselength) {
             Serial.println("IF");
             for (uint16_t pulselen = prePulselength; pulselen < newPulselength; pulselen++) {
                  pwm.setPWM(i, 0, pulselen);
                //Serial.println(pulselen);
             }
        }
  
        else {
              Serial.println("ELSE");
              for (uint16_t pulselen = prePulselength; pulselen > newPulselength; pulselen--) {
                   pwm.setPWM(i, 0, pulselen);
                   //Serial.println(pulselen);
              }
        }

    preAngleServo[i] = newAngleServo[i]; //update the angle moved in array
    delay(100);
  
} //end of moveServo function




void firstMove(uint8_t i) {
uint16_t newPulselength = map(newAngleServo[i], 0, servoMaxAngle[i], SERVOMIN[i], SERVOMAX[i]);
Serial.println("firstMove Function Start on Servo: " + String(i,DEC));
pwm.setPWM(i, 0, newPulselength);
preAngleServo[i] = newAngleServo[i]; //update the angle moved in array
delay(2000);
} //end of firstMove function






void smoothMoveServo() {
uint16_t prePulselength[(servoCount)];
uint16_t newPulselength[(servoCount)];

for (int i=0; i < servoCount; i++){           // Populate the Pulselength arrays
Serial.print("preAngleServo and newAngleServo:");
Serial.print(preAngleServo[i]);
Serial.print(";");
Serial.println(newAngleServo[i]);
prePulselength[i] = map(preAngleServo[i], 0, servoMaxAngle[i], SERVOMIN[i], SERVOMAX[i]);
newPulselength[i] = map(newAngleServo[i], 0, servoMaxAngle[i], SERVOMIN[i], SERVOMAX[i]);
Serial.print("smoothMoveServo Function build array: "+ String(i,DEC) +";");
Serial.print(prePulselength[i]);
Serial.print(";");
Serial.println(newPulselength[i]);
}

//Exit from function if newPulselength is out of range
for (int i=0; i < servoCount; i++){ 
  if ((newPulselength[i]>SERVOMAX[i]) or (newPulselength[i] < SERVOMIN[i])){return ;}
}



Serial.print("compare result:");
Serial.println((compareArrays(prePulselength,newPulselength,servoCount)));


while (!compareArrays(prePulselength,newPulselength,servoCount)){ //while Pulselength arrays are not same
yield();
//(!Arrays.equals(prePulselength, newPulselength)) {
  for (int i=0; i < servoCount; i++){
    Serial.println();
    Serial.print("smoothMoveServo Function Start on Servo: "+ String(i,DEC) +";");
      if  (newPulselength[i] > prePulselength[i]) {
           Serial.print("IF;" + String(newPulselength[i],DEC) + ";");
           prePulselength[i]++;
           pwm.setPWM(i, 0, prePulselength[i]);
           Serial.println(prePulselength[i]);
      }
                else if (prePulselength[i] > newPulselength[i]){
                    Serial.print("ELSE;");
                    prePulselength[i]--;
                    pwm.setPWM(i, 0, prePulselength[i]);
                             Serial.println(prePulselength[i]);
                        }

       if (prePulselength[i] == newPulselength[i]){
          preAngleServo[i] = newAngleServo[i]; //update the angle moved in array

          if (x != 0 or y != 0 or z != 0) {                 //if xyz input comes from serial or app
          myXYZ[0]=x; myXYZ[1]=y; myXYZ[2]=z;              //update the new coordinates in array
          Serial.print("New updated XYZ=");
          Serial.print(myXYZ[0]);
          Serial.print(myXYZ[1]);
          Serial.println(myXYZ[2]);
          }
          
             }      
        }
}



} // end of smoothMoveServo


// compareArrays function returns True if both arrays have same content.
boolean compareArrays(uint16_t arrayA[],uint16_t arrayB[], int numItems) {
    boolean same = true;
    int j = 0;
    while(j<numItems && same) { 
     // Serial.println(j);
     // Serial.println(arrayA[j]);
     //  Serial.println(arrayB[j]);
      same = (arrayA[j] == arrayB[j]);
      //Serial.println(String(same,DEC));
      j++;
    }
    return same;
    }



/////////////////////////////////////////////////////////////////////////////////////////
void handleRoot() {
 
Serial.println(server.arg(0) + "##" + server.arg(1) + "##" + server.arg(2)); 
String userInput = server.arg(0);
Serial.println("userInput received is " + userInput);

Serial.println(server.arg("xGforce"));
Serial.println(server.arg("yGforce"));
Serial.println(server.arg("zGforce"));

////////////////////////////////////////////////

 int xGforce=server.arg("xGforce").toInt();
 int yGforce=server.arg("yGforce").toInt();
 int zGforce=server.arg("zGforce").toInt();

 
  Serial.println(xGforce);
   Serial.println(yGforce);
    Serial.println(zGforce);
//find xyz from previous angle if myXYZ array is empty or the motors were moved by adjusting sliders in webpage
if ((myXYZ[0]==0) or (lastInput=="web")){
findxyzFromAngle();
}

x=myXYZ[0];   //present xyz values
y=myXYZ[1];
z=myXYZ[2];

 if (xGforce > 5) {
  Serial.println("increase x");
  x=myXYZ[0]+4;
  lastInput="app";}

if (yGforce > 5) {
  Serial.println("increase y");
  y=myXYZ[1]+4;
  lastInput="app";}


  if (xGforce < -3) {
    Serial.println("decrease x");
  x=myXYZ[0]-4;
  lastInput="app";}

   if (yGforce < -3) {
    Serial.println("decrease y");
  y=myXYZ[1]-4;
  lastInput="app";}

  Serial.print("xyz from mobile:");
   Serial.print(x);
   Serial.print(";");
   Serial.print(y);
   Serial.print(";");
   Serial.print(z);
   Serial.println(";");
 
//////////////////////////////////////////////////////

if(userInput.indexOf("S0") == 0){
  userInput.remove(0,3);
  newAngleServo[0]= userInput.toInt(); //cut (S0,) preceding servo number and save angle in array.
  lastInput="web";
  Serial.println("Input received: Servo:0, NewAngle:" + userInput);
  }
else if(userInput.indexOf("S1") == 0){
  userInput.remove(0,3);
  newAngleServo[1]=userInput.toInt();  //cut (S1,) preceding servo number and save angle in array.
  lastInput="web";
  Serial.println("Input received: Servo:1, NewAngle:" + userInput);
  }
else if(userInput.indexOf("S2") == 0){
  userInput.remove(0,3);
  newAngleServo[2]=userInput.toInt();  //cut (S2,) preceding servo number and save angle in array.
  lastInput="web";
  Serial.println("Input received: Servo:2, NewAngle:" + userInput);
  }
else if(userInput.indexOf("S3") == 0){
  userInput.remove(0,3);
  newAngleServo[3]=userInput.toInt();  //cut (S3,) preceding servo number and save angle in array.
  lastInput="web";
  Serial.println("Input received: Servo:3, NewAngle:" + userInput);
  }
else if(userInput.indexOf("S4") == 0){
  userInput.remove(0,3);
  newAngleServo[4]=userInput.toInt();  //cut (S4,) preceding servo number and save angle in array.
  lastInput="web";
  Serial.println("Input received: Servo:4, NewAngle:" + userInput);
  }
else if(userInput.indexOf("S5") == 0){
  userInput.remove(0,3);
  newAngleServo[5]=userInput.toInt();  //cut (S5,) preceding servo number and save angle in array.
  lastInput="web";
  Serial.println("Input received: Servo:5, NewAngle:" + userInput);
  }


String webpage = "<!DOCTYPE HTML>\r\n";
webpage += "<html>\r\n";
webpage += "<head>";
webpage += "<meta name='mobile-web-app-capable' content='yes' />";
webpage += "<meta name='viewport' content='width=device-width' />";
webpage+="<style>";
webpage+=".button{background-color:turquoise; border: none;color: white; padding: 15px 32px;text-align: center;text-decoration: none;display: inline-block;font-size: 16px;margin: 4px 2px;cursor: pointer;";

webpage+="</style>";
webpage += "</head>";

webpage += "<body>";

/* <--------- Use following if you want to code each slider individualy. I have used for loop to make all required sliders ------------>

webpage+="<p>servo0</p>";
webpage += "<left><input type=\"range\" min=\"0\" max=\"270\" value=\""+String(preAngleServo[0],DEC)+"\" step=\"1\" onchange=\"showValue(this.value)\" size=\"80\" align=\"left\" > </input></left>";
webpage += "<span id=\"range\">"+String(preAngleServo[0],DEC)+"</span>";
webpage += "<script type=\"text/javascript\">";
webpage += "function showValue(newValue){document.getElementById(\"range\").innerHTML=newValue;";
webpage += "var req = new XMLHttpRequest();";
webpage += "req.open('POST', '?' + 'angleValue=S0,' + newValue, true);";
webpage += "req.send();";
webpage += "}";
webpage += "</script>";



webpage+="<p>servo1</p>";
webpage += "<left><input type=\"range\" min=\"0\" max=\"270\" value=\""+String(preAngleServo[1],DEC)+"\" step=\"1\" onchange=\"showValue1(this.value)\" size=\"80\" align=\"left\" > </input></left>";
webpage += "<span id=\"range1\">"+String(preAngleServo[1],DEC)+"</span>";
webpage += "<script type=\"text/javascript\">";
webpage += "function showValue1(newValue1){document.getElementById(\"range1\").innerHTML=newValue1;";
webpage += "var req = new XMLHttpRequest();";
webpage += "req.open('POST', '?' + 'angleValue=S1,' + newValue1, true);";
webpage += "req.send();";
webpage += "}";
webpage += "</script>";
*/


for(int i=0; i<servoCount; i++){

webpage+="<p>servo"+String(i,DEC)+"</p>";
webpage += "<left><input type=\"range\" min=\"0\" max=\""+String(servoMaxAngle[i],DEC)+"\" value=\""+String(preAngleServo[i],DEC)+"\" step=\"1\" onchange=\"showValue"+String(i,DEC)+"(this.value)\" size=\"80\" align=\"left\" > </input></left>";
webpage += "<span id=\"range"+String(i,DEC)+"\">"+String(preAngleServo[i],DEC)+"</span>";
webpage += "<script type=\"text/javascript\">";
webpage += "function showValue"+String(i,DEC)+"(newValue"+String(i,DEC)+"){document.getElementById(\"range"+String(i,DEC)+"\").innerHTML=newValue"+String(i,DEC)+";";
webpage += "var req = new XMLHttpRequest();";
webpage += "req.open('POST', '?' + 'angleValue=S"+String(i,DEC)+",' + newValue"+String(i,DEC)+", true);";
webpage += "req.send();";
webpage += "}";
webpage += "</script>";
}


webpage += "</body></html>\r\n";

server.send(200, "text/html", webpage);    
}

void handleNotFound(){
  //digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  //digitalWrite(led, 0);
}


///////////////////////////////////////////////////////////////////////////////////////

void setServoPulse(uint8_t n, double pulse) {
  double pulselength;
  
  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= 60;   // 60 Hz
  Serial.print(pulselength); Serial.println(" us per period"); 
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength); Serial.println(" us per bit"); 
  pulse *= 1000;
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}

///////////////////////////////////////////////////////////////////////////////////////


