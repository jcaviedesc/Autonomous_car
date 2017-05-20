//#include <Ultrasonic.h>
#include <Servo.h>
/* arduino atmega puente-h
     6       12      10
     7       13      15
     9       15      7
     10      16      2*/
#define MOTOR_1_FORWARD 6
#define MOTOR_1_BACK 7
#define MOTOR_2_FORWARD 9
#define MOTOR_2_BACK 10
//frente
#define TRIG 3
#define ECHO 2
//suelo
#define TRIG_S 11
#define ECHO_S 12
// el servo
#define HEAD_SERVO 5

#define OBSTRACLE_DISTANCE 17.0
#define SUELO_DISTANCE 18.0
#define TURN_DELAY 200


#define LOG false

//Ultrasonic ultrasonic(TRIG, ECHO);
Servo headServo;

int servoAngle = 90;
int angleStep = 30;

float distance = 0;

void setup()
{
  pinMode(MOTOR_1_FORWARD, OUTPUT);
  pinMode(MOTOR_1_BACK,    OUTPUT);
  pinMode(MOTOR_2_FORWARD, OUTPUT);
  pinMode(MOTOR_2_BACK,    OUTPUT);
  
  stopMove();

  headServo.attach(HEAD_SERVO);
  pinMode(13,OUTPUT);
  if (LOG) Serial.begin( 9600 );
}

void loop()
{
  updateHeadAngle();

  checkDistance(TRIG_S, ECHO_S);

  moove();

  delay(10);
}

void checkDistance(char trig, char pecho)
{ digitalWrite(13, HIGH);
  distance =  calcula_distance(trig, pecho);
  if (LOG) Serial.println(distance);
  digitalWrite(13,LOW);
}

void moove()
{ if (distance < SUELO_DISTANCE) {
    checkDistance(TRIG, ECHO);
    if ( distance > OBSTRACLE_DISTANCE )
    {
      if (LOG) Serial.println("FORWARD");

      goForward();
      delay(TURN_DELAY);
    } else
    {
      stopMove();
      checkObstracle();
    }
  } else {
    stopMove();
    checkObstracle();
  }
}

void checkObstracle()
{
  int obsLeft  = 0;
  int obsRight = 0;

  // Count the obstacles from left and right side
  for (servoAngle = 0; servoAngle <= 180; servoAngle += 30)
  {
    headServo.write(servoAngle);
    delay(TURN_DELAY);

    checkDistance(TRIG,ECHO);
    if (distance < OBSTRACLE_DISTANCE && servoAngle < 90) obsLeft++;
    else if (distance < OBSTRACLE_DISTANCE) obsRight++;
  }

  if (LOG) Serial.print("TURN");

  if (obsLeft && obsRight)
  {
    goBack();

    delay(TURN_DELAY * 2);

    if (obsLeft < obsRight) goLeft();
    else goRight();

    delay(TURN_DELAY);
  }
  else if (obsRight)
  {
    goLeft();

    delay(TURN_DELAY);
  }
  else if (obsLeft)
  {
    goRight();

    delay(TURN_DELAY);
  }
  else
  {
    goForward();

    delay(TURN_DELAY);
  }
}

void updateHeadAngle()
{
  headServo.write(servoAngle);

  servoAngle += angleStep;

  if (servoAngle >= 150)
  {
    servoAngle = 150;

    angleStep *= -1;
  }

  if (servoAngle <= 30)
  {
    servoAngle = 30;

    angleStep *= -1;
  }
}

void goForward()
{
  digitalWrite(MOTOR_1_FORWARD, HIGH);
  digitalWrite(MOTOR_1_BACK,    LOW);
  digitalWrite(MOTOR_2_FORWARD, HIGH);
  digitalWrite(MOTOR_2_BACK,    LOW);
}

void goBack()
{
  digitalWrite(MOTOR_1_FORWARD, LOW);
  digitalWrite(MOTOR_1_BACK,    HIGH);
  digitalWrite(MOTOR_2_FORWARD, LOW);
  digitalWrite(MOTOR_2_BACK,    HIGH);
}

void goLeft()
{
  digitalWrite(MOTOR_1_FORWARD, LOW);
  digitalWrite(MOTOR_1_BACK,    HIGH);
  digitalWrite(MOTOR_2_FORWARD, HIGH);
  digitalWrite(MOTOR_2_BACK,    LOW);
}

void goRight()
{
  digitalWrite(MOTOR_1_FORWARD, HIGH);
  digitalWrite(MOTOR_1_BACK,    LOW);
  digitalWrite(MOTOR_2_FORWARD, LOW);
  digitalWrite(MOTOR_2_BACK,    HIGH);
}

void stopMove()
{
  digitalWrite(MOTOR_1_FORWARD, LOW);
  digitalWrite(MOTOR_1_BACK,    HIGH);
  digitalWrite(MOTOR_2_FORWARD, LOW);
  digitalWrite(MOTOR_2_BACK,    HIGH);
  delay(100);
  digitalWrite(MOTOR_1_BACK, LOW);
  digitalWrite(MOTOR_2_BACK, LOW);
}
float calcula_distance(char ptrig, char pecho) {
  float duration;
  digitalWrite(ptrig, HIGH); // genera el pulso de trigger por 10us
  delay(0.02);
  digitalWrite(ptrig, LOW);
  duration = pulseIn(pecho, HIGH); // Lee el tiempo del Echo
  return (duration / 2) / 29; // calcula la distancia en centimetros
}