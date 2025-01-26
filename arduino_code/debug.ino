//by Divino Fire <github.com/divinofire>
//date: 18/09/2022

//Debug a table on SD card by passing commands via serial monitor


//make sure you have connected the SD card appropriately (lookup the SPI pin connection for your microcontroller)

#define DEBUG_TABLE_ENABLED //enable the DEBUG_Table() function
#include <DatabaseOnSD.h>


MyTable debugTable("debug.csv"); //change the table name here to your desired name

void setup(){
Serial.begin(9600); //start serial monitor with a baud rate of 115200
debugTable.printSDstatus(); //[optional] print the initialization status of SD card
debugTable.emptyTable(); // [optional] clear table contents
debugTable.begin(1, 4); //begin with 1 row and 4 columns with no cell data [only if table is empty]
// debugTable.begin(); //debug a non empty table 
//number of columns is rigid but not so with the number of rows

}


void loop(){
  DEBUG_Table(debugTable);
}

/*

upload the above code and open the serial monitor
adjust the baud rate to match with the one set in the code
turn on 'no line ending' in the arduino serial monitor

send any of the following commands to start debugging your table
    append // append empty row to the end of table
    cols // print the number of columns
    del=2 // delete row with index 2
    delete // delete last row
    empty // empy table
    insert=3 // insert empty row at row index 3
    name // print the name of the table
    print // print table
    read=0,1 // read cell at position 0,1
    row=0 // print row 0
    rows // print the number of rows
    write=2,3+data // write data to cell at postion 2,3

//all paramters after '=' can be modified. e.g. read=0,2
//send 'print' to see changes on the table

*/