#include <Arduino.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

//*****keypad*****
const int FILAS = 4;
const int COLUMN = 4;

byte pinesFila[FILAS] = {11, 10, 9, 8};
byte pinesColumna[COLUMN] = {7, 6, 5, 4};

char teclas[FILAS][COLUMN] =
    {
        '1','2','3','A',
        '4','5','6','B',
        '7','8','9','C',
        '*','0','#','D',
};

Keypad teclado = Keypad(makeKeymap(teclas), pinesFila, pinesColumna, FILAS, COLUMN);

//*****Display*******

const int MAX_ROWS = 2;
const int MAX_CHARS = 16;

byte estrella[] = {
  B00000,
  B00100,
  B00100,
  B11111,
  B01110,
  B01010,
  B10001,
  B00000
};

byte corazon[] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};

byte calavera[] = {
  B00000,
  B01110,
  B11111,
  B10101,
  B11111,
  B01110,
  B01010,
  B00000
};


LiquidCrystal_I2C lcd(32, MAX_CHARS, MAX_ROWS);

//******WIRE******

const short ADRESS_ESCLAVO = 1;

//*******JUEGO*****

int nivel;
uint8_t secuencia[99];

// A: FACIL  B:MEDIO  C:DIFICIL
char dificultad;
int cantidadJugadores;
int jugadorActual;
int reintentosJ1;
int reintentosJ2;

// Correcto:'C' Falló:'F'
char resultado = 'C';

void setup()
{
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.createChar(0,estrella);
  lcd.createChar(1,calavera);
  lcd.createChar(2,corazon);
  randomSeed(analogRead(A0));
}

void loop()
{
  lcd.clear();
  for (int i=0;i<8;i++){
    lcd.write(0);
    lcd.write(' ');
  }
  lcd.setCursor(0,1);
  for (int i=0;i<8;i++){
    lcd.write(' ');
    lcd.write(0);
  }
  lcd.setCursor(4,0);
  lcd.print(" SIMON ");
  lcd.setCursor(4,1);
  lcd.print(" DICE! ");
  delay(2000);

  //HAGO TODAS LAS CONFIGURACIONES
  configuracionInicial();
  

  //EMPIEZA EL JUEGO HASTA QUE ALGUEN QUEDE SIN VIDAS
  while (reintentosJ1 >= 0 && reintentosJ2 >=0)
  {
    mostrarInformacionDelJuego();
    delay (400);
    
    //SE COMUNICA CON EL ESCLAVO Y SE RECIBE LA RESPUESTA
    resultado = reproducirNivel();

    //SE VERIFICA LA RESPUESTA Y SE REALIZA LO QUE CORRESPONDA A ESTA
    if (resultado == 'C'){
      pasarDeNivel();
    }else {
      manejarNivelIncorrecto();
    }
    delay(1000);
  }

  //SE MUESTRA LA INFORMACIÓN DEL FI
  finDelJuego();
}


//SE MANEJA EL FIN DE LA PARTIDA
void finDelJuego(){
  bool salir=false;

  if (cantidadJugadores==1){
    lcd.clear();
    lcd.write(1);
    lcd.print("FIN DEL JUEGO!");
    lcd.write(1);
    lcd.setCursor(0,1);
    lcd.print("Puntaje:");
    lcd.print(nivel-1);
  }else{
    lcd.clear();
    lcd.write(' ');
    lcd.write(0);
    lcd.print("GANADOR: ");
    lcd.print((jugadorActual%2)+1);
    lcd.write(' ');
    lcd.write(0);
    lcd.setCursor(0,1);
    lcd.print(" Puntaje:");
    lcd.print(nivel-1);
  }
  while (!salir){

    if (teclado.getKey()=='#'){
      salir=true;
    }
  }

}


//SE MANEJA LA RESPUESTA CORRECTA
void pasarDeNivel(){
  lcd.clear();
  lcd.print("  ");
  lcd.write(0);
  lcd.write(0);
  lcd.print("CORRECTO");
  lcd.write(0);
  lcd.write(0);
  nivel++;
  agregarASecuencia();
  if (cantidadJugadores==2){
    jugadorActual= (jugadorActual%2)+1;
  }
}

//SE MANEJA EL FALLO EN LA RESPUESTA
void manejarNivelIncorrecto(){
  lcd.clear();
  lcd.print("  ");
  lcd.write(1);
  lcd.print("INCORRECTO");
  lcd.write(1);
  if (jugadorActual==1){
    reintentosJ1--;
  }else{
    reintentosJ2--;
  }
}

//SE MUESTRA EL ESTADO ACTUAL DE LA PARTIDA
void mostrarInformacionDelJuego(){
  lcd.clear();
  lcd.print("J:");
  lcd.print(jugadorActual);
  lcd.print("  NroES:");
  lcd.print(nivel);
  lcd.setCursor(0,1);
  lcd.print("Reintentos:");
  if (jugadorActual==1){
  for (int i=0;i<reintentosJ1;i++)
    lcd.write(2);
  } else{
  for (int i=0;i<reintentosJ2;i++)
    lcd.write(2);
  }
}

//SE CONFIGURAN TODAS LAS OPCIONES PARA EMPEZAR DE CERO
void configuracionInicial()
{
  bool configuracionCompleta=false;
  bool seleccionado;

  while (!configuracionCompleta){
  
  // CONFIGURO JUGADORES
  seleccionado = false;
  lcd.clear();
  lcd.print("Nro Jugadores");
  lcd.setCursor(0,1);
  lcd.print("1.Uno - 2.Dos");
  while (!seleccionado)
  {
    char tecla = teclado.getKey();
    if (tecla == '1')
    {
      cantidadJugadores = 1;
      seleccionado = true;
    }
    if (tecla == '2')
    {
      cantidadJugadores = 2;
      seleccionado = true;
    }
  }

  // CONFIGURO DIFICULTAD
  seleccionado = false;
  lcd.clear();
  lcd.print("Nivel: A.Bajo");
  lcd.setCursor(0,1);
  lcd.print("B.Medio C.Alto");
  while (!seleccionado)
  {
    char tecla = teclado.getKey();
    if (tecla == 'A')
    {
      dificultad = 'A';
      reintentosJ1 = 3;
      reintentosJ2 = 3;
      seleccionado = true;
    }
    if (tecla == 'B')
    {
      dificultad = 'B';
      reintentosJ1 = 2;
      reintentosJ2 = 2;
      seleccionado = true;
    }
    if (tecla == 'C')
    {
      dificultad = 'C';
      reintentosJ1 = 1;
      reintentosJ2 = 1;
      seleccionado = true;
    }
  }

  //CONFIRMO ELECCIÓN
  seleccionado = false;
  lcd.clear();
  lcd.print("Jugadores:");
  lcd.print(cantidadJugadores);
  lcd.setCursor(0,1);
  lcd.print("Dificultad:");
  lcd.print(dificultad);

  delay(2000);
  lcd.clear();
  lcd.print("  Confirmar?");
  lcd.setCursor(0,1);
  lcd.print("  *-NO    #-SI  ");

  while (!seleccionado){
    char tecla = teclado.getKey();
    if (tecla=='*'){
      seleccionado=true;
    }
    if (tecla=='#'){
      seleccionado=true;
      configuracionCompleta=true;
    }
  }  
  }

  // COLOCO NIVEL EN 1 JUGADOR ACTUAL EN 1 Y ASIGNO EL PRIMER VALOR
  nivel = 1;
  jugadorActual=1;
  agregarASecuencia();

}


// AGREGO NUEVO ELEMENTO A LA SECUENCIA SEGÚN DIFICULTAD
void agregarASecuencia()
{
  if (dificultad == 'A')
  {
    secuencia[nivel - 1] = random(3);
  }
  if (dificultad == 'B')
  {
    secuencia[nivel - 1] = random(4);
  }
  if (dificultad == 'C')
  {
    secuencia[nivel - 1] = random(5);
  }
}


//ENVIA LA SECUENCIA AL ESCLAVO Y RECIVE LA EL RESULTADO DE LA RESPUESTA
char reproducirNivel()
{
  Wire.beginTransmission(ADRESS_ESCLAVO);
  Wire.write(secuencia, nivel);
  Wire.endTransmission();

  delay(nivel * 1000);

  char respuesta;
  Wire.requestFrom(ADRESS_ESCLAVO, sizeof(char));
  delay(500);
  if (Wire.available())
  {
    while (Wire.peek() == '-')
    {
      Wire.read();
      delay(2000);
      Wire.requestFrom(ADRESS_ESCLAVO, sizeof(char));
    }
    if (Wire.available())
    {
      respuesta = Wire.read();
    }
  }
  return respuesta;
}
