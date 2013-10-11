
/************************************************************************
 * AdvJoystickDriver.c
 *
 * This driver provides additional functionality to the default joystick
 * handling routines provided by RobotC.
 *
 * Some of the features include:
 *    1. More convenient naming scheme for easier coding.
 *       ie. joy1.left.x instead of joystick.joy1_x1
 *    2. Tracks the change in state of a button in order to perform an action
 *       only once when the button is pressed or released.
 *       ie. if(joy1Press(1)) ...
 *    3. Tracks the state of the hat and provides an enumeration for the different
 *       positions. Also indicates a change in state of the hat.
 *    4. Provides for remapping joystick axis through a table lookup.
 *       This is used to remove the deadband and to perform non-linear scaling.
 *    5. Provides a boolean switch like action for changes in joystick axis. This
 *       enables performing an action when an axis leaves from center or returns to center
 *       ie. if(joy1.left.x_sw == kAxisToZero) ...
 *    6. Provides a logical separation between physical joysticks and driver actions.
 *       This allows either joystick to act as driver 1 or driver 2.
 *    7. Most features can be enabled or disabled and no code will be generated for
 *       disabled features.
 *
 *  Revision 1.0 - Jan 2009
 *  Author: C. George
 */


#if !defined(int16)
#define int16 int
#endif

#if !defined(int32)
#define int32 long
#endif

#define ADV_JOYSTICK_DRIVER_C

/************************************************************************
 * Enable or Disable features of this advanced joystick driver
 */

#define ENABLE_AXIS_REMAP         1      // This enables non-linear mapping from lookup table
#define ENABLE_AXIS_SWITCH_CHECK  1     // This enables checking for joystick axis motion
#define ENABLE_PLAYER_SWAP        1     // This enables player swapping



/************************************************************************
 * Macros to make button processing code easier to read
 */

#if ENABLE_AXIS_SWITCH_CHECK
typedef enum
{
   kAxisToZero   = -1,      // Set when a joystick axis has just returned to zero
   kAxisNoSwitch = 0,      // Set when there is has been no switch
   kAxisFromZero = 1      // Set when a joystick axis has just left zero
} TAxisSwitch;
#endif

typedef struct
{
   int16 x;            // current value of joystick
   int16 y;

#if ENABLE_AXIS_SWITCH_CHECK
   TAxisSwitch x_sw;       // convert joystick movement to switch type action
   TAxisSwitch y_sw;
#endif

} TStick;

typedef enum
{
  kHatCentered = 0,
  kHatUp = 1,
  kHatUpRight = 2,
  kHatRight = 3,
  kHatDownRight = 4,
  kHatDown = 5,
  kHatDownLeft = 6,
  kHatLeft = 7,
  kHatUpLeft = 8
} THatPosition;

typedef struct
{
   TStick  left;         // x,y values for the left analog thumbstick
   TStick  right;         // x,y values for the right analog thumbstick
   THatPosition   hat;       // enumeration for tophat position
   bool    hat_press;      // set true when a tophat is pressed
   int16    btn;         // stores the state of the buttons
   int16    btn_down;      // set true when a button is pressed
   int16    btn_up;         // set true when a button is releaseed
} TAdvJoystick;

TAdvJoystick joy1;         // Joystick information for Driver 1
TAdvJoystick joy2;         // Joystick information for Driver 2

#if ENABLE_PLAYER_SWAP
bool bSwitchDrivers;      // Set when Drive 1 = Joystick 2 and Driver 2 = Joystick 1
bool bSwitchingDrivers;     // Set while the Driver Swap buttons are depressed
bool bDriverSwitched;      // Set when bSwitchingDrivers goes from True to False

TAdvJoystick joyA;         // temporary variables to make driver swapping logic easier
TAdvJoystick joyB;
#endif

/************************************************************************
 * Macros to make button processing code easier to read
 */

#define joy1Hold(bnum)        (joy1.btn           & (1 << (bnum - 1)))
#define joy1Press(bnum)       (joy1.btn_down      & (1 << (bnum - 1)))
#define joy1Release(bnum)     (joy1.btn_up        & (1 << (bnum - 1)))
#define joy2Hold(bnum)        (joy2.btn           & (1 << (bnum - 1)))
#define joy2Press(bnum)       (joy2.btn_down      & (1 << (bnum - 1)))
#define joy2Release(bnum)     (joy2.btn_up        & (1 << (bnum - 1)))

#define joyBtn1  (1 << 0)
#define joyBtn2  (1 << 1)
#define joyBtn3  (1 << 2)
#define joyBtn4  (1 << 3)
#define joyBtn5  (1 << 4)
#define joyBtn6  (1 << 5)
#define joyBtn7  (1 << 6)
#define joyBtn8  (1 << 7)
#define joyBtn9  (1 << 8)
#define joyBtn10 (1 << 9)
#define joyBtn11 (1 << 10)               // NOTE: Buttons 11 & 12 aren't being sent by the FMS
#define joyBtn12 (1 << 11)

#define joyBtn(bnum) (1 << (bnum - 1))


// Joystick driver included here so the AdvJoystick variables show up first in the debug
// variable window
#if !defined(JOYSTICKDRIVER_C)
#include "JoystickDriver.c"
#endif

TJoystick joystickPrev;            // Temporary variable to track changes in joystick state

/************************************************************************
 * Include the axis remapping code if required.
 */

#if ENABLE_AXIS_REMAP

// This lookup table converts joystick values to power % based on a
// log-log function.

int axis_map[] = {
       2,    4,    6,    8,   10,   12,   14,   16,   18,   20,
      22,   25,   27,   29,   32,   34,   37,   39,   42,   44,
      47,   50,   52,   55,   58,   61,   64,   67,   70,   73,
      77,   80,   83,   87,   90,   94,   97,  101,  105,  109,
     113,  117,  121,  125,  129,  134,  138,  143,  147,  152,
     157,  162,  167,  172,  177,  183,  188,  194,  200,  205,
     212,  218,  224,  231,  237,  244,  251,  258,  265,  273,
     280,  288,  296,  304,  313,  321,  330,  339,  348,  358,
     368,  378,  388,  398,  409,  420,  432,  443,  455,  468,
     480,  493,  507,  520,  534,  549,  564,  579,  595,  611,
     628,  645,  662,  680,  699,  718,  738,  759,  779,  801,
     823,  846,  870,  894,  920,  946,  972,  1000,  1000};

// Applies the lookup table for a given value.
int remapAxis(int value)
{
    int t=1;               // since the mapping function is symmetric
                        // we only store the positive half.
    if (value < 0)            // check here if the input value is negative
    {
       value *= -1;         // take the absolute value
        t=-1;               // remember to invert the result
        value += 1;            // correction to allow 100% power
    }
    else
    {
       value += 1;            // correction to allow 100% power since
    }

    if (value < 11)            // deadband removal
        return 0;

    value -= 11;            // deadband values not included in table

    t *= axis_map[value];      // lookup new value and correct for negative input
    return t;
}

#endif


/************************************************************************
 * initJoystick()
 *
 * Call this function once at the beginning of a program to initialize
 * internal variables.
 */

void initJoystick()
{
   // Initialize joystick structures to prevent false button press at startup
   memcpy(joystick, joystickCopy, sizeof(joystick));
   memcpy(joystickPrev, joystick, sizeof(joystick));

#if ENABLE_PLAYER_SWAP
   bSwitchDrivers    = false;
   bSwitchingDrivers = false;
   bDriverSwitched   = false;
#endif

#if ENABLE_AXIS_SWITCH_CHECK
   // Initialize joystick axis structures to prevent false axis motion at startup
   joyA.left.x = 0;
   joyA.left.y = 0;
   joyA.right.x = 0;
   joyA.right.y = 0;

   joyB.left.x = 0;
   joyB.left.y = 0;
   joyB.right.x = 0;
   joyB.right.y = 0;
#endif

}

/************************************************************************
 * processAnalogStick()
 *
 * This private function performs all processing for the x and y axis of an analog
 * thumbstick.
 */

void processAnalogStick(TStick& stick, int joy_x, int joy_y )
{
   int tx, ty;

   tx = stick.x;
   ty = stick.y;

#if ENABLE_AXIS_REMAP
   stick.x = remapAxis(joy_x);
   stick.y = remapAxis(joy_y);
#else
   stick.x = joy_x;
   stick.y = joy_y;
#endif

#if ENABLE_AXIS_SWITCH_CHECK
   stick.x_sw = kAxisNoSwitch;
   if (tx != 0 && stick.x == 0)
   {
      stick.x_sw = kAxisToZero;
   }
   else if (stick.x != 0 && tx == 0)
   {
      stick.x_sw = kAxisFromZero;
   }

   stick.y_sw = kAxisNoSwitch;
   if (ty != 0 && stick.y == 0)
   {
      stick.y_sw = kAxisToZero;
   }
   else if (stick.y != 0 && ty == 0)
   {
      stick.y_sw = kAxisFromZero;
   }
#endif

}


/************************************************************************
 * updateJoystick()
 *
 * Periodically call this function to update the joy1 and joy2 variables from
 * the current joystick values.
 *
 */

void updateJoystick()
{
   // Get the new joystick values
   memcpy(joystick, joystickCopy, sizeof(joystick));


#if ENABLE_PLAYER_SWAP
   // Button Handling Routines
   joyA.btn        = joystick.joy1_Buttons;
   joyA.btn_down   = (joystick.joy1_Buttons ^ joystickPrev.joy1_Buttons) & joystick.joy1_Buttons;
   joyA.btn_up     = (joystick.joy1_Buttons ^ joystickPrev.joy1_Buttons) & (~joystick.joy1_Buttons);

   joyA.hat       = (THatPosition)(joystick.joy1_TopHat+1);
   joyA.hat_press = (joystickPrev.joy1_TopHat != joystick.joy1_TopHat && joystick.joy1_TopHat != -1);

   joyB.btn        = joystick.joy2_Buttons;
   joyB.btn_down = (joystick.joy2_Buttons ^ joystickPrev.joy2_Buttons) & joystick.joy2_Buttons;
   joyB.btn_up   = (joystick.joy2_Buttons ^ joystickPrev.joy2_Buttons) & (~joystick.joy2_Buttons);

   joyB.hat       = (THatPosition)(joystick.joy1_TopHat+1);
   joyB.hat_press = (joystickPrev.joy2_TopHat != joystick.joy2_TopHat && joystick.joy2_TopHat != -1);

   // Process the Analog Thumbsticks
   processAnalogStick(joyA.left,  joystick.joy1_x1, joystick.joy1_y1);
   processAnalogStick(joyA.right, joystick.joy1_x2, joystick.joy1_y2);
   processAnalogStick(joyB.left,  joystick.joy2_x1, joystick.joy2_y1);
   processAnalogStick(joyB.right, joystick.joy2_x2, joystick.joy2_y2);

   // Check for a player swap:
   //    Pressing Button 9 makes that joystick player 1
   //    Pressing Button 10 makes that joystick player 2

   bDriverSwitched = false;

   // Check if the joystick to player mapping should be normal
   if( (joystick.joy1_Buttons & joyBtn9) || (joystick.joy2_Buttons & joyBtn10) )
   {
      bSwitchDrivers = false;
      bSwitchingDrivers = true;
   }
   else if( (joystick.joy1_Buttons & joyBtn10) || (joystick.joy2_Buttons & joyBtn9) )
   {
      bSwitchDrivers = true;
      bSwitchingDrivers = true;
   }
   else
   {
      if (bSwitchingDrivers)
      {
         bSwitchingDrivers = false;
         bDriverSwitched = true;
      }
   }

   // Switch players if required
   if (bSwitchDrivers)
   {
      memcpy(joy1, joyB, sizeof(joy1));
      memcpy(joy2, joyA, sizeof(joy2));
   }
   else
   {
      memcpy(joy1, joyA, sizeof(joy1));
      memcpy(joy2, joyB, sizeof(joy2));
   }

#else
   joy1.btn        = joystick.joy1_Buttons;
   joy1.btn_down   = (joystick.joy1_Buttons ^ joystickPrev.joy1_Buttons) & joystick.joy1_Buttons;
   joy1.btn_up     = (joystick.joy1_Buttons ^ joystickPrev.joy1_Buttons) & (~joystick.joy1_Buttons);

   joy1.hat       = (THatPosition)(joystick.joy1_TopHat + 1);
   joy1.hat_press = (joystickPrev.joy1_TopHat != joystick.joy1_TopHat && joystick.joy1_TopHat != -1);

   joy2.btn        = joystick.joy2_Buttons;
   joy2.btn_down = (joystick.joy2_Buttons ^ joystickPrev.joy2_Buttons) & joystick.joy2_Buttons;
   joy2.btn_up   = (joystick.joy2_Buttons ^ joystickPrev.joy2_Buttons) & (~joystick.joy2_Buttons);

   joy2.hat       = (THatPosition)(joystick.joy2_TopHat + 1);
   joy2.hat_press = (joystickPrev.joy2_TopHat != joystick.joy2_TopHat && joystick.joy2_TopHat != -1);

   // Process the Analog Thumbsticks
   processAnalogStick(joy1.left, joystick.joy1_x1, joystick.joy1_y1);
   processAnalogStick(joy1.right, joystick.joy1_x2, joystick.joy1_y2);
   processAnalogStick(joy2.left, joystick.joy2_x1, joystick.joy2_y1);
   processAnalogStick(joy2.right, joystick.joy2_x2, joystick.joy2_y2);
#endif

   // Save the current settings as the previous settings
   memcpy(joystickPrev, joystick, sizeof(joystick));
}
