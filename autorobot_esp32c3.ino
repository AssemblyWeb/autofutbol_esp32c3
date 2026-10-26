#include <WiFi.h>
#include <WebServer.h>

//ESP32 MICRO-C3
//CODE V2.6 - Corregido error de caracteres

// PWM
#define ENA 2
#define ENB 27
bool turbo = false;
const int VELOCIDAD_NORMAL = 153;
const int VELOCIDAD_TURBO = 255;

// Pines L298N
#define IN1 0
#define IN2 4
#define IN3 32
#define IN4 25

// WIFI
const char* ssid = "Prof. Perino";
const char* password = "123pizza";

WebServer server(80);

// Funciones
void cambiarTurbo();
void adelante();
void atras();
void izquierda();
void derecha();
void detener();
void cargarPagina();
void procesarComando(String comando);

// Página HTML con estilos de "hover" y "active"
String pagina = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">

<style>
body{
  font-family: Arial;
  text-align: center;
  background: #e9e9e9;
  margin-top: 30px;
  user-select: none;
  -webkit-user-select: none;
  touch-action: manipulation;
}

.container{
  display: inline-block;
  background: white;
  padding: 20px;
  border-radius: 20px;
  box-shadow: 0px 0px 15px rgba(0,0,0,0.3);
}

button{
  width: 140px;
  height: 70px;
  font-size: 18px;
  margin: 5px;
  border: none;
  border-radius: 15px;
  cursor: pointer;
  background-color: #3498db;
  color: white;
  font-weight: bold;
  transition: background-color 0.1s, transform 0.1s;

  user-select: none;
  -webkit-user-select: none;
  touch-action: manipulation;
}

button:hover {
  background-color: #2980b9;
}

button:active {
  background-color: #1f618d;
  transform: scale(0.95);
}

.stop{
  width: 150px;
  height: 80px;
  font-size: 22px;
  background-color: #e74c3c;
}

.stop:hover {
  background-color: #c0392b;
}

.stop:active {
  background-color: #922b21;
  transform: scale(0.95);
}
</style>
</head>

<body>

<div class="container">
  <h1>Control Auto ESP32</h1>
  <h1>V2.6</h1>

  <div>
    <button
      onmousedown="enviar('F')"
      onmouseup="enviar('S')"
      ontouchstart="enviar('F')"
      ontouchend="enviar('S')">
      AVANZAR
    </button>
  </div>

  <div>
    <button
      onmousedown="enviar('L')"
      onmouseup="enviar('S')"
      ontouchstart="enviar('L')"
      ontouchend="enviar('S')">
      IZQUIERDA
    </button>

    <button
      class="stop"
      onclick="enviar('T')">
      NITRO
    </button>

    <button
      onmousedown="enviar('R')"
      onmouseup="enviar('S')"
      ontouchstart="enviar('R')"
      ontouchend="enviar('S')">
      DERECHA
    </button>
  </div>

  <div>
    <button
      onmousedown="enviar('B')"
      onmouseup="enviar('S')"
      ontouchstart="enviar('B')"
      ontouchend="enviar('S')">
      RETROCEDER
    </button>
  </div>
</div>

<script>
function enviar(comando){
    fetch('/' + comando)
    .catch(error => console.log(error));
}

document.addEventListener('mouseup', function(){
    enviar('S');
});

document.addEventListener('touchend', function(){
    enviar('S');
});

document.addEventListener("contextmenu", function(e){
    e.preventDefault();
});

document.addEventListener("selectstart", function(e){
    e.preventDefault();
});
</script>

</body>
</html>
)rawliteral";

void setup(){
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nIniciando ESP32...");

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  analogWrite(ENA, VELOCIDAD_NORMAL);
  analogWrite(ENB, VELOCIDAD_NORMAL);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  detener();

  // Configurar Wi-Fi en modo Access Point (AP)
  WiFi.softAP(ssid, password);
  WiFi.setTxPower(WIFI_POWER_15dBm);

  Serial.println("Red Wi-Fi creada con éxito.");
  Serial.print("Nombre de Red (SSID): ");
  Serial.println(ssid);
  Serial.print("IP del servidor web: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", cargarPagina);
  server.on("/F", [](){ procesarComando("F"); });
  server.on("/B", [](){ procesarComando("B"); });
  server.on("/L", [](){ procesarComando("L"); });
  server.on("/R", [](){ procesarComando("R"); });
  server.on("/S", [](){ procesarComando("S"); });
  server.on("/T", [](){ procesarComando("T"); });

  server.begin();
  Serial.println("Servidor web iniciado.");
}

void loop(){
  server.handleClient();
}

void cargarPagina(){
  Serial.println("[WEB] Página principal solicitada desde un cliente.");
  server.send(200, "text/html", pagina);
}

void procesarComando(String comando){
  if(comando == "F"){ adelante(); }
  else if(comando == "B"){ atras(); }
  else if(comando == "L"){ izquierda(); }
  else if(comando == "R"){ derecha(); }
  else if(comando == "T"){ cambiarTurbo(); }
  else{ detener(); }

  server.send(200, "text/plain", "OK");
}

void adelante(){
  Serial.println("-> Comando recibido: AVANZAR (F)");
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void atras(){
  Serial.println("-> Comando recibido: RETROCEDER (B)");
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void izquierda(){
  Serial.println("-> Comando recibido: IZQUIERDA (L)");
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void derecha(){
  Serial.println("-> Comando recibido: DERECHA (R)");
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void cambiarTurbo(){
  turbo = !turbo;
  if(turbo){
    Serial.println("-> Comando recibido: NITRO ACTIVADO (T)");
    analogWrite(ENA, VELOCIDAD_TURBO);
    analogWrite(ENB, VELOCIDAD_TURBO);
  } else {
    Serial.println("-> Comando recibido: NITRO DESACTIVADO (T)");
    analogWrite(ENA, VELOCIDAD_NORMAL);
    analogWrite(ENB, VELOCIDAD_NORMAL);
  }
}

void detener(){
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}