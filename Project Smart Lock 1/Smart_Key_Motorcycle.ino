#include <EEPROM.h>
#include <SPI.h>
#include <MFRC522.h>

#define relay 8
#define buzzer 7
#define ledHijau 6

boolean ketemu = false;
boolean masterMode = false;

int suksesMembaca;
int tap=1;

byte storedCard[4];
byte readCard[4];
byte masterCard[4];

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);


void setup() {
  pinMode(ledHijau, OUTPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(relay,OUTPUT);
  digitalWrite(relay,HIGH);
  digitalWrite(buzzer, LOW);
  digitalWrite(ledHijau,HIGH);

  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
  Serial.println(F("Smartlock v1.1"));
  DetilPembaca();

  if(EEPROM.read(1) !=143)
  {
    Serial.println(F("Kartu Master tidak ada"));
    Serial.println(F("Sialakan Scan kartu anda sebagai master"));
    do
    {
      suksesMembaca = mengambilID();
      digitalWrite(ledHijau, HIGH);
      digitalWrite(buzzer,HIGH);
      delay(100);
      digitalWrite(ledHijau, LOW);
      digitalWrite(buzzer,LOW);
      delay(100);
      digitalWrite(ledHijau, HIGH);
      digitalWrite(buzzer,HIGH);
      delay(100);
      digitalWrite(buzzer,LOW);
      
    }
    while(!suksesMembaca);
    for(int k = 0; k<4; k++)
    {
      EEPROM.write(2+k, readCard[k]);
    }
    EEPROM.write(1, 143);
  }
    Serial.println(F(""));
    Serial.println(F("ID Kartu Master"));
    for(int i=0; i<4; i++)
    {
      masterCard[i] = EEPROM.read(2+i);
      Serial.print(masterCard[i], HEX);
    }
    Serial.println("");
    Serial.println(F("Alat siap digunakan"));
    Serial.println(F("Scan kartu anda"));
    Serial.println(" ");
    digitalWrite(ledHijau, HIGH);
    delay(100);
    digitalWrite(ledHijau, LOW);
    delay(100);
    digitalWrite(ledHijau, HIGH);
    delay(100);
    digitalWrite(ledHijau, LOW);
    delay(100);
    

}

void loop() {
digitalWrite(ledHijau,HIGH);
 do{
  suksesMembaca = mengambilID();
  if(masterMode)
  {
    Led();
  }
 }
 while (!suksesMembaca);

 if(masterMode)
 {
  if(isMaster(readCard))
  {
    Serial.println(F("Kartu master Telah di Scan"));
    Serial.println(F("Keluar dari Mode Master"));
    Serial.println(F(""));
    masterMode = false;
    return;
  }
     else
     {
      if(cariID(readCard))
      {
        Serial.println(F("Kartu ditemukan, kartu dihapus..."));
        hapusID(readCard);
        Serial.println(F("kartu berhasil dihapus"));
      }
      else
      {
        Serial.println(F("Kartu tidak ada, mendaftarkan kartu..."));
        menulisID(readCard);
        Serial.println(F("kartu berhasil didaftarkan"));
      }
  }
 }
 else
 {
  if(isMaster(readCard))
  {
    masterMode=true;
    Serial.println(F("Selamat datang di master mode"));
    int count = EEPROM.read(0);
    Serial.print(count);
    Serial.println(F(" ID tersimpan pada EEPROM"));
    Serial.println(F("Scan kartu untuk menambahkan / menghapus"));
    Serial.println(F(""));
    
  }
  else
  {
    
    if(cariID(readCard))
    {
      
      aksesDiterima();
      tap++;
      Serial.println(F(""));
      
    }
    else
    {
      
      aksesDitolak();
      Serial.println(F(""));
    }
  }
 }
 
}

void aksesDiterima()
{
  Serial.println(F("Akeses diterima"));
 
  if(tap==1)
  {
    digitalWrite(ledHijau,LOW);
    digitalWrite(relay,LOW);
    digitalWrite(buzzer,HIGH);
    delay(100);
    digitalWrite(buzzer,LOW);
    digitalWrite(ledHijau,HIGH);
    delay(200);
    digitalWrite(ledHijau,LOW);
    Serial.println(F("Motor dinyalakan"));

  }
  else
  {
    digitalWrite(ledHijau,LOW);
    digitalWrite(relay,HIGH);
    digitalWrite(buzzer,HIGH);
    delay(100);
    digitalWrite(buzzer,LOW);
    digitalWrite(ledHijau,HIGH);
    delay(200);
    digitalWrite(ledHijau,LOW);
    delay(200);
    digitalWrite(ledHijau,HIGH);
    delay(200);
    digitalWrite(ledHijau,LOW);
    Serial.println(F("Motor dimatikan"));
    tap=0;
  }
}

void aksesDitolak()
{
  Serial.println(F("Akses ditolak"));
  for(int i=0; i<6; i++)
  {
    digitalWrite(ledHijau,LOW);
    delay(200);
    digitalWrite(ledHijau,HIGH);
    digitalWrite(buzzer,HIGH);
    delay(200);
    digitalWrite(ledHijau,LOW);
    digitalWrite(buzzer,LOW);
  }
}

void menulisID(byte a[])
{
  if(!cariID(a))
  {
    int num = EEPROM.read(0);
    int  mulai = (num*4)+6;
    num++;
    EEPROM.write(0,num);
    for(int j=0; j<4; j++)
    {
      EEPROM.write(mulai+j,a[j]);
    }
     digitalWrite(ledHijau,LOW);
    delay(200);
    digitalWrite(ledHijau,HIGH);
    digitalWrite(buzzer,HIGH);
    delay(200);
    digitalWrite(ledHijau,LOW);
    digitalWrite(buzzer,LOW);
    delay(200);
    digitalWrite(ledHijau,HIGH);
    delay(200);
    digitalWrite(ledHijau,LOW);
    delay(200);
    digitalWrite(ledHijau,HIGH);
    delay(200);
    Serial.println(F("Berhasil menmabahkan ID ke EEPROM"));
  }
  else
  {
     digitalWrite(ledHijau,LOW);
    delay(200);
    digitalWrite(ledHijau,HIGH);
    delay(200);
    digitalWrite(ledHijau,LOW);
    delay(200);
    digitalWrite(ledHijau,HIGH);
    delay(200);
    digitalWrite(ledHijau,LOW);
    delay(200);
    digitalWrite(ledHijau,HIGH);
    Serial.println(F("Gagal!! ID atau EEPROM anda ada yang error"));
  }
}

void hapusID(byte a[])
{
  if(!cariID (a) )
  {
    Led();
    Serial.println(F("Gagal!! ID atau EEPROM anda ada yang error"));
  }
  else
  {
    int num = EEPROM.read(0);
    int slot;
    int mulai;
    int looping;
    int i;
    int count = EEPROM.read(0);
    slot = cariIDSLOT(a);
    mulai = (slot * 4)+2;
    looping = ((num-slot)*4);
    num--;
    EEPROM.write(0,num);
    for (i = 0; i<looping; i++)
    {
      EEPROM.write(mulai +i, EEPROM.read(mulai+4+i));
    }
    for( int k= 0; k<4; k++)
    {
      EEPROM.write (mulai +i+k,0);
    }
    digitalWrite(ledHijau,LOW);
    digitalWrite(buzzer,LOW);
    delay(300);
    digitalWrite(ledHijau,HIGH);
     digitalWrite(buzzer,HIGH);
    delay(300);
    digitalWrite(ledHijau,LOW);
     digitalWrite(buzzer,LOW);
    delay(300);
    digitalWrite(ledHijau,HIGH);
     digitalWrite(buzzer,HIGH);
    delay(300);
    digitalWrite(ledHijau,LOW);
     digitalWrite(buzzer,LOW);
    delay(300);
    digitalWrite(ledHijau,HIGH);
    delay(300);
    Serial.println(F("ID berhasil dihapus dari EEPROM"));
    
  }
}

boolean cariIDSLOT(byte find[])
{
  int count = EEPROM.read(0);
  for(int i=1; i<=count; i++)
  {
    bacaID(i);
    if(cekBit(find, storedCard))
    {
      return i;
      break;
    }
  }
}


boolean cariID(byte find[])
{
  int count = EEPROM.read(0);
  for(int i = 1; i<= count; i++)
  {
    bacaID(i);
    if(cekBit (find, storedCard))
    {
      return true;
      break;
    }
    else
    {
      
    }
  }
  return false;
}

void bacaID(int nomer)
{
  int mulai = (nomer*4)+2;
  for(int i=0; i<4; i++)
  {
    storedCard[i] = EEPROM.read(mulai + i);
  }
}

boolean isMaster(byte coba[])
{
  if(cekBit (coba, masterCard))
  {
    return true;
  }
  else
  {
    return false;
  }
}
boolean cekBit (byte a[], byte b[])
{
  if(a[0] != NULL)
  {
    ketemu = true;
    }
    for(int k =0; k<4; k++)
    {
      if(a[k] != b[k])
      {
        ketemu = false;
      }
    }
    if(ketemu)
    {
      return true;
    }
    else
    {
      return false;
    }
}

int mengambilID()
{
  if(!mfrc522.PICC_IsNewCardPresent())
  {
    return 0;
  }
  if(!mfrc522.PICC_ReadCardSerial())
  {
    return 0;
  }
  Serial.println(F("Kartu Terscan : "));
  for(int i=0; i<4; i++)
  {
    readCard[i] = mfrc522.uid.uidByte[i];
    Serial.print(readCard[i],HEX);
  }
  Serial.println("");
  mfrc522.PICC_HaltA();
  return 1;
}

void DetilPembaca()
{
   byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("Versi Software MFRC522: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (Tidak diketahui)"));
  Serial.println("");
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(F("PERINGATAN: Komunikasi Gagal, apakah MFRC522 terhubung dengan benar?"));
    while(true);
  }
}

void Led()
{
    digitalWrite(ledHijau, LOW);
    delay(100);
    digitalWrite(ledHijau, HIGH);
    delay(100);
    digitalWrite(ledHijau, LOW);
    delay(100);
    digitalWrite(ledHijau, HIGH);
    delay(100);
}

void standarMode()
{
  digitalWrite(ledHijau,HIGH);
  digitalWrite(relay,LOW);
}
