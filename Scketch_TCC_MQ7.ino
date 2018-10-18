//#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <DS1307.h>
#include <DHT.h>
#include "MQ7.h"

#define DHTPIN 7     // pino DHT conectado na porta 7
#define DHTTYPE DHT22   // DHT 22  (AM2302)
MQ7 mq7(A1,5.0);

DHT dht(DHTPIN, DHTTYPE); // Inicializa o sensor DHT 16mhz
File arquivoTXT;
const int chipSelect = 10; //Módulo SD ligado à porta digital 10
DS1307 rtc(A2, A3); //Modulo RTC DS1307 ligado as portas A2 e A3 do Arduino

//Variáveis
int chk;
float hum;  //Armazena valores de umidade
float temp; //Armazena valores de temperatura

//*****PROGRAMANDO SD CARD
void setup() {

  Serial.begin(9600);
  //Wire.begin();
  SD.begin(10);      // o parametro e' o pino conectado ao CS do modulo
  dht.begin();
  pinMode(1, INPUT); // Valor digital sensor


  //ACIONANDO O RELÓGIO
  rtc.halt(false);

  //As linhas abaixo setam a data e hora do modulo
  //e podem ser comentada apos a primeira utilizacao
  //rtc.setDOW(FRIDAY);      //Define o dia da semana
  //rtc.setTime(11, 36, 0);     //Define o horario
  //rtc.setDate(3, 8, 2018);   //Define o dia, mes e ano

  //Definicoes do pino SQW/Out
  rtc.setSQWRate(SQW_RATE_1);
  rtc.enableSQW(true);

  arquivoTXT = SD.open("data.txt", FILE_WRITE);
  if (arquivoTXT) {
    arquivoTXT.println("Data    Hora    Concentracao (PPM)    Concentracao (ug/m3)    Temperatura (C)    Umidade (%)");
    arquivoTXT.close();
    Serial.println("Gravando dados.");
  } else {
    Serial.println("Erro ao abrir ou criar o arquivo texto.txt.");
  }
  Serial.println("card initialized.");
}


//FUNÇÃO CONVERSÃO PPM PARA ug/m³
float converterPPM(float ppm) {
  float PesoMolec = 28.01;
  float Conc = ((ppm * PesoMolec * 1000) / 24.5);
  return Conc;
}




//*****MENU PRINCIPAL
void loop() {
  
  float mq7PPM = mq7.getPPM();
  float Concentracao = converterPPM(mq7PPM);
  float Resistencia = mq7.getSensorResistance();
  float Ratio = mq7.getRatio();

  //Lê os valores de umidade e temperatura e armazena em hum e temp
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  // Verifica se o sensor DHT22 esta respondendo
  if (isnan(hum) || isnan(temp)) {
    Serial.println("Falha ao ler dados do sensor DHT !!!");
  }

  Serial.print("Data: ");
  Serial.print(rtc.getDateStr(FORMAT_SHORT));
  Serial.print(" ");
  Serial.print("Hora: ");
  Serial.print(rtc.getTimeStr());
  Serial.print(" ");
  Serial.println(rtc.getDOWStr(FORMAT_SHORT));

  Serial.print("Concentração CO (ppm): ");
  Serial.println(mq7PPM,2);

  Serial.print("Concentração CO (ug/m3): ");
  Serial.println(Concentracao);

  Serial.print("Res: ");
  Serial.println(Resistencia);

  Serial.print("Ratio: ");
  Serial.println(Ratio);

  Serial.print("Temperatura (C): ");
  Serial.println(temp, 2); // Mostra a temperatura com 2 casas decimais

  Serial.print("Umidade (%): ");
  Serial.println(hum);

  File arquivoTXT = SD.open("data.txt", FILE_WRITE);

  if (arquivoTXT) {

    arquivoTXT.print(rtc.getDateStr(FORMAT_SHORT));
    arquivoTXT.print("    ");
    arquivoTXT.print(rtc.getTimeStr());
    arquivoTXT.print("    ");
    arquivoTXT.print(mq7PPM);
    arquivoTXT.print("    ");
    arquivoTXT.print(Concentracao);
    arquivoTXT.print("    ");
    arquivoTXT.print(temp, 2); // Mostra a temperatura com 2 casas decimais
    arquivoTXT.print("    ");
    arquivoTXT.println(hum);
  }
  else {
    Serial.println("error opening data.txt");
  }
  arquivoTXT.close();
  delay(5000);
}




