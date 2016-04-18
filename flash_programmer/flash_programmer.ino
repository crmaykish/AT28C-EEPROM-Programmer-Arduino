// AT28C EEPROM Programmer for Arduino Mega
// Colin Maykish (cmaykish.com)
// 04/17/2016

// Width in bits of address bus (64K has 13, 256K has 15)
const int ADDR_BUS_WIDTH = 13;

// Chip Enable
const int CE = 2;
// Ouput Enable
const int OE = 3;
// Write Enable
const int WE = 4;
// Data bus pins
const int I[] = { 22, 23, 24, 25, 26, 27, 28, 29 };
// Address bus pins
const int A[] = { 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42 };

// Byte array to write to EEPROM
const byte HEX_FILE[] = { 0x0, 0x0, 0x0, 0x0, 0x0 };

void setup() {
	Serial.begin(9600);

	// Define control pins and turn them all off
	pinMode(OE, OUTPUT);
	pinMode(WE, OUTPUT);
	pinMode(CE, OUTPUT);
	digitalWrite(OE, HIGH);
	digitalWrite(WE, HIGH);
	digitalWrite(CE, HIGH);

	// Set address bus to outputs
	for (int i = 0; i < ADDR_BUS_WIDTH; i++) {
		pinMode(A[i], OUTPUT);
	}

	// Write hex file to EEPROM
	for (int i = 0; i < sizeof(HEX_FILE); i++) {
		writeByte(i, HEX_FILE[i]);
	}
}

void loop() {
	while (Serial.available() > 0) {
		int b = Serial.parseInt();
		Serial.println(readByte(b));
	}
}

// Set data bus to INPUT or OUTPUT
void setDataBusMode(int mode) {
	if (mode == INPUT || mode == OUTPUT) {
		for (int i = 0; i < 8; i++) {
			pinMode(I[i], mode);
		}
	}
}

// Write an address to the address bus
void setAddress(int addr) {
	for (int i = 0; i < ADDR_BUS_WIDTH; i++) {
		int a = (addr & (1 << i)) > 0;
		digitalWrite(A[i], a);
	}
}

void writeByte(int addr, byte val) {
	digitalWrite(OE, HIGH);
	setDataBusMode(OUTPUT);
	setAddress(addr);

	// Send data value to data bus
	for (int i = 0; i < 8; i++) {
		int a = (val & (1 << i)) > 0;
		digitalWrite(I[i], a);
	}

	// Commit data write
	digitalWrite(CE, LOW);
	delay(10);
	digitalWrite(WE, LOW);
	delay(10);
	digitalWrite(WE, HIGH);
	delay(10);
	digitalWrite(CE, HIGH);
	delay(10);
}

byte readByte(int addr) {
	byte data = 0;

	setDataBusMode(INPUT);

	// Write the addr
	for (int i = 0; i < ADDR_BUS_WIDTH; i++) {
		int a = (addr & (1 << i)) > 0;
		digitalWrite(A[i], a);
	}

	digitalWrite(CE, LOW);
	digitalWrite(OE, LOW);
	delay(10);

	// Read data bus
	for (int i = 0; i < 8; i++) {
		int d = digitalRead(I[i]);
		data += (d << i);
	}

	digitalWrite(OE, HIGH);
	digitalWrite(CE, HIGH);
	delay(10);

	return data;
}
