from django.shortcuts import render, redirect
from django.http import JsonResponse
import asyncio
from bleak import BleakClient
from asgiref.sync import sync_to_async

# Variabel global
client = None  # Variabel untuk menyimpan koneksi BleakClient
loop = asyncio.get_event_loop()

async def connect_ble():
    global client
    if client is None:
        client = BleakClient("D4:8A:FC:60:81:2E")
        await client.connect()
    return client.is_connected

async def receive_data():
    global client
    if client is not None and client.is_connected:
        value = await client.read_gatt_char("beb5483e-36e1-4688-b7f5-ea07361b26a8")
        if value:
            received_data = value.decode()
            if received_data == "1":
                # Mengirim feedback ke ESP32 melalui karakteristik RX
                feedback_value = "1"  # Nilai string yang akan dikirim
                feedback_bytes = feedback_value.encode('utf-8')  # Konversi string ke byte array
                await client.write_gatt_char("1d5616fb-de0a-4354-b680-d291333dc25a", feedback_bytes)
                return 1
    return 0

# Fungsi sinkron untuk menghubungkan BLE
@sync_to_async
def sync_connect_ble():
    return asyncio.run(connect_ble())

# Fungsi sinkron untuk menerima data dari BLE
@sync_to_async
def sync_receive_data():
    return asyncio.run(receive_data())

def home(request):
    return render(request, 'home.html')

def mode(request):
    return render(request, 'mode.html')

def connect_ble_view(request):
    if request.method == 'GET' and 'start' not in request.GET:
        return render(request, 'connect_ble.html')
    
    connected = loop.run_until_complete(sync_connect_ble())
    connected = bool(connected)  # Pastikan nilai dikonversi menjadi boolean
    request.session['ble_connected'] = connected
    return JsonResponse({'connected': connected})  # Tampilkan status koneksi sebagai JSON

def get_counter(request):
    data = loop.run_until_complete(sync_receive_data())
    return JsonResponse({'data': data})
