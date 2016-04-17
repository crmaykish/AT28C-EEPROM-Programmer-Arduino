// AM29F040 Flash Memory Programmer

const int CE = 4;
const int OE = 2;
const int WE = 3;
const int DQ[] = {22, 23, 24, 25, 26, 27, 28, 29};
const int A[] = {30, 31, 32, 33, 34, 35, 36, 37, 38};

void setup() {
  pinMode(OE, OUTPUT);
  pinMode(WE, OUTPUT);
  pinMode(CE, OUTPUT);

  digitalWrite(OE, HIGH);
  digitalWrite(WE, HIGH);
  digitalWrite(CE, HIGH);

  // Set address bus to outputs
  for (int i = 0; i < 8; i++){
    pinMode(A[i], OUTPUT);
  }

  Serial.begin(9600);

  delay(50);
}

void loop() {
  while (Serial.available() > 0){
    int b = Serial.parseInt();
    readByte(b);
    delay(5);
  }
}

void storeValue(int addr, int val){
  writeByte(0x5555, 0xAA);
  writeByte(0x2AAA, 0x55);
  writeByte(0x5555, 0xA0);
  writeByte(addr, val);
}

void writeByte(int addr, int val){
  Serial.print("Write: ");
  Serial.print(val);
  Serial.print(" to ");
  Serial.println(addr);
  
  // Set data to outputs
  for (int i = 0; i < 8; i++){
    pinMode(DQ[i], INPUT);
  }

  // set address
  for (int i = 0; i < 8; i++){
    int a = (addr & (1 << i)) > 0;
    digitalWrite(A[i], a);
  }

  // set data
  if (val < 256) {
    for (int i = 0; i < 8; i++){
      int a = (val & (1 << i)) > 0;
      digitalWrite(DQ[i], a);
    }
  }

  digitalWrite(CE, LOW);
  digitalWrite(WE, LOW);
  delay(10);
  digitalWrite(WE, HIGH);
  digitalWrite(CE, HIGH);
}

void readByte(int addr){
  // Set data pins to inputs
  for (int i = 0; i < 8; i++){
    pinMode(DQ[i], INPUT);
  }

  // Write the addr
  for (int i = 0; i < 8; i++){
    int a = (addr & (1 << i)) > 0;
    digitalWrite(A[i], a);
  }

  digitalWrite(CE, LOW);
  digitalWrite(OE, LOW);
  delay(1);

  // Read data bus
  Serial.print("Read val: ");
  for (int i = 0; i < 8; i++){
    int d = digitalRead(DQ[i]);
    Serial.print(d);
    Serial.print(" ");
  }
  Serial.println("");

  digitalWrite(OE, HIGH);
  digitalWrite(CE, HIGH);
}

