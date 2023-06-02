const int enPin = 8;
const int stepXPin = 2; // X.STEP
const int dirXPin = 5;  // X.DIR
const int stepYPin = 3; // Y.STEP
const int dirYPin = 6;  // Y.DIR
const int stepZPin = 4; // Z.STEP
const int dirZPin = 7;  // Z.DIR

int stepPin = stepYPin;
int dirPin = dirYPin;

const int stepsPerRev = 800;
int pulseWidthMicros = 100; // microseconds
int millisBtwnSteps = 250;

float distanceToMove = 15.0;  // Distancia a recorrer en cm
float stepsPerCm = stepsPerRev;  // Pasos necesarios por cm

void setup() {
  Serial.begin(9600);
  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, LOW);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  Serial.println(F("CNC Shield Initialized"));
}

void loop() {
  Serial.println(F("Running clockwise"));
  digitalWrite(dirPin, HIGH); // Enables the motor to move in a particular direction

  // Calcula la cantidad de pasos necesarios para mover la distancia deseada
  int stepsToMove = distanceToMove * stepsPerCm;
 
  // Realiza los pasos necesarios para mover la distancia
  for (int i = 0; i < stepsToMove; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(pulseWidthMicros);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(millisBtwnSteps);
  }
  delay(1000); // Un segundo de retraso

  Serial.println(F("Running counter-clockwise"));
  digitalWrite(dirPin, LOW); // Cambia la dirección de rotación

  // Realiza los pasos necesarios para mover la distancia en sentido contrario
  for (int i = 0; i < stepsToMove; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(pulseWidthMicros);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(millisBtwnSteps);
  }
  delay(1000);
}