# Detector de Inundaciones STM32-Bluepill
Detector de inundaciones con alarma

# Objetivo
El principal objetivo de este proyecto es desarrollar un sistema de detección y alerta de inundaciones en tiempo real, diseñado para monitorear de manera continua el nivel de agua en zonas con riesgo de inundación, como un cuarto de cable u otras áreas vulnerables. Para esto se utilizó un microcontrolador STM32 Bluepill. La misma fue programada con un código bare-metal en lenguaje C para configurar las instrucciones y los periféricos a utilizar.

# Introducción
Un sensor de nivel de agua mide la cantidad de agua que se encuentra  tapando al sensor al momento de la medición. Esto se realiza debido a que el PCB está hecho de placas conductivas, es por esto que cuando el agua alcanza un determinado nivel la conductividad entre placas cambia junto con la medición. En la figura a continuación se presenta el sensor a utilizar en la medición. El sensor cuenta con un LED el cual marca el estado encendido o apagado del mismo y tres pines de salida. El primero, en orden de arriba hacia abajo es el GND o tierra, el segundo es para la tensión de alimentación del mismo y el último es la salida de información analógica. Destacar que para que el sensor funcione correctamente este debe estar alimentado con 5V.

Por otra parte, se utilizaran tres LEDs con sus respectivas resistencias como indicadores del monitoreo, los tres niveles de monitoreo serán representado con cada color de LED siguiendo la distribución que se presenta a continuación:

Azul: Nivel de agua aceptable.
Amarillo: Nivel de agua elevado.
Rojo: Nivel de agua crítico.

En caso de que el nivel del agua alcance el estado crítico, además se encenderá una alarma para informar al usuario de la situación.

Por último, el dispositivo también posee 2 siete segmentos para informar acerca del nivel de agua en tiempo real. Los mismos no se encuentran conectados directamente a la placa sino que se encuentran conectados cada uno con su respectivo circuito integrado para luego estos ser conectados a la STM32 Bluepill. Los integrados (CD4511) funcionan como conversores BCD (Binary Coded Decimal), es decir la información que es enviada desde la placa hasta los 7 segmentos es convertida a decimal por estos.

# Desarrollo
En la configuración de los puertos del microcontrolador, se define que los puertos B8 hasta B15 estarán en modo salida, para los 7 segmentos. Por otro lado, el puerto A1 se establece como entrada para recibir los datos proporcionados por el sensor de detección de agua. Este puerto se configura como entrada analógica, ya que el sensor entrega valores que deben ser leídos de esta manera.

El microcontrolador utiliza un conversor ADC para transformar la señal analógica del sensor en valores digitales. A partir de estos valores, se calcula el porcentaje correspondiente al nivel de agua mediante una ecuación lineal que toma como referencia los valores extremos del sensor. La salida del sensor se escala de forma que el porcentaje calculado varía entre 0% y 99%. Los valores obtenidos se dividen en decenas y unidades, que son codificados para controlar los segmentos de los displays mediante los puertos B.
Dado que se busca digitalizar los valores analógicos del sensor en términos de un porcentaje se realizó un ajuste lineal. Como se mencionó previamente, se tomará su valor más bajo hasta el más alto del ADC ajustándose a un intervalo de 0% a  99%. Para hacerlo, se calcula un factor que transforma cada lectura del ADC en una proporción equivalente dentro del rango porcentual. Este factor se obtiene dividiendo el rango porcentual (de 0 a 99) por el número máximo de pasos o valores que el ADC puede medir. Donde el valor 0 del ADC va a ser el  0% y el 4096 va a ser el 99%, siempre siguiendo una ecuación del tipo y=ax+b .

Luego para enviar los datos a los 7 segmentos, el valor del porcentaje se descompone en dos partes: la decena y la unidad. Primero, el porcentaje se divide entre 10 para obtener la decena y se utiliza el operador módulo para obtener el valor de la unidad. Luego, estos valores se ajustan a un formato hexadecimal adecuado para mostrarse en los displays. La decena se multiplica por 4096 para convertirla en un valor que ocupe los bits más significativos, mientras que la unidad se multiplica por 256 para ajustarla a los siguientes bits. Estos dos valores ajustados se suman y se almacenan en la variable "suma", enviandose a los puertos correspondientes. Esto permite visualizar el porcentaje en los displays.

Adicionalmente, el dispositivo incluye indicadores visuales con LEDs en los puertos C, que señalan si el nivel de agua está por debajo de un límite inferior, entre dos límites establecidos o por encima de un límite superior. Siendo el puerto C-15 el limite inferior (Azul), los valores considerados intermedios C-14 (amarillo) y el limite superior C-13 (rojo). Estos indicadores se encienden o apagan en función de comparaciones realizadas con el porcentaje calculado.También incorpora un sistema de alarma basado en el nivel de agua detectado. Esta alarma está controlada a través de un pin del puerto A (A-10). Si el nivel de agua supera el límite superior definido, la alarma se activa junto con el LED correspondiente, además de mantener la señal en alto en el pin destinado a la alarma. Por el contrario, si el nivel de agua está por debajo de este límite, la señal se desactiva.

El programa opera mediante un esquema basado en dos estados: uno para gestionar las interrupciones por tiempo (SysTick) y otro para realizar la lectura y procesamiento de datos del ADC. Este sistema de estados asegura que las lecturas y actualizaciones se realicen de manera sincronizada y eficiente, alternando entre la adquisición de datos del sensor y la actualización de los displays e indicadores.

# Conclusión
En conclusión, a lo largo de trabajo se desarrolló en el diseño y desarrollo de un sistema de detección y alerta de inundaciones en tiempo real, utilizando un microcontrolador STM32 Bluepill programado en bare-metal. El sistema integra un sensor de nivel de agua cuya señal analógica es procesada para calcular el porcentaje del nivel detectado, representado en displays de siete segmentos y acompañado por indicadores visuales con LEDs que reflejan diferentes niveles de alerta. Además, incluye una alarma sonora para niveles críticos.
