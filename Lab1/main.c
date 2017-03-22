
// Print out "hello world" on both the console and the LCD screen.

#include <stdio.h>
#include "supportFiles/display.h"

#define QUARTER_WIDTH DISPLAY_WIDTH/4
#define QUARTER_HEIGHT DISPLAY_HEIGHT/4
#define HALF_WIDTH DISPLAY_WIDTH/2
#define HALF_HEIGHT DISPLAY_HEIGHT/2
#define TRIANGLE_WIDTH DISPLAY_WIDTH/9
#define TRIANGLE_HEIGHT DISPLAY_HEIGHT/2.5
#define CIRCLE_RADIUS DISPLAY_WIDTH/10
#define PADDING 20

#define DISPLAY_ORANGE 0xFFA5

int main() {
    /*
	display_init();  // Must init all of the software and underlying hardware for LCD.
	display_fillScreen(DISPLAY_BLACK);  // Blank the screen.
	display_setCursor(0, 0);            // The upper left of the LCD screen.
	display_setTextColor(DISPLAY_RED);  // Make the text red.
	display_setTextSize(2);             // Make the text a little larger.
	display_println("hello world (on the LCD)!");    // This actually prints the string to the LCD.
	printf("hello world!\n\r");  // This prints on the console.
	*/

    display_init();
    display_fillScreen(DISPLAY_BLACK);
    display_drawLine(0, 0, 319, 239, DISPLAY_GREEN);
    display_drawLine(319, 0, 0, 239, DISPLAY_GREEN);
    display_fillTriangle(HALF_WIDTH, TRIANGLE_HEIGHT, HALF_WIDTH - TRIANGLE_WIDTH, PADDING, HALF_WIDTH + TRIANGLE_WIDTH, PADDING, DISPLAY_YELLOW);
    display_fillCircle(HALF_WIDTH + QUARTER_WIDTH, HALF_HEIGHT, CIRCLE_RADIUS, DISPLAY_ORANGE);
    display_drawTriangle(HALF_WIDTH, DISPLAY_HEIGHT - TRIANGLE_HEIGHT, HALF_WIDTH - TRIANGLE_WIDTH, DISPLAY_HEIGHT - PADDING, HALF_WIDTH + TRIANGLE_WIDTH, DISPLAY_HEIGHT - PADDING, DISPLAY_YELLOW);
    display_drawCircle(HALF_WIDTH - QUARTER_WIDTH, HALF_HEIGHT, CIRCLE_RADIUS, DISPLAY_RED);
}

// This function must be defined but can be left empty for now.
// You will use this function in a later lab.
// It is called in the timer interrupt service routine (see interrupts.c in supportFiles).
void isr_function() {
    // Empty for now.
}
