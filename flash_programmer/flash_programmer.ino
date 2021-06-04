/*
 AT28C EEPROM Programmer for Arduino Mega
 Colin Maykish (github.com/crmaykish)
 
 04/17/2016 - original release
 06/03/2021 - expanded serial port interface

 Serial interface supports two commands: read and write
 
 Read: Reads a single byte at the 16-bit address.
    Format: RDXXXX where XXXX is a hex number,
      e.g. RD00FF reads the byte at address 0xFF
      
    Output: Write the byte in hex format to the serial port
    
 Write: Writes a byte to the given 16-bit address.
    Format: WRXXXXYY where XXXX is a hex number and YY is a hex byte,
    e.g. WRABCD10 writes 0x10 to 0xABCD
    
    Output: Write 'DONE' to the serial port if the write was successful

 All commands must be terminated with a newline ('\n')

 Write command requires address and data values be padded with leading 0s to their full length.
 Read command does not require leading 0s
*/

#define DELAY_TIME    2   // I don't know why this delay is required, but without it, writes are missed frequently

// Control pins
#define CHIP_ENABLE   7
#define OUTPUT_ENABLE 8
#define WRITE_ENABLE  9

// I/O pins
const int I[] = {46, 47, 48, 49, 50, 51, 52, 53};

// Address pins
const int A[] = {26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40};

// Input buffers
char readAddr[5];
char readData[3];

void setup()
{
	// Define control pins and turn them all off
	pinMode(OUTPUT_ENABLE, OUTPUT);
	pinMode(WRITE_ENABLE, OUTPUT);
	pinMode(CHIP_ENABLE, OUTPUT);
	digitalWrite(OUTPUT_ENABLE, HIGH);
	digitalWrite(WRITE_ENABLE, HIGH);
	digitalWrite(CHIP_ENABLE, HIGH);

	// Set address bus to outputs
	for (int i = 0; i < (sizeof(A) / sizeof(int) ); i++)
	{
		pinMode(A[i], OUTPUT);
	}

	Serial.begin(115200);
}

void loop()
{
	while (Serial.available() > 0)
	{
    String input = Serial.readStringUntil('\n');
    String command = input.substring(0, 2);

    input.substring(2, 6).toCharArray(readAddr, 5);

    uint16_t addr = (uint16_t)strtol(readAddr, NULL, 16);
    
    if (command.equals("RD"))
    {
      Serial.println(readByte(addr), HEX);
    }

    else if (command.equals("WR")){
      input.substring(6, 8).toCharArray(readData, 3);
      byte data = (byte)strtol(readData, NULL, 16);
      writeByte(addr, data);
      Serial.println("DONE");
    }
    else
    {
      Serial.println("Bad input: " + input);
    }
	}
}

// Set data bus to INPUT or OUTPUT
void setDataBusMode(int mode)
{
	if (mode == INPUT || mode == OUTPUT)
	{
		for (int i = 0; i < 8; i++)
		{
			pinMode(I[i], mode);
		}
	}
}

// Write an address to the address bus
void setAddress(int addr)
{
	for (int i = 0; i < (sizeof(A) / sizeof(int) ); i++)
	{
		int a = (addr & (1 << i)) > 0;
		digitalWrite(A[i], a);
	}
}

void writeByte(uint16_t addr, byte val)
{
	digitalWrite(OUTPUT_ENABLE, HIGH);
	setDataBusMode(OUTPUT);
	setAddress(addr);

	// Send data value to data bus
	for (int i = 0; i < 8; i++)
	{
		int a = (val & (1 << i)) > 0;
		digitalWrite(I[i], a);
	}

	// Commit data write
	digitalWrite(CHIP_ENABLE, LOW);
	delay(DELAY_TIME);
	digitalWrite(WRITE_ENABLE, LOW);
	delay(DELAY_TIME);
	digitalWrite(WRITE_ENABLE, HIGH);
	delay(DELAY_TIME);
	digitalWrite(CHIP_ENABLE, HIGH);
	delay(DELAY_TIME);
}

byte readByte(uint16_t addr)
{
	byte data = 0;

	setDataBusMode(INPUT);

	// Write the addr
	for (int i = 0; i < (sizeof(A) / sizeof(int) ); i++)
	{
		int a = (addr & (1 << i)) > 0;
		digitalWrite(A[i], a);
	}

	digitalWrite(CHIP_ENABLE, LOW);
	digitalWrite(OUTPUT_ENABLE, LOW);
	delay(DELAY_TIME);

	// Read data bus
	for (int i = 0; i < 8; i++)
	{
		int d = digitalRead(I[i]);
		data += (d << i);
	}

	digitalWrite(OUTPUT_ENABLE, HIGH);
	digitalWrite(CHIP_ENABLE, HIGH);
	delay(DELAY_TIME);

	return data;
}
