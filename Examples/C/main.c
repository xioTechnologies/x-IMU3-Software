// This file was generated by Examples/C/generate_main.py

#include "Helpers.h"
#include <stdio.h>

void BluetoothConnection();

void Commands();

void DataLogger();

void FileConverter();

void GetPortNames();

void NetworkAnnouncement();

void Ping();

void PortScanner();

void SerialConnection();

void TcpConnection();

void UdpConnection();

void UsbConnection();

int main(int argc, const char* argv[])
{
    printf("Select example\n");
    printf("A. BluetoothConnection.c\n");
    printf("B. Commands.c\n");
    printf("C. DataLogger.c\n");
    printf("D. FileConverter.c\n");
    printf("E. GetPortNames.c\n");
    printf("F. NetworkAnnouncement.c\n");
    printf("G. Ping.c\n");
    printf("H. PortScanner.c\n");
    printf("I. SerialConnection.c\n");
    printf("J. TcpConnection.c\n");
    printf("K. UdpConnection.c\n");
    printf("L. UsbConnection.c\n");
    switch (GetKey())
    {
        case 'A':
            BluetoothConnection();
            break;
        case 'B':
            Commands();
            break;
        case 'C':
            DataLogger();
            break;
        case 'D':
            FileConverter();
            break;
        case 'E':
            GetPortNames();
            break;
        case 'F':
            NetworkAnnouncement();
            break;
        case 'G':
            Ping();
            break;
        case 'H':
            PortScanner();
            break;
        case 'I':
            SerialConnection();
            break;
        case 'J':
            TcpConnection();
            break;
        case 'K':
            UdpConnection();
            break;
        case 'L':
            UsbConnection();
            break;
    }
}
