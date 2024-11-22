<!-- ![release](https://raw.githubusercontent.com/deaafrizal/cuybot-rtos/main/docs/assets/image-3.png) -->

# 📌 **Catatan Penting untuk Instalasi PlatformIO**
> **Panduan Solusi Error untuk Pengguna Windows**

---

## ⚠️ **Deskripsi Masalah**
Ketika melakukan build atau upload program, kalian mungkin menemukan error dengan pesan seperti:  
- `FreeRTOS: No such file or directory`  
- `WiFi: blablabla error`  

Jangan khawatir! Ini adalah masalah umum dan **bisa diselesaikan dengan mudah**.

---

## 🛠️ **Solusi Langkah Demi Langkah**

### 1️⃣ **Cari Folder PlatformIO**  
Buka lokasi folder **PlatformIO** di komputer kalian.  
Lokasi default biasanya:  
```plaintext
C:\Users\{nama user}\.platformio\packages
```

### 2️⃣ **Hapus Folder Tertentu**
Temukan dan **hapus folder** bernama:  
```plaintext
framework-arduinoespressif32
```
⚠️ Jangan khawatir, folder ini akan diinstal ulang secara otomatis nanti.

### 3️⃣ **Trigger Instalasi Ulang Library**
- Buka file platformio.ini di vscode.
- **save** kembali file tersebut.  
- kalian akan melihat proses **loading** otomatis untuk menginstal ulang library yang dibutuhkan.
- tunggu sampai loading selesai

### 4️⃣ **Coba Lagi**
Setelah proses reinstall selesai, coba build/upload program kalian kembali.  
Tunggu hingga muncul pesan berikut:  
```plaintext
SUCCESS
```

---

## 🎉 **Selesai!**
Jika kalian mengikuti langkah-langkah ini, program kalian seharusnya sudah bisa di-build atau di-upload tanpa masalah. 🚀

**Happy Coding!**