#include <AFMotor.h>
#include <Servo.h>
#include <NewPing.h>
#include <SoftwareSerial.h>

SoftwareSerial murphy(2, 13);         // RX, TX

AF_DCMotor motorkiridepan(1);
AF_DCMotor motorkanandepan(4);

AF_DCMotor motorkiribelakang(2);
AF_DCMotor motorkananbelakang(3);

Servo myservo;

const int pin_trigger = A1;
const int pin_echo = A0;

NewPing sonar(pin_trigger, pin_echo, 350);

int jarak;
int jarakkiri, jarakkanan;
int arahservo;

String arah;

const unsigned long interval=500;
unsigned long MillisSebelumnya=0;

void kirimjarak(int Jarak) {
  murphy.print(Jarak);
  murphy.print(" CM-");
}

unsigned long printJarak(unsigned long waktu) {
  if(waktu - MillisSebelumnya >= interval) {
    kirimjarak(jarak);
    MillisSebelumnya = waktu;
    return MillisSebelumnya;
  }
}

int bacaping() {
  delay(70);
  unsigned int uS = sonar.ping();
  jarak = uS/US_ROUNDTRIP_CM;
  return jarak;
}

void servotengah() {
  myservo.write(81);
  delay(15);
  arahservo = 81;
  
}

void servokanan() {
  myservo.write(36);
  delay(15);
  arahservo = 36;
}

void servokiri() {
  myservo.write(126);
  delay(15);
  arahservo = 126;
}

void mundur() {
  motorkiridepan.run(BACKWARD);
  motorkanandepan.run(BACKWARD);
  motorkiribelakang.run(BACKWARD);
  motorkananbelakang.run(BACKWARD);

  motorkiridepan.setSpeed(200);
  motorkanandepan.setSpeed(200);
  motorkiribelakang.setSpeed(200);
  motorkananbelakang.setSpeed(200);
}

void maju() {
  motorkiridepan.run(FORWARD);
  motorkanandepan.run(FORWARD);
  motorkiribelakang.run(FORWARD);
  motorkananbelakang.run(FORWARD);

  motorkiridepan.setSpeed(200);
  motorkanandepan.setSpeed(200);
  motorkiribelakang.setSpeed(200);
  motorkananbelakang.setSpeed(200);
}

void belokkiri() {
  motorkiridepan.run(BACKWARD);
  motorkanandepan.run(FORWARD);
  motorkiribelakang.run(BACKWARD);
  motorkananbelakang.run(FORWARD);

  motorkiridepan.setSpeed(150);
  motorkanandepan.setSpeed(200);
  motorkiribelakang.setSpeed(150);
  motorkananbelakang.setSpeed(200);
}

void belokkanan() {
  motorkiridepan.run(FORWARD);
  motorkanandepan.run(BACKWARD);
  motorkiribelakang.run(FORWARD);
  motorkananbelakang.run(BACKWARD);

  motorkiridepan.setSpeed(200);
  motorkanandepan.setSpeed(150);
  motorkiribelakang.setSpeed(200);
  motorkananbelakang.setSpeed(150);
}

void balikarah() {
  motorkiridepan.run(BACKWARD);
  motorkanandepan.run(FORWARD);
  motorkiribelakang.run(BACKWARD);
  motorkananbelakang.run(FORWARD);

  motorkiridepan.setSpeed(200);
  motorkanandepan.setSpeed(200);
  motorkiribelakang.setSpeed(200);
  motorkananbelakang.setSpeed(200);
}

void berhenti() {
  motorkiridepan.run(RELEASE);
  motorkanandepan.run(RELEASE);
  motorkiribelakang.run(RELEASE);
  motorkananbelakang.run(RELEASE);
}

void bandingkan_jarak() {
  if(jarakkiri > jarakkanan) {
    belokkiri();
    delay(315);
  }
  else if(jarakkanan > jarakkiri) {
    belokkanan();
    delay(315);
  }
  else {
    balikarah();
    delay(500);
  }
}

void autoDrive(unsigned long waktu) {
  if(arahservo != 81) {
    servotengah();
  }
  bacaping();
  printJarak(waktu);
  
  if(jarak < 30) {
    berhenti();
    servokanan();
    delay(500);
    jarakkanan = bacaping();
    delay(500);
    kirimjarak(jarakkanan);

    servokiri();
    delay(500);
    jarakkiri = bacaping();
    delay(500);
    kirimjarak(jarakkiri);

    servotengah();
    delay(100);
    bandingkan_jarak();
  }
  else {
    maju();
  }

  if(murphy.available()) {
    arah = "";
  }
}

void setup() {
  // put your setup code here, to run once:
  murphy.begin(9600);
  murphy.setTimeout(10);

  //servo
  myservo.attach(10);

  servokanan();
  delay(500);
  servokiri();
  delay(500);
  servotengah();
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long MillisSekarang = millis();
  
  while(murphy.available() > 1) {
    arah = murphy.readString();
  }

  if(arah == "Auto Drive" || arah == "auto Drive" || arah == "autopilot") {
    autoDrive(MillisSekarang);
  }
  else
  if(arah != "Auto Drive" || arah != "auto Drive" || arah != "autopilot") {
    if(arahservo != 81) {
      servotengah();
    }
    bacaping();
    printJarak(MillisSekarang);
    
    if(jarak < 30) {
      berhenti();
      
      while(arah == "MU") {
        mundur();
        if(murphy.available()) {
          break;
        }
      }

      if(arah == "back" || arah == "mundur") {
        mundur();
        delay(500);
        berhenti();
      }

      while(arah == "KI") {
        servokiri();
        belokkiri();
        if(murphy.available()) {
          break;
        }
      }

      if(arah == "left" || arah == "kiri") {
        servokiri();
        belokkiri();
        delay(400);
        berhenti();
      }

      while(arah == "KA") {
        servokanan();
        belokkanan();
        if(murphy.available()) {
          break;
        }
      }

      if(arah == "right" || arah == "kanan") {
        servokanan();
        belokkanan();
        delay(400);
        berhenti();
      }
      arah = "";
    }
    else
    if(arah != "Auto Drive" || arah != "auto Drive" || arah != "autopilot" && jarak >= 30) {
      if(arah == "forward" || arah == "maju") {
        servotengah();
        maju();
      }

      while(arah == "KI") {
        servokiri();
        belokkiri();
        if(murphy.available()) {
          break;
        }
      }

      if(arah == "left" || arah == "kiri") {
        servokiri();
        belokkiri();
        delay(400);
        berhenti();
      }

      while(arah == "KA") {
        servokanan();
        belokkanan();
        if(murphy.available()) {
          break;
        }
      }

      if(arah == "right" || arah == "kanan") {
        servokanan();
        belokkanan();
        delay(400);
        berhenti();
      }

      if(arah == "MU") {
        mundur();
      }

      if(arah == "back" || arah == "mundur") {
        mundur();
        delay(500);
        berhenti();
      }

      if(arah == "stop" || arah == "berhenti") {
        berhenti();
      }
      arah = "";
    }
  }
}
