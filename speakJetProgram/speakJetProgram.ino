


 unsigned char sayThis[] = {20, 96, 21, 114, 22, 88, 23, 5, 183, 7, 159, 146, 164, 147, 151, 145, 176}; 

void setup() 
{ 

  Serial.begin(9600);
  for (int i=0; i<sizeof(sayThis); i++) {
    Serial.print(sayThis[i],CHAR);
  }

}

void loop(){}

