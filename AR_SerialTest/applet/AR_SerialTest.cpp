#include "WProgram.h"
uint16_t requested(void);
void printLEDcolorHex(void);
uint8_t parseHex(char c);
void changeLED(void);
void respond();
#include "string.h"
#include "EEPROM.h"
#include "avr/pgmspace.h"
#include "AF_XPort.h"
#include "AFSoftSerial.h"
/* Xport settings:
   *** Channel 1
   Baudrate 57600, I/F Mode 4C, Flow 02
   Port 00080
   Connect Mode : C4
   Send '+++' in Modem Mode enabled
   Show IP addr after 'RING' enabled
   Auto increment source port disabled
   Remote IP Adr: --- none ---, Port 00000
   Disconn Mode : 80  Disconn Time: 00:03
   Flush   Mode : 77
*/

// Strings stored in flash of the HTML we will be xmitting
const char http_404header[] PROGMEM = "HTTP/1.1 404 Not Found\nServer: arduino\nContent-Type: text/html\n\n<html><head><title>404</title></head><body><h1>404: Sorry, that page cannot be found!</h1></body>";
const char http_header[] PROGMEM = "HTTP/1.0 200 OK\nServer: arduino\nContent-Type: text/html\n\n";
const char html_header[] PROGMEM = "<html><head><title>XPort Color picker</title></head> <body>";
const char javascript[] PROGMEM = "<script type=\"text/javascript\">function getScrollY(){var scrOfX = 0,scrOfY=0;if(typeof(window.pageYOffset)=='number'){scrOfY=window.pageYOffset;scrOfX=window.pageXOffset;}else if(document.body&&(document.body.scrollLeft||document.body.scrollTop)){scrOfY=document.body.scrollTop;scrOfX=document.body.scrollLeft;}else if(document.documentElement&&(document.documentElement.scrollLeft||document.documentElement.scrollTop)){scrOfY=document.documentElement.scrollTop;scrOfX=document.documentElement.scrollLeft;}return scrOfY;}document.write(\"<style type='text/css'>.colorpicker201{visibility:hidden;display:none;position:absolute;background:#FFF;border:solid 1px #CCC;padding:4px;z-index:999;filter:progid:DXImageTransform.Microsoft.Shadow(color=#D0D0D0,direction=135);}.o5582brd{padding:0;width:12px;height:14px;border-bottom:solid 1px #DFDFDF;border-right:solid 1px #DFDFDF;}a.o5582n66,.o5582n66,.o5582n66a{font-family:arial,tahoma,sans-serif;text-decoration:underline;font-size:9px;color:#666;border:none;}.o5582n66,.o5582n66a{text-align:center;text-decoration:none;}a:hover.o5582n66{text-decoration:none;color:#FFA500;cursor:pointer;}.a01p3{padding:1px 4px 1px 2px;background:whitesmoke;border:solid 1px #DFDFDF;}</style>\");function getTop2(){csBrHt=0;if(typeof(window.innerWidth)=='number'){csBrHt=window.innerHeight;}else if(document.documentElement&&(document.documentElement.clientWidth||document.documentElement.clientHeight)){csBrHt=document.documentElement.clientHeight;}else if(document.body&&(document.body.clientWidth||document.body.clientHeight)){csBrHt=document.body.clientHeight;}ctop=((csBrHt/2)-115)+getScrollY();return ctop;}var nocol1=\"&#78;&#79;&#32;&#67;&#79;&#76;&#79;&#82;\",clos1=\"&#67;&#76;&#79;&#83;&#69;\",tt2=\"&#70;&#82;&#69;&#69;&#45;&#67;&#79;&#76;&#79;&#82;&#45;&#80;&#73;&#67;&#75;&#69;&#82;&#46;&#67;&#79;&#77;\",hm2=\"&#104;&#116;&#116;&#112;&#58;&#47;&#47;&#119;&#119;&#119;&#46;\";hm2+=tt2;tt2=\"&#80;&#79;&#87;&#69;&#82;&#69;&#68;&#32;&#98;&#121;&#32;&#70;&#67;&#80;\";function getLeft2(){var csBrWt=0;if(typeof(window.innerWidth)=='number'){csBrWt=window.innerWidth;}else if(document.documentElement&&(document.documentElement.clientWidth||document.documentElement.clientHeight)){csBrWt=document.documentElement.clientWidth;}else if(document.body&&(document.body.clientWidth||document.body.clientHeight)){csBrWt=document.body.clientWidth;}cleft=(csBrWt/2)-125;return cleft;}function setCCbldID2(objID,val){document.getElementById(objID).value=val;}function setCCbldSty2(objID,prop,val){switch(prop){case \"bc\":if(objID!='none'){document.getElementById(objID).style.backgroundColor=val;};break;case \"vs\":document.getElementById(objID).style.visibility=val;break;case \"ds\":document.getElementById(objID).style.display=val;break;case \"tp\":document.getElementById(objID).style.top=val;break;case \"lf\":document.getElementById(objID).style.left=val;break;}}function putOBJxColor2(OBjElem,Samp,pigMent){if(pigMent!='x'){setCCbldID2(OBjElem,pigMent);setCCbldSty2(Samp,'bc',pigMent);}setCCbldSty2('colorpicker201','vs','hidden');setCCbldSty2('colorpicker201','ds','none');}function showColorGrid2(OBjElem,Sam){var objX=new Array('00','33','66','99','CC','FF');var c=0;var z='\"'+OBjElem+'\",\"'+Sam+'\",\"\"';var xl='\"'+OBjElem+'\",\"'+Sam+'\",\"x\"';var mid='';mid+='<table bgcolor=\"#FFFFFF\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" style=\"border:solid 0px #F0F0F0;padding:2px;\"><tr>';mid+=\"<td colspan='18' align='left' style='font-size:10px;background:#6666CC;color:#FFF;font-family:arial;'>&nbsp;Chromatic Selection Palette</td></tr><tr><td colspan='18' align='center' style='margin:0;padding:2px;height:12px;' ><input class='o5582n66' type='text' size='12' id='o5582n66' value='#FFFFFF'><input class='o5582n66a' type='text' size='2' style='width:14px;' id='o5582n66a' onclick='javascript:alert(\\\"click on selected swatch below...\\\");' value='' style='border:solid 1px #666;'>&nbsp;|&nbsp;<a class='o5582n66' href='javascript:onclick=putOBJxColor2(\"+z+\")'><span class='a01p3'>\"+nocol1+\"</span></a>&nbsp;&nbsp;&nbsp;&nbsp;<a class='o5582n66' href='javascript:onclick=putOBJxColor2(\"+xl+\")'><span class='a01p3'>\"+clos1+\"</span></a></td></tr><tr>\";var br=1;for(o=0;o<6;o++){mid+='</tr><tr>';for(y=0;y<6;y++){if(y==3){mid+='</tr><tr>';}for(x=0;x<6;x++){var grid='';grid=objX[o]+objX[y]+objX[x];var b=\"'\"+OBjElem+\"', '\"+Sam+\"','#\"+grid+\"'\";mid+='<td class=\"o5582brd\" style=\"background-color:#'+grid+'\"><a class=\"o5582n66\"  href=\"javascript:onclick=putOBJxColor2('+b+');\" onmouseover=javascript:document.getElementById(\"o5582n66\").value=\"#'+grid+'\";javascript:document.getElementById(\"o5582n66a\").style.backgroundColor=\"#'+grid+'\";  title=\"#'+grid+'\"><div style=\"width:12px;height:14px;\"></div></a></td>';c++;}}}mid+=\"</tr><tr><td colspan='18' align='right' style='padding:2px;border:solid 1px #FFF;background:#FFF;'><a href='\"+hm2+\"' style='color:#666;font-size:8px;font-family:arial;text-decoration:none;letter-spacing:1px;'>\"+tt2+\"</a></td></tr></table>\";var ttop=getTop2();setCCbldSty2('colorpicker201','tp',ttop);document.getElementById('colorpicker201').style.left=getLeft2();setCCbldSty2('colorpicker201','vs','visible');setCCbldSty2('colorpicker201','ds','block');document.getElementById('colorpicker201').innerHTML=mid;}</script>";
const char form[] PROGMEM = "<p><h1>Arduino Webserver: LED color picker!</h1><p><p><center><div id=\"colorpicker201\" class=\"colorpicker201\"></div> <form name=\"input\" action=\"\" method=\"get\"> Pick LED Color: <input type=\"button\" onclick=\"showColorGrid2('input_field_1','sample_1');\" value=\"...\">&nbsp;<input type=\"text\" ID=\"input_field_1\" size=\"9\" name=\"color\" value=\"";
const char form2[] PROGMEM = "\">&nbsp;<input type=\"text\" ID=\"sample_1\" size=\"1\" value=\"\"> <input type=\"submit\" value=\"Submit\"> </form></center></body></html>";

// RGB led connections
#define bluePin  9   // Blue LED,   connected to digital pin 9
#define greenPin 10  // Green LED, connected to digital pin 10
#define redPin 11  // Red LED,  connected to digital pin 11

// EEprom locations for the data (for permanent storage)
#define RED_EADDR 1
#define GREEN_EADDR 2
#define BLUE_EADDR 3

char linebuffer[128];    // a large buffer to store our data

// keep track of how many connections we've got
int requestNumber = 0;

// the xport!
#define XPORT_RX        2
#define XPORT_TX        3
#define XPORT_RESET     4
#define XPORT_CTS       6
#define XPORT_RTS       0 // not used
#define XPORT_DTR       0 // not used
AF_XPort xport = AF_XPort(XPORT_RX, XPORT_TX, XPORT_RESET, XPORT_DTR, XPORT_RTS, XPORT_CTS);

//////////////////////////////////////////////////////

void setup()  {
  pinMode(redPin,   OUTPUT);   // sets the pins as output
  pinMode(greenPin, OUTPUT);   
  pinMode(bluePin,  OUTPUT); 
  
  analogWrite(redPin, EEPROM.read(RED_EADDR));   // Write current values to LED pins
  analogWrite(greenPin, EEPROM.read(GREEN_EADDR)); 
  analogWrite(bluePin,  EEPROM.read(BLUE_EADDR));  
  
  Serial.begin(57600);
  Serial.println("serial port ready");
  xport.begin(57600);
  xport.reset();
  Serial.println("XPort ready");
}

void loop()               
{
  uint16_t ret;
  ret = requested();
  if (ret == 404) {
     xport.flush(250);
     // first the stuff for the web client
     xport.ROM_print(http_404header);    
     xport.disconnect();
  } else if (ret == 200) {
    changeLED();
    respond();
    
    Serial.print("Requested! No. ");
    Serial.println(requestNumber);
    delay(4000);
    pinMode(XPORT_RESET, HIGH);
    delay(50); 
    pinMode(XPORT_RESET, LOW);
    
    requestNumber++;
  }
}
uint16_t requested(void) {
  uint8_t read, x;
  char *found;
  //Serial.println("Waiting for connection...");
  while (1) {
    read = xport.readline_timeout(linebuffer, 128, 200);
    //Serial.println(read, DEC);   // debugging output
    if (read == 0)     // nothing read (we timed out)
      return 0;
    if (read)          // we got something! 
       Serial.println(linebuffer);

   if (strstr(linebuffer, "GET / ")) {
      return 200;   // a valid request!
    }
   if (strstr(linebuffer, "GET /?")) { 
      return 200;   // a valid CGI request!
   }
   if(strstr(linebuffer, "GET ")) {
      return 404;    // some other file, which we dont have
   }
  }
}

// shortcut procedure just prints out the #xxxxxx values stored in EEPROM
void printLEDcolorHex(void) {
  uint8_t temp;
  
  xport.print('#');
  temp = 255-EEPROM.read(RED_EADDR);
  if (temp <= 16)  
     xport.print('0'); // print a leading zero
  xport.print(temp , HEX);   // Write current values to LED pins
  temp = 255-EEPROM.read(GREEN_EADDR);
  if (temp <= 16)   
     xport.print('0'); // print a leading zero
  xport.print(temp , HEX);   // Write current values to LED pins
    temp = 255-EEPROM.read(BLUE_EADDR);
  if (temp <= 16)   
     xport.print('0'); // print a leading zero
  xport.print(temp , HEX);   // Write current values to LED pins
}

// read a Hex value and return the decimal equivalent
uint8_t parseHex(char c) {
    if (c < '0')
      return 0;
    if (c <= '9')
      return c - '0';
    if (c < 'A')
       return 0;
    if (c <= 'F')
       return (c - 'A')+10;
}


// check to see if we got a colorchange request
void changeLED(void) {
    char *found=0;
    uint8_t red, green, blue;

    // Look for a ? style GET command    
    found = strstr(linebuffer, "?color=%23"); // "?color=#" GET request
    if (found) {
      // announce that we received a proper command
      Serial.println("changing color");
      found += 10; // skip forward in the string to the data part
      
      // extract the #xxxxxx data
      red = parseHex(found[0]) * 16 + parseHex(found[1]);
      found += 2;
      green = parseHex(found[0]) * 16 + parseHex(found[1]);
      found += 2;
      blue = parseHex(found[0]) * 16 + parseHex(found[1]);
      
      // Debug output
      Serial.print("red = "); Serial.print(red, HEX);
      Serial.print(" green = "); Serial.print(green, HEX);
      Serial.print(" blue = "); Serial.print(blue, HEX);
      Serial.println("");
      
      // save the data to the EEPROM for long term storage
      EEPROM.write(RED_EADDR, 255-red);  
      EEPROM.write(GREEN_EADDR, 255-green);  
      EEPROM.write(BLUE_EADDR, 255-blue);  
       
      // Set the LED. Since its common-anode, we have to invert the values (255 is off, not on)
      analogWrite(redPin,   255-red);   // Write current values to LED pins
      analogWrite(greenPin, 255-green); 
      analogWrite(bluePin,  255-blue);  
  }      
}


// Return the HTML place
void respond(){
  uint8_t temp;
  xport.flush(50);
  // first the stuff for the web client
  xport.ROM_print(http_header);
  // next the start of the html header
  xport.ROM_print(html_header);
  // the CSS code that will change the box to the right color when we first start
  xport.print("<style type=\"text/css\"> <!-- #sample_1 { background-color: "); printLEDcolorHex(); xport.print("; } --> </style>");
  // the javascript for the color picker
  xport.ROM_print(javascript);
  // the form, first part...
  xport.ROM_print(form);
  // insert color name here, which is the default 'value' for the 
  printLEDcolorHex();
  // the second half of the form
  xport.ROM_print(form2);
  // get rid of any other data left
  xport.flush(255);
  // disconnecting doesnt work on the xport direct by default? we will just use the timeout which works fine.
  xport.disconnect();
}
   

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

