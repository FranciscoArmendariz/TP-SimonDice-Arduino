# TP-SimonDice-Arduino

## Descripción
Juego de Simon Dice realizado para dos placas Arduino UNO.
Creado en VS Code con la extensión Platform.io para utilizár en Tinkercad aunque debería funcionar en placas Aruino reales.

## Proyecto en Tinkercad
https://www.tinkercad.com/things/1Xzqpk0wd0N

## Ayuda
En hardware real es muy probable que tengas que modificar el "adress" del Display I2C en la siguiente linea del maestro:
```
61  LiquidCrystal_I2C lcd(<LCD adress>, MAX_CHARS, MAX_ROWS);
```
el valor que corresponde depende del modelo del display.

## Componentes usados
| COMPONENTE  | CANTIDAD |
| ------------- | ------------- |
| Arduino UNO | 2 |
| Proto-Board | 1 |
| Display Lcd 16x2 I2C | 1 |
| Buzzer | 1 |
| Resistencias 220 | 10 |
| Botones | 5 |
| Led Verde | 1 |
| Led Rojo | 1 |
| Led Amarillo | 1 |
| Led Azul | 1 |
| Led Naranja | 1 |

## Notas
Proyecto realizado como trabajo practico para la materia "Desarrollo en sistemas Embebidos" de la Universidad Nacional de Lanús.
