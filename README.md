# Sistem Counter Push-up dan Sit-up

Proyek ini adalah **Sistem Counter Push-up dan Sit-up** yang terhubung ke server ESP32 melalui Bluetooth Low Energy (BLE). Sistem ini menghitung gerakan push-up dan sit-up yang benar dan salah, memberikan umpan balik secara real-time, serta menyimpan data ke dalam file CSV untuk pelacakan riwayat.

## Fitur
- Terhubung ke server ESP32 melalui BLE.
- Menghitung gerakan push-up/sit-up yang benar dan salah.
- Mengirimkan umpan balik ke server ESP32 berdasarkan gerakan.
- Menyimpan hasil (nama, mode, gerakan benar/salah, waktu) ke dalam file CSV.
- Mereset server ESP32 dan menghapus data CSV.

## Instalasi

### Prasyarat
Sebelum menjalankan proyek ini, pastikan Anda telah menginstal:
- Python 3.x
- Django
- Bleak
- Pandas
- bleak
- asgiref

### Langkah-langkah Instalasi
1. **Kloning repository**:  
   git clone https://github.com/AndriSatia/Tugas_Akhir  
   cd direktori-repo-anda

2. **Buat virtual environment (Jika belum pernah membuat)**:  
    python -m venv env  
    source env/bin/activate   # Di Windows: env\Scripts\activate

3. **Instal dependensi: Gunakan perintah berikut untuk menginstal paket yang dibutuhkan**:
    - `pip install django`
    - `pip install pandas`
    - `pip install asgiref`
    - `pip install bleak`
    - Jika terjadi error saat menginstal bleak gunakan kode ini: `pip install https://github.com/hbldh/bleak/archive/refs/heads/develop.zip`

4. **Siapkan proyek Django**:  
    - Pindah ke folder di mana proyek Django Anda berada.  
    - Migrasikan database (jika diperlukan):  
        `python manage.py migrate`  
    Pastikan server ESP32 sedang berjalan dengan pengaturan Bluetooth dan UUID yang benar.

## Penggunaan
1. **Jalankan server Django**:  
    python manage.py runserver

2. **Buka browser dan akses http://localhost:8000 untuk melihat antarmuka web.**

3. **Halaman**:  
    - Connect BLE: Menghubungkan ke server ESP32 melalui Bluetooth.
    - Push-up Counter: Mulai menghitung gerakan push-up.
    - Sit-up Counter: Mulai menghitung gerakan sit-up.
    - History: Melihat data yang disimpan di file CSV.

4. **Pengelolaan Data**:
    - Semua data gerakan (gerakan benar dan salah) disimpan di file CSV yang terletak di data/history.csv.
    - Anda dapat mereset server ESP32 atau menghapus data CSV menggunakan tombol yang tersedia di antarmuka.

5. **API**:
    - `/connect_ble_view`: Menghubungkan ke perangkat ESP32 melalui BLE.
    - `/get_counter`: Mengambil jumlah push-up atau sit-up saat ini.
    - `/reset_esp32_view`: Mereset server ESP32.
    - `/add_data_to_csv`: Menambahkan data baru ke file CSV.
    - `/clear_csv`: Menghapus semua data di file CSV namun tetap menyimpan header.

## Koneksi BLE
- Sistem ini terhubung ke perangkat ESP32 dengan alamat MAC 34:B7:DA:F8:88:FA. Anda dapat mengubahnya sesuai dengan alamat ESP32 Anda.
- Proyek ini membaca dan menulis ke UUID tertentu untuk karakteristik BLE. Pastikan UUID di ESP32 sesuai dengan yang ada di kode.

## Catatan Penting
- Proyek ini digunakan untuk keperluan internal dan hanya boleh diedit oleh kontributor terdaftar.
- Fitur BLE menggunakan library bleak, jadi pastikan perangkat BLE Anda sudah dikonfigurasi dengan benar sebelum menjalankan proyek ini.
