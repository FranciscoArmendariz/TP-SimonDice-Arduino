#include <Arduino.h>
#include <Wire.h>

// 0=verde 1=rojo 2=amarillo 3=azul 4=naranja
int botones[5] = {7, 6, 5, 4, 2};

// 0=verde 1=rojo 2=amarillo 3=azul 4=naranja
int leds[5] = {12, 11, 10, 9, 8};
int pinBuzzer = 3;

// 0=verde 1=rojo 2=amarillo 3=azul 4=naranja
int tonosBuzzer[5] = {262, 294, 330, 349, 392};

int i;
bool nuevaSecuencia = false;
bool generarRespuesta = false;
uint8_t secuencia[99];
int tamanioSecuencia = 0;

//Sin definir:'-' Correcto:'C' Falló:'F'
char respuesta = '-';


void setup()
{
  DDRB = B00011111;
  DDRD = B00001000;
  Wire.begin(1);
  Wire.onReceive(recibirSecuencia);
  Wire.onRequest(enviarRespuesta);
}

void loop()
{
  if (nuevaSecuencia)
  {
    //ASIGNO LA RESPUESTA COMO "INCONCLUSA"
    respuesta='-';
    int pasoRespuesta=0;

    //REPRODUZCO LA SECUENCIA RECIBIDA
    for (i = 0; i < tamanioSecuencia; i++)
    {
      digitalWrite(leds[secuencia[i]], HIGH);
      tone(pinBuzzer, tonosBuzzer[secuencia[i]], 500);
      delay(500);
      digitalWrite(leds[secuencia[i]], LOW);
      noTone(pinBuzzer);
      delay(500);
    }

    //ENTRO EN UN LOO QUE ESPERA TERMINAR LA RESPUESTA O EQUIVOCARSE
    while (pasoRespuesta < tamanioSecuencia && pasoRespuesta != -1)
    {
      for (i = 0; i < 5; i++)
      {
        if (digitalRead(botones[i]) == HIGH)
        {
          //SI ME EQUIVOCO LO INDICO EN LA RESPUESTA Y COLOCA pasoRespuesta EN -1 PARA SALIR DEL BUCLE
          if (i != secuencia[pasoRespuesta]){
            tone(pinBuzzer, 100);
            delay(100);
            noTone(pinBuzzer);
            delay(200);
            tone(pinBuzzer, 100, 300);
          respuesta = 'F';
          pasoRespuesta = -1;
          //SI TONO EL BOTÓN CORRECTO PASO AL SIGUENTE ELEMENTO DE LA SECUENCIA
          } else{
          digitalWrite(leds[i], HIGH);
          tone(pinBuzzer, tonosBuzzer[i], 500);
          delay(500);
          digitalWrite(leds[i], LOW);
          noTone(pinBuzzer);
          pasoRespuesta++;
          }
        }
      }
    }
    //SI TERMINO EL BUCLE SIN EQUIVOCARME DESIGNO LA RESPUESTA COMO "CORRECTA"
    if (respuesta != 'F'){
      respuesta = 'C';
        delay(500);
        tone(pinBuzzer,523);
        delay(100);
        noTone(pinBuzzer);
        tone(pinBuzzer,587);
        delay(100);
        noTone(pinBuzzer);
        tone(pinBuzzer,659);
        delay(100);
        noTone(pinBuzzer);
    }
    nuevaSecuencia = false;
  }
}

//CARGO LA SECUENCIA NUEVA Y LA INDICO CON nuevaSecuencia
void recibirSecuencia(int size)
{
  int i;
  for (i = 0; i < size; i++)
  {
    if (Wire.available())
    {
      secuencia[i] = Wire.read();
    }
  }
  tamanioSecuencia = size;
  nuevaSecuencia = true;
}

//ENVIO LA RESPUESTA, INCONCLUSA:'-' INCORRECTA:'F' CORRECTA:'C'
void enviarRespuesta()
{
  Wire.write(respuesta);
}
