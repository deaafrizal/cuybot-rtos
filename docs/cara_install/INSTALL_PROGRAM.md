# 🛠️ **Panduan Fresh Install Cuybot FreeRTOS**

Setelah memiliki source code **Cuybot FreeRTOS** di PC/laptop masing-masing, kalian siap untuk menginstall program ke **ESP32-C3-Supermini**.

---

## 📂 **Tahapan Penginstallan**
1. **Download Source Code** (Ada 2 metode)
2. **Update / Install Program**

---

## 📥 **TAHAPAN 1: Download Program**
Ada dua cara untuk mendownload program / source code **Cuybot FreeRTOS**:
1. **Download via Branch** [Panduannya klik di sini](https://github.com/deaafrizal/cuybot-rtos/blob/main/docs/cara_download/DOWNLOAD_VIA_BRANCH.md)

2. **Download via Releases** [Panduannya klik di sini](https://github.com/deaafrizal/cuybot-rtos/blob/main/docs/cara_download/DOWNLOAD_VIA_RELEASES.md)

tutorial berada di dalam folder docs/cara_download [di sini](https://github.com/deaafrizal/cuybot-rtos/blob/main/docs/cara_download/)

---

## 📌 **Catatan Penting**
Jika ESP32-C3-Supermini kalian sudah memiliki program lain sebelumnya, mengikuti panduan ini akan:  
- **Menghapus program lama**  
- **Mengisi ESP32-C3-Supermini dengan program Cuybot FreeRTOS**

---

## 🚀 **Langkah-langkah Install**
### **1️⃣ Persiapan Awal**
1. **Ekstrak Folder Source Code**  
   Buka folder **cuybot-rtos** yang telah kalian download dan ekstrak.

2. **Pastikan Software VSCode**  
   Jika belum memiliki Visual Studio Code (VSCode), **download** dan **install** terlebih dahulu.

3. **Buka File Workspace**  
   Di folder **cuybot-rtos**, cari file dengan ekstensi **`.workspace`**:  
   ![alt text](https://github.com/deaafrizal/cuybot-rtos/blob/main/assets/workspace.png)  
   Klik file tersebut untuk membuka proyek di VSCode.

---

### **2️⃣ Konfigurasi Proyek**
4. **Install PlatformIO**  
   Saat membuka proyek, VSCode akan otomatis meminta kalian menginstall ekstensi **PlatformIO**.  
   Jika notifikasi tidak muncul, install secara manual:  
   ![alt text](https://github.com/deaafrizal/cuybot-rtos/blob/main/assets/platformio.png)

5. **Periksa File `platformio.ini`**  
   Buka file `platformio.ini` yang ada di folder proyek:  
   ![alt text](https://github.com/deaafrizal/cuybot-rtos/blob/main/assets/platformioini.png)  
   Pastikan file sudah benar, lalu tekan **CTRL+S** untuk menyimpan.

6. **Tunggu Konfigurasi Ulang**  
   VSCode akan mengkonfigurasi ulang proyek, tunggu hingga selesai.

---

### **3️⃣ Flash ESP32**
7. **Hubungkan ESP32 ke Laptop/PC**  
   Cabut ESP32 dari board, lalu colokkan menggunakan kabel USB.

8. **Erase Flash**  
   Di sidebar PlatformIO, cari menu **"Erase Flash"**, lalu klik erase flash nya:  
   ![alt text](https://github.com/deaafrizal/cuybot-rtos/blob/main/assets/erase.png)

9. **Upload Filesystem Image**  
   Setelah sukses, klik **"Upload Filesystem Image"** di sidebar PlatformIO.

10. **Upload Program**  
    Terakhir, klik **"Upload"** atau **"Upload and Monitor"** untuk mengisi program ke ESP32.

---

### **4️⃣ Uji Coba**
11. **Periksa Jaringan WiFi**  
    Setelah proses selesai, cabut ESP32, pasang ke board, dan nyalakan.  
    Cek WiFi di laptop atau HP, harusnya muncul jaringan dengan nama:  
    ```
    cuybot-xxx
    ```
    **`xxx`** adalah identifikasi unik setiap ESP32.

12. **Koneksi WiFi**  
    Gunakan perangkat kalian untuk terhubung ke WiFi **cuybot**, dengan password:  
    ```
    cuybot123
    ```

13. **Akses Web Interface**  
    - Buka browser (disarankan **Chrome**).  
    - Ketik alamat:  
      ```
      http://cuybot.local
      ```
    - Jika tidak dapat diakses, coba gunakan IP:  
      ```
      http://192.168.4.1
      ```

---

🎉 **Selesai!** kalian sekarang dapat menggunakan dan mengontrol **Cuybot FreeRTOS**. Selamat mencoba!

Jika mendapatkan error ketika proses instalasi, silahkan cek panduan ini [Panduan Troubleshooting](https://github.com/deaafrizal/cuybot-rtos/blob/main/docs/panduan_error/TROUBLESHOOTING.md)