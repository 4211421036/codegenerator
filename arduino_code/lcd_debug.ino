#define EBOARD_I2C 0x1
#define EBOARD_LCD 0x1
#include <eBoard.h>
SoccerBoard board;
LCD lcd(board);
DynamixelBoard dBoard(board);

AX12Servo links(dBoard,1);
AX12Servo rechts(dBoard,2);

int main()
{
    links.storePosition(40);
    rechts.storePosition(200);
    //lcd.changeID(0x3C);
    board.sleep(1);
    lcd.clear();
    lcd.print(links.getPosition());
    lcd.print(1,0,rechts.getPosition());
    dBoard.action();
    board.sleep(1);
    lcd.changeBackground(true);
    lcd.print(2,0,links.getPosition());
    lcd.print(3,0,rechts.getPosition());
    return 0;
  }
