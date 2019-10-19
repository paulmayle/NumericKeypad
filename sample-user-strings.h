#define LOCK_CODE {'1', '2', '3', '4', '5', '6'}
/*
 Use \n to end line
 Use \001 for 100mS delay 
 Use \002 for 500mS delay 
 Use \003 for 2S delay 

 Use \004 for left Control key
 Use \005 for left Alt key
 Use \006 for Both left alt and control keys
 Use \007 windows key

*/

#define DELAY_100 "\001"
#define DELAY_500 "\002"
#define DELAY_2000 "\003"

#define LEFT_CONTROL "\004"
#define LEFT_ALT "\005"
#define LEFT_CTL_ALT "\006"
#define WINDOWS "\007"
#define WINDOWS_RUN "\007r" DELAY_100

#define NEWLINE "\n"



// To double the keys available the Numeric Lock key toggles between two sets:
// ================ Set 1 is where the led is OFF but blinks on once a second (numlock off)=========================

#define btn0 "0"
#define btn1 "1"
#define btn2 "2"
#define btn3 "3"
#define btn4 "four\n"
#define btn5 "button 5\n"
#define btn6 "button 6\n"
#define btn7 "7\n"
#define btn8 "button 8\n"
#define btn9 "button 9\n"

// non numeric
#define btnDot "button .\n"
#define btnBack "button backspace\n"
#define btnPlus "button +\n"
#define btnMinus "button -\n"
#define btnStar "button *\n"
#define btnSlash "slash"
#define btnEnter "enter"


// =================== Set 2 is where the led is ON but blinks off once a second (numlock on) =====================


#define nbtn1 "n button 1\n"
#define nbtn2 "n button 2\n"
#define nbtn3 "n button 3\n"
#define nbtn4 "n button 4\n"
#define nbtn5 "n button 5\n"
#define nbtn6 "n button 6\n"
#define nbtn7 "n button 7\n"
#define nbtn8 "n button 8\n"
#define nbtn9 "n button 9\n"
#define nbtn0 "n button 0\n"
// non numeric
#define nbtnDot "n button .\n"
#define nbtnBack "n button backspace\n"
#define nbtnPlus "n button +\n"
#define nbtnMinus "n button -\n"
#define nbtnStar "n button *\n"
#define nbtnSlash "n button /\n"
#define nbtnEnter "n button Enter\n"
