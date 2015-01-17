/*
 * Simple data logger.

Modifed version of
https://github.com/greiman/SdFat/blob/master/SdFat/examples/dataLogger/dataLogger.ino
by Bill Greiman
Modified by John Boxall 14/1/15 for tronixlabs.com tutorial
*/

// begin section A
#include <SdFat.h>
// SD chip select pin.  Be sure to disable any other SPI devices such as Enet.
const uint8_t chipSelect = SS;

// Interval between data records in milliseconds. 
// The interval must be greater than the maximum SD write latency plus the
// time to acquire and write data to the SD to avoid overrun errors.
// Run the bench example to check the quality of your SD card.
const uint32_t SAMPLE_INTERVAL_MS = 200;

// Log file base name.  Must be six characters or less.
#define FILE_BASE_NAME "DATA" // this is the start of the file name created
//------------------------------------------------------------------------------
// File system object.
SdFat sd;
// Log file.
SdFile file;
// Time in micros for next data record.
uint32_t logTime;
//==============================================================================
// Error messages stored in flash.
#define error(msg) error_P(PSTR(msg))
//------------------------------------------------------------------------------
void error_P(const char* msg) {
  sd.errorHalt_P(msg);
}
// end section A

void setup() 
{
  Serial.begin(9600);
  while (!Serial) {} // wait for Leonardo
  delay(1000);
  
  // begin section B
  // set up file name and open card for writing
  const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
  char fileName[13] = FILE_BASE_NAME "00.CSV";
  
  // Initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
  // breadboards.  use SPI_FULL_SPEED for better performance.
  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();
  
  // Find an unused file name.
  if (BASE_NAME_SIZE > 6) {
    error("FILE_BASE_NAME too long");
  }
  while (sd.exists(fileName)) 
  {
    if (fileName[BASE_NAME_SIZE + 1] != '9') 
    {
      fileName[BASE_NAME_SIZE + 1]++;
    } else if (fileName[BASE_NAME_SIZE] != '9') 
    {
      fileName[BASE_NAME_SIZE + 1] = '0';
      fileName[BASE_NAME_SIZE]++;
    } else 
    {
      error("Can't create file name");
    }
  }
  if (!file.open(fileName, O_CREAT | O_WRITE | O_EXCL)) error("file.open");
  // end section B
}

void logData() 
{
  int a0, a1;
  // Write data header - this is the first line of the file that will be created on the memory card
  // Ideal for column headings and so forth
  file.print(F("A0, A1"));
  file.println(); // start new line in file
  
  // now log some random data from two analogue inputs
  for (int i=0; i<100; i++)
  {
    // read values from analogue inputs
    a0=analogRead(0);
    a1=analogRead(1);
    
    // now write the values to a new line in the text file, separated by a comma
    // comma ideal for text delimiting when opened in a spreadsheet
    file.print(a0);
    file.write(',');
    file.print(a1);
    file.println(); // new line
  }
}

void loop() 
{
  Serial.println(F("Start logging"));
  logData();

  // Force data to SD and update the directory entry to avoid data loss.
  if (!file.sync() || file.getWriteError()) error("write error"); 
  file.close();
  Serial.println(F("Finished logging"));
  
  // do nothing!
  do {} while (1);
}
