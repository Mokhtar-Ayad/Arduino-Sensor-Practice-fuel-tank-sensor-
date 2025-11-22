// --- SENSOR & LED PINS ---
const int trigPin = 9;  // HC-SR04 Trigger pin
const int echoPin = 10; // HC-SR04 Echo pin

int ledgreen = 4; // Green: Fuel OK
int ledred = 8;   // Red: Low fuel warning

// --- TANK CALIBRATION (MUST BE ACCURATE) ---
const int TANK_HEIGHT_CM = 20; // Total tank depth
const int SENSOR_OFFSET_CM = 2; // Sensor blind spot / offset

const int MAX_USABLE_HEIGHT = TANK_HEIGHT_CM - SENSOR_OFFSET_CM; // 100% capacity height

double low_fuel_threshold_percent = 30.0; // Warning threshold

void setup() {
  // Pin initialization
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledgreen, OUTPUT);
  pinMode(ledred, OUTPUT);
  
  Serial.begin(9600); // Start serial monitor
}

// Measures distance (cm) from sensor to liquid surface
long getDistanceCm() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  // Time-to-distance conversion formula
  long distanceCm = duration * 0.034 / 2;
  
  return distanceCm;
}

void loop() {
  // 1. Get raw distance
  long distanceCm = getDistanceCm();
  
  // 2. Calculate liquid height: (Total Height) - (Space Above Liquid)
  int liquid_height = TANK_HEIGHT_CM - distanceCm;
  
  // 3. Constrain height to valid range (0 to 100%)
  liquid_height = constrain(liquid_height, 0, MAX_USABLE_HEIGHT);
  
  // 4. Convert height to percentage
  double actual_fuel_tank_percent = ((double)liquid_height / MAX_USABLE_HEIGHT) * 100.0;
  
  // --- DEBUG OUTPUT ---
  Serial.print("Level: ");
  Serial.print(actual_fuel_tank_percent);
  Serial.println(" %");

  // --- LED LOGIC ---
  if (actual_fuel_tank_percent <= low_fuel_threshold_percent) {
    // LOW FUEL: Flash Red warning
    digitalWrite(ledgreen, LOW);  
    digitalWrite(ledred, LOW);
    delay(60);
    digitalWrite(ledred, HIGH);
    delay(60);
    
  } else {
    // FUEL OK: Green ON, Red OFF
    digitalWrite(ledred, LOW);    
    digitalWrite(ledgreen, HIGH);
    delay(500); // Reduce loop speed when level is adequate
  }
}