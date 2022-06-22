#include <Arduino.h>

/* USE ONLY ONE OPTION AT TIME IF NEEDED */
//#define debug             /* uncomment for use with debug with avr-debugger */
#define serialDebug       /* uncomment for use with debug - serial monitor: baud rate=9600*/

#ifdef debug
#include <avr8-stub.h>    /* include avr-debugger lib */
#endif

#define game1Magnet 22      /* Slučka */
#define game2Magnet 23      /* Memory */
#define game3Magnet 24      /* basket */
#define game4Magnet 25      /* bludisko */

/* GAME1 Setup */
#define game1SensorLeft 46               /* left game3Sensor */
#define game1SensorRight 45               /* right game3Sensor */
#define game1SensorConductor 47               /* conductor */
#define game1GreenLED 48       /* green led */
#define game1RedLED 49         /* red led */
#define game1GameInterval 5000     /* game-over time */

bool magnetOutput[4];
int magnetPins[]={game1Magnet,game2Magnet,game3Magnet,game4Magnet};
int game1GameState;              /* current game state - 0: ready, 1: left-right, 2: right-left, -1: restarting */
int game1LastGameState = 0;      /* last game state - 0: ready, 1: left-right, 2: right-left, -1: restarting */
uint32_t game1LastTime = 0;  /* storing game-over time */
uint8_t game1HandledLEDPin;
uint16_t game1HandledLEDInterval;
uint16_t game1HandledLEDDelay;
uint32_t game1LastLEDTime;    /* storing LED handling time */
bool game1LastLedState = false;
bool game1IsLEDReady = true;        /* is ready after LED handling */

/* functions declaration */
void BuzzWire();
void game1CheckStartConditions();
void game1HandleLED(int ledPIN, uint16_t onInterval, uint16_t offInterval);
void handleMagnet();


/* GAME2 Setup */
/* LEDs */
#define game2LED1 8          /* LED 1 */
#define game2LED2 9          /* LED 2 */
#define game2LED3 10         /* LED 3 */
#define game2LED4 11         /* LED 4 */
#define game2LED5 12         /* LED 5 */
#define game2RedLED 13       /* red led */
#define game2GreenLED 7      /* green led */

/* GAME2 buttons */
#define game2Button1 2
#define game2Button2 3
#define game2Button3 4
#define game2Button4 5
#define game2Button5 6
#define game2StartButton 14

/* game2 helpful arrays */
int game2LedPins[] = {game2LED1, game2LED2, game2LED3, game2LED4, game2LED5, game2RedLED, game2GreenLED};
int game2ButtonPins[] = {game2Button1, game2Button2, game2Button3, game2Button4, game2Button5, game2StartButton};
bool game2LastButtonState[] = {HIGH, HIGH, HIGH, HIGH, HIGH};
bool game2IsWrong;                   /* is entered sequence wrong? */

/* game2 */
uint8_t game2HandledLEDPin;
uint16_t game2HandledLEDInterval;
uint16_t game2HandledLEDDelay;
uint8_t game2CipherCursor = 0;       /* cursor for handling ciphers in sequence */
uint8_t game2LedCursor = 0;          /* cursor for led sequence */
uint8_t game2GeneratedSequence[5];   /* storing generated sequence */
uint8_t game2PressedSequence[5];     /* storing pressed sequence */
uint32_t game2LastLEDTime;           /* storing LED handling time */
uint32_t game2LastTime;              /* storing game-over time */
bool game2LastLedState = false;
bool game2IsLEDReady = true;         /* is ready after LED handling */
bool game2GameStarted = false;

/* game2 functions declaration */
void game2HandleGame();
int game2HandleButton();
void game2HandleLED(int ledPIN, uint16_t onInterval, uint16_t offInterval);
void game2GenerateNumber();
void game2LightSequence();
void game2ClearArray(uint8_t *array, uint8_t size);
void game2CheckSequence();


/* GAME3 Setup */
#define game3Trigger 31
#define game3Echo 30
#define game3Sensor A0              /* ball game3Sensor */
#define game3GreenLED 42             /* green led */
#define game3RedLED 43               /* red led */
#define game3GameInterval 15000      /* game-over time */
#define game3MinimumPoints 3         /* minimum game3Points to win */


uint8_t game3Points;
uint32_t game3LastTime;              /* storing game-over time */
uint8_t game3HandledLEDPin;
uint16_t game3HandledLEDInterval;
uint16_t game3HandledLEDDelay;
uint32_t game3LastLEDTime;           /* storing LED handling time */
uint32_t game3LastBall,game3BallTime,game3ScoredTime,game3LastScoredTime;
bool game3IsRunning = false;         /* is game running? */
bool game3LastLedState = false;
bool game3IsLEDReady = true;         /* is ready after LED handling */
bool game3LastBallState;

/* GAME3 functions declaration */
void game3HandleGame();
void game3AddPoint();
void game3HandleLED(int ledPIN, uint16_t onInterval, uint16_t offInterval);
void game3HandleSensor();

/* Game4 SETUP */
#define game4SensorFirst 38              /* sensor at 1st position of ball in labyrinth */
#define game4SensorSecond 39             /* sensor at 2nd position of ball in labyrinth */
#define game4GreenLED 40                 /* green led */
#define game4RedLED 41                   /* red led */
#define game4GameInterval 10000          /* game-over time */

int game4GameState;                      /* current game state - 0: ready, 1: left-right, 2: right-left, -1: restarting */
int game4LastGameState = 0;              /* last game state - 0: ready, 1: left-right, 2: right-left, -1: restarting */
uint8_t game4HandledLEDPin;
uint16_t game4HandledLEDInterval;
uint16_t game4HandledLEDDelay;
uint32_t game4LastTime;                  /* storing LED handling/game-over time */
bool game4LastLedState = false;
bool game4IsLEDReady = true;             /* is ready after LED handling */

/* GAME4 function declaration */
void game4HandleGame();
void game4HandleLED(int ledPIN, uint16_t onInterval, uint16_t offInterval);
void game4CheckStartConditions();

/**************
 * VOID SETUP *
 *************/

void setup() {

#ifdef debug
    debug_init();
#endif

    /* pins mode */
    pinMode(game1Magnet,OUTPUT);
    pinMode(game2Magnet,OUTPUT);
    pinMode(game3Magnet,OUTPUT);
    pinMode(game4Magnet,OUTPUT);

    pinMode(game1SensorLeft, INPUT_PULLUP);
    pinMode(game1SensorRight, INPUT_PULLUP);
    pinMode(game1SensorConductor, INPUT_PULLUP);
    pinMode(game1RedLED, OUTPUT);
    pinMode(game1GreenLED, OUTPUT);



#ifdef serialDebug
    Serial.begin(9600);
#endif

    /* pins mode */
    for (int i = 0; i <= 6; i++)
        pinMode(game2LedPins[i], OUTPUT);
    for (int i = 0; i <= 5; i++)
        pinMode(game2ButtonPins[i], INPUT_PULLUP);

    /* pins mode */
    pinMode(game3RedLED, OUTPUT);
    pinMode(game3GreenLED, OUTPUT);
    //pinMode(game3Sensor, INPUT_PULLUP);
    pinMode(game3Trigger,OUTPUT);
    pinMode(game3Echo,INPUT);
    //attachInterrupt(digitalPinToInterrupt(game3Sensor), game3AddPoint, RISING);

    /* pins mode */
    pinMode(game4SensorFirst, INPUT_PULLUP);
    pinMode(game4SensorSecond, INPUT_PULLUP);
    pinMode(game4RedLED, OUTPUT);
    pinMode(game4GreenLED, OUTPUT);
}

/**************
 * VOID LOOP *
 *************/

void loop() {
    BuzzWire();
    game2HandleGame();
    game3HandleGame();
    game4HandleGame();
    handleMagnet();
    //Serial.println(millis());

}

void handleMagnet(){
    for(int i=0;i<4;i++){
        digitalWrite(magnetPins[i],magnetOutput[i]);
    }
}

/* MAIN FUNCTION  */
void BuzzWire() {
    unsigned long currentTime;

    /* restart after touching conductor */
    if ((digitalRead(game1SensorConductor) == LOW) && (game1LastGameState != -1)) {
        game1LastGameState = -1;
        game1HandleLED(game1RedLED, 3000, 0);       //turn on red LED
        game1GameState = 0;
    }

    /* restart after time-out */
    if (game1GameState == 1 || game1GameState == 2) {

        currentTime = millis();

        if ((currentTime - game1LastTime) >= game1GameInterval) {
            game1GameState = 0;
            game1LastTime = currentTime;
            game1HandleLED(game1RedLED, 3000, 0);   //turn on red LED
        }
    }

    game1CheckStartConditions();

    /* handle WIN: left->right */
    if ((game1GameState == 1) && (digitalRead(game1SensorRight) == LOW)) {
        game1HandleLED(game1GreenLED, 3000, 0);     //turn on green LED
        game1GameState = 0;
        magnetOutput[0]=true;
    }

    /* handle WIN: right->left */
    if ((game1GameState == 2) && (digitalRead(game1SensorLeft) == LOW)) {
        game1HandleLED(game1GreenLED, 6000, 0);     //turn on green LED
        game1GameState = 0;
        magnetOutput[0]=true;
    }

    /* turn off the LED after it was lit on */
    if (game1LastLedState)
        game1HandleLED(game1HandledLEDPin, game1HandledLEDInterval, game1HandledLEDDelay);
}

void game1CheckStartConditions() {

    /* ready to start? left->right */
    if (game1IsLEDReady && (game1GameState == 0) && ((digitalRead(game1SensorLeft) == LOW)) && (game1LastGameState != 1)) {
        game1GameState = 1;      /* left->right */
        game1HandleLED(game1GreenLED, 1000, 0);
        game1LastTime = millis();    /* restart the timer */

        /* ready to start? right->left */
    } else if (game1IsLEDReady && (game1GameState == 0) && (digitalRead(game1SensorRight) == LOW) && (game1LastGameState != 2)) {
        game1GameState = 2;      /* right->left */
        game1HandleLED(game1GreenLED, 1000, 0);
        game1LastTime = millis();    /* restart the timer */
    }
    game1LastGameState = game1GameState;
}

void game1HandleLED(int ledPIN, uint16_t onInterval, uint16_t offInterval) {

    if (!game1LastLedState) {
        game1IsLEDReady = false;
        /* write data to global variables */
        game1HandledLEDPin = ledPIN;
        game1HandledLEDInterval = onInterval;
        game1HandledLEDDelay = offInterval;
        digitalWrite(game1HandledLEDPin, HIGH);  /* turn on the LED */
        game1LastLEDTime = millis();      /* reset the timer for LED */
        game1LastLedState = true;
    }

    /* turn off the LED */
    if (millis() - game1LastLEDTime >= game1HandledLEDInterval) {
        digitalWrite(game1HandledLEDPin, LOW);
    }

    /* delay after turning off the LED */
    if ((millis() - game1LastLEDTime) >= (game1HandledLEDInterval + game1HandledLEDDelay)) {
        game1LastLedState = false;
        game1IsLEDReady = true;
        digitalWrite(game1HandledLEDPin, LOW);
        magnetOutput[0]=false;
    }
}

/*  GAME2  */
/* MAIN FUNCTION */
void game2HandleGame(){

    /* waits for start button to be pressed */
    if (!game2GameStarted) {
        game2GameStarted = !digitalRead(game2StartButton);
    } else {
        /* generate sequence */
        if (game2GeneratedSequence[0] == 0 && game2IsLEDReady) {
            game2GenerateNumber();
            game2IsLEDReady = false;

#ifdef serialDebug
            Serial.print("Generated sequence:");
            for (int i = 0; i < 5; i++)
                Serial.print(game2GeneratedSequence[i]);
            Serial.println();
#endif
        }

        /* lights the generated sequence */
        if (game2LedCursor < 5) {
            game2IsLEDReady = false;
            game2LightSequence();
        }
        game2CheckSequence();

        /* resets GAME-OVER timer*/
        if (game2LedCursor == 5) {
            game2LastTime = millis();
            game2LedCursor = 6;
        }

        /* handle GAME-OVER: time-out*/
        if (game2LedCursor == 6 && millis() - game2LastTime >= 10000) {
            game2HandleLED(game2RedLED, 3000, 0);
            /* resets everything */
            game2CipherCursor = 0;
            game2ClearArray(game2GeneratedSequence, 5);
            game2ClearArray(game2PressedSequence, 5);
            game2LedCursor = 0;
            game2GameStarted = false;
#ifdef serialDebug
            Serial.println("Time-out!");
#endif
        }
    }

    if (game2LastLedState)
        game2HandleLED(game2HandledLEDPin, game2HandledLEDInterval, game2HandledLEDDelay);

}

/* returns pressed button */
int game2HandleButton() {

    int response = -1;   /* response if no button was pressed */
    if (game2IsLEDReady) {
        for (int8_t i = 0; i < 5; i++) {
            bool buttonState = digitalRead(game2ButtonPins[i]);
            if (buttonState == HIGH && game2LastButtonState[i] == LOW)
                response = i + 1;
            game2LastButtonState[i] = buttonState;
        }
    }
    return response;
}

/* Handling LEDs */
void game2HandleLED(int ledPIN, uint16_t onInterval, uint16_t offInterval) {

    if (!game2LastLedState) {
        game2IsLEDReady = false;
        /* write data to global variables */
        game2HandledLEDPin = ledPIN;
        game2HandledLEDInterval = onInterval;
        game2HandledLEDDelay = offInterval;
        digitalWrite(game2HandledLEDPin, HIGH);  /* turn on the LED */
        game2LastLEDTime = millis();     /* reset the timer for LED */
        game2LastLedState = true;
    }

    /* turn off the LED */
    if (millis() - game2LastLEDTime >= game2HandledLEDInterval) {
        digitalWrite(game2HandledLEDPin, LOW);
    }

    /* delay after turning off the LED */
    if ((millis() - game2LastLEDTime) >= (game2HandledLEDInterval + game2HandledLEDDelay)) {
        game2LastLedState = false;
        game2IsLEDReady = true;
        magnetOutput[1]=false;
    }
}

void game2GenerateNumber() {
    for (int i = 0; i < 5; i++) {
        game2GeneratedSequence[i] = (int) random(1, 6);  /* generate cipher, save it to array */
    }
}

/* Lights the LED sequence after generation */
void game2LightSequence() {
    if (!game2LastLedState) {
        game2HandleLED(game2LedPins[(game2GeneratedSequence[game2LedCursor]) - 1], 500, 500);
        game2LedCursor = game2LedCursor + 1;
    }
}

/* as the name suggest - clears arrays*/
void game2ClearArray(uint8_t *array, uint8_t size) {
    for (int i = 0; i < size; i++)
        array[i] = 0;
}

void game2CheckSequence() {
    int i;
    int button = game2HandleButton();
    if (button != -1) {
        if (game2CipherCursor <= 4) {
            game2PressedSequence[game2CipherCursor] = button;     /* save pressed button to array */

#ifdef serialDebug
            Serial.print("Pressed button: ");
            Serial.println(button);
#endif
            /* lights led corresponding to first 4 pressed button */
            if (game2CipherCursor <= 3)
                game2HandleLED(game2LedPins[button - 1], 500, 100);

            /* after last button was pressed */
            if (game2CipherCursor == 4) {


#ifdef serialDebug
                Serial.println("Pressed sequence: ");
                for (i = 0; i < 5; i++)
                    Serial.print(game2PressedSequence[i]);
                Serial.println();
#endif

                /* checks if the correct sequence was pressed */
                for (i = 0; i < 5; i++) {
                    if (game2PressedSequence[i] != game2GeneratedSequence[i]) {
                        game2IsWrong = true;
                    }
                }

                /* if correct sequence */
                if (!game2IsWrong) {

#ifdef serialDebug
                    Serial.println("Sequence is correct!");
#endif

                    game2HandleLED(game2GreenLED, 3000, 2000);
                    magnetOutput[1]=true;
                    game2CipherCursor = 0;
                    game2ClearArray(game2GeneratedSequence, 5);
                    game2ClearArray(game2PressedSequence, 5);
                    game2LedCursor = 0;
                    game2IsWrong = false;
                    game2GameStarted = false;
                    return;

                    /* handle GAME-OVER: wrong sequence */
                } else {

#ifdef serialDebug
                    Serial.println("Sequence is incorrect!");
#endif

                    game2HandleLED(game2RedLED, 3000, 2000);
                    /* resets everything */
                    game2CipherCursor = 0;
                    game2ClearArray(game2GeneratedSequence, 5);
                    game2ClearArray(game2PressedSequence, 5);
                    game2LedCursor = 0;
                    game2IsWrong = false;
                    game2GameStarted = false;

                    return;
                }
            }
            game2CipherCursor++;
        }
    }
}

/*  GAME3 */
/* MAIN FUNCTION  */
void game3HandleGame() {
    game3ScoredTime=millis();
    if(game3IsRunning&&(game3ScoredTime-game3LastScoredTime>=18)){
        game3HandleSensor();
        game3LastScoredTime=game3ScoredTime;
    }
    /* handle WIN */
    if ((game3Points >= game3MinimumPoints) && !game3LastLedState) {
        game3IsRunning = false;
        game3Points = 0;
        game3HandleLED(game3GreenLED, 3000, 2000);
        magnetOutput[2]=true;
    }

    /* resets the game-over timer */
    if (game3IsLEDReady && !game3IsRunning) {
        game3LastTime = millis();
        game3IsRunning = true;
    }

    /* handle GAME-OVER */
    if ((millis() - game3LastTime >= game3GameInterval) && !game3LastLedState) {
        game3IsRunning = false;
        game3Points = 0;
        game3HandleLED(game3RedLED, 3000, 2000);
    }

    /* blinking green LED when game is running */
    if (!game3LastLedState && game3IsRunning)
        game3HandleLED(game3GreenLED, 500, 500);

    /* turn off the LED after it was lit on */
    if (game3LastLedState)
        game3HandleLED(game3HandledLEDPin, game3HandledLEDInterval, game3HandledLEDDelay);
}

/* function after being interrupted */
void game3AddPoint() {
    static unsigned long lastInterruptTime = 0;
    unsigned long interruptTime = millis();

    /* Debouncing interrupt signal with delay */
    if (interruptTime - lastInterruptTime > 200) {
        if (game3IsRunning)
            game3Points++;
    }
    lastInterruptTime = interruptTime;
}

void game3HandleLED(int ledPIN, uint16_t onInterval, uint16_t offInterval) {

    if (!game3LastLedState) {
        game3IsLEDReady = false;
        /* write data to global variables */
        game3HandledLEDPin = ledPIN;
        game3HandledLEDInterval = onInterval;
        game3HandledLEDDelay = offInterval;
        digitalWrite(game3HandledLEDPin, HIGH);  /* turn on the LED */
        game3LastLEDTime = millis();                     /* reset the timer for LED */
        game3LastLedState = true;
    }

    /* turn off the LED */
    if (millis() - game3LastLEDTime >= game3HandledLEDInterval) {
        digitalWrite(game3HandledLEDPin, LOW);

    }

    /* delay after turning off the LED */
    if ((millis() - game3LastLEDTime) >= (game3HandledLEDInterval + game3HandledLEDDelay)) {
        game3LastLedState = false;
        game3IsLEDReady = true;
        magnetOutput[2]=false;
    }
}

void game3HandleSensor(){
    float time;
    uint16_t distance;
/*
    digitalWrite(game3Trigger,HIGH);
    delayMicroseconds(10);
    digitalWrite(game3Trigger,LOW);
    delayMicroseconds(2);
    time = pulseIn(game3Echo,HIGH);*/

    distance = analogRead(game3Sensor);
#ifdef serialDebug
    Serial.println(distance);
#endif

    game3BallTime=millis();
    if((game3LastBallState==false)&&(game3BallTime-game3LastBall>=750)){
        if((game3LastBallState==false)&&(distance>=150)){
            game3Points++;
            game3LastBall=millis();
            game3LastBallState=true;
        }

    }
    else if((game3LastBallState==true)&&(distance<150)){
        game3LastBallState=false;
    }

}

void game4HandleGame() {
    unsigned long currentTime = millis();

    /* handling GAME-OVER: time-out */
    if (game4GameState != 0 && (currentTime - game4LastTime >= game4GameInterval)) {
        game4LastGameState = -1;
        game4GameState = 0;
        game4HandleLED(game4RedLED, 2000, 1000);
    }

    game4CheckStartConditions();

    /* handling WIN: 1st->2nd */
    if ((game4GameState == 1) && (digitalRead(game4SensorSecond) == LOW)) {
        game4HandleLED(game4GreenLED, 2000, 0);
        magnetOutput[3]=true;
        game4GameState = 0;
        game4LastTime = millis();
    }

    /* handling WIN: 2nd->1st */
    if ((game4GameState == 2) && (digitalRead(game4SensorFirst) == LOW)) {
        game4HandleLED(game4GreenLED, 4000, 0);
        magnetOutput[3]=true;
        game4GameState = 0;
        game4LastTime = millis();
    }

    if (game4LastLedState)
        game4HandleLED(game4HandledLEDPin, game4HandledLEDInterval, game4HandledLEDDelay);
}

void game4HandleLED(int ledPIN, uint16_t onInterval, uint16_t offInterval) {

    if (!game4LastLedState) {
        game4IsLEDReady = false;
        /* write data to global variables */
        game4HandledLEDPin = ledPIN;
        game4HandledLEDInterval = onInterval;
        game4HandledLEDDelay = offInterval;
        digitalWrite(game4HandledLEDPin, HIGH);  /* turn on the LED */
        game4LastTime = millis();        /* reset the timer for LED */
        game4LastLedState = true;
    }

    /* turn off the LED */
    if (millis() - game4LastTime >= game4HandledLEDInterval) {
        digitalWrite(game4HandledLEDPin, LOW);

    }

    /* delay after turning off the LED */
    if ((millis() - game4LastTime) >= (game4HandledLEDInterval + game4HandledLEDDelay)) {
        game4LastLedState = false;
        game4IsLEDReady = true;
        digitalWrite(game4HandledLEDPin, LOW);
        magnetOutput[3]=false;
    }
}

void game4CheckStartConditions() {

    /* is ball at 1st end? */
    if (game4IsLEDReady && (game4GameState == 0) && ((digitalRead(game4SensorFirst) == LOW)) && (game4LastGameState != 1)) {
        game4GameState = 1;
        game4HandleLED(game4GreenLED, 1000, 0);
        game4LastTime = millis();

        /* is ball at 2nd end? */
    } else if (game4IsLEDReady && (game4GameState == 0) && (digitalRead(game4SensorSecond) == LOW) && (game4LastGameState != 2)) {
        game4GameState = 2;
        game4HandleLED(game4GreenLED, 1000, 0);
        game4LastTime = millis();
    }
    game4LastGameState = game4GameState;
}


