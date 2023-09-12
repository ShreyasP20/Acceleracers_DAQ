'''
        Read Gyro and Accelerometer by Interfacing Raspberry Pi with MPU6050 using Python
	http://www.electronicwings.com
'''
import smbus					#import SMBus module of I2C
from time import sleep          #import

#some MPU6050 Registers and their Address
PWR_MGMT_1   = 0x6B
SMPLRT_DIV   = 0x19
CONFIG       = 0x1A
GYRO_CONFIG  = 0x1B
INT_ENABLE   = 0x38
ACCEL_XOUT_H = 0x3B
ACCEL_YOUT_H = 0x3D
ACCEL_ZOUT_H = 0x3F


def MPU_Init():
	bus.write_byte_data(Device_Address, SMPLRT_DIV, 7)
	
	bus.write_byte_data(Device_Address, PWR_MGMT_1, 1)
	
	bus.write_byte_data(Device_Address, CONFIG, 0)
	
	bus.write_byte_data(Device_Address, GYRO_CONFIG, 24)
	
	bus.write_byte_data(Device_Address, INT_ENABLE, 1)

def read_raw_data(addr):
        high = bus.read_byte_data(Device_Address, addr)
        low = bus.read_byte_data(Device_Address, addr+1)
    
        value = ((high << 8) | low)
        
        if(value > 32768):
                value = value - 65536
        return value


bus = smbus.SMBus(1)
Device_Address = 0x68   

MPU_Init()

print (" Reading Data of Gyroscope and Accelerometer")

while True:
	
	acc_x = read_raw_data(ACCEL_XOUT_H)
	acc_y = read_raw_data(ACCEL_YOUT_H)
	acc_z = read_raw_data(ACCEL_ZOUT_H)
	

	Ax = acc_x/16384.0
	Ay = acc_y/16384.0
	Az = acc_z/16384.0
	
	print ("\tAx=%.2f g" %Ax, "\tAy=%.2f g" %Ay, "\tAz=%.2f g" %Az) 	
	sleep(1)
