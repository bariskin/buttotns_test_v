#include <SPI.h>
#include "GyverTimers.h"
#include <AmperkaFET.h>

#define KSVET  2
#define KLEV   3
#define KPRAV  4
#define KAVAR  5
#define KGUD   6
#define KSTOP  7
#define KDEK   8
#define KREZ   9


#define  M1  0
#define  M2  1
#define  M3  2
#define  M4  3
#define  M5  4
#define  M6  5
#define  M7  6
#define  M8  7
#define  M9  0
#define  M10 1
#define  M11 2
#define  M12 3
#define M13 4
 
#define ON 1
#define OFF  0
#define IDL 2

#define PIN_CS  A0

 byte counterKSVET = 0;
 byte counterKLEV  = 0;   
 byte counterKPRAV = 0;
 byte counterKAVAR = 0; 
 byte counterKGUD  = 0; 
 byte counterKSTOP = 0;
 byte counterKDEK  = 0; 
 byte counterKREZ  = 0;   
 byte counterKAVAR_second = 0;

/* дефайны констант */
 #define KSVET_PRESS_0 0
 #define KSVET_PRESS_1 1
 #define KSVET_PRESS_2 2
 #define KSVET_PRESS_3 3
 #define KSVET_PRESS_4 4
 byte counterKSVET_PRESS = KSVET_PRESS_0;
 
 #define KLEV_PRESS_0 0
 #define KLEV_PRESS_1 1
 #define KLEV_PRESS_2 2
 byte counterKLEV_PRESS = KLEV_PRESS_0;

 #define KPRAV_PRESS_0 0
 #define KPRAV_PRESS_1 1
 #define KPRAV_PRESS_2 2
 byte counterKPRAV_PRESS = KPRAV_PRESS_0;

 #define KAVAR_PRESS_0 0
 #define KAVAR_PRESS_1 1
 #define KAVAR_PRESS_2 2
 byte counterKAVAR_PRESS = KAVAR_PRESS_0;

 #define KREZ_PRESS_0 0
 #define KREZ_PRESS_1 1
 #define KREZ_PRESS_2 2
 byte counterKREZ_PRESS = KREZ_PRESS_0;

 #define KDEK_PRESS_0 0
 #define KDEK_PRESS_1 1
 #define KDEK_PRESS_2 2
 byte counterKDEK_PRESS = KDEK_PRESS_0;

 byte flagKSVET = OFF;
 byte flagKLEV  = OFF;
 byte flagKPRAV = OFF;
 byte flagKAVAR = OFF; 
 byte flagKGUD  = OFF; 
 byte flagKSTOP = OFF; 
 byte flagKDEK  = OFF; 
 byte flagKREZ  = OFF;

byte stateKSVET  = LOW;
byte stateKLEV   = LOW;
byte stateKPRAV  = LOW;
byte stateKAVAR  = LOW;
byte stateKGUD   = LOW; 
byte stateKSTOP  = LOW; 
byte stateKDEK   = LOW; 
byte stateKREZ   = LOW;

byte current_button; 
byte counterTIMER1_B = 0; 
byte counterTIMER2_A = 0;
int  counterTIMER2_B = 0; 

byte flagtoggleLev   = ON;
byte flagtogglePrav  = ON;
byte flagtoggleKavar = ON;

byte togglecounterLev  = 0;
byte togglecounterPrav = 0;


volatile int counterKSVET_OFF = 0;

byte flagKLEVenable  = OFF;
byte flagKPRAVenable = OFF;
byte flagKAVARenable = OFF;
byte flagKREZenable  = OFF;
byte flagKGUDenable  = OFF; 

byte event_GAB_F_L_R = OFF;
byte event_AVAR = OFF;
byte event_KPRAV_LEV = OFF;

byte mutex = OFF;
          
// создаём объект mosfet для работы со сборкой силовых ключей
// передаём номер пина выбора устройств на шине SPI
// и количество устройств подключённых в цепочке
FET mosfet(PIN_CS, 2);

void setup() {
   pinMode(KSVET, INPUT);
   pinMode(KLEV,  INPUT);
   pinMode(KPRAV, INPUT);
   pinMode(KAVAR, INPUT);
   pinMode(KGUD,  INPUT);
   pinMode(KSTOP, INPUT);
   pinMode(KDEK,  INPUT);
   pinMode(KREZ,  INPUT);
   
   Serial.begin(115200);
  
   Timer1.setPeriod(2000);                 // попадаем раз в 2 ms обработчик 
   Timer1.enableISR(CHANNEL_A);            // Запускаем прерывание (по умолч. канал А)
   Timer1.enableISR(CHANNEL_B);            // Запускаем прерывание (по умолч. канал B)
   Timer2.setPeriod(2000);                 // Попадаем раз в 5 ms обработчик 
  // Timer2.enableISR(CHANNEL_A);
   Timer2.enableISR(CHANNEL_B);
   
   current_button = KSVET;

 // начало работы с силовыми ключами
   mosfet.begin();
}

void loop() {


}

// Прерывание А таймера 1
ISR(TIMER1_A)
 { 
    readButtons(); 
 }

ISR(TIMER1_B)// мигание 90 раз в минуту
{  
  counterTIMER1_B++;
  
  if(counterTIMER1_B > 200)  // 60000ms :180 = 166ms; 180 - это 90 миганий в минуту; 83(раз)  *  2 (ms)  = 166 (ms)  
  {
    counterTIMER1_B = 0;
 /******************************************************/   
    if(flagKLEVenable == ON)
      {  
       if (mutex == OFF)//*************************MUTEX
        { 
         if(flagtoggleLev == ON)
         {
          //Serial.println("KLEV ON");
          mosfet.digitalWrite(0, M7, HIGH);
          flagtoggleLev = OFF;

           togglecounterLev++;
           if(togglecounterLev >= 90)  // 90 миганий Lev и отключить
           {
             togglecounterLev = 0;
             flagKLEVenable = OFF; 

             event_KPRAV_LEV = OFF; 
           }
         }
         else if(flagtoggleLev == OFF)
         {
          //Serial.println("KLEV OFF");
          mosfet.digitalWrite(0, M7, LOW);
          flagtoggleLev = ON;
         } 
        } //*************************MUTEX  
      }
     else if(flagKLEVenable == OFF)
      {
        // Serial.println("KLEV OFF");
         mosfet.digitalWrite(0, M7, LOW);
         flagKLEVenable = IDL; 
         togglecounterLev = 0;
         flagtoggleLev = ON;

         event_KPRAV_LEV = OFF;
      } 
     else if(flagKLEVenable == IDL)
       {
         // находимся здесь при выключенном состоянии Lev
       }
/******************************************************/ 
    if(flagKPRAVenable == ON)
      {  
       if (mutex == OFF)//*************************MUTEX
        {
        if(flagtogglePrav == ON)
         {
          
         // Serial.println("PRAV ON");
          mosfet.digitalWrite(0, M8, HIGH);
          flagtogglePrav = OFF;

           togglecounterPrav++;
           if(togglecounterPrav >= 90)  // 90 миганий Prav и отключить
           {
             togglecounterPrav = 0;
             flagKPRAVenable = OFF;
             
           }
         }
         else if(flagtogglePrav == OFF)
         {
          //Serial.println("PRAV OFF");
          mosfet.digitalWrite(0, M8, LOW);
          flagtogglePrav = ON;
         } 
       } //*************************MUTEX
      }
     else if(flagKPRAVenable == OFF)
      {
          //Serial.println("KPRAV OFF");
          mosfet.digitalWrite(0, M8, LOW);  // выключить Prav
          flagKPRAVenable = IDL;
          togglecounterPrav = 0;  
          flagtogglePrav = ON;

          event_KPRAV_LEV = OFF;
      } 

     else if(flagKPRAVenable == IDL)
       {
         // находимся здесь при выключенном состоянии Prav
       }



   if(flagKAVARenable == ON)
      {
         if(flagtoggleKavar == ON)
         {
         // Serial.println("KAVAR ON");
          mosfet.digitalWrite(1, M9, HIGH); // ключ M9
          mosfet.digitalWrite(0, M7, HIGH); //Serial.println("KLEV ON");  M7
          mosfet.digitalWrite(0, M8, HIGH); //Serial.println("PRAV ON");  M8
          
          flagtoggleKavar = OFF;
         }
         else if(flagtoggleKavar == OFF)
         {
          //Serial.println("KAVAR OFF");
          mosfet.digitalWrite(1, M9, LOW); //M9
          mosfet.digitalWrite(0, M7, LOW); //Serial.println("KLEV OFF");  M7 
          mosfet.digitalWrite(0, M8, LOW); //Serial.println("PRAV OFF");  M8
          
          flagtoggleKavar = ON;
         }   
      }
     else if(flagKAVARenable == OFF)
      {
         //Serial.println("KAVAR OFF");
         mosfet.digitalWrite(1, M9, LOW);
         mosfet.digitalWrite(0, M7, LOW); //Serial.println("KLEV OFF");  
         mosfet.digitalWrite(0, M8, LOW); //Serial.println("PRAV OFF");
         flagKAVARenable = IDL; 
         flagtoggleKavar = ON;   
      } 
     else if(flagKAVARenable == IDL)
       {
         // находимся здесь при выключенном состоянии Lev
       } 
   }  
}

// Прерывание А таймера 2
ISR(TIMER2_A) 
{
}

// Прерывание B таймера 2
ISR(TIMER2_B) 
{
  
  if (flagKGUDenable == ON)
  {
     counterTIMER2_B++;
  
     if(counterTIMER2_B == 2000) // через 10 сек выключить KGUD  5ms * 2000 = 10 сек
       {
         counterTIMER2_B = 0;
         flagKGUDenable = OFF; 
         Serial.println("KGUD OFF");
         mosfet.digitalWrite(1, M10 , LOW);
       }   
   }

  else if (flagKGUDenable == OFF)
  {
    
    
  }
  
}
void readButtons(void)
{
  switch(current_button)
  {
   case KSVET:   //1
   
      HandlerKSVET();
     // goto next state
       current_button = KLEV;

   break;
/***************************/ 
   case KLEV:   //2
   
      HandlerKLEV();
     // goto next state
     current_button = KPRAV;
     
   break;
/***************************/  
 case  KPRAV:  //3
 
   HandlerKPRAV();
  // goto next state
  current_button = KAVAR;
 
 break;
 /***************************/
  case  KAVAR: //4
  
   HandlerKAVAR();
   // goto next state
  current_button = KGUD;
  
 break;
 /***************************/
  case  KGUD:   //5
  
   HandlerKGUD();
   // goto next state
   current_button = KSTOP;
  
 break;
 /***************************/ 
  case  KSTOP:  //6
  
   HandlerKSTOP();
  // goto next state
   current_button = KDEK;
   
 break;
 /***************************/ 
  case  KDEK:   //7
  
  HandlerKDEK();
 // goto next state
  current_button = KREZ;
  
 break;
 /***************************/ 
   case  KREZ:   //8
   
   HandlerKREZ();
  // goto next state
  current_button = KSVET;
  
 break;
 /***************************/   
   }  
}


void HandlerKSVET(void)
{
     stateKSVET = digitalRead(KSVET);
      
     if(stateKSVET) 
      {
        counterKSVET_OFF++;
        
        if((counterKSVET_OFF > 250) && (counterKSVET_PRESS != KSVET_PRESS_0))  // 3 секунды держим и выключаем KSVET
        {
           counterKSVET_PRESS = KSVET_PRESS_0;
           counterKSVET_OFF = 0;
           Serial.println("KSVET OFF");
           mosfet.digitalWrite(0, M1, LOW);  // выключить GAB_F M1
           mosfet.digitalWrite(0, M2, LOW);  // выключить GAB_B M2
           mosfet.digitalWrite(0, M3, LOW);  // выключить GAB_L M3
           mosfet.digitalWrite(0, M4, LOW);  // выключить GAB_R M4
           mosfet.digitalWrite(0, M5, LOW);  // выключить BLIZH M5
           mosfet.digitalWrite(0, M6, LOW);  // выключить DAL   M6

           event_GAB_F_L_R = OFF;
        }
        else
          {
            if(flagKSVET == OFF) // кнопка до этого была отжата
            {
             counterKSVET++;
           
             if(counterKSVET > 2)
             {
              flagKSVET = ON;
              
              if(counterKSVET_PRESS == KSVET_PRESS_0)  // при  первом нажатии
              {
                 counterKSVET_PRESS = KSVET_PRESS_1;
              }
              switch(counterKSVET_PRESS)
              
                {
                 case KSVET_PRESS_1:
                     /* 
                      *  блок выходов габаритов, который включается и выключается в зависимости от KAVAR
                      *  
                     */
                      {
                        Serial.println("GAB_F ON");
                        mosfet.digitalWrite(0, M1, HIGH);
  
                        // Serial.println(" GAB_L ON");
                        mosfet.digitalWrite(0, M3, HIGH);
                      
                       // Serial.println(" GAB_R ON");
                        mosfet.digitalWrite(0, M4, HIGH);
  
                         event_GAB_F_L_R = ON;
                      }  
                    // Serial.println(" GAB_B ON");
                     mosfet.digitalWrite(0, M2, HIGH);
                    
               
                   counterKSVET_PRESS = KSVET_PRESS_2;
                   counterKSVET_OFF = 0;
                 break;
                 
                 case KSVET_PRESS_2:
                   Serial.println("BLIZH ON");
                    mosfet.digitalWrite(0, M5, HIGH);
                   counterKSVET_PRESS = KSVET_PRESS_3;
                   counterKSVET_OFF = 0;
                 break;
                 
                 case KSVET_PRESS_3:
                   Serial.println("DAL ON");
                   mosfet.digitalWrite(0, M6, HIGH);
                   counterKSVET_PRESS = KSVET_PRESS_4;
                   counterKSVET_OFF = 0;
                 break;
                 
                 case KSVET_PRESS_4: 
                   Serial.println("DAL OFF");
                   mosfet.digitalWrite(0, M6, LOW);
                   counterKSVET_PRESS = KSVET_PRESS_3;
                   counterKSVET_OFF = 0;
                 break; 
            
              }      
             }
           }
          }
      }
      else if(!stateKSVET && flagKSVET == ON)
       {
           flagKSVET = OFF;
          // Serial.println("KSVET OFF");
           counterKSVET = 0;
          // counterKSVET_OFF = 0;
       } 
}
 /*
  * Обработчик кнопки KLEV
 */
void HandlerKLEV(void)
 {
    stateKLEV= digitalRead(KLEV);
    
    if(stateKLEV&&(flagKLEV == OFF))
      {
        counterKLEV++;

        counterKAVAR_second++;                            // дополнительное управление  аварийкой 
      
        // TODO
         if(counterKAVAR_second > 250)                    // дополнительное управление  аварийкой 
         { 
            flagKLEV = ON;
            HadlerKAVAR_function();
         }
       
        else if(counterKLEV > 2)
         {
           flagKLEV = ON;
          
         // Serial.println("KLEV ON");

            if(counterKLEV_PRESS == KLEV_PRESS_0)        // при  первом нажатии
              {
                 counterKLEV_PRESS= KLEV_PRESS_1;
              }

              switch(counterKLEV_PRESS)
                {
                  case KLEV_PRESS_1:
                     flagKLEVenable = ON;                // включить мигание KLEV
                     counterKLEV_PRESS= KLEV_PRESS_2;
                     
                     flagKPRAVenable = OFF;               // выключить мигание KPRAV
                     counterKPRAV_PRESS= KPRAV_PRESS_0;

                      if(event_GAB_F_L_R == ON)
                        {
                         //Serial.println("GAB_F ON");
                         mosfet.digitalWrite(0, M1, HIGH);
  
                         //Serial.println(" GAB_L OFF");         // выключить GAB_L
                         mosfet.digitalWrite(0, M3, LOW);
                      
                         //Serial.println(" GAB_R ON");          // включить GAB_R
                         mosfet.digitalWrite(0, M4, HIGH);

                      }
                          
                       if(event_AVAR == ON)
                         {
                           flagKAVARenable = OFF;          // выключить мигание KAVAR
                         } 

                       event_KPRAV_LEV = ON;              // признака включения поворота лююбого 
                    
                  break;
                  case KLEV_PRESS_2:
                     flagKLEVenable = OFF;                 // выключить мигание KLEV
                     counterKLEV_PRESS= KLEV_PRESS_0;

                        if(event_AVAR == ON)
                         {
                           flagKAVARenable = ON;             // включить мигание KAVAR
                            //Serial.println("GAB_F OFF");
                            mosfet.digitalWrite(0, M1, LOW);
                            
                          // Serial.println(" GAB_L OFF");
                            mosfet.digitalWrite(0, M3, LOW);
                      
                          // Serial.println(" GAB_R ON");
                            mosfet.digitalWrite(0, M4, LOW);   
                         } 

                        else if(event_GAB_F_L_R == ON)
                        {
                           //Serial.println("GAB_F ON");
                           mosfet.digitalWrite(0, M1, HIGH);
  
                           //Serial.println(" GAB_L OFF");         // включить GAB_L
                           mosfet.digitalWrite(0, M3, HIGH);
                      
                           //Serial.println(" GAB_R ON");          // включить GAB_R
                           mosfet.digitalWrite(0, M4, HIGH);
                        }

                      event_KPRAV_LEV = OFF;                       // признака выключения поворота 
                     
                  break;
                }
         }
       }
      else if(!stateKLEV&&(flagKLEV == ON))
       {
          flagKLEV = OFF;
          //Serial.println("KLEV OFF");
           counterKLEV = 0;
           counterKAVAR_second = 0;                              // дополнительное управление  аварийкой 
       }
  }

 /*
  * Обработчик кнопки KPRAV
 */
 
 void HandlerKPRAV(void)
  {
    stateKPRAV= digitalRead(KPRAV);
    if(stateKPRAV&&(flagKPRAV == OFF))
      {
        counterKPRAV++;

        counterKAVAR_second++;                                   // дополнительное управление  аварийкой 
      
        // TODO
         if(counterKAVAR_second > 250)                           // дополнительное управление  аварийкой 
         { 
            flagKLEV = ON;
            HadlerKAVAR_function();
         }
       
         else if(counterKPRAV > 2)
         {
          flagKPRAV = ON;
          Serial.println("KPRAV ON");

            if(counterKPRAV_PRESS == KPRAV_PRESS_0)        // при  первом нажатии
              {
                 counterKPRAV_PRESS= KLEV_PRESS_1;
              }

              switch(counterKPRAV_PRESS)
                {
                  case KPRAV_PRESS_1:
                  
                     flagKPRAVenable = ON;                // включить мигание KPRAV
                     counterKPRAV_PRESS= KPRAV_PRESS_2;
                     
                     flagKLEVenable = OFF;                // выключить мигание KLEV
                     counterKLEV_PRESS= KLEV_PRESS_0;
  
                   if(event_GAB_F_L_R == ON)
                        {
                        //Serial.println("GAB_F ON");
                         mosfet.digitalWrite(0, M1, HIGH);
  
                        // Serial.println(" GAB_L ON");
                         mosfet.digitalWrite(0, M3, HIGH);
                      
                        // Serial.println(" GAB_R OFF");     // при любом раскладе выключить GAB_R
                         mosfet.digitalWrite(0, M4, LOW);
                      }
                    
                     if(event_AVAR == ON)
                       {
                         flagKAVARenable = OFF;          // выключить мигание KAVAR
                       } 

                      event_KPRAV_LEV = ON;               // включить признак включения поворота
                      
                  break;
                  case KPRAV_PRESS_2:
                     flagKPRAVenable = OFF;               // выключить мигание KPRAV
                     counterKPRAV_PRESS= KPRAV_PRESS_0;
                     
                       if(event_AVAR == ON)
                         {
                           flagKAVARenable = ON;        // включить мигание KAVAR

                           //Serial.println("GAB_F OFF");
                            mosfet.digitalWrite(0, M1, LOW);
  
                          // Serial.println(" GAB_L OFF");
                            mosfet.digitalWrite(0, M3, LOW);
                      
                          // Serial.println(" GAB_R ON");
                            mosfet.digitalWrite(0, M4, LOW);  
                         } 

                        else if(event_GAB_F_L_R == ON)
                         {
                           //Serial.println("GAB_F ON");
                           mosfet.digitalWrite(0, M1, HIGH);
  
                           //Serial.println(" GAB_L OFF");         // включить GAB_L
                           mosfet.digitalWrite(0, M3, HIGH);
                      
                           //Serial.println(" GAB_R ON");          // включить GAB_R
                           mosfet.digitalWrite(0, M4, HIGH);
                        }
                             
                         event_KPRAV_LEV = OFF;    
                     
                  break;
                }
          }
       }
      else if(!stateKPRAV&&(flagKPRAV == ON))
       {
          flagKPRAV = OFF;
          //Serial.println("KPRAV OFF");
           counterKPRAV = 0;
           counterKAVAR_second = 0;                              // дополнительное управление  аварийкой 
       }
  }
 void HandlerKAVAR(void)
  {
    stateKAVAR= digitalRead(KAVAR);
    
    if(stateKAVAR&&(flagKAVAR == OFF))
      {
        counterKAVAR++; 
        
       if(counterKAVAR > 2)
         {
          flagKAVAR = ON;
          
           HadlerKAVAR_function();
      
         }
       }
      else if(!stateKAVAR&&(flagKAVAR == ON))
       {
          flagKAVAR = OFF;
          //Serial.println("KAVAR OFF");
          counterKAVAR = 0;
       }
  }
 void HandlerKGUD(void)
 {
    stateKGUD = digitalRead(KGUD);
    
    if(stateKGUD&&(flagKGUD == OFF))
      {
       counterKGUD++;
       
       if(counterKGUD > 2)
         {
           flagKGUD = ON;
           
           if(flagKGUDenable == OFF)  // только если было отключено 
           {
            flagKGUDenable  = ON; 
            Serial.println("KGUD ON");       //M10
            mosfet.digitalWrite(1, 1, HIGH);
           }
         }
       }
      else if(!stateKGUD&&(flagKGUD == ON))
       {
          flagKGUD = OFF;
          counterTIMER2_B = 0;
          flagKGUDenable  = OFF; 
          Serial.println("KGUD OFF");
          mosfet.digitalWrite(1, 1 , LOW);
          counterKGUD = 0;
       } 
 } 
 void HandlerKSTOP(void)
 {
  stateKSTOP = digitalRead(KSTOP);
    if(stateKSTOP&&(flagKSTOP == OFF))
      {
       counterKSTOP++;
       
       if(counterKSTOP > 2)
         {
          flagKSTOP = ON;
          Serial.println("KSTOP ON");
          mosfet.digitalWrite(1, 2, HIGH);
          
         }
       }
      else if(!stateKSTOP&&(flagKSTOP == ON))
       {
          flagKSTOP = OFF;
          Serial.println("KSTOP OFF");
          mosfet.digitalWrite(1, 2, LOW);
          
           counterKSTOP = 0;
       }
 } 
 void HandlerKDEK(void)
 {
  stateKDEK = digitalRead(KDEK);
    if(stateKDEK&&(flagKDEK == OFF))
      {
       counterKDEK++;
       
       if(counterKDEK > 2)
         {
          flagKDEK = ON;
          Serial.println("KDEK ON");

          if(counterKDEK_PRESS == KDEK_PRESS_0)        // при  первом нажатии
              {
                 counterKDEK_PRESS = KDEK_PRESS_1;
              }

              switch(counterKDEK_PRESS)
                {
                  case KDEK_PRESS_1:
                      // включить  KDEK
                      Serial.println("KDEK ON");
                      mosfet.digitalWrite(1, 3, HIGH);
                      counterKDEK_PRESS= KDEK_PRESS_2;  
                  break;
                  case KDEK_PRESS_2:
                     // выключить  KDEK

                     Serial.println("KDEK OFF");
                     mosfet.digitalWrite(1, 3, LOW);
                     counterKDEK_PRESS= KREZ_PRESS_1;
                  break;
                }
         }
       }
      else if(!stateKDEK&&(flagKDEK == ON))
       {
          flagKDEK = OFF;
          //Serial.println("KDEK OFF");
           counterKDEK = 0;
       }  
 }
 
 void HandlerKREZ(void)
 {
    stateKREZ= digitalRead(KREZ);
    if(stateKREZ&&(flagKREZ == OFF))
      {
       counterKREZ++;
       
       if(counterKREZ > 2)
         {
          flagKREZ = ON;
          Serial.println("KREZ ON");

          if(counterKREZ_PRESS == KREZ_PRESS_0)        // при  первом нажатии
              {
                 counterKREZ_PRESS = KREZ_PRESS_1;
              }

              switch(counterKREZ_PRESS)
                {
                  case KREZ_PRESS_1:
                     // включить  KREZ
                      Serial.println("KREZ ON");
                      mosfet.digitalWrite(1, 4, HIGH);
                      counterKREZ_PRESS= KREZ_PRESS_2;  
                  break;
                  case KREZ_PRESS_2:
                      // выключить  KREZ
                     Serial.println("KREZ OFF");
                     mosfet.digitalWrite(1, 4, LOW);
                     counterKREZ_PRESS= KREZ_PRESS_1;
                  break;
                }
          
         }
       }
      else if(!stateKREZ&&(flagKREZ == ON))
       {
          flagKREZ = OFF;
          //Serial.println("KREZ OFF");
           counterKREZ = 0;
       }
 }

void  HadlerKAVAR_function(void)
  {      
          if(counterKAVAR_PRESS == KAVAR_PRESS_0)        // при  первом нажатии KAVAR
              {
                 counterKAVAR_PRESS = KAVAR_PRESS_1;
              }

              switch(counterKAVAR_PRESS)
                {
                  case KAVAR_PRESS_1:
                  
                       flagKAVARenable = ON;                   // включить мигание KAVAR
                      
                       //Serial.println("GAB_F OFF");          // выключить GAB_F
                       mosfet.digitalWrite(0, 0, LOW);
                    
                       //Serial.println(" GAB_L OFF");         // выключить GAB_L
                       mosfet.digitalWrite(0, 2, LOW);
                    
                       //Serial.println(" GAB_R OFF");         // выключить GAB_R
                       mosfet.digitalWrite(0, 3, LOW);

                       event_AVAR = ON;                        // признак что аварийка включена

                        {
                           mutex = ON;
                           {
                           //Serial.println("KAVAR OFF");
                            mosfet.digitalWrite(1, M9, LOW); //M9 
                            mosfet.digitalWrite(0, M7, LOW); //Serial.println("KLEV OFF");  M7 
                            mosfet.digitalWrite(0, M8, LOW); //Serial.println("PRAV OFF");  M8
                        
                            
                             
                            flagKPRAVenable = OFF;                // выключить мигание KPRAV
                            counterKPRAV_PRESS= KPRAV_PRESS_0;
                          
                            flagKLEVenable = OFF;                 // выключить мигание KLEV
                            counterKLEV_PRESS= KLEV_PRESS_0;
                           
                          
                             //Serial.println("KAVAR OFF");
                            mosfet.digitalWrite(1, M9, LOW); //M9
                            mosfet.digitalWrite(0, M7, LOW); //Serial.println("KLEV OFF");  M7 
                            mosfet.digitalWrite(0, M8, LOW); //Serial.println("PRAV OFF");  M8
                           }
                            mutex = OFF; 
                        }
                       
                       counterKAVAR_PRESS= KAVAR_PRESS_2;  
                  break;
                  case KAVAR_PRESS_2:
                       flagKAVARenable = OFF;               // выключить мигание KAVAR
                       counterKAVAR_PRESS= KAVAR_PRESS_0;

                       event_AVAR = OFF;                      // признак что аварийка выключена

                        mutex = ON;
                           {

                            mosfet.digitalWrite(1, M9, LOW); //M9
                            mosfet.digitalWrite(0, M7, LOW); //Serial.println("KLEV OFF");  M7 
                            mosfet.digitalWrite(0, M8, LOW); //Serial.println("PRAV OFF");  M8
                       
                            flagKPRAVenable = OFF;               // выключить мигание KPRAV
                            counterKPRAV_PRESS= KPRAV_PRESS_0;

                            flagKLEVenable = OFF;                // выключить мигание KLEV
                            counterKLEV_PRESS= KLEV_PRESS_0;
 
                            mosfet.digitalWrite(1, M9, LOW); //M9
                            mosfet.digitalWrite(0, M7, LOW); //Serial.println("KLEV OFF");  M7 
                            mosfet.digitalWrite(0, M8, LOW); //Serial.println("PRAV OFF");  M8
                          }

                         mutex = OFF;
                         event_KPRAV_LEV = OFF;               // признака вык

                       
                       
                      if(event_GAB_F_L_R == ON)
                        {
                        //Serial.println("GAB_F ON");
                        mosfet.digitalWrite(0, 0, HIGH);
  
                        // Serial.println(" GAB_L ON");
                        mosfet.digitalWrite(0, 2, HIGH);
                      
                       // Serial.println(" GAB_R ON");
                        mosfet.digitalWrite(0, 3, HIGH);
                      } 

                      
                  break;
                }
  
  
 }
