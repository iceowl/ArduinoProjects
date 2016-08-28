/*
This sketch demonstrates how to receive data from SimbleeCloud.

This sketch works in combination with the ButtonFlashToModule.html (a multiple view
html page that demonstrates how to send data to SimbleeCloud from html).

To use this sketch, you will need to do the following:
1. Signup on admin.simbleecloud.com for an account (its free)
2. Add a module with your simblee factory esn (output on the serial monitor)
4. Add a pool (with the esn 0x00000002)
3. Add the module esn to the ButtonFlashToModule.html page.
5. Compile and upload this sketch
6. Connect to your simblee with the SimbleeForMobile app on your smart phone
7. Open https://service.simbleecloud.com/ButtonFlashToModule.html with an internet
   browser to view the resultspwd
   
*/

/*
 * Copyright (c) 2015 RF Digital Corp. All Rights Reserved.
 *
 * The source code contained in this file and all intellectual property embodied in
 * or covering the source code is the property of RF Digital Corp. or its licensors.
 * Your right to use this source code and intellectual property is non-transferable,
 * non-sub licensable, revocable, and subject to terms and conditions of the
 * SIMBLEE SOFTWARE LICENSE AGREEMENT.
 * http://www.simblee.com/licenses/SimbleeSoftwareLicenseAgreement.txt
 *
 * THE SOURCE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.
 *
 * This heading must NOT be removed from this file.
 */

#include "SimbleeForMobile.h"
#include "SimbleeForMobileClient.h"
#include "SimbleeCloud.h"

// your user id from the admin.simbleecloud.com (required to authorize the module)
unsigned int userID = 0x8c7b58ac;

SimbleeForMobileClient client;
SimbleeCloud cloud(&client);

int led = 3;     // Green led on the RGB Shield

void setup()
{
  Serial.begin(9600);
  
  pinMode(led, OUTPUT);

  printf("Module ESN is 0x%08x\n", cloud.myESN);
  cloud.userID = userID;

  SimbleeForMobile.begin();
}

void ui()
{
  if (cloud.connect())
    printf("active\n");
  else
    printf("failed\n");
}

void loop()
{
  // process must be called in the loop for SimbleeForMobile
  SimbleeForMobile.process();
  if (! SimbleeForMobile.active)
    return;

  // process must be called in the loop for SimbleeForMobileCloud
  cloud.process();
  if (! cloud.active())
    return;
}

void SimbleeCloud_onReceive(unsigned int originESN, const unsigned char *payload, int len)
{
  if (payload[0] == 'S')
  {

  }
  else if (payload[0] == 'E')
  {

  }
  else
  {
    if (payload[0] == '1')
      digitalWrite(led, HIGH);
    else
      digitalWrite(led, LOW);
  }   
}
