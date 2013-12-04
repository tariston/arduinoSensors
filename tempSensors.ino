#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 10
#define wait_long 5000
#define wait_med 3000
#define wait_short 1000

OneWire data_stream(ONE_WIRE_BUS);
DallasTemperature temp_sensors(&data_stream);


void setup(void) {
 Serial.begin(9600); 
}

void loop(void) {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte address[8];
  float celcius, fahrenheit;
  
  Serial.print("{Temperature,\n");
  while(data_stream.search(address)) {
    Serial.print("[");
    Serial.write("ROM=");
    for(i=0;i<8;i++) {
      Serial.print(address[i],HEX); 
      if(i!=7)
        Serial.print(":");
    }
    Serial.print(",");
    if(OneWire::crc8(address, 7) == address[7]){
     switch(address[0]) {
       case 0x10:
         Serial.print("Chip=DS18S20");
         type_s = 1;
         break;
       case 0x28:
         Serial.print("Chip=DS18B20");
         type_s = 0;
         break;
       case 0x22:
         Serial.print("Chip=INVALID");
         break;
     }
     Serial.print(",");
     data_stream.reset();
     data_stream.select(address);
     data_stream.write(0x44, 1);
     
     delay(wait_short);
     
     present = data_stream.reset();
     data_stream.select(address);
     data_stream.write(0xBE);
     
     Serial.print("Data=");
     Serial.print(present, HEX);
     for(i=0;i<9;i++) {
       data[i] = data_stream.read();
       Serial.print(data[i],HEX);
       if(i<8)
         Serial.print(":");
     }
     Serial.print(",");
     
     Serial.print("CRC=");
     Serial.print(OneWire::crc8(data,8), HEX);
     
     Serial.print(",");
     
     temp_sensors.setResolution(address, 9);
     Serial.print("Resolution:");
     Serial.print(temp_sensors.getResolution(address), DEC);
     
     Serial.print(",");
     
     print_temperature(address);
          
     Serial.println("],");
   }
    else {
      Serial.print("CRC=INVALID");
      Serial.print("],");   
    }
  }
  
  data_stream.reset_search();
  Serial.print("}");
  Serial.println();
  delay(wait_long);
}

void print_temperature(DeviceAddress device_address) {
 float tempC = temp_sensors.getTempC(device_address);
 Serial.print("Celsius:");
 Serial.print(tempC);
 Serial.print(",");
 Serial.print("Fahrenheit:");
 Serial.print(DallasTemperature::toFahrenheit(tempC));
}
