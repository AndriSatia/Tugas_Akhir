from django.shortcuts import render
from django.http import JsonResponse
import asyncio
import keyboard
from bleak import BleakScanner, BleakClient

async def scan_and_pair(target_name):
    counter = 0  # Counter untuk menyimpan jumlah data "True" yang diterima
    scanner = BleakScanner()
    devices = await scanner.discover()
    target_device = None
    for device in devices:
        if device.name == target_name:
            target_device = device
            break
    
    if target_device:
        async with BleakClient(target_device) as client:
            while not keyboard.is_pressed('q'):  # Terus membaca data sampai tombol 'q' ditekan
                value = await client.read_gatt_char("beb5483e-36e1-4688-b7f5-ea07361b26a8")
                if value:
                    received_data = value.decode()
                    print("Received:", received_data)
                    if received_data == "True":
                        counter += 1  # Menambah counter jika data yang diterima adalah "True"
                        print("Counter: ", counter)
                await asyncio.sleep(0.1)  # Menghindari overloading CPU
    else:
        print(f"Device {target_name} not found.")

    return counter

def home(request):
    return render(request, 'home.html')

def get_counter(request):
    counter = asyncio.run(scan_and_pair("Server"))  # Memanggil fungsi scan_and_pair untuk mendapatkan nilai counter
    return JsonResponse({'counter': counter})
