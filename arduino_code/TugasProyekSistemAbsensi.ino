/*
Tugas Progjek Mikrokontroller
Sistem Absensi Berbasis Arduino menggunakan sensor RFID
@copy-writing oleh
- DELVIRA AMBARWATI
- Shofa Khoerunnisa
- GALIH RIDHO UTOMO
- LAMTIAR PUTRI ANISA SITOHANG
##flowchart
Begin
Inisialisasi Module yang diperlu
  - SoftwareSerial
  - Penyingkatan nama sensor [pin TX, RX]
Inisialisasi Struktur Data Charater Identitas
Pemanggilan Database kelas
Deklarasi nilai check siswa untuk memeriksa dan batasi input user
Inisialisasi fungsi setup
  Komunikasi boud rate 9600 untuk melihat dan memasukan data pada serial command prompt arduino
  Komunikasi boud rate 9600 untuk mengaktifkan sensor RFID dalam mengirimkan data TX dan RX
  Pesan Komunikasi yang ditampilkan String formated
Inisialisasi fungsi loop
  Jika:
    Syarat komunikasi dua arah tersedia
  Do:
    Pemanggilan fungsi Check Indentitas
  SELESAI
Inisialisasi fungsi Check Indentitas
  Pesan Komunikasi yang ditampilkan String formated
  Ketika:
    Negasi Serial Komunikasi tersedia
  Do:
    Membaca String Komunikasi yang dimasukan oleh Pengguna
    Menghilangkan Space White / Spasi pada Command yang dimasukan Pengguna sebagai Data
    Menampilkan NIM yang dimasukan Pengguna
    Insialisasi keadaan mula-mula tidak ada input yang dimasukan
    Untuk i sebanyaka nilai insialisasi sC <- Increament Data
      Jika:
        Nilai yang dimasukan Pengguna berupa NIM sama dengan nilai yang ada di database
      Do:
        Pesan Komunikasi yang ditampilkan String formated dengan Database ditampilkan sesuai indetintas yang dimasukan
        Assign keadaan sebagai benar
      Break
    Jika:
      Tidak didapat dicari atau tidak dapat ditemukan pada database
    Do:
      Pesan Komunikasi yang ditampilkan String formated
      Looping
  SELESAI

*/
#include <SoftwareSerial.h>
SoftwareSerial rf(0, 1);
struct sT {char NIM[11]; char Nama[30];};
sT sTs[23] = {
 {"4211420075","BARNAS ATTALLA"},{"4211421001","IIN KURNIANINGSIH"},{"4211421002","DELVIRA AMBARWATI"},{"4211421004","DIANICA MAULINA"},{"4211421006","Shofa Khoerunnisa"},{"4211421008","SHOFINA MUMTAZ MAHARDHIKA"},{"4211421009","ACHMAD HUSAIN"},{"4211421011","ARLA ZAHRANI FATIKHAH"},{"4211421014","Novi Fitria"},{"4211421017","PRADIPTA KUSUMA YUDHA"},{"4211421018","Cahya Ade Basuki"},{"4211421019","BELLA NATASYA MULYANINGRUM"},{"4211421022","ALI MUNTOHA"},{"4211421026","DESCANIA PUTRI SHARA NITA"},{"4211421027","ANGGELA FERA TRI ASTUTI"},{"4211421031","ILHAM RAMADHAN PRASETYO"},{"4211421032","Aisah Nuria Amanah"},{"4211421033","JEREMY REIHANDRO"},{"4211421035","RISWARI SETYO NUGRAHENI"},{"4211421036","GALIH RIDHO UTOMO"},{"4211421038","AULIA IKHSANI FAUZIAH"},{"4211421041","VINAYA MIFTA ALIFAH"},{"4211421042","LAMTIAR PUTRI ANISA SITOHANG"}};
int sC = 23;
void setup() {Serial.begin(9600); rf.begin(9600); Serial.println("Sistem Absensi Berbasis RFID"); Serial.println("Silakan tempelkan kartu RFID Anda...");}
void loop() {if (Serial.available()) {checksT();}}
void checksT() {
    Serial.println("Membaca NIM dari RFID...");
    while (!Serial.available());
    String nR = Serial.readStringUntil('\n');
    nR.trim(); Serial.print("NIM yang dimasukkan: "); Serial.println(nR);
    bool found = false;
    for (int i = 0; i < sC; i++) {
        if (nR == String(sTs[i].NIM)) {
            Serial.println("Selamat datang, " + String(sTs[i].Nama)); Serial.println("Anda berhasil masuk kelas."); found = true; break;
        }
    }
    if (!found) {Serial.println("NIM tidak dikenal. Akses ke kelas ditolak."); Serial.println("Silakan tempelkan kartu RFID Anda kembali...");}
}
