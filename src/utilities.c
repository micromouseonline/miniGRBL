#include "grbl.h"
#include "utilities.h"


RESET_TYPE resetSource;

void getResetSource(void) {
  resetSource = OTHER_RESET;
  // test the reset flags in order because the pin reset is always set.
  if (RCC_GetFlagStatus(RCC_FLAG_SFTRST)) {
    resetSource = SOFT_RESET;
  } else if (RCC_GetFlagStatus(RCC_FLAG_PORRST)) {
    resetSource = POWER_ON_RESET;
  } else if (RCC_GetFlagStatus(RCC_FLAG_PINRST)) {
    resetSource = PIN_RESET;
  } else {
    resetSource = OTHER_RESET;
  } // The flags must be cleared manually after use
  RCC_ClearFlag();
}

void checkReset(void) {
  switch (resetSource) {
    case SOFT_RESET:
      printPgmString(PSTR("\r\nSoft reset\r\n"));
      break;
    case POWER_ON_RESET:
      printPgmString(PSTR("\r\nPower-on reset\r\n"));
      break;
    case PIN_RESET:
      printPgmString(PSTR("\r\nPin reset\r\n"));
      break;
    default:
      printPgmString(PSTR("\r\nUnknown reset - PANIC!\r\n"));
      break;
  }
}
