import serial
import tkinter as tk
from threading import Thread
import time

# =========================
# SET SERIAL PORT
# =========================
PORT = "COM8"
BAUD = 115200

try:
    ser = serial.Serial(PORT, BAUD, timeout=1)
    print("Serial Connected :", PORT)
except:
    print("ERROR: Serial tidak bisa dibuka")
    exit()


# =========================
# FUNGSI KONTROL LED
# =========================

def led1_off():
    ser.write(b'1')

def led1_on():
    ser.write(b'a')

def led2_off():
    ser.write(b'2')

def led2_on():
    ser.write(b'b')

def led3_off():
    ser.write(b'3')

def led3_on():
    ser.write(b'c')


# ALL CONTROL
def all_on():
    ser.write(b'o')   # kirim karakter o

def all_off():
    ser.write(b'f')   # kirim karakter f


# =========================
# MEMBACA DATA SERIAL
# =========================

def read_serial():
    while True:
        try:
            if ser.in_waiting > 0:

                data = ser.readline().decode('utf-8', errors='ignore').strip()

                print("DATA:", data)  # DEBUG supaya terlihat di terminal

                if data == "PB1":
                    status_label.config(text="Push Button 1 DITEKAN")

                elif data == "PB2":
                    status_label.config(text="Push Button 2 DITEKAN")

        except:
            pass

        time.sleep(0.1)


# =========================
# USER INTERFACE
# =========================

root = tk.Tk()
root.title("STM32 LED Control Panel")
root.geometry("300x420")

title = tk.Label(root,text="CONTROL LED STM32",font=("Arial",14))
title.pack(pady=10)

# LED1
tk.Button(root,text="LED1 ON",width=15,command=led1_on).pack(pady=2)
tk.Button(root,text="LED1 OFF",width=15,command=led1_off).pack(pady=2)

# LED2
tk.Button(root,text="LED2 ON",width=15,command=led2_on).pack(pady=2)
tk.Button(root,text="LED2 OFF",width=15,command=led2_off).pack(pady=2)

# LED3
tk.Button(root,text="LED3 ON",width=15,command=led3_on).pack(pady=2)
tk.Button(root,text="LED3 OFF",width=15,command=led3_off).pack(pady=2)


# ALL CONTROL
tk.Button(root,text="ALL ON",width=20,bg="green",fg="white",command=all_on).pack(pady=6)
tk.Button(root,text="ALL OFF",width=20,bg="red",fg="white",command=all_off).pack(pady=6)


# Status Push Button
status_label = tk.Label(root,text="Status Push Button : -",font=("Arial",12))
status_label.pack(pady=20)


# Thread membaca serial
thread = Thread(target=read_serial)
thread.daemon = True
thread.start()

root.mainloop()
