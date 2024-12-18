#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Endereço do LCD e as dimensões (normalmente 0x27 ou 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);  // 16 colunas e 2 linhas

//Pinos
const int Pino_Termometro = 2;
const int Pino_Rele_1 = 8;
const int Pino_Rele_2 = 7;

//Temperaturas
float T0;
float T;

//Variaveis
bool Permissao_Aquecer = true;
bool Medindo = false;

//Tempo
unsigned long Inicio;
unsigned long segundos;

//Variaveis para mudança de Delay do Rele em funçao da temperatura escolhida
int Tempo_Delay_Rele = 300;

OneWire oneWire(Pino_Termometro); // Cria um objeto OneWire
DallasTemperature sensor(&oneWire); // Informa a referencia da biblioteca dallas temperature para Biblioteca onewire

//Funcoes e  Chamadas
void Ligar_LCD()
{
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);  
  lcd.print("Tempo:");
  lcd.setCursor(15, 0);  
  lcd.print("s");
  lcd.setCursor(0, 1);
  lcd.print("Temp.:");
  lcd.setCursor(15, 1);  
  lcd.print("C");
}

void Iniciar_Rele()
{
  pinMode(Pino_Rele_1,OUTPUT);
  pinMode(Pino_Rele_2,OUTPUT);
  digitalWrite(Pino_Rele_1,HIGH);  //HIGH == Off
  digitalWrite(Pino_Rele_2,HIGH);  //LOW == On
}

void Iniciar_Termometro()
{
  sensor.begin();
}

void Medir_Temperatura()
{
  sensor.requestTemperatures();
  T = sensor.getTempCByIndex(0);
  Serial.println((String)T);
  lcd.setCursor(7, 1);
  lcd.print((String)T);
  lcd.setCursor(7, 0);
  segundos = (millis() - Inicio) / 1000;
  lcd.print((String)segundos);
}

void Aquecer() 
{
  
  digitalWrite(Pino_Rele_1,LOW);
  digitalWrite(Pino_Rele_2,LOW);
  delay(Tempo_Delay_Rele);
  digitalWrite(Pino_Rele_1,HIGH);
  digitalWrite(Pino_Rele_2,HIGH);

}
void Mudanca_de_Delay(int Temperatura_escolhida, int Delay_escolhido)
{
  if(T >= Temperatura_escolhida)
  {
    Tempo_Delay_Rele = Delay_escolhido;
  }
}

void Desbuga_Termometro()
{
  sensor.requestTemperatures();
  T = sensor.getTempCByIndex(0);
  lcd.setCursor(7, 1);
  lcd.print((String)T);
  T = sensor.getTempCByIndex(0);
  lcd.setCursor(7, 1);
  lcd.print((String)T);
}

void setup() {

  Ligar_LCD();
  Iniciar_Rele();
  Desbuga_Termometro();
  Iniciar_Termometro();
  Serial.begin(9600);
  
}

void loop() {
  if (Serial.read() == 'b')
  {
    Inicio = millis();
    Desbuga_Termometro();
    Medindo = true;
    T0 = sensor.getTempCByIndex(0);
  }
  if (Medindo)
  {
    unsigned long millisStart = millis(); //o tempo que demora pra esse escopo rodar
    Medir_Temperatura();
    
    //Quando bater o valor escolhido mudar o Tempo do Rele para o delay escolhido
    Mudanca_de_Delay(70, 500);
    Mudanca_de_Delay(90, 700);
    

    //Para Nao-Aquecer
    if(T >= (segundos/60) + T0 + 1)
    {
      Permissao_Aquecer = false;
    }

    //Para Aquecer
    if(T < (segundos/60) + T0 - 0)
    {
      Permissao_Aquecer = true;
    }

    if(Permissao_Aquecer)
    {   
      Aquecer(); 
    }
    unsigned long millisEnd = millis();

    int idelay = 1000 - (millisEnd - millisStart);
    if (idelay < 0)
    {
      return;
    }
    delay(idelay);

    if(Serial.availableForWrite()){
      if (Serial.read() == 'd')
      {
        Medindo = false;
      }
    }
  }
}

