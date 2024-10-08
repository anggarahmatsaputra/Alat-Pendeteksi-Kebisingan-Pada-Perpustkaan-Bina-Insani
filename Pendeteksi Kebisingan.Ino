#include <Wire.h>
#include <LiquidCrystal_I2C.h>


#define BLYNK_TEMPLATE_ID "TMPL6I0xL2bfl"
#define BLYNK_TEMPLATE_NAME "PerpustkaanBIU"
#define BLYNK_AUTH_TOKEN ""

LiquidCrystal_I2C lcd(0x27, 16, 2);                // Inisialisasi objek LCD dengan alamat I2C 0x27, dan ukuran 16x2
const int pinSensorSuara = 34;                     // Pin untuk sensor suara DF-Robot
const int pinPlay = 27;                            // Pin untuk mengaktifkan pemutaran suara pada modul ISD1820
const int pinLedMerah = 14;                        // Pin untuk LED merah
const int pinLedKuning = 12;                       // Pin untuk LED kuning
const int pinLedHijau = 13;                        // Pin untuk LED hijau
char auth[] = "";  // Token Blynk
char ssid[] = "Infinix NOTE 40S";                           // Nama jaringan WiFi Anda
char pass[] = "anggara12";                        // Password WiFi Anda

bool isConnected = false; // Status koneksi WiFi

void setup() {
  Serial.begin(9600);             // Mulai komunikasi serial
  Blynk.config(auth);             // Konfigurasi Blynk tanpa mulai koneksi
  lcd.init();                     // Inisialisasi LCD
  lcd.backlight();                // Nyalakan backlight LCD
  pinMode(pinPlay, OUTPUT);       // Atur pinPlay sebagai output
  pinMode(pinLedMerah, OUTPUT);   // Atur pin LED merah sebagai output
  pinMode(pinLedKuning, OUTPUT);  // Atur pin LED kuning sebagai output
  pinMode(pinLedHijau, OUTPUT);   // Atur pin LED hijau sebagai output

  connectWiFi();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Blynk.run(); // Jalankan koneksi Blynk jika terhubung ke WiFi
    isConnected = true;
  } else {
    isConnected = false;
    connectWiFi(); // Coba koneksi ulang jika terputus
  }

  int nilaiSensor = analogRead(pinSensorSuara);       // Baca nilai dari sensor suara
  float desibel = map(nilaiSensor, 0, 1023, 30, 130);  // Konversi nilai sensor suara menjadi desibel dengan range yang lebih luas

  // Terapkan faktor kalibrasi untuk meningkatkan sensitifitas
  desibel = desibel * 1.2;  // Contoh faktor kalibrasi 1.2, sesuaikan dengan kebutuhan

  Serial.print("Nilai sensor suara: ");
  Serial.print(nilaiSensor);
  Serial.print(" | Desibel: ");
  Serial.println(desibel);  // Tampilkan nilai sensor suara dalam desibel di monitor serial

  lcd.setCursor(0, 0);  // Posisi kursor di kolom ke-0 baris 1
  lcd.print("Desibel: ");
  lcd.print(desibel);  // Tampilkan nilai desibel di LCD
  lcd.print("   ");    // Pastikan menghapus karakter sisa

  lcd.setCursor(0, 1);  // Posisi kursor di kolom ke-0 baris 2
  lcd.print("                ");  // Hapus baris kedua untuk menghindari tumpang tindih

  if (desibel > 55) {
    digitalWrite(pinLedMerah, HIGH);  // Nyalakan LED merah
    digitalWrite(pinLedKuning, LOW);  // Matikan LED kuning
    digitalWrite(pinLedHijau, LOW);   // Matikan LED hijau
    Serial.println("WARNING: Suara melebihi 55 dB!");
    if (isConnected) {
      Blynk.logEvent("peringatan_Suara_berisik");
    }
    lcd.setCursor(0, 1);              // Posisi kursor di kolom ke-0 baris 2
    lcd.print("Suara berisik     ");  // Tampilkan pesan "Suara berisik" di LCD
    digitalWrite(pinPlay, HIGH);      // Aktifkan pemutaran suara
  } else if (desibel < 55 && desibel >= 45) {
    digitalWrite(pinLedMerah, LOW);    // Matikan LED merah
    digitalWrite(pinLedKuning, HIGH);  // Nyalakan LED kuning
    digitalWrite(pinLedHijau, LOW);    // Matikan LED hijau
    Serial.println("Suara sedikit berisik");
    lcd.setCursor(0, 1);           // Posisi kursor di kolom ke-0 baris 2
    lcd.print("sedikit berisik  ");  // Tampilkan pesan "Suara sedikit berisik" di LCD
    digitalWrite(pinPlay, LOW);    // Matikan pemutaran suara
  } else if (desibel < 45) {
    digitalWrite(pinLedMerah, LOW);   // Matikan LED merah
    digitalWrite(pinLedKuning, LOW);  // Matikan LED kuning
    digitalWrite(pinLedHijau, HIGH);  // Nyalakan LED hijau
    Serial.println("senyap");
    lcd.setCursor(0, 1);           // Posisi kursor di kolom ke-0 baris 2
    lcd.print("senyap           ");  // Tampilkan pesan "Perpustakaan senyap" di LCD
    digitalWrite(pinPlay, LOW);    // Matikan pemutaran suara
  }

  if (isConnected) {
    Blynk.virtualWrite(V0, desibel);     // Kirim nilai desibel ke server Blynk
    Blynk.virtualWrite(V1, desibel);     // Persentase Blynk

  }

  delay(1000); // Tunggu 1 detik sebelum pengukuran berikutnya
}

void connectWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nConnected to WiFi");
      Blynk.connect(); // Coba koneksi Blynk setelah terhubung ke WiFi
    } else {
      Serial.println("\nFailed to connect to WiFi");
    }
  }
}
