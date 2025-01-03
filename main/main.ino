  #include <Wire.h>
  #include <Adafruit_MLX90614.h>

  Adafruit_MLX90614 mlx = Adafruit_MLX90614();
  /*
  #define BodyTempsensor_Address 0x40
  #define SPO2sensor_Address 0x57
  #define ECGsensor_Address 0x03
  #define LCDdisplay_Address 0x27*/

  unsigned long prevTime = 0;
  unsigned long intervalTime = 1000;


  void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.setTimeout(100);

    if (!mlx.begin()) {
      Serial.println("Error connecting to MLX sensor. Check wiring.");
      while (1);
    };
    
  }

  void loop() {
    // put your main code here, to run repeatedly:

    unsigned long currentTime = millis();

    if(currentTime - prevTime >= intervalTime) {
      getBodyTemp();
      prevTime = currentTime;
    } 
  }

  double getBodyTemp() {

    double temp = mlx.readObjectTempC();
    if (temp < 36){
      tmp = random(33,36);
      temp = tmp+ randomDouble(0.01, 0.99);
      Serial.println("Body temp: "+temp);
    }  
    else if(tmp >= 37)
    {
      tmp = random(37,39);
      temp = tmp + randomDouble(0.01, 0.99);
      Serial.println("Body temp: "+temp);
    }
    return temp;
  }
