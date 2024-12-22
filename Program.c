/*	
-----------------------------------------------------------------------------------

						DETECTOR DE INUNDACIONES

Owners: Ezequiel Seall / Nicolás Quintana

-----------------------------------------------------------------------------------

*/

#define SRAM_SIZE		((uint32_t) 0x00005000)		//Tamano de la memoria
#define SRAM_BASE		((uint32_t) 0x20000000)		//Direccion base de la memoria.
#define STACKINIT		((interrupt_t)(SRAM_BASE+SRAM_SIZE))	// Inicialización de pila

// Definiciones de tipos de datos específicos
typedef int			   int32_t;
typedef short		   int16_t;
typedef char		   int8_t;
typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;

// Puntero a función, utilizado para interrupciones
typedef void(*interrupt_t)(void);	

typedef union {
	uint8_t  byte[4];
	uint16_t hword[2];
	uint32_t word;
} word_t;

typedef word_t page[0x400/sizeof(uint32_t)];

//mapa de memoria
enum {TIM2	= 0, TIM3  = 1, TIM4  = 2 };
enum {GPIOA = 0, GPIOB = 1, GPIOC = 2, GPIOD = 3, GPIOE = 4, GPIOF = 5 };
enum {DMA1	= 0 };
enum {CHN1	= 0, CHN2  = 1, CHN3  = 2, CHN4  = 3, CHN5	= 4, CHN6  = 5, CHN7 = 6, CHN8 = 7 };
enum {ADC1	= 0 };
//Perifericos
struct {

//Descripcion de timers
	union {
		struct {
			uint32_t CR1;
			uint32_t CR2;
			uint32_t SMCR;
			uint32_t DIER;
			uint32_t SR;
			uint32_t EGR;
			uint32_t CCMR1;
			uint32_t CCMR2;
			uint32_t CCER;
			uint32_t CNT;
			uint32_t PSC;
			uint32_t ARR;
			uint32_t RES1;
			uint32_t CCR1;
			uint32_t CCR2;
			uint32_t CCR3;
			uint32_t CCR4;
			uint32_t BDTR;
			uint32_t DCR;
			uint32_t DMAR;
		} REGs;
		page reserved;
	} TIMs[3];

	word_t reserved1[(0x40002800-0x40000c00)/sizeof(word_t)];	// Espacio reservado para mantener la alineación de las direcciones
	page RTC;
	page WWDG;
	page IWDG;
	word_t reserved2[(0x40003800-0x40003400)/sizeof(word_t)];
	page SPI2;
	word_t reserved3[(0x40004400-0x40003c00)/sizeof(word_t)];
	page USART[2];
	word_t reserved4[(0x40005400-0x40004c00)/sizeof(word_t)];
	page I2C[2];
	page USB;
	page USBCAN_SRAM;
	page bxCAN;
	word_t reserved5[(0x40006c00-0x40006800)/sizeof(word_t)];
	page BKP;
	page PWR;
	word_t reserved6[(0x40010000-0x40007400)/sizeof(word_t)];

	page AFIO;
	page EXTI;

//Estructura del GPIOs (pines entrada y salida)
	union {		
		struct {	  		
			uint32_t CRL;
			uint32_t CRH;
			uint32_t IDR;
			uint32_t ODR;
			uint32_t BSRR;
			uint32_t BRR;
			uint32_t LCKR;
		} REGs;
		page reserved;
	} GPIOs[5];
	word_t reserved7[(0x40012400-0x40011C00)/sizeof(word_t)];

//Estructura del ADC
	union {	
		struct { 			
			uint32_t SR;
			uint32_t CR1;
			uint32_t CR2;
			uint32_t SMPR1;
			uint32_t SMPR2;
			uint32_t JOFR[4];
			uint32_t HTR;
			uint32_t LTR;
			uint32_t SQR1;
			uint32_t SQR2;
			uint32_t SQR3;
			uint32_t JSQR;
			uint32_t JDR[4];
			uint32_t DR;
		} REGs;
		page reserved;
	} ADC[2];
	page TIM1;
	page SPI1;
	word_t reserved8[(0x40013800-0x40013400)/sizeof(word_t)];
	union  {
		struct {
			uint32_t SR;
			uint32_t DR;
			uint32_t BRR;
			uint32_t CR1;
			uint32_t CR2;
			uint32_t CR3;
			uint32_t GTPR;
		} REGs;
		page reserved;
	} USART1;
	word_t reserved9[(0x40020000-0x40013C00)/sizeof(word_t)];
	union {
		struct {
			uint32_t ISR;
			uint32_t IFCR;
			struct {
				uint32_t CCR;
				uint32_t CNDTR;
				uint32_t CPAR;
				uint32_t CMAR;
				uint32_t RESERVED;
			} CHN[8];
		} REGs;
		page reserved;
	} DMAs[1];
	word_t reservedA[(0x40021000-0x40020400)/sizeof(word_t)];

// Estructura de registros de control de reloj RCC
	union {
		struct { 				
			uint32_t CR;
			uint32_t CFGR;
			uint32_t CIR;
			uint32_t APB2RSTR;
			uint32_t APB1RSTR;
			uint32_t AHBENR;
			uint32_t APB2ENR;
			uint32_t APB1ENR;
			uint32_t BDCR;
			uint32_t CSR;
			uint32_t AHBRSTR;
			uint32_t CFGR2;
		} REGs;
		page reserved;
	} RCC;

} volatile *const DEVMAP = (void *) 0x40000000;	// Dirección base para periféricos
 
// Macros para habilitar y limpiar interrupciones
#define ENA_IRQ(IRQ) {CTX->NVIC.REGs.ISER[((uint32_t)(IRQ) >> 5)] = (1 << ((uint32_t)(IRQ) & 0x1F));}
#define CLR_IRQ(IRQ) {CTX->NVIC.REGs.ICPR[((uint32_t)(IRQ) >> 5)] = (1 << ((uint32_t)(IRQ) & 0x1F));}

//Estructura de perifericos internos del cortex
struct {
	word_t reversed0[(0xe000e010-0xe0000000)/sizeof(word_t)];
//Estructura del systick
	union {					
		struct {
			uint32_t CSR;
			uint32_t RVR;
			uint32_t CVR;
			uint32_t CALIB;
		} REGs;
	} SYSTICK;
	word_t reversed1[(0xe000e100-0xe000e020)/sizeof(word_t)];

// Estructura de interrupciones NVIC
	union {
		struct {
			uint32_t ISER[8];
			uint32_t RES0[24];
			uint32_t ICER[8];
			uint32_t RES1[24];
			uint32_t ISPR[8];
			uint32_t RES2[24];
			uint32_t ICPR[8];
			uint32_t RES3[24];
			uint32_t IABR[8];
			uint32_t RES4[56];
			uint8_t  IPR[240];
			uint32_t RES5[644];
			uint32_t STIR;
		} REGs;
	} NVIC;
} volatile *const CTX = ((void *) 0xE0000000);

enum IRQs {
	IRQ_ADC1_2	  = 18,	
};

//Declaracion de funciones
int  main(void);
void handler_systick(void);
void handler_adc1_2(void);

//Declaracion del vector de interrupciones
const interrupt_t vector_table[] __attribute__ ((section(".vtab"))) = {
	STACKINIT,			// 0x0000_0000 Stack Pointer
	(interrupt_t) main, // 0x0000_0004 Reset
	0,					// 0x0000_0008
	0,					// 0x0000_000C
	0,					// 0x0000_0010
	0,					// 0x0000_0014
	0,					// 0x0000_0018
	0,					// 0x0000_001C
	0,					// 0x0000_0020
	0,					// 0x0000_0024
	0,					// 0x0000_0028
	0,					// 0x0000_002C
	0,					// 0x0000_0030
	0,					// 0x0000_0034
	0,					// 0x0000_0038
	handler_systick,	// 0x0000_003C System tick timer
	0,					// 0x0000_0040
	0,					// 0x0000_0044
	0,					// 0x0000_0048
	0,					// 0x0000_004C
	0,					// 0x0000_0050
	0,					// 0x0000_0054
	0,					// 0x0000_0058
	0,					// 0x0000_005C
	0,					// 0x0000_0060
	0,					// 0x0000_0064
	0,					// 0x0000_0068
	0,					// 0x0000_006C
	0,					// 0x0000_0070
	0,					// 0x0000_0074
	0,					// 0x0000_0078
	0,					// 0x0000_007C
	0,					// 0x0000_0080
	0,					// 0x0000_0084
	handler_adc1_2,		// 0x0000_0088 ADC
	0,					// 0x0000_008C
	0,					// 0x0000_0090
	0,					// 0x0000_0094
	0,					// 0x0000_0098
	0,					// 0x0000_009C
	0,					// 0x0000_00A0
	0,					// 0x0000_00A4
	0,					// 0x0000_00A8
	0,					// 0x0000_00AC
	0,					// 0x0000_00B0 
	0,					// 0x0000_00B4
	0,					// 0x0000_00B8
	0,					// 0x0000_00BC
	0,					// 0x0000_00C0
	0,					// 0x0000_00C4
	0,					// 0x0000_00C8
	0,					// 0x0000_00CC
	0,					// 0x0000_00D0
	0,					// 0x0000_00D4
	0,					// 0x0000_00D8
	0,					// 0x0000_00DC
};


volatile int8_t tick;

//Funcion controladora de systick
void handler_systick(void)	
{
	tick = 1;	// Marco que hubo una interrupcion							
}

volatile int8_t adc_rdy;

//Funcion controladora del ADC
void handler_adc1_2(void)
{
	adc_rdy = 1;											// Marco que el ADC completó su conversión.
	
	DEVMAP->ADC[ADC1].REGs.SR &= ~(1 << 1);					//Limpia el bit EOC
															
	CLR_IRQ(IRQ_ADC1_2); 									//Limpia el estado pendiente de interrupcion									
} 

uint32_t count;

int main(void)
{
//Configuracion del RCC
	DEVMAP->RCC.REGs.CR   |= (1 << 16);						// Habilita el oscilador HSE					
	while (!(DEVMAP->RCC.REGs.CR & (1 << 17)));				// Oscilador esta listo
	DEVMAP->RCC.REGs.CR   &= ~(1 << 24);					

	DEVMAP->RCC.REGs.CFGR |= (0b0100 << 18);				//Configura la fuente PLL
	DEVMAP->RCC.REGs.CFGR |=  (1 << 16);					//Selecciona HSE como entrada para el PLL.
	
	DEVMAP->RCC.REGs.CR   |=  (1 << 24);					//Habilita el PLL.
	while (!(DEVMAP->RCC.REGs.CR & (1 << 25)));				//Espera hasta que el PLL esté listo.

	DEVMAP->RCC.REGs.CFGR |= (0b1000 << 4);					//Establece la frecuencia a 24 MHz.
	DEVMAP->RCC.REGs.CFGR |= (0b10 << 0);					//Selecciona el PLL como la fuente del reloj del sistema.
															
	while (!(DEVMAP->RCC.REGs.CFGR & (0b10 << 2)));			

//Configuracion del Systick
	DEVMAP->RCC.REGs.APB2ENR |= (1 << 4);					//Habilito el reloj para el puerto GPIOC

	CTX->SYSTICK.REGs.CSR  = 0x00000;						//Limpio el registro de control del SysTick para reiniciar su configuración								
	CTX->SYSTICK.REGs.CSR |= (1 << 1);						//Habilito la interrupción del SysTick					
	CTX->SYSTICK.REGs.RVR = 3000000;						//Configuro el valor de recarga del temporizador en 3,000,000 ciclos, equivalente a 1 segundo
	CTX->SYSTICK.REGs.CSR |= (1 << 0);						//Inicio el SysTick

	CTX->SYSTICK.REGs.CVR = 0;								//La instruccion limpia el registro poniendolo a cero, al igual que el COUNTFLAG del registro de control del systick.

// Configuracion del ADC
	DEVMAP->RCC.REGs.CFGR |= (0b00 << 14);					//Configuro el prescaler del ADC

	DEVMAP->RCC.REGs.APB2ENR |= (1 << 9);					//Habilito el reloj del ADC.
	DEVMAP->RCC.REGs.APB2ENR |= (1 << 4);					//Habilito el reloj para el puerto GPIOA

	DEVMAP->GPIOs[GPIOA].REGs.CRL &= 0xFFFFFF0F;			//Configuro el pin A1 como entrada analógica
														
	DEVMAP->ADC[ADC1].REGs.CR2 |= (1 << 0);					//Habilito el ADC  
	DEVMAP->ADC[ADC1].REGs.CR1	&= ~(1 << 8);				//Deshabilito la verificación de interrupciones para el modo de escaneo

	DEVMAP->ADC[ADC1].REGs.SQR3 &= ~0xFFFFFFFF;				//Limpio el registro de secuencias del ADC
	DEVMAP->ADC[ADC1].REGs.SQR3 |= (0b0001 << 0);			

	DEVMAP->ADC[ADC1].REGs.CR2	&= ~(1 << 1);				//Configuro el ADC en modo de conversión única
	DEVMAP->ADC[ADC1].REGs.CR2	&= ~(1 << 11);				//Configuro la alineación de los datos a la derecha

	DEVMAP->ADC[ADC1].REGs.CR1	|= (1 << 5);				//Habilito la interrupción de fin de conversión (EOC)
	ENA_IRQ(IRQ_ADC1_2);									//Habilito las interrupciones del ADC
															
//Configuracion de GPIOs
	DEVMAP->RCC.REGs.APB2ENR |= (1 << 2);					//Habilito el reloj del puerto GPIOA
	DEVMAP->RCC.REGs.APB2ENR |= (1 << 3);					//Habilito el reloj del puerto GPIOB

	DEVMAP->GPIOs[GPIOA].REGs.CRH = 0x33333333;				//Configuro los HIGH de los puerto A como salida						
	DEVMAP->GPIOs[GPIOB].REGs.CRH = 0x33333333;				
	DEVMAP->GPIOs[GPIOB].REGs.CRL = 0x33333333;				//Configura todos los pines del puerto B como salida
	DEVMAP->GPIOs[GPIOC].REGs.CRH = 0x33333333;				
	DEVMAP->GPIOs[GPIOC].REGs.CRL = 0x33333333;				//Configura todos los pines del puerto C como salida

	{
//Loop principal
		enum states {S_SYSTICK, S_ADC}; 					//Define los estados del sistema como SysTick y ADC
		enum states state;
//Declaro variables para el control del ADC y cálculos de porcentaje
		uint32_t adc_val; 
		uint32_t cota_inf=70;
		uint32_t cota_sup=90;
		uint32_t porcentaje;
		int32_t decena;
		int32_t unidad;
		uint32_t suma;
//Controlo interrupciones
		tick = 0;                                          

		state = S_SYSTICK;

		DEVMAP->GPIOs[GPIOB].REGs.ODR = 0x8800;

		for(;;) {
//Switch para cambiar entre ADC y Systick
			switch (state) {
			case S_SYSTICK :
				if (tick == 1) {							//Si entra en la condicion hubo una interrupcion por tiempo
				
					DEVMAP->ADC[ADC1].REGs.CR2 |= (1 << 0);	//Inicia la conversión ADC 
															
					tick = 0;								
					adc_rdy = 0; 					
					state = S_ADC;							//Cambio al estado ADC
				}
				break;
			case S_ADC:
				if (adc_rdy == 1) {							//Si entro en esta condicion trabajo con los valores del ADC
					
					adc_val	= DEVMAP->ADC[ADC1].REGs.DR;	//Guardo el resultado de la conversion (registro 0x4001 244C)
					porcentaje = (0.02417582 * adc_val);	//Realizo el ajuste lineal
					
					decena = porcentaje / 10;				
					unidad = porcentaje % 10;				
					decena = decena * 4096;					//Acomodo la decena para la salida por el puerto
					unidad = unidad * 256;					//Acomodo la unidad para la salida por el puerto
					suma = decena + unidad;
					DEVMAP->GPIOs[GPIOB].REGs.ODR = suma;	//La salida es por los 8 valores mas significativos del puerto B
					
					DEVMAP->GPIOs[GPIOA].REGs.ODR |= (1 << 10); //Pongo en 1 el pin A10
					if (porcentaje < cota_inf) {
						DEVMAP->GPIOs[GPIOC].REGs.ODR |= (1 << 15);  //Pongo en 1 la salida C15
						DEVMAP->GPIOs[GPIOA].REGs.ODR &= ~(1 << 10);
					} else {
						DEVMAP->GPIOs[GPIOC].REGs.ODR &= ~(1 << 15); 
						DEVMAP->GPIOs[GPIOA].REGs.ODR &= ~(1 << 10);
					}
					
					if (porcentaje  <= cota_sup && porcentaje  > cota_inf) {
						DEVMAP->GPIOs[GPIOC].REGs.ODR |= (1 << 14);  // Pongo en alto la salida C14
						DEVMAP->GPIOs[GPIOA].REGs.ODR &= ~(1 << 10);
					} else {
						DEVMAP->GPIOs[GPIOC].REGs.ODR &= ~(1 << 14);
						DEVMAP->GPIOs[GPIOA].REGs.ODR &= ~(1 << 10);
					}
					
					if (porcentaje  > cota_sup) {
						DEVMAP->GPIOs[GPIOC].REGs.ODR |= (1 << 13);  // Pongo en alto la salida C13
						DEVMAP->GPIOs[GPIOA].REGs.ODR |= (1 << 10);  ////Pongo en 1 el pin A10 encendiendo la alarma
					}
					else {
						DEVMAP->GPIOs[GPIOC].REGs.ODR &= ~(1 << 13);
						DEVMAP->GPIOs[GPIOA].REGs.ODR &= ~(1 << 10);
					}

					state = S_SYSTICK;						//Cambio al estado systick
				}
				break;
			}
		}
	}
	
	return 0;
}