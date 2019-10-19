/*
  Keypad password project
*/

#include "HID-Project.h"

#include <Keypad.h>
#include <TimerOne.h>                           // Header file for TimerOne library
#include <TimerThree.h>                           // Header file for TimerOne library

#include "user-strings.h"

#define TIMER_US 100000                         // 100mS set timer duration in microseconds 
#define TIMER_LED 1000                         // 1mS set timer duration in microseconds 
#define TIMER_LED 1000                         // 100uS set timer duration in microseconds 
#define TICK_COUNTS_ON 10                          // 1S worth of timer ticks
#define TICK_COUNTS_OFF 1
#define INIT_COUNT 100                     //  to divide the 10uS interupt by 100


volatile int  flashDuration = 0;

volatile long tick_count = TICK_COUNTS_ON;         // Counter for 2S
volatile bool in_long_isr = false;              // True if in long interrupt
volatile long password_entry_timer = 0;         // counter from startup how long user has to enter a pin

const byte ROWS = 5;
const byte COLS = 6;
// Define the Keymap
char keys[ROWS][COLS] = {
  {'A', 'B', 'C', 'E', 'd', 'F'},
  {'G', 'n', 'H', '1', '4', '7'},
  {'I', '/', '0', '2', '5', '8'},
  {'-', '*', '.', '3', '6', '9'},
  {'J', 'K', 'L', 'r', 'M', '+'}

};
byte rowPins[ROWS] = { 8, 9, 10, 14, 15 };
byte colPins[COLS] = { 2, 18, 4, 19, 20, 7 };


#define LED_OFF 0
#define LED_RED 4
#define LED_GREEN 2
#define LED_BLUE 1
#define LED_YELLOW 6
#define LED_PURPLE 5
#define LED_WHITE 7
#define LED_CYAN 3


// indicates the user is entering a password
boolean enteringPassword = false;
boolean locked = true;
boolean loggedInLEDState = LOW;

int lockState = 0;

int lockCode[] = LOCK_CODE;
boolean lockedOut = false;

// Create the Keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


//The Analog pins(A0,A1,A2,A3) can also be configured as digital pins(D18,D19,D20,D21).
#define led_red 3
#define led_green 5
#define led_blue 6
#define brightness_red 20
#define brightness_green 120
#define brightness_blue 20

boolean led_red_state = false;
boolean led_green_state = false;
boolean led_blue_state = false;

boolean led_flash_red_state = false;
boolean led_flash_green_state = false;
boolean led_flash_blue_state = false;

boolean enterPasswordMode = true;
boolean numLockOn = false;

void setup() {

  // Sends a clean report to the host. This is important on any Arduino type.
  BootKeyboard.begin();
  Consumer.begin();

  pinMode(led_red, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_blue, OUTPUT);

  kpd.setHoldTime(1200);
  kpd.addEventListener(keypadEvent); // Add an event listener for this keypad
  Timer1.initialize(TIMER_US);                  // Initialise timer 1
  Timer1.attachInterrupt( timerIsr );           // attach the ISR routine here
  Timer3.initialize(TIMER_LED);                  // Initialise timer 1
  Timer3.attachInterrupt( timerIsr3 );           // attach the ISR routine here

}


void loop() {
  char key = kpd.getKey();
}

void keypadEvent(KeypadEvent key) {

  switch (kpd.getState()) {
    case PRESSED:
      if (enterPasswordMode) {
        // check the next key in the sequence
        if (!isLocked(key)) {
          enterPasswordMode = false;
          numLockOn = false;
        }
      } else {
        if (locked) {
          // not in password mode so send the normal key:
          SendNormalKeys(key);
        } else {
          SendPasswordKeys(key);
        }
      }
      break;

    case RELEASED:
      break;

    case HOLD:
      if (key == 'n') {
        enterPasswordMode = !enterPasswordMode;    // toggle password mode - blink the led .
        reset();
        if (enterPasswordMode == false) {
          numLockOn = true;        // Toggle the num lock state.
          setLed(LED_GREEN, numLockOn);
        }
      }
      break;
  }
}

void SendPasswordKeys(KeypadEvent key) {

  switch (key) {
    case 'n' :
      numLockOn = !numLockOn;        // Toggle the num lock state.
      break;
    // escape
    case 'e' :
      BootKeyboard.write(KEY_ESC);
      break;
    // calculator
    case 'c' :
      //Calculator
      Consumer.write(CONSUMER_CALCULATOR);
      Keyboard.releaseAll();
      break;
    // tab
    case 't' :
      BootKeyboard.write(KEY_TAB);
      break;
    // backspace
    case 'd' :
    case 'r' :
    case '=' :
    case '/' :
    case '*' :
    case '-' :
    case '+' :
    case '.' :
    case '0' :
    case '1' :
    case '2' :
    case '3' :
    case '4' :
    case '5' :
    case '6' :
    case '7' :
    case '8' :
    case '9' :
      printPassword(key);
  }
}

void SendNormalKeys(KeypadEvent key) {

  switch (key) {
    case 'n' :
      numLockOn = !numLockOn;        // Toggle the num lock state.
      setLed(LED_GREEN, numLockOn);
      break;
    // escape
    case 'e' :
      BootKeyboard.write(KEY_ESC);
      break;

    // calculator
    case 'c' :
      //Calculator
      Consumer.write(CONSUMER_CALCULATOR);
      Keyboard.releaseAll();
      break;

    // tab
    case 't' :
      BootKeyboard.write(KEY_TAB);
      break;

    // backspace
    case 'd' :
      BootKeyboard.write(KEY_BACKSPACE);
      break;

    // enter (Return)
    case 'r' :
      BootKeyboard.write(KEY_ENTER);
      break;
    // = / * - +
    case '=' :
    case '/' :
    case '*' :
    case '-' :
    case '+' :
      BootKeyboard.print(key);
      break;
    case '.' :
      if (numLockOn) {
        BootKeyboard.print(key);
      } else {
        BootKeyboard.write(KEY_DELETE);
      }
      break;
    case '0' :
      if (numLockOn) {
        BootKeyboard.print(key);
      } else {
        BootKeyboard.write(KEY_INSERT);
      }
      break;
    case '1' :
      if (numLockOn) {
        BootKeyboard.print(key);
      } else {
        BootKeyboard.write(KEY_END);
      }
      break;
    case '2' :
      if (numLockOn) {
        BootKeyboard.print(key);
      } else {
        BootKeyboard.write(KEY_DOWN_ARROW);
      }
      break;
    case '3' :
      if (numLockOn) {
        BootKeyboard.print(key);
      } else {
        BootKeyboard.write(KEY_PAGE_DOWN);
      }
      break;
    case '4' :
      if (numLockOn) {
        BootKeyboard.print(key);
      } else {
        BootKeyboard.write(KEY_LEFT_ARROW);
      }
      break;
    case '5' :
      if (numLockOn) {
        BootKeyboard.print(key);
      }
      break;
    case '6' :
      if (numLockOn) {
        BootKeyboard.print(key);
      } else {
        BootKeyboard.write(KEY_RIGHT_ARROW);
      }
      break;
    case '7' :
      if (numLockOn) {
        BootKeyboard.print(key);
      } else {
        BootKeyboard.write(KEY_HOME);
      }
      break;
    case '8' :
      if (numLockOn) {
        BootKeyboard.print(key);
      } else {
        BootKeyboard.write(KEY_UP_ARROW);
      }
      break;
    case '9' :
      if (numLockOn) {
        BootKeyboard.print(key);
      } else {
        BootKeyboard.write(KEY_PAGE_UP);
      }
      break;
  }
  flashLed(LED_WHITE);
}



void sendKeys( const char *numLockPacket, const char *normalPacket) {

 
  const char *packet;
  if (numLockOn) {
    // numlock must be pressed each time - like a sticky shift key, so toggle off after each use
    packet = numLockPacket;
    numLockOn = false;
  } else {
    packet = normalPacket;
  }

  while (*packet != '\0') {
    setLed( LED_RED );
    switch (*packet) {
      case 1 :
        delay(100);
        break;
      case 2:
        delay(500);
        break;
      case 3 :
        delay(2000);
        break;

      // control
      case 4:
        Keyboard.press(KEY_LEFT_CTRL);
        break;
      // alt
      case 5:
        Keyboard.press(KEY_LEFT_ALT);
        break;
      // ctrl-alt
      case 6:
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        break;

      case 7:
        Keyboard.press(KEY_LEFT_GUI);
        break;

      case 8:
        Keyboard.press(KEY_UP_ARROW);
        break;

      default :

        flashLed(LED_PURPLE);
        Keyboard.print(*packet);
        delay(40);
        Keyboard.releaseAll();
    }
    packet++;
  }
  setLed(LED_OFF);
}

/**
   This is the main routing function
   It checks the key that was pressed, then sends the associated sequence using sendKeys()
*/

void printPassword(int key) {

  switch (key) {
    case '1':
      sendKeys( nbtn1, btn1);
      break;
    case '2':
      sendKeys( nbtn2, btn2);
      break;
    case '3':
      sendKeys( nbtn3, btn3);
      break;
    case '4':
      sendKeys( nbtn4, btn4);
      break;
    case '5':
      sendKeys( nbtn5, btn5);
      break;
    case '6':
      sendKeys( nbtn6, btn6);
      break;
    case '7':
      sendKeys( nbtn7, btn7);
      break;
    case '8':
      sendKeys( nbtn8, btn8);
      break;
    case '9':
      sendKeys( nbtn9, btn9);
      break;
    case '0':
      sendKeys( nbtn0, btn0);
      break;
    // non numeric
    case '.':
      sendKeys( nbtnDot, btnDot);
      break;
    case 'd':
      sendKeys( nbtnBack, btnBack);
      break;
    case '+':
      sendKeys( nbtnPlus, btnPlus);
      break;
    case '-':
      sendKeys( nbtnMinus, btnMinus);
      break;
    case '*':
      sendKeys( nbtnStar, btnStar);
      break;
    case '/':
      sendKeys( nbtnSlash, btnSlash);
      break;
    case 'r':
      sendKeys( nbtnEnter, btnEnter);
      break;
  }
}



void reset(bool set) {
  locked = true;
  lockedOut = false;
  lockState = 0;
  enteringPassword = false;
  password_entry_timer = 0;
  loggedInLEDState = LOW;
  numLockOn = false;
  if (!set) {
    lockedOut = false;
  }
  setLed(LED_OFF);
}

void reset() {
  reset(true);
}



boolean isLocked(int key) {

  // Escape key so exit password
  if (key == 'e') {
    lockState = 0;
    lockedOut = true;
    return false;
  }

  // press the numlock again to clear the buffer and start again
  if (key == 'n') {
    // Reset the locked out flag and allow user another go
    lockState = 0;
    lockedOut = false;
    return true;
  }

  if (lockedOut) {
    return true;
  }
  enteringPassword = true;
  if (lockCode[lockState] == key) {
    // next character in the pin is correct
    lockState++;
    // enteringPassword = true;
    flashLed(LED_YELLOW);

  } else {
    // even though the user is locked out don't let them know - keep flashing the led
    flashLed(LED_YELLOW);

    lockState = 0;
    //lockedOut = true;
    return true;
  }

  size_t codeLen = sizeof(lockCode) / sizeof(lockCode[0]);
  if (lockState >= codeLen) {
    locked = false;
    return false;
  }
  return true;

}

void setLed(int color, bool state) {
  if (state) {
    setLed(color);
  } else {
    setLed(LED_OFF);
  }
}


void flashLed(int color) {
  if (color & 1) {
    // blue
    led_flash_blue_state = true;
  }
  if (color & 2) {
    // green
    led_flash_green_state = true;
  }
  if (color & 4) {
    // red
    led_flash_red_state = true;
  }
  flashDuration = 50;
}

void toggleLed(int color) {

  if (color & 1) {
    // blue
    led_blue_state = !led_blue_state;
  }
  if (color & 2) {
    // green
    led_green_state = !led_green_state;
  }
  if (color & 4) {
    // red
    led_red_state = !led_red_state;
  }


}

void setLed(int color) {

  led_blue_state = false;
  led_green_state = false;
  led_red_state = false;

  if (color & 1) {
    // blue
    led_blue_state = true;
  }
  if (color & 2) {
    // green
    led_green_state = true;
  }
  if (color & 4) {
    // red
    led_red_state = true;
  }
}



// --------------------------
// timerIsr3() 100 milli second interrupt ISR()
// Called every time the hardware timer 3 times out.
// --------------------------
void timerIsr3()
{
  if (--flashDuration > 0) {
    analogWrite( led_red,  led_flash_red_state * brightness_red );
    analogWrite( led_green, led_flash_green_state *  brightness_green );
    analogWrite( led_blue, led_flash_blue_state * brightness_blue );
  } else {
    flashDuration = 0;
    analogWrite( led_red, led_red_state * brightness_red );
    analogWrite( led_green, led_green_state *  brightness_green );
    analogWrite( led_blue, led_blue_state * brightness_blue );
  }
}

// --------------------------
// timerIsr() 100 milli second interrupt ISR()
// Called every time the hardware timer 1 times out.
// --------------------------
void timerIsr()
{

  if (enterPasswordMode) {
    if (++password_entry_timer > 100) {
      enterPasswordMode = false;
      locked = true;
      numLockOn = true;        // Toggle the num lock state.
      setLed(LED_GREEN);
      return;
    }
    if (!enteringPassword) {
      // not yet started entering the password so flash blue
      toggleLed(LED_BLUE);
    }
  } else {
    if (!locked) {
      // Logged in to password mode
      /*
       * Set the flash cycle using TICK_COUNTS_ON and TICK_COUNTS_OFF
       */
      if (!(--tick_count))
      {
        if (loggedInLEDState == LOW) {
          tick_count = TICK_COUNTS_ON;
          loggedInLEDState = HIGH;
        } else {
          tick_count = TICK_COUNTS_OFF;                      // Reload
          loggedInLEDState = LOW;
          setLed(LED_OFF);
        }
        /*
          Flash the LED green or red
        */
        if (numLockOn) {
          toggleLed(LED_RED);
        } else {
          toggleLed(LED_GREEN);
        }
      }
    }
  }
}
