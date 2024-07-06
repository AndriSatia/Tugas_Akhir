import os
import pandas as pd
from django.http import JsonResponse
from django.shortcuts import render
from django.views.decorators.csrf import csrf_exempt
from django.conf import settings
import asyncio
from bleak import BleakClient
from asgiref.sync import sync_to_async

# Variabel global
client = None  # Variabel untuk menyimpan koneksi BleakClient
loop = asyncio.get_event_loop()

counter = 0  # Counter untuk menyimpan jumlah data "True" yang diterima
wrong = 0

async def connect_ble():
    global client
    if client is None:
        client = BleakClient("D4:8A:FC:60:81:2E")
        await client.connect()
    return client.is_connected

async def receive_data():
    global client, counter, wrong
    if client is not None and client.is_connected:
        value = await client.read_gatt_char("1d5616fb-de0a-4354-b680-d291333dc25a")
        if value:
            received_data = value.decode()
            if received_data == "1":
                # Mengirim feedback ke ESP32 melalui karakteristik RX
                feedback_value = "1"
                feedback_bytes = feedback_value.encode('utf-8')
                await client.write_gatt_char("1d5616fb-de0a-4354-b680-d291333dc25a", feedback_bytes)
                counter += 1
                print('Counter: ', counter)
                return 1
            elif received_data == "2":
                # Mengirim feedback ke ESP32 melalui karakteristik RX
                feedback_value = "1"
                feedback_bytes = feedback_value.encode('utf-8')
                await client.write_gatt_char("1d5616fb-de0a-4354-b680-d291333dc25a", feedback_bytes)
                wrong += 1
                print('Received: False')
                print('Wrong: ', wrong)
                return 2
    return 0

async def reset_esp32():
    global client
    if client is not None and client.is_connected:
        reset_value = "reset"
        reset_bytes = reset_value.encode('utf-8')
        await client.write_gatt_char("1d5616fb-de0a-4354-b680-d291333dc25a", reset_bytes)
        return True
    return False

# Fungsi sinkron untuk menghubungkan BLE
@sync_to_async
def sync_connect_ble():
    return asyncio.run(connect_ble())

# Fungsi sinkron untuk menerima data dari BLE
@sync_to_async
def sync_receive_data():
    return asyncio.run(receive_data())

# Fungsi sinkron untuk mereset ESP32 Client
@sync_to_async
def sync_reset_esp32():
    return asyncio.run(reset_esp32())

def push_count(request):
    return render(request, 'push_up_count.html')

def sit_count(request):
    return render(request, 'sit_up_count.html')

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

@csrf_exempt
def reset_esp32_view(request):
    if request.method == 'POST':
        success = loop.run_until_complete(sync_reset_esp32())
        return JsonResponse({'status': 'success' if success else 'error', 'message': 'ESP32 reset successfully' if success else 'Failed to reset ESP32'})
    return JsonResponse({'status': 'error', 'message': 'Invalid request method.'})

# Bagian baca CSV
# Fungsi untuk membaca isi file CSV
def read_csv(file_path=os.path.join(settings.BASE_DIR, 'data/history.csv')):
    if os.path.isfile(file_path):
        df = pd.read_csv(file_path)
        return df
    else:
        return pd.DataFrame(columns=['Nama', 'Mode', 'Gerakan Benar', 'Gerakan Salah', 'Waktu'])

# Fungsi untuk menambahkan data ke file CSV
@csrf_exempt
def add_data_to_csv(request):
    if request.method == 'POST':
        name = request.POST.get('name')
        mode = request.POST.get('mode')  # Tambahkan pengambilan mode
        correct_moves = request.POST.get('correct_moves')
        incorrect_moves = request.POST.get('incorrect_moves')
        time = request.POST.get('time')
        
        new_data = {
            'Nama': [name],
            'Mode': [mode],  # Tambahkan mode ke data baru
            'Gerakan Benar': [correct_moves],
            'Gerakan Salah': [incorrect_moves],
            'Waktu': [time]
        }
        
        new_df = pd.DataFrame(new_data)
        
        file_path = os.path.join(settings.BASE_DIR, 'data/history.csv')
        if os.path.isfile(file_path):
            new_df.to_csv(file_path, mode='a', header=False, index=False)
        else:
            new_df.to_csv(file_path, mode='w', header=True, index=False)
        
        return JsonResponse({'status': 'success', 'message': 'Data berhasil ditambahkan.'})
    return JsonResponse({'status': 'error', 'message': 'Invalid request method.'})

# Fungsi untuk menghapus semua data di file CSV tetapi menjaga headernya
@csrf_exempt
def clear_csv(request):
    if request.method == 'POST':
        file_path = os.path.join(settings.BASE_DIR, 'data/history.csv')
        if os.path.isfile(file_path):
            df = pd.read_csv(file_path)
            header = df.columns.tolist()
            pd.DataFrame(columns=header).to_csv(file_path, mode='w', header=True, index=False)
            return JsonResponse({'status': 'success', 'message': 'Semua data telah dihapus kecuali header.'})
        return JsonResponse({'status': 'error', 'message': 'File tidak ditemukan.'})
    return JsonResponse({'status': 'error', 'message': 'Invalid request method.'})

def history(request):
    return render(request, 'history.html')

def data_csv(request):
    df = read_csv()
    data = df.to_dict('records')  # Mengonversi DataFrame menjadi daftar objek Python
    return JsonResponse({'data': data})
