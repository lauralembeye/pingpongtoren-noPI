/* Raw IR decoder sketch!
This sketch/program uses the Arduno and a PNA4602 to
decode IR received. This can be used to make a IR receiver
(by looking for a particular code)
or transmitter (by pulsing an IR LED at ~38KHz for the
durations detected
Code is public domain, check out www.ladyada.net and adafruit.com
for more tutorials!
*/
 #include "mcc_generated_files/mcc.h"


// the maximum pulse we'll listen for - 65 milliseconds is a long time
#define MAXPULSE 65000
 
// what our timing resolution should be, larger is better
// as its more 'precise' - but too large and you wont get
// accurate timing
#define RESOLUTION 20
 
// we will store up to 100 pulse pairs (this is -a lot-)
uint16_t pulses[100][2]; // pair is high and low pulse
uint8_t currentpulse = 0; // index for pulses we're storing
uint16_t averageLow = 0;
uint16_t averageHigh = 0;

void printpulses(void) {
  printf("\n\r\n\rReceived: \n\rOFF \tON");
  uint8_t i;
  for ( i = 0; i < currentpulse; i++) {
    
    printf("%d",pulses[i][0] * RESOLUTION);
    printf(" usec, ");
    averageHigh +=pulses[i][0] *RESOLUTION;
    
    printf("%d",pulses[i][1] * RESOLUTION);
    printf(" usec");
    averageLow +=pulses[i][1] * RESOLUTION;
  }
   printf("Number of Pulses: ");
   printf(currentpulse);
   printf("Print average of Low Pulses: ");
   printf(averageLow/currentpulse);// print the average
   printf("Print average of High Pulses: ");
   printf(averageHigh/currentpulse);// print the average
  // print it in a 'array' format
  printf("int IRsignal[] = {");
  printf("// ON, OFF (in 10's of microseconds)");
  for (i = 0; i < currentpulse-1; i++) {
    printf("\t"); // tab
    printf("%d",pulses[i][1] * RESOLUTION / 10);
    printf(", ");
    printf("%d", pulses[i+1][0] * RESOLUTION / 10);
    printf(",\r\n");
  }
  printf("\t"); // tab
  printf("%d",pulses[currentpulse-1][1] * RESOLUTION / 10);
  printf(", 0};");
  
 }

void loop(void) {
  uint16_t highpulse, lowpulse; // temporary storage timing
  highpulse = lowpulse = 0; // start out with no pulse length
  
  
// while (digitalRead(IRpin)) { // this is too slow!
    while (out_GetValue()) {
     // pin is still HIGH
  
     // count off another few microseconds
     highpulse++;
     
     __delay_us(RESOLUTION);

     
     // If the pulse is too long, we 'timed out' - either nothing
     // was received or the code is finished, so print what
     // we've grabbed so far, and then reset
     if ((highpulse >= MAXPULSE) && (currentpulse != 0)) {
      printf(highpulse);
      printf("\r\n");
      printf(currentpulse);
      printf("\r\n");
      printf(MAXPULSE);
      printf("\r\n");
       printpulses();
       currentpulse=0;
       return;
     }
  }
  // we didn't time out so lets stash the reading
  pulses[currentpulse][0] = highpulse;
  
  // same as above
  while (! out_GetValue()) {
  //while (!IRpin_PIN & (1<<IRpin)){
     // pin is still LOW
     lowpulse++;
     //delayMicroseconds(RESOLUTION);
     if ((lowpulse >= MAXPULSE) && (currentpulse != 0)) {
       printpulses();
       currentpulse=0;
       return;
     }
  }
  pulses[currentpulse][1] = lowpulse;
 
  // we read one high-low pulse successfully, continue!
  currentpulse++;
}