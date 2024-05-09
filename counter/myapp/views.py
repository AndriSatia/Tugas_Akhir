from django.shortcuts import render
from django.http import JsonResponse
import asyncio
import keyboard
from bleak import BleakClient

counter = 0
client = None  # Variabel untuk menyimpan koneksi BleakClient

async def connect_and_receive():
    global client
    if client is None:  # Buat koneksi jika belum ada
        client = BleakClient("E8:6B:EA:DF:B7:5A")
        await client.connect()
    value = await client.read_gatt_char("beb5483e-36e1-4688-b7f5-ea07361b26a8")
    if value:
        received_data = value.decode()
        if received_data == "True":
            return 1
    return 0

def home(request):
    return render(request, 'home.html')

def get_counter(request):
    global counter
    data = asyncio.run(connect_and_receive())  # Panggil fungsi connect_and_receive
    if data == 1:
        counter += 1
    return JsonResponse({'counter': counter})
