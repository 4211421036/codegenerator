#include <Arduino.h>
#include "AT24CM01.h"

AT24CM01 EEPROM;

#define TEST_BOOL   (true)
#define TEST_CHAR   ('A')
#define TEST_UCHAR  (254) 
#define TEST_BYTE   (254)
#define TEST_SHORT  (-32767)
#define TEST_WORD   (65535U)
#define TEST_INT    (-32767)
#define TEST_UINT   (65535U)
#define TEST_LONG   (-2147483646L)
#define TEST_ULONG  (4294967295UL)
#define TEST_FLOAT  (-2.12345F)
#define TEST_DOUBLE (-3.27677)

#define TEST_ARRAYCHARINDEX 5
#define TEST_ARRAYBYTEINDEX 5

/* 1 Byte Arrays */
char testCharArray[TEST_ARRAYCHARINDEX] = {'H','E','L','L', 'O'};
byte testByteArray[TEST_ARRAYBYTEINDEX] = {254,253,252,251,250};

void VariableTestWrite();
void VariableTestRead();

void setup() 
{
  Serial.begin(115200);

  EEPROM.begin();  

  VariableTestWrite();
  VariableTestRead(); 
}

void loop(){}

void VariableTestWrite()
{
  Serial.print("\n\n ...........Writing Variables.............");
  Serial.print("\n          bool ("); Serial.print(sizeof(bool)); Serial.print(") "); TEST_BOOL ? Serial.print("True") : Serial.print("False");  EEPROM.writeBool(0, TEST_BOOL);
  Serial.print("\n          byte ("); Serial.print(sizeof(byte)); Serial.print(") "); Serial.print(TEST_BYTE); EEPROM.writeByte(2, TEST_BYTE);
  Serial.print("\n          char ("); Serial.print(sizeof(char)); Serial.print(") "); Serial.print(TEST_CHAR); EEPROM.writeChar(4, TEST_CHAR);
  Serial.print("\n unsigned char ("); Serial.print(sizeof(unsigned char)); Serial.print(") "); Serial.print(TEST_UCHAR); EEPROM.writeUnsignedChar(6, TEST_UCHAR);
  Serial.print("\n         short ("); Serial.print(sizeof(short)); Serial.print(") "); Serial.print(TEST_SHORT); EEPROM.writeShort(8, TEST_SHORT);
  Serial.print("\n          word ("); Serial.print(sizeof(word)); Serial.print(") "); Serial.print(TEST_WORD); EEPROM.writeShort(12, TEST_WORD);
  Serial.print("\n           int ("); Serial.print(sizeof(int)); Serial.print(") "); Serial.print(TEST_INT); EEPROM.writeShort(16, TEST_INT);
  Serial.print("\n  unsigned int ("); Serial.print(sizeof(unsigned int)); Serial.print(") "); Serial.print(TEST_UINT); EEPROM.writeUnsignedInt(24, TEST_UINT);
  Serial.print("\n          long ("); Serial.print(sizeof(long)); Serial.print(") "); Serial.print(TEST_LONG); EEPROM.writeLong(32, TEST_LONG);
  Serial.print("\n unsigned long ("); Serial.print(sizeof(unsigned long)); Serial.print(") "); Serial.print(TEST_ULONG); EEPROM.writeLong(40, TEST_ULONG);  
  Serial.print("\n         float ("); Serial.print(sizeof(float)); Serial.print(") "); Serial.print(TEST_FLOAT, 5); EEPROM.writeFloat(48, TEST_FLOAT);
  Serial.print("\n        double ("); Serial.print(sizeof(double)); Serial.print(") "); Serial.print(TEST_DOUBLE, 5); EEPROM.writeDouble(56, TEST_DOUBLE);
  
  Serial.print("\n    array char ("); Serial.print(TEST_ARRAYCHARINDEX); Serial.print(") ");
  for(uint8_t x = 0; x < TEST_ARRAYCHARINDEX; x++)
    Serial.print(testCharArray[x]); 
  EEPROM.writeCharArray(64, testCharArray, TEST_ARRAYCHARINDEX);

  Serial.print("\n    array byte ("); Serial.print(TEST_ARRAYBYTEINDEX); Serial.print(") ");
  for(uint8_t x = 0; x < TEST_ARRAYBYTEINDEX; x++)
    Serial.print(testByteArray[x]); 
  EEPROM.writeByteArray((64 + TEST_ARRAYCHARINDEX), testByteArray, TEST_ARRAYBYTEINDEX);
}

void VariableTestRead()
{
  char buffCharArray[TEST_ARRAYCHARINDEX];
  byte buffByteArray[TEST_ARRAYBYTEINDEX];

  Serial.print("\n\n ..........Reading Variables..............");

  Serial.print("\n          bool ("); Serial.print(sizeof(bool)); Serial.print(") "); EEPROM.readBool(0) ? Serial.print("True") : Serial.print("False");
  Serial.print("\n          byte ("); Serial.print(sizeof(byte)); Serial.print(") "); Serial.print(EEPROM.readByte(2));
  Serial.print("\n          char ("); Serial.print(sizeof(char)); Serial.print(") "); Serial.print(EEPROM.readChar(4));
  Serial.print("\n unsigned char ("); Serial.print(sizeof(unsigned char)); Serial.print(") "); Serial.print( EEPROM.readUnsignedChar(6));
  Serial.print("\n         short ("); Serial.print(sizeof(short)); Serial.print(") "); Serial.print(EEPROM.readShort(8));
  Serial.print("\n          word ("); Serial.print(sizeof(word)); Serial.print(") "); Serial.print(EEPROM.readword(12));
  Serial.print("\n           int ("); Serial.print(sizeof(int)); Serial.print(") "); Serial.print(EEPROM.readInt(16)); 
  Serial.print("\n  unsigned int ("); Serial.print(sizeof(unsigned int)); Serial.print(") "); Serial.print(EEPROM.readUnsignedInt(24)); 
  Serial.print("\n          long ("); Serial.print(sizeof(long)); Serial.print(") "); Serial.print(EEPROM.readLong(32)); 
  Serial.print("\n unsigned long ("); Serial.print(sizeof(unsigned long)); Serial.print(") "); Serial.print( EEPROM.readUnsignedLong(40)); 
  Serial.print("\n         float ("); Serial.print(sizeof(float)); Serial.print(") "); Serial.print(EEPROM.readFloat(48), 5);
  Serial.print("\n        double ("); Serial.print(sizeof(double)); Serial.print(") "); Serial.print(EEPROM.readDouble(56), 5); 

  EEPROM.readCharArray(64, buffCharArray, TEST_ARRAYCHARINDEX);
  Serial.print("\n    array char ("); Serial.print(TEST_ARRAYCHARINDEX); Serial.print(") ");
  for(uint8_t x = 0; x < TEST_ARRAYCHARINDEX; x++)
    Serial.print(buffCharArray[x]); 

  EEPROM.readByteArray((64 + TEST_ARRAYCHARINDEX), buffByteArray, TEST_ARRAYBYTEINDEX); 
  Serial.print("\n    array byte ("); Serial.print(TEST_ARRAYBYTEINDEX); Serial.print(") ");
  for(uint8_t x = 0; x < TEST_ARRAYBYTEINDEX; x++)
    Serial.print(buffByteArray[x]);
}