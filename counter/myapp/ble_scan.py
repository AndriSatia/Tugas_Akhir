import asyncio
from bleak import BleakScanner, BleakClient

async def scan_and_count(target_name):
    counter = 0

    scanner = BleakScanner()
    devices = await scanner.discover()
    target_device = next((device for device in devices if device.name == target_name), None)
    
    if target_device:
        async with BleakClient(target_device) as client:
            while True:
                value = await client.read_gatt_char("beb5483e-36e1-4688-b7f5-ea07361b26a8")
                if value and value.decode() == "True":
                    counter += 1
                    print(counter)
                await asyncio.sleep(0.1)
    else:
        print(f"Device {target_name} not found.")

async def run_ble_scan(target_name):
    while True:
        await scan_and_count(target_name)

def get_counter_value():
    return asyncio.run(scan_and_count("Server"))
