#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <EEPROM.h> 
#include <SPI.h>

#define Button 4 
#define buzzerpin D1

Max72xxPanel matrix = Max72xxPanel(D8, 1, 1);


int buttonState=0;    // Состояние кнопки
int Timer;            // Настройки таймера
long currentTime;     // Переменная текущего системного врмемени
long StartTime;       // Время запуска устройства
int FirstPress=0;
int DeviceMode =1;    // Режим работы устройства. 0 таймер работает. 1 Пауза. 2 режим настройки таймера.
int ButtonPressed=0;
int PressLong=0;      // Признак было ли последний раз длительное нажатие или нет. 
long ButtonTime;
int InitialTimer[11] ={30,60,120,180,240,300,360,420,480,540,600}; // Массив с вариантами значение таймера в сек.
long LastTimerTime;  // Отсчет 1ой секунды для шага таймера.
int InitialTimerkey =1;
int InitialTimerkeyOld =1;
int ShowTimer; 
char ShowTimerChar[10] = {'0','1','2','3','4','5','6','7','8','9'};
int key;

unsigned long ticker_next;
String tape = "Hello from RobotClass!";
int spacer = 1;
int width = 5 + spacer;

const byte ten[8] = {
    0b11111010,
    0b10001011,
    0b10001010,
    0b10001010,
    0b10001010,
    0b10001010,
    0b10001010,
    0b11111010,
};
const byte half[8] = {
    0b10000010,
    0b01000011,
    0b00100010,
    0b01110010,
    0b10011010,
    0b01000100,
    0b00100010,
    0b11110001,
};

void setup()
{
matrix.setIntensity(4);
  
EEPROM.begin(64);
int InitialTimerkey =EEPROM.read(0);
InitialTimerkeyOld=InitialTimerkey;

  Serial.begin(115200);
  pinMode(Button, INPUT);
  StartTime = millis();
  Timer=InitialTimer[InitialTimerkey];


                                                      

//чтение таймера из  EPPROM  

// Serial.print("EEPROM: ");
// Serial.println(myInteger);
}

//void handleTicker(){
//    for ( int i = 0 ; i < width * tape.length() + matrix.width() - 1 - spacer; i++ ) {
//        matrix.fillScreen(LOW);
//
//        int letter = i / width;
//        int x = (matrix.width() - 1) - i % width;
//        int y = (matrix.height() - 8) / 2;
//
//        while ( x + width - spacer >= 0 && letter >= 0 ) {
//            if ( letter < tape.length() ) {
//                matrix.drawChar(x, y, tape[letter], HIGH, LOW, 1);
//            }
//            letter--;
//            x -= width;
//        }
//
//        matrix.write();
//        delay(50);
//    }
//}

void loop()
{
    delay(50);

// Показываю цифры на экране сколько минут.
ShowTimer=int(Timer/60);
if (ShowTimer>=1  and ShowTimer<10 ) {
matrix.drawChar (1,1,ShowTimerChar[ShowTimer], HIGH,LOW,1);
}

// Если 10 минут, то рисую цифру отдельно.
if (ShowTimer==10)
   for ( int y = 0; y < 8; y++ ) {
        for ( int x = 0; x < 8; x++ ) {
           matrix.drawPixel(x, y, ten[y] & (1<<x));
        }
   }

// Если 30 секунд, то рисую 1/2.  
 if (InitialTimerkey==0 )
   for ( int y = 0; y < 8; y++ ) {
        for ( int x = 0; x < 8; x++ ) {
           matrix.drawPixel(x, y, half[y] & (1<<x));
        }
   }


   
    buttonState = digitalRead(Button);
    currentTime = millis(); 
    if (buttonState ==1)
     {
      
      if (ButtonPressed==0)
         {
            ButtonPressed=1;
            ButtonTime = millis();
         } else
                {
                if (currentTime >= ButtonTime+3000) // проверяю что кнопка удерживалась 3 сек
                   {
                        PressLong=1;
                        if (InitialTimerkey!=InitialTimerkeyOld)  
                                                    {
                                                                       // Запись тайемра в EPPROM  
                                                                        EEPROM.write(0, InitialTimerkey);
                                                                       EEPROM.commit();
                                                                       EEPROM.end(); 
                                                                      Serial.print("-----------------------Запись тайемра в EPPROM ---------------------------------");
                                                      InitialTimerkeyOld=InitialTimerkey;
                                                      matrix.fillScreen(LOW);
                                                      matrix.write();
                                                         for (int freq = 1000; freq > 500; freq=freq-10) {
                                                          tone(buzzerpin,freq,100);
                                                          delay(5);
                                                          }
                                                    }
                                                    
                        if (currentTime <= StartTime+6000)  // проверяю что кнопка удерживалась 3 сек в течении 6  сек с момента запуска устройства b n
                                {
                                                                                     
                                                            DeviceMode=2;
                                                           tone(buzzerpin,500,500);
                                                           delay(500);
                                                           

                                 } else 
                                          {
                                       
                                                            DeviceMode=1;
                                                            tone(buzzerpin,1000,500);
                                                            delay(100);
                                                         ;
                                                            }
                                          
                   } else
                       {
                           PressLong=0;
                       }
                }
        FirstPress =1;        
     }  
    if (buttonState==0)
    {    

       if (FirstPress==1 and PressLong==0 and DeviceMode!=2 and ButtonPressed==1) // Если первое нажатие уже было, это короткое нажатие, режим не настройки и кнопка перед этим была нажата, то
          {
             if (DeviceMode==0) { // если мы в режиме отчета то просто сбрасываем таймер на максимум
              Timer=InitialTimer[InitialTimerkey];
               tone(buzzerpin,1000,200);
             } 
              if (DeviceMode==1 ) { 
                                                      
              DeviceMode=0;
              tone(buzzerpin,1000,200);
             } 
            matrix.fillScreen(LOW);
            matrix.write(); 
           }  
    if (DeviceMode==2 and PressLong==0 and ButtonPressed==1) 
        {
           if (InitialTimerkey < sizeof(InitialTimer)/sizeof(InitialTimer[0])-1) {
                    InitialTimerkey=InitialTimerkey+1;
                    Timer=InitialTimer[InitialTimerkey];
                    }else
                       {
                        InitialTimerkey=0;
                        Timer=InitialTimer[InitialTimerkey];
                        }
           matrix.fillScreen(LOW);
           matrix.write();
        }

          
           
    ButtonPressed =0;
    }
    
    if (DeviceMode==0)
     {
         if (currentTime >= LastTimerTime+1000)  { // Проверяю прошла ли 1 сек для отсчета таймера
          Timer=Timer-1;



            if (ShowTimer>=1  and ShowTimer<10 ) {
                if (key==0x1) {
                  key=0x0;
                  }else {
                    key=0x1;
                  }
             matrix.drawPixel(7, 2, key);
             matrix.drawPixel(7, 5, key);                    
                  }else  {
                    key=0x0;
                  }
            if (ShowTimer<1) {
              
// Отсчет секунд на экране
int k;
   for ( int y = 0; y < 8; y++ ) {
        for ( int x = 0; x < 8; x++ ) {
            k=k+1;
            if (k<=Timer) {
            matrix.drawPixel(x, y, HIGH);
                          } else {
            matrix.drawPixel(x, y, LOW);
                          }
 Serial.print(y);  
 Serial.println(x);                           
                  }
           }  
k=0;

           
                   }
                   
// Сигнал каждую секунду последние 5 секунд.  
LastTimerTime=currentTime;
if (Timer >0 and Timer <= 5) {
  tone(buzzerpin,1200,200);
}
// Сигнал по окончанию таймера
if (Timer ==0 ) {
  tone(buzzerpin,200,1000);
  Timer=InitialTimer[InitialTimerkey];
  DeviceMode=1;
}         
         }
         
    }



matrix.write(); // вывод всех пикселей на матрицу
    
 //   Serial.print("DeviceMode: ");  
 //   Serial.println(DeviceMode); 
 //   Serial.print("Timer: ");  
 //   Serial.println(Timer); 
 //   Serial.print("PressLong: ");  
 //   Serial.println(PressLong); 
 //   Serial.print("FirstPress: ");  
//    Serial.println(FirstPress); 
 //   Serial.print("InitialTimerkey: ");  
//    Serial.println(InitialTimerkey); 
//    Serial.print("sizeof(InitialTimer): ");  
//    Serial.println(sizeof(InitialTimer)/sizeof(InitialTimer[0])-1); 
//   Serial.print("ShowTimer: ");  
// Serial.println(ShowTimer);
//    Serial.print("ShowTimerChar: ");  
//    Serial.println(ShowTimerChar);
//    Serial.print("Key: ");  
//    Serial.println(key);   


    }

 
    

