/*
 * oled.h
 *
 *  Created on: 2/10/2024
 *      Author: lbarreras
 */

#ifndef OLED_H_
#define OLED_H_

/*
 * Direccion del esclavo OLED en el bus I2C. Esta direccion es usada
 * por el microcontrolador para comunicarse con la pantalla. La mayoria
 * de las pantallas OLED manejan esta direccion por defecto, que es 0x3C.
 * Permite enviar y recibir datos de configuracion y visualizacion.
 */
#define OLED_ADRESS 0x3C

/*
 * Byte de control que indica que se enviara un comando a la pantalla OLED.
 * Estos comandos son usados para realizar configuraciones internas como
 * ajustar el brillo, el encendido, la orientacion, entre otros aspectos
 * importantes del display.
 */
#define OLED_CONTROLBYTE_CONFIG  0x00

/*
 * Byte de control que indica que los siguientes datos son para visualizar
 * en la pantalla. Estos datos pueden incluir pixeles, texto o graficos
 * que se veran reflejados directamente en el display una vez enviados.
 */
#define OLED_CONTROLBYTE_DISPLAY 0x40

/*
 * Estas constantes definidas para las columnas estan comentadas, ya que
 * probablemente no se usan en la implementacion actual. Representan las
 * direcciones de columnas para situar graficos o texto en ubicaciones
 * especificas de la pantalla.
 */
// #define COL_1 0x00  // Columna 1
// #define COL_2 0x20  // Columna 2
// #define COL_3 0x40  // Columna 3
// #define COL_4 0x60  // Columna 4

/*
 * Envia una secuencia de bytes (datos) a la OLED utilizando el protocolo
 * I2C. La funcion toma un puntero al handler I2C, el array con los datos
 * que se desean enviar, y el tamaño del array. Esto es util para dibujar
 * en el display, como cuando se escribe texto o graficos.
 */
void sendDataBytes(I2C_Handler_t *ptrHandlerI2C, char *dataToSent, uint8_t sizeArray);

/*
 * Envia un unico byte de comando a la OLED a traves del bus I2C. Este comando
 * puede ser usado para modificar configuraciones del display, como apagarlo,
 * cambiar el contraste o mover el cursor a una posicion especifica para dibujar.
 */
void sendCommandByte(I2C_Handler_t *ptrHandlerI2C, char command);

/*
 * Inicializa la pantalla OLED configurando los parametros esenciales para su
 * funcionamiento. Esto incluye establecer la resolucion, orientacion, y otras
 * configuraciones basicas. Esta funcion debe ser llamada antes de cualquier
 * intento de dibujar o enviar datos a la pantalla.
 */
void startOLED(I2C_Handler_t *ptrHandlerI2Ctr);

/*
 * Alterna el estado de la pantalla OLED entre encendido y apagado. Esta
 * funcion permite controlar si el contenido actual del display se muestra
 * o se oculta, sin borrar los datos almacenados en la memoria de la OLED.
 */
void toggleDisplay(I2C_Handler_t *ptrHandlerI2Ctr);

/*
 * Limpia completamente la pantalla OLED, borrando cualquier contenido
 * visible. La pantalla se pone en negro enviando un comando para apagar
 * los pixeles. Util para reiniciar la pantalla sin reconfigurarla.
 */
void clearDisplay(I2C_Handler_t *ptrHandlerI2Ctr);

/*
 * Detiene el funcionamiento de la pantalla OLED, apagandola completamente.
 * A diferencia de toggleDisplay, esta funcion apaga el display y detiene
 * su comunicacion, por lo que ya no acepta comandos hasta ser reiniciada.
 */
void stopOLED(I2C_Handler_t *ptrHandlerI2Ctr);

/*
 * Establece la pagina (o fila) en la que se dibujara en la pantalla OLED.
 * Las pantallas OLED suelen dividirse en paginas de 8 bits de alto, lo que
 * permite escribir texto o graficos en ubicaciones especificas del display.
 */
void setPage(I2C_Handler_t *ptrHandlerI2Ctr, uint8_t page);

/*
 * Define la direccion de la columna en la que se empezara a escribir en la
 * pantalla. Esto permite situar el cursor en la ubicacion horizontal deseada
 * antes de enviar datos, facilitando el control preciso del contenido.
 */
void setColumnAddress(I2C_Handler_t *ptrHandlerI2Ctr, uint8_t page);

/*
 * Convierte un caracter en un array de bytes que representa su forma en
 * pixeles para poder ser dibujado en la pantalla OLED. Este array puede
 * ser utilizado luego para enviar los datos correspondientes a la pantalla.
 */
char* letterTochar(uint8_t character);

/*
 * Dibuja un mensaje completo en la pantalla OLED. Recorre el mensaje
 * caracter por caracter, convirtiendolo y enviandolo a la pantalla.
 * Utiliza la funcion letterTochar para cada caracter en el mensaje.
 */
void drawMSG(I2C_Handler_t *ptrHandlerI2Ctr, char *msg);

/*
 * Dibuja un mensaje en una pagina especifica de la pantalla OLED. Esto
 * es util para mostrar texto o graficos en una seccion determinada sin
 * modificar otras partes del contenido ya dibujado en la pantalla.
 */
void drawSinglePageMSG(I2C_Handler_t *ptrHandlerI2Ctr, char *msg, uint8_t page);

#endif /* OLED_H_ */
