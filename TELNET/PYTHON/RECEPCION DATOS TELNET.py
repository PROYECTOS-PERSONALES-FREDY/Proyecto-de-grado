#https://www.youtube.com/watch?v=cgKK-rBm8VE&ab_channel=TheHouseofRouting


import getpass
import telnetlib
import serial


#Variable para getionar la data de la conexion
equipo = "192.168.89.237"
conexion = telnetlib.Telnet(equipo,23)

print(conexion.read_all().decode('ascii'))

user = input("Please enter your remote account: ")
password = getpass.getpass()
conexion.read_until(b"login: ")
conexion.write(user.encode('ascii') + b"\n")
if password:
    conexion.read_until(b"Password: ")
    conexion.write(password.encode('ascii') + b"\n")

conexion.write(b"ls\n")
conexion.write(b"exit\n")
print(conexion.read_all().decode('ascii'))

