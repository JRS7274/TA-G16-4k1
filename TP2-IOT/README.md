# TP2 - TA - G16
## Enunciado
Mediante la plataforma Wokwi o Arduino IDE o similar diseñar un circuito utilizando una placa Esp32 que permita procesar los siguientes comandos recibidos desde un bot de telegram 
 1. __/start__ - Para iniciar la comunicación con el bot. Esto genera un mensaje de bienvenida indicando los comandos que son posibles de procesar con su descripción respectiva. 
 2. __/led<led><on/off>__ - Que permita encender o apagar un led de la placa de desarrollo indicando en led el GPIO correspondiente al led verde (23) o al azul (2) y la acción on/off. Por ejemplo, el comando: /led23on encenderá el led verde de la placa. 
 3. __/dht22__ – Que permite informar los valores de humedad y temperatura del sensor. 
 4. __/pote__ - Que permite informar el valor de voltaje (0-3.3v) según la lectura del potenciómetro 
 5. __/platiot__ enviar los valores de humedad y temperatura del sensor DHT22 a la plataforma IoT elegida por los estudiantes (Arduino Cloud/ThingSpeak/Otra que se investigue) de modo tal que dichos valores sean visualizados en un dashboard o panel provisto por la plataforma propuesta. 
 6. __/display<conmando>__ - Que permite mostrar el estado de los componentes en el display de la placa de desarrollo. Por ejemplo: si se envía desde Telegram /displayled mostrará el estado

Se valora especialmente el uso de botones en el bot para disparar los comandos anteriores. 

## Algunas consideraciones
Dentro de libraries se encuentra una libreria propia para la lectura del potenciómetro
Para el manejo del display se utilizó la libreria <Adafruit_SH110X.h>
