// AT28C Flash Memory Programmer

const int CE = 2;
const int OE = 3;
const int WE = 4;
const int I[] = {22, 23, 24, 25, 26, 27, 28, 29};
const int A[] = {30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42};

void setup() {
  pinMode(OE, OUTPUT);
  pinMode(WE, OUTPUT);
  pinMode(CE, OUTPUT);

  digitalWrite(OE, HIGH);
  digitalWrite(WE, HIGH);
  digitalWrite(CE, HIGH);

  // Set address bus to outputs
  for (int i = 0; i < 13; i++){
    pinMode(A[i], OUTPUT);
  }

  Serial.begin(9600);

  delay(50);

  writeByte(0, 15);
  writeByte(1, 23);
  delay(100);
  
}

void loop() {
  while (Serial.available() > 0){
    int b = Serial.parseInt();
    readByte(b);
    delay(10);
  }
}

void writeByte(int addr, int val){
  Serial.print("Write: ");
  Serial.print(val);
  Serial.print(" to ");
  Serial.println(addr);

  // Disable output
  digitalWrite(OE, HIGH);
  delay(50);
  
  // Set data to outputs
  for (int i = 0; i < 8; i++){
    pinMode(I[i], OUTPUT);
  }

  // set address
  for (int i = 0; i < 13; i++){
    int a = (addr & (1 << i)) > 0;
    digitalWrite(A[i], a);
  }

  // set data
  if (val < 256) {
    for (int i = 0; i < 8; i++){
      int a = (val & (1 << i)) > 0;
      digitalWrite(I[i], a);
    }
  }

  digitalWrite(CE, LOW);
  delay(10);
  digitalWrite(WE, LOW);
  delay(10);
  digitalWrite(WE, HIGH);
  delay(10);
  digitalWrite(CE, HIGH);
  delay(10);
}

void readByte(int addr){
  // Set data pins to inputs
  for (int i = 0; i < 8; i++){
    pinMode(I[i], INPUT);
  }

  // Write the addr
  for (int i = 0; i < 13; i++){
    int a = (addr & (1 << i)) > 0;
    digitalWrite(A[i], a);
  }

  digitalWrite(CE, LOW);
  digitalWrite(OE, LOW);
  delay(10);

  // Read data bus
  Serial.print("Read val: ");
  for (int i = 0; i < 8; i++){
    int d = digitalRead(I[i]);
    Serial.print(d);
    Serial.print(" ");
  }
  Serial.println("");

  digitalWrite(OE, HIGH);
  digitalWrite(CE, HIGH);
  delay(10);
}

