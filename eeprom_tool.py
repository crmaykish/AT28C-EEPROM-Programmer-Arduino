# Python CLI for Parallel EEPROM Programmer
# Colin Maykish
# May 2021

import argparse
import serial
import time


def main():
    parser = argparse.ArgumentParser(description="Parallel EEPROM Programmer")

    parser.add_argument("-d", "--device", action="store", type=str, nargs=1)
    parser.add_argument("-r", "--read", action="store_true")
    parser.add_argument("-w", "--write", action="store_true")
    parser.add_argument("-e", "--erase", action="store_true")
    parser.add_argument("-f", "--file", action="store", type=str, nargs=1)

    args = parser.parse_args()

    # Open serial port
    ser = serial.Serial(args.device[0], 115200)

    time.sleep(1)

    if not ser.is_open:
        print("Failed to open " + ser.name)
        exit(1)

    ser.flushInput()

    print("Connected to " + ser.name + " at " + str(ser.baudrate))

    addr = 0

    # Open binary file
    with open(args.file[0], mode='rb') as file:
        contents = file.read()

        if args.write:
            for b in contents:
                command = "WR" + str(addr).zfill(5) + str(b).zfill(3) + '\n'
                b = command.encode()
                print("Writing: " + command)
                ser.write(b)
                addr += 1

                # Wait for response
                response = ser.readline().decode().strip()

                if response != "DONE":
                    print(response)

    ser.close()
    print("Closed " + ser.name)


main()
