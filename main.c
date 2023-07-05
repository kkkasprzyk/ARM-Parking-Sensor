/// PROJEKT z czujnikiem ultradzwiekowym ////
//Autorzy: Krzysztof Kasprzyk & Rafal Piwowarczyk

#include "MKL05Z4.h" 
#include "frdm_bsp.h"
#include "TPM.h"
#include "lcd1602.h"								
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pit.h"

#define ECHO 7  /// Wyjscie nadajace sygnal ultradzwiekowy
#define TRIG 6 /// Wejscie odbierajace sygnal ultradzwiekowy
#define S1_MASK	(1<<9)		// Maska dla klawisza S1
#define S2_MASK	(1<<10)		// Maska dla klawisza S2
#define S3_MASK	(1<<11)		// Maska dla klawisza S2
#define S4_MASK	(1<<12)		// Maska dla klawisza S2
#define LED_G		(1<<9)		// Maska dla diody 
#define LED_B		(1<<10)		// Maska dla diody 

float milimetry = 0.0447065688987272;
uint32_t ultrawynik_mm=0;
uint32_t zmiana=-1;
uint32_t sendPulse(uint32_t delay);
char OutputArray[10];		
//Glosnik - zmienne
float freq;
uint16_t	mod_curr=0;	// Poczatkowy MOD
uint8_t k_curr=50;					// Poczatkowy wspólczynnik wypelnienia K=50%
uint16_t	ampl;
/////

//Funkcje///////////////////////
void PortMux(void);
void SetupPins(void);
void screen_refresh(uint32_t x);
void MaskEnable(void);
void speaker(uint16_t mod);
void LED_Init(void);
void Klaw_Init(void);
///////////////////////



int main(void) {
	
	// Inicjalizacja funkcji
	PWM_Init();		
	Klaw_Init();
	LED_Init();
	MaskEnable();
	PortMux();																				 
	SetupPins();
	LCD1602_Init();
	
	//Inicjalizacja wyswietlacza - poczatkowa
	LCD1602_Backlight(TRUE);
	LCD1602_ClearAll();
	LCD1602_SetCursor(5,0);
	LCD1602_Print("Czujnik");
	LCD1602_SetCursor(3,1);
	LCD1602_Print("Odleglosci");
	
	while(1){
			
			if(zmiana==0){ // milimetry
				
				TPM0->CONTROLS[3].CnV = 0;
				PTB->PDOR|=LED_B; PTB->PDOR|=LED_G;	PTB->PCOR|=LED_G;	
				ultrawynik_mm = sendPulse(10)*milimetry; ///milimetry
				
				if(ultrawynik_mm){ // sprawdza czy wynik jest bledny
					sprintf(OutputArray,"%i",ultrawynik_mm);
					LCD1602_ClearAll();
					LCD1602_SetCursor(3,0);
					LCD1602_Print("Odleglosc:");
					LCD1602_SetCursor(5,1);
					LCD1602_Print(OutputArray);
					LCD1602_Print(" mm");
					screen_refresh(500000); // opoznienie potrzebne do wyswietlania
				}
			}
			
			if(zmiana==1){ //centymetry
				
				TPM0->CONTROLS[3].CnV = 0;
				PTB->PDOR|=LED_B; PTB->PDOR|=LED_G;	PTB->PCOR|=LED_B;	
				ultrawynik_mm = sendPulse(10)*milimetry/10;
					
					if(ultrawynik_mm){// sprawdza wynik 
					sprintf(OutputArray,"%i",ultrawynik_mm);
					LCD1602_ClearAll();
					LCD1602_SetCursor(3,0);
					LCD1602_Print("Odleglosc:");
					LCD1602_SetCursor(5,1);
					LCD1602_Print(OutputArray);
					LCD1602_Print(" cm");
					screen_refresh(500000); // opoznienie potrzebne do wyswietlania
					}
			}
			
			if(zmiana==2){// cale
				
				TPM0->CONTROLS[3].CnV = 0;
				PTB->PDOR|=LED_B; PTB->PDOR|=LED_G;	PTB->PCOR|=LED_B;PTB->PCOR|=LED_G;	
					
				ultrawynik_mm = sendPulse(10)*milimetry/25.4; 
				if(ultrawynik_mm){//sprawdzanie wyniku
					sprintf(OutputArray,"%i",ultrawynik_mm);
					LCD1602_ClearAll();
					LCD1602_SetCursor(3,0);
					LCD1602_Print("Odleglosc:");
					LCD1602_SetCursor(5,1);
					LCD1602_Print(OutputArray);
					LCD1602_Print(" cali");
					screen_refresh(500000); // opoznienie potrzebne do wyswietlania
				}
			}
			
			if(zmiana==3){	/// czujnik Parkowania 
				
				PTB->PDOR|=LED_B; PTB->PDOR|=LED_G;
				
				  ultrawynik_mm = sendPulse(10)*milimetry/10; /// centymetry 
					if(ultrawynik_mm){
					sprintf(OutputArray,"%i",ultrawynik_mm);
					
						// ZAKRESY DZIALANIA CZUJNIKA PARKOWANIA 
									if(ultrawynik_mm<10){  
										LCD1602_SetCursor(10,1);
										LCD1602_Print("X");
												mod_curr=654;
									} 
									
									if(ultrawynik_mm>=10 & ultrawynik_mm<=20) {
										DELAY(50);
										LCD1602_SetCursor(10,1);
										LCD1602_Print("XX");
										mod_curr=654; 
									}
									
									if(ultrawynik_mm>=21 & ultrawynik_mm<=40) {
										DELAY(250);
										LCD1602_SetCursor(10,1);
										LCD1602_Print("XXX");
										mod_curr=654; 
									}
									
									if(ultrawynik_mm>=41 & ultrawynik_mm<=100) {
										DELAY(500);
										LCD1602_SetCursor(10,1);
										LCD1602_Print("XXXX");
										mod_curr=654; 
									}
									
									if(ultrawynik_mm>=101) {
										DELAY(750);
										LCD1602_SetCursor(10,1);
										LCD1602_Print("XXXXX");
										mod_curr=654; 
									}
									
									
					speaker(mod_curr);	 // Funkcja odpowiadajaca za obsluge glosnika korzysta z TPM.c 
					
// Wyswietlanie czujnik PARKOWANIA ///									
					LCD1602_ClearAll();
					LCD1602_SetCursor(0,0);
					LCD1602_Print(">> Parkowanie <<");	
					LCD1602_SetCursor(0,1);
					LCD1602_Print(OutputArray);	
					LCD1602_Print("cm -> ");
					screen_refresh(50000); 
				}
				
			
			}
			
			/// wybor funkcji na przyciskach 
	
			if(!(PTA->PDIR&S1_MASK)) { ///milimetry
					zmiana=0;
			}
			
			if(!(PTA->PDIR&S2_MASK)) { ///centymetry
					zmiana=1;
			}
			
			if(!(PTA->PDIR&S3_MASK)) { ///cale
					zmiana=2;
			}
			
			if(!(PTA->PDIR&S4_MASK)) { ///czujnik parkowania
					zmiana=3;
			}
			
	} // koniec while - petli glownej
}  /// koniec maina 



void speaker(uint16_t mod){

			TPM0->MOD = mod;										
			ampl=((int)mod_curr*k_curr)/100;	// Wspólczynnik wypelnienia k przeliczony dla aktualnego MOD
			TPM0->CONTROLS[3].CnV = ampl; 
			TPM0->SC |= TPM_SC_CMOD(1);							// Wlacz licznik TPM0
			DELAY(100);
			TPM0->CONTROLS[3].CnV = 0;
}
	


void MaskEnable() {
	SIM->SCGC5 |=SIM_SCGC5_PORTB_MASK;
}

void PortMux() {
	PORTB->PCR[   TRIG  ]  |= PORT_PCR_MUX(1);
	PORTB->PCR[   ECHO  ]  |= PORT_PCR_MUX(1);
}



void SetupPins() { 
	PTB->PDDR |= (0 << ECHO);	 // Ustawienie pinu echo jako wejscie																			
  PTB->PDDR |= (1 <<TRIG);		// Ustawienie pinu trig jako wyjscie	
 
}


uint32_t sendPulse(uint32_t delay) {
	
	
	uint32_t counter=0;
	PTB->PDOR |= (1 << TRIG); //wyslamy sygnal na TRIG
	screen_refresh(delay);
	PTB->PCOR |= (1 << TRIG);  // konczy wysylac sygnal
	
	while(PTB->PDIR & (1 << ECHO)){ //jesli odbiera sygnal to wchodzi do while

		counter++;
	
	}
	return counter;
}


void screen_refresh(uint32_t x){   
		for(uint32_t delay = 0; delay < (x*10); delay++) 
	__nop();
}

void LED_Init(void)
{
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;      // Wlaczenie portu B
	PORTB->PCR[9] |= PORT_PCR_MUX(1);	
	PORTB->PCR[10] |= PORT_PCR_MUX(1);
	PTB->PDDR |= (1<<9)|(1<<10);	// Ustaw na 1 bity  9 i 10 – rola jako wyjscia
	PTB->PDOR|= (1<<9)|(1<<10);	// Zgas G i B diody
}



void Klaw_Init(void)
{
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;		// Wlaczenie portu A
	PORTA->PCR[9] |= PORT_PCR_MUX(1);
	PORTA->PCR[10] |= PORT_PCR_MUX(1);
	PORTA->PCR[11] |= PORT_PCR_MUX(1);
	PORTA->PCR[12] |= PORT_PCR_MUX(1);
	PORTA->PCR[9] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	PORTA->PCR[10] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	PORTA->PCR[11] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	PORTA->PCR[12] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
}
