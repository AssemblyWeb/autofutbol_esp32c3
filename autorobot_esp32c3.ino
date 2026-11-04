/* ==========================================================================
 * PROYECTO: Control de Auto RC con ESP32-C3 Super Mini y Puente H (L298N)
 * CURSO: Procedimientos Técnicos (PCT)
 * DESCRIPCIÓN: Este programa convierte el ESP32 en una antena Wi-Fi (Access Point)
 * para que te puedas conectar con el celular y manejar un coche robot mediante
 * una página web interactiva.
 * ========================================================================== */

#include <WiFi.h>       // Librería para que el ESP32 pueda crear su propia red Wi-Fi
#include <WebServer.h>  // Librería para montar un servidor web dentro del microcontrolador

// Identificación del código
// CODE V2.9 - Estabilizado contra bloqueos y Watchdog (sistema de seguridad interno)

// ==========================================
// CONFIGURACIÓN DE PINES Y VELOCIDADES
// ==========================================

// Pines PWM (Controlan la velocidad de los motores enviando pulsos eléctricos)
#define ENA 7           // Pin que regula la velocidad del motor izquierdo
#define ENB 10          // Pin que regula la velocidad del motor derecho

bool turbo = false;                     // Variable para saber si el modo nitro está activado (verdadero/falso)
const int VELOCIDAD_NORMAL = 153;       // Velocidad estándar (valor entre 0 y 255)
const int VELOCIDAD_TURBO = 255;        // Velocidad máxima (255 = voltaje completo)

// Pines de Dirección del L298N (Deciden si el motor gira hacia adelante o hacia atrás)
#define IN1 0           // Control motor izquierdo (Adelante)
#define IN2 1           // Control motor izquierdo (Atrás)
#define IN3 3           // Control motor derecho (Adelante)
#define IN4 4           // Control motor derecho (Atrás)

// ==========================================
// CONFIGURACIÓN DE LA RED WI-FI
// ==========================================
const char* ssid = "Prof. Perino";      // Nombre de la red Wi-Fi que emitirá tu ESP32
const char* password = "123pizza";      // Contraseña para conectarse al auto (mínimo 8 caracteres)

// Creamos el servidor web en el puerto estándar 80 (HTTP)
WebServer server(80);

// ==========================================
// DECLARACIÓN DE FUNCIONES (El índice del programa)
// ==========================================
// Aquí le avisamos al ESP32 que más abajo vamos a escribir estas funciones
void cambiarTurbo();
void adelante();
void atras();
void izquierda();
void derecha();
void detener();
void cargarPagina();
void procesarComando(String comando);

// ==========================================
// CÓDIGO DE LA PÁGINA WEB (HTML, CSS y JavaScript)
// ==========================================
// Esta variable guarda toda la interfaz visual que ves en la pantalla de tu celular.
String pagina = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
/* Diseño visual (CSS) para que los botones se vean ordenados y modernos tipo consola */
body{
  font-family: Arial;
  text-align: center;
  background: #e9e9e9;
  margin-top: 30px;
  user-select: none;           /* Evita que se seleccione texto por accidente al tocar */
  -webkit-user-select: none;
  touch-action: manipulation;  /* Mejora la respuesta táctil en celulares */
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
  background-color: #3498db;   /* Color azul de los botones */
  color: white;
  font-weight: bold;
  transition: background-color 0.1s, transform 0.1s;
  user-select: none;
  touch-action: manipulation;
}
button:hover { background-color: #2980b9; }
button:active { background-color: #1f618d; transform: scale(0.95); /* Efecto de botón presionado */ }
.stop{
  width: 150px;
  height: 80px;
  font-size: 22px;
  background-color: #e74c3c;    /* Color rojo especial para el botón de Turbo/Nitro */
}
.stop:hover { background-color: #c0392b; }
.stop:active { background-color: #922b21; transform: scale(0.95); }
</style>
</head>
<body>
<div class="container">
  <h1>Control Auto ESP32</h1>
  <h1>V2.9</h1>
  
  <!-- Botón para ir hacia adelante -->
  <div>
    <button onmousedown="enviar('F')" onmouseup="enviar('S')" ontouchstart="enviar('F')" ontouchend="enviar('S')">AVANZAR</button>
  </div>
  
  <!-- Botones de giro y turbo -->
  <div>
    <button onmousedown="enviar('L')" onmouseup="enviar('S')" ontouchstart="enviar('L')" ontouchend="enviar('S')">IZQUIERDA</button>
    <button class="stop" onclick="enviar('T')">NITRO</button>
    <button onmousedown="enviar('R')" onmouseup="enviar('S')" ontouchstart="enviar('R')" ontouchend="enviar('S')">DERECHA</button>
  </div>
  
  <!-- Botón para retroceder -->
  <div>
    <button onmousedown="enviar('B')" onmouseup="enviar('S')" ontouchstart="enviar('B')" ontouchend="enviar('S')">RETROCEDER</button>
  </div>
</div>

<script>
// Código en JavaScript que corre dentro del navegador del celular
let ultimoEnvio = 0;
function enviar(comando){
    // Control de velocidad: Evita saturar el servidor enviando demasiadas peticiones juntas
    let ahora = Date.now();
    if (ahora - ultimoEnvio < 50 && comando === 'S') return; 
    ultimoEnvio = ahora;

    // Hace una petición web invisible al ESP32 indicando qué letra (comando) ejecutar
    fetch('/' + comando).catch(error => console.log(error));
}

// Seguridad para soltar el motor automáticamente cuando sacas el dedo de la pantalla
document.addEventListener('mouseup', () => enviar('S'));
document.addEventListener('touchend', () => enviar('S'));
// Bloquea el menú contextual y selección para que parezca una aplicación real
document.addEventListener("contextmenu", e => e.preventDefault());
document.addEventListener("selectstart", e => e.preventDefault());
</script>
</body>
</html>
)rawliteral";

// ==========================================
// CONFIGURACIÓN INICIAL (Se ejecuta una sola vez al encender)
// ==========================================
void setup(){
  Serial.begin(115200);   // Inicia la comunicación serie para ver mensajes en la PC
  delay(1000);
  Serial.println("\nIniciando ESP32-C3 Super Mini V2.9...");

  // Configuramos los pines de velocidad (PWM) como salidas
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Aplicamos la velocidad normal por defecto a ambos motores
  analogWrite(ENA, VELOCIDAD_NORMAL);
  analogWrite(ENB, VELOCIDAD_NORMAL);

  // Configuramos los pines de dirección del puente H como salidas
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  detener();              // Nos aseguramos de que el auto arrancar quieto

  // Creamos la red Wi-Fi propia del ESP32 (Modo Access Point / AP)
  WiFi.softAP(ssid, password);
  WiFi.setTxPower(WIFI_POWER_15dBm); // Ajusta la potencia de la antena Wi-Fi

  Serial.println("Red Wi-Fi creada con éxito.");
  Serial.print("IP del servidor web: ");
  Serial.println(WiFi.softAPIP()); // Muestra la dirección IP (ej. 192.168.4.1)

  // Vinculamos las rutas web con las acciones correspondientes en C++
  server.on("/", cargarPagina);                  // Página principal
  server.on("/F", [](){ procesarComando("F"); }); // Adelante (Forward)
  server.on("/B", [](){ procesarComando("B"); }); // Atrás (Backward)
  server.on("/L", [](){ procesarComando("L"); }); // Izquierda (Left)
  server.on("/R", [](){ procesarComando("R"); }); // Derecha (Right)
  server.on("/S", [](){ procesarComando("S"); }); // Detenerse (Stop)
  server.on("/T", [](){ procesarComando("T"); }); // Turbo (Nitro)

  server.begin();         // Arranca el servidor web
  Serial.println("Servidor web iniciado.");
}

// ==========================================
// BUCLE PRINCIPAL (Se repite infinitamente)
// ==========================================
void loop(){
  server.handleClient();  // Revisa constantemente si algún celular entró a la página o apretó un botón
  yield(); // Libera un poco de tiempo de CPU para evitar que el sistema de seguridad (Watchdog) reinicie el chip por estar ocupado
}

// ==========================================
// FUNCIONES DE CONTROL Y ACCIÓN
// ==========================================

// Envía la página HTML al navegador del celular
void cargarPagina(){
  server.send(200, "text/html", pagina);
}

// Lee qué letra mandó la página web y decide qué función del auto ejecutar
void procesarComando(String comando) {
  
  // Evaluamos la orden que llegó por la red Wi-Fi paso a paso
  if(comando == "F"){ 
    adelante(); 
  } 
  else if(comando == "B"){ 
    atras(); 
  } 
  else if(comando == "L"){ 
    izquierda(); 
  } 
  else if(comando == "R"){ 
    derecha(); 
  } 
  else if(comando == "T"){ 
    cambiarTurbo(); 
  } 
  else{ 
    detener(); // Red de seguridad: si llega cualquier otra cosa por error, frena el auto
  }

  server.send(200, "text/plain", "OK"); // Responde al celular que la orden llegó bien
}

// Motor Izquierdo Adelante, Motor Derecho Adelante
void adelante(){
  Serial.println("-> AVANZAR (F)");
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

// Motor Izquierdo Atrás, Motor Derecho Atrás
void atras(){
  Serial.println("-> RETROCEDER (B)");
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

// Giro sobre su eje: Motor Izquierdo Adelante, Motor Derecho Atrás
void izquierda(){
  Serial.println("-> IZQUIERDA (L)");
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

// Giro sobre su eje: Motor Izquierdo Atrás, Motor Derecho Adelante
void derecha(){
  Serial.println("-> DERECHA (R)");
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

// Alterna entre velocidad normal y máxima potencia (Nitro)
void cambiarTurbo(){
  turbo = !turbo; // Cambia el estado (si estaba en falso pasa a verdadero, y viceversa)
  if(turbo){
    Serial.println("-> NITRO ACTIVADO");
    analogWrite(ENA, VELOCIDAD_TURBO);
    analogWrite(ENB, VELOCIDAD_TURBO);
  } else {
    Serial.println("-> NITRO DESACTIVADO");
    analogWrite(ENA, VELOCIDAD_NORMAL);
    analogWrite(ENB, VELOCIDAD_NORMAL);
  }
}

// Apaga todos los pines de dirección para frenar los motores
void detener(){
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}