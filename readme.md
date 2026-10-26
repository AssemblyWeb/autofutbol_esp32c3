# Auto Robot de Fútbol - ESP32 C3 & L298N

Este proyecto consiste en el diseño, conexionado y programación de un prototipo de **Auto Robot para jugar al Fútbol**, desarrollado específicamente para los alumnos de la **Escuela de Educación Secundaria Técnica N° 1 “Crucero General Belgrano”**, dentro del espacio curricular de **Sistemas Tecnológicos 3**.

El objetivo es aplicar conceptos de electrónica digital, control de motores por modulación de ancho de pulso (PWM) y programación de microcontroladores utilizando el chip ESP32 C3.

---

## 🛠️ Componentes Utilizados

El hardware central del robot se basa en la interacción entre la lógica de control y la etapa de potencia:

*   **Microcontrolador:** ESP32 C3 (Encargado de recibir las órdenes de control y generar las señales PWM y digitales).
*   **Driver de Potencia:** Puente H L298N (Módulo encargado de manejar la corriente de los motores y definir el sentido de giro).
*   **Alimentación:** 2x Baterías Li-ion 18650 recargables (Proporcionan entre $7.4\text{V}$ y $8.4\text{V}$ en serie).
*   **Actuadores:** 2x Motores de Corriente Continua (DC) con caja reductora.

---

## ⚙️ ¿Cómo Funciona?

El robot opera dividiendo las tareas en tres etapas clave:

1.  **Control Lógico (ESP32 C3):** El microcontrolador procesa las instrucciones de movimiento (ya sea por comandos de una app, Bluetooth, Wi-Fi o una rutina automática). A partir de esto, activa sus pines digitales para definir direcciones y pines con salida PWM para regular la velocidad.
2.  **Etapa de Potencia (Puente H L298N):** Dado que los pines del ESP32 solo entregan $3.3\text{V}$ y muy pocos miliamperios, el L298N actúa como un interruptor de alta velocidad. Utiliza la energía directa de las baterías 18650 para mover los motores, siguiendo las órdenes de baja señal recibidas del ESP32.
3.  **Alimentación Compartida:** El circuito aprovecha el regulador integrado de $5\text{V}$ del Puente H para alimentar la placa ESP32 por su pin `5V (In)`. *(Nota de seguridad: Recuerda que mientras el jumper de 5V esté puesto, el robot se autoalimenta y no se debe conectar el cable USB a la computadora).*

---

## 📏 Reglamento Técnico de Competición

Para participar en los encuentros de fútbol robótico, el prototipo debe cumplir estrictamente con las siguientes especificaciones técnicas de diseño mecánico:

*   **Dimensiones de la Base:** Máximo de $20 \times 20 \text{ cm}$ (Ancho x Largo).
*   **Peso Máximo:** Hasta $1.5 \text{ kg}$ con baterías incluidas.
*   **Profundidad de la Pala (Mecanismo de empuje):** No debe superar los $35 \text{ mm}$.
*   **Margen de Tolerancia:** Se permite un $\pm10\%$ de margen en las mediciones mecánicas finales durante la inspección técnica previa al partido.

---

## 🔌 Diagrama de Conexión (Pinout)

La distribución de los pines configurada en el código y el hardware es la siguiente:

| ESP32 C3 Pin | Dirección | Módulo L298N | Función |
| :--- | :---: | :--- | :--- |
| **5V (In)** | $\leftarrow$ | **5V (Lógico)** | Alimentación lógica del microcontrolador |
| **G (GND)** | $\leftrightarrow$ | **GND** | Masa Común (Esencial para la referencia) |
| **Pin D7** | $\rightarrow$ | **ENA** | Control de Velocidad (PWM) - Motor Izquierdo |
| **Pin D10** | $\rightarrow$ | **ENB** | Control de Velocidad (PWM) - Motor Derecho |
| **Pin D0** | $\rightarrow$ | **IN1** | Dirección 1 - Motor Izquierdo |
| **Pin D1** | $\rightarrow$ | **IN2** | Dirección 2 - Motor Izquierdo |
| **Pin D3** | $\rightarrow$ | **IN3** | Dirección 1 - Motor Derecho |
| **Pin D4** | $\rightarrow$ | **IN4** | Dirección 2 - Motor Derecho |

---

## 🚀 ¿Cómo se Ejecuta el Proyecto?

Para poner en marcha el robot por primera vez, sigue estos pasos:

### 1. Preparación del Entorno
*   Descarga e instala el **Arduino IDE**.
*   Ve a *Archivo > Preferencias* y agrega la URL del gestor de tarjetas de Espressif para habilitar el soporte de ESP32.
*   Instala las placas `esp32` desde el *Gestor de Tarjetas*.

### 2. Carga del Código (Modo Programación)
1.  **Desconecta la batería 18650** del switch general del robot o **quita el Jumper de 5V** del Puente H. *(¡Paso crítico para proteger tu PC!)*.
2.  Conecta el ESP32 C3 a la computadora mediante un cable USB de datos.
3.  Selecciona la placa correspondiente (`ESP32C3 Dev Module`) y el puerto COM asignado.
4.  Carga el código fuente del sketch haciendo clic en el botón **Subir** (Flecha hacia la derecha).

### 3. Puesta en Marcha en la Pista
1.  Una vez finalizada la carga con éxito, desconecta el cable USB.
2.  Asegúrate de que el **Jumper de 5V** del Puente H esté colocado en su posición.
3.  Coloca el robot en el suelo y enciende el interruptor de las baterías 18650.
4.  ¡El sistema iniciará de forma autónoma listo para el juego!

