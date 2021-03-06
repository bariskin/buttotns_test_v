
#define KPRAV  4

#define ON   1
#define OFF  0

boolean lastReading = false;  // флаг предыдущего состояния кнопки
boolean reading = false ;     // флаг текущего состояния кнопки 
byte counterKPRAV = 0;


byte flagKPRAV = OFF;
byte flagKPRAV_KVAR  = OFF;

 
void setup() {
  // put your setup code here, to run once:
 pinMode(KPRAV, INPUT);

  Serial.begin(9600);
}

void loop() {
  
  delay(16);
  HandlerKPRAV();

}

  void HandlerKPRAV(void)
    {
       reading  = digitalRead(KPRAV);
      
       if(reading && lastReading)
        {
          counterKPRAV++;
  
           if(counterKPRAV > 150 && flagKPRAV_KVAR  != ON )                    // дополнительное управление  аварийкой 
           { 
              flagKPRAV_KVAR  = ON;
              flagKPRAV = ON;
              
              Serial.println("KPRAV_KVAR  ON");   
           }
    
        }

       else if(counterKPRAV > 5 && flagKPRAV != ON )
           {
            for (int i = 0;i < 20; i++ )
            {
              reading  = digitalRead(KPRAV);     
              if(!reading) 
              {                 
                 flagKPRAV_KVAR  = ON;
                 flagKPRAV = ON;
                 
                 Serial.println("KPRAV ON"); 
                 break;             
               }    
            }      
           }
  
        else if(!reading&&(flagKPRAV == ON))
         {
            Serial.println("KPRAV OFF");
            flagKPRAV = OFF;
            flagKPRAV_KVAR  = OFF; 
            counterKPRAV = 0;
         }
  
          
        lastReading = reading;     
    }
