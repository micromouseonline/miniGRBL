/*
  coolant_control.c - coolant control methods
  Part of Grbl

  Copyright (c) 2012-2016 Sungeun K. Jeon for Gnea Research LLC

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "grbl.h"


void coolant_init() {
#ifdef AVRTARGET
  COOLANT_FLOOD_DDR |= (1 << COOLANT_FLOOD_BIT); // Configure as output pin
#ifdef ENABLE_M7
  COOLANT_MIST_DDR |= (1 << COOLANT_MIST_BIT);
#endif
#endif
#ifdef STM32F103C8
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_COOLANT_FLOOD_PORT, ENABLE);
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = 1 << COOLANT_FLOOD_BIT;
  GPIO_Init(COOLANT_FLOOD_PORT, &GPIO_InitStructure);

  RCC_APB2PeriphClockCmd(RCC_COOLANT_MIST_PORT, ENABLE);
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = 1 << COOLANT_MIST_BIT;
  GPIO_Init(COOLANT_MIST_PORT, &GPIO_InitStructure);
  ResetFloodEnablebit();
  ResetMistEnablebit();
#endif
  coolant_stop();
}


// Returns current coolant output state. Overrides may alter it from programmed state.
uint8_t coolant_get_state() {
  uint8_t cl_state = COOLANT_STATE_DISABLE;
  //#if defined(AVRTARGET) || defined(STM32F103C8)
  //#if defined(STM32F103C8)
#ifdef INVERT_COOLANT_FLOOD_PIN
  //if (bit_isfalse(
  if (bit_istrue(GPIO_ReadOutputData(COOLANT_FLOOD_PORT), (1 << COOLANT_FLOOD_BIT))) {
    cl_state &= COOLANT_STATE_FLOOD;
  }
  //#ifdef AVRTARGET
  //		COOLANT_FLOOD_PORT
  //#else
  //		GPIO_ReadOutputData(COOLANT_FLOOD_PORT)
  //#endif
  //		,(1 << COOLANT_FLOOD_BIT))) {
#else
  //if (bit_istrue(
  if (bit_istrue(GPIO_ReadOutputData(COOLANT_FLOOD_PORT), (1 << COOLANT_FLOOD_BIT))) {
    //#ifdef AVRTARGET
    //		COOLANT_FLOOD_PORT
    //#else
    //GPIO_ReadOutputData(COOLANT_FLOOD_PORT)
    //#endif
    //,(1 << COOLANT_FLOOD_BIT))) {
    //#endif
    cl_state |= COOLANT_STATE_FLOOD;
  }
#endif
#ifdef ENABLE_M7
#ifdef INVERT_COOLANT_MIST_PIN
  //      if (bit_isfalse(
  if (bit_istrue(GPIO_ReadOutputData(COOLANT_FLOOD_PORT), (1 << COOLANT_MIST_BIT))) {
    cl_state &= COOLANT_STATE_MIST;
  }
  //#ifdef AVRTARGET
  //		  COOLANT_MIST_PORT
  //#else
  //		  GPIO_ReadOutputData(COOLANT_MIST_PORT)
  //    #endif
  //		  ,(1 << COOLANT_MIST_BIT))) {
#else
  //      if (bit_istrue(
  if (bit_istrue(GPIO_ReadOutputData(COOLANT_MIST_PORT), (1 << COOLANT_MIST_BIT))) {
    cl_state |= COOLANT_STATE_MIST;
  }
  //#ifdef AVRTARGET
  //		  COOLANT_MIST_PORT
  //#else
  //		  GPIO_ReadOutputData(COOLANT_MIST_PORT)
#endif
  //		  ,(1 << COOLANT_MIST_BIT))) {
  //    #endif
  //     cl_state |= COOLANT_STATE_MIST;
  //   }
#endif
  //#endif
  return (cl_state);
}


// Directly called by coolant_init(), coolant_set_state(), and mc_reset(), which can be at
// an interrupt-level. No report flag set, but only called by routines that don't need it.
void coolant_stop() {
#if defined(AVRTARGET) || defined(STM32F103C8)
#ifdef INVERT_COOLANT_FLOOD_PIN
#ifdef AVRTARGET
  COOLANT_FLOOD_PORT |= (1 << COOLANT_FLOOD_BIT);
#else
  GPIO_SetBits(COOLANT_FLOOD_PORT, COOLANT_FLOOD_BIT);
#endif
#else
#ifdef AVRTARGET
  COOLANT_FLOOD_PORT &= ~(1 << COOLANT_FLOOD_BIT);
#else
  //GPIO_Write(DIRECTION_PORT, (GPIO_ReadOutputData(DIRECTION_PORT) & ~DIRECTION_MASK) | (st.dir_outbits & DIRECTION_MASK));
  //GPIO_ResetBits(COOLANT_FLOOD_PORT, COOLANT_FLOOD_BIT);
  //GPIO_ResetBits(COOLANT_FLOOD_PORT,1 << COOLANT_FLOOD_BIT);
  ResetFloodEnablebit();

#endif
#endif
#ifdef ENABLE_M7
#ifdef INVERT_COOLANT_MIST_PIN
#ifdef AVRTARGET
  COOLANT_MIST_PORT |= (1 << COOLANT_MIST_BIT);
#else
  //GPIO_SetBits(COOLANT_MIST_PORT, COOLANT_MIST_BIT);
  SetMistEnablebit();
#endif
#else
#ifdef AVRTARGET
  COOLANT_MIST_PORT &= ~(1 << COOLANT_MIST_BIT);
#else
  //	GPIO_ResetBits(COOLANT_MIST_PORT, COOLANT_MIST_BIT);
  ResetMistEnablebit();

#endif
#endif
#endif
#endif
}


// Main program only. Immediately sets flood coolant running state and also mist coolant,
// if enabled. Also sets a flag to report an update to a coolant state.
// Called by coolant toggle override, parking restore, parking retract, sleep mode, g-code
// parser program end, and g-code parser coolant_sync().
void coolant_set_state(uint8_t mode) {
  if (sys.abort) {
    return;  // Block during abort.
  }

  if (mode == COOLANT_DISABLE) {

    coolant_stop();

  } else {

#if defined(AVRTARGET) || defined(STM32F103C8)
    if (mode & COOLANT_FLOOD_ENABLE) {
#ifdef INVERT_COOLANT_FLOOD_PIN
#ifdef AVRTARGET
      COOLANT_FLOOD_PORT &= ~(1 << COOLANT_FLOOD_BIT);
#else
      GPIO_ResetBits(COOLANT_FLOOD_PORT, COOLANT_FLOOD_BIT);
#endif
#else
#ifdef AVRTARGET
      COOLANT_FLOOD_PORT |= (1 << COOLANT_FLOOD_BIT);
#else
      SetFloodEnablebit();

      //GPIO_SetBits(COOLANT_FLOOD_PORT, COOLANT_FLOOD_BIT);
      //GPIO_SetBits(COOLANT_FLOOD_PORT,1 << COOLANT_FLOOD_BIT);
      //		GPIO_SetBits(GPIOB,COOLANT_FLOOD_BIT);
#endif
#endif
    }

#ifdef ENABLE_M7
    if (mode & COOLANT_MIST_ENABLE) {
#ifdef INVERT_COOLANT_MIST_PIN
#ifdef AVRTARGET
      COOLANT_MIST_PORT &= ~(1 << COOLANT_MIST_BIT);
#else
      //GPIO_ResetBits(COOLANT_MIST_PORT, COOLANT_MIST_BIT);
      ResetMistEnablebit();
#endif
#else
#ifdef AVRTARGET
      COOLANT_MIST_PORT |= (1 << COOLANT_MIST_BIT);
#else
      SetMistEnablebit();

      // GPIO_SetBits(COOLANT_MIST_PORT, COOLANT_MIST_BIT);
#endif
#endif
    }
#endif
#endif
  }
  sys.report_ovr_counter = 0; // Set to report change immediately
}


// G-code parser entry-point for setting coolant state. Forces a planner buffer sync and bails
// if an abort or check-mode is active.
void coolant_sync(uint8_t mode) {
  if (sys.state == STATE_CHECK_MODE) {
    return;
  }
  protocol_buffer_synchronize(); // Ensure coolant turns on when specified in program.
  coolant_set_state(mode);
}
