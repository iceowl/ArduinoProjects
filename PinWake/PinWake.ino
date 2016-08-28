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

int myPinCallback(uint32_t ulPin)
{
  for(int i = 0;i < 5;i++) {
  digitalWrite(4, HIGH);
  delay(1000);
  digitalWrite(4, LOW);
  }
  return 0;  // don't exit Simblee_ULPDelay
}

void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  
  // processing handled by exiting Simblee_ULPDelay
  pinMode(5, INPUT);
  Simblee_pinWake(5, HIGH);
  
  // processing handled by myPinCallback
  pinMode(6, INPUT);
  Simblee_pinWakeCallback(6, HIGH, myPinCallback);
}

void loop() {
  Simblee_ULPDelay(INFINITE);
  
  if (Simblee_pinWoke(5))
  {
    for(int i = 0; i < 6; i++){
    digitalWrite(3, HIGH);
    delay(1000);
    digitalWrite(3, LOW);
    }
    
    Simblee_resetPinWake(5);
  }
  else
  {
     for(int i = 0; i < 2; i++){
    Serial.println("timed out");
    digitalWrite(2, HIGH);
    delay(250);
    digitalWrite(2, LOW);
     }
  }
}
