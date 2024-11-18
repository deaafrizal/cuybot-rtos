Cara mengganti metode sistem line following:

saat ini ada 2 metode yang diterapkan.

1. metode PID controller
2. metode fuzzy controller

metode PID perlu tuning di variabel kp, ki & kd disesuaikan dengan kondisi track & dinamo motor.

metode Fuzzy tanpa perlu tuning kp, ki, kd, cukup atur beberapa variable penentu untuk mendapatkan hasil belok yang lebih smooth.

cara ganti metode:
1. pilih mau pake metode yang mana, secara default sistem ini menggunakan PID controller yang belum di tuning dengan optimal (harus melakukan testing pendekatan masing2 karena berbeda enviroment)
2. jika ingin mengganti menggunakan fuzzy, cukup rename saja file ini (jangan dihapus), ganti nama file LineFollowingTask.cpp menjadi LineFollowingTask.cpp.PID dan LineFollowingTask.h menjadi LineFollowingTask.h.PID 
3. untuk mengaktifkan fuzzy controller, ganti nama file LineFollowingTask.cpp.fuzzy menjadi LineFollowingTask.cpp saja, berikut juga dengan LineFollowingTask.h.fuzzy ganti menjadi LineFollowingTask.h saja
4. save dan upload program kembali lalu lakukan pengetesan di track

untuk saat ini belum diterapkan mode untuk switch mode line following antara metode pid ataupun fuzzy. kedepannya akan dibuatkan fitur pemilihan line following berdasarkan metode2 tertentu dan juga dengan fitur opsi sistem line following analog atau digital.

Metode Fuzzy sangat cocok untuk track sederhana dan proses tuning yang lebih mudah dibandingkan dengan PID controller. 
Dinamo motor dibawah 100RPM direkomendasikan menggunakan fuzzy controller ataupun metode lainnya yang lebih sederhana.

jika motor diatas 100RPM, maka PID controller akan bekerja lebih optimal.

kondisi track, kelengketan roda, jarak sensor ke roda, panjang & lebar bodi juga sangat mempengaruhi dalam pemilihan metode line following system.