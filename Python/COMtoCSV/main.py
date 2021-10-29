import csv
import serial
import time


if __name__ == '__main__':
    serialPort = serial.Serial(
        port="COM5", baudrate=115200, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE
    )
    serialString = ""  # Used to hold data coming over UART
    while 1:
        # Wait until there is data waiting in the serial buffer
        if serialPort.in_waiting > 0:

            # Read data out of the buffer until a carraige return / new line is found
            serialString = serialPort.readline()

            # Print the contents of the serial data
            try:
                print(serialString.decode("Ascii"))
                with open('data.csv', 'a', newline='', encoding='utf-8-sig') as f:
                    writer = csv.writer(f, delimiter=',')
                    writer.writerow(serialString.decode("Ascii").strip().split(','))
            except:
                pass