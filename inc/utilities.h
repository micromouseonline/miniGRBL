/*
 * File:   utilities.h
 * Author: peterharrison
 *
 * Created on 03 March 2013, 12:32
 */

#ifndef UTILITIES_H
#define	UTILITIES_H

#ifdef	__cplusplus
extern "C" {
#endif

  typedef   enum {
    SOFT_RESET,
    POWER_ON_RESET,
    PIN_RESET,
    OTHER_RESET
  }
  RESET_TYPE;

  extern RESET_TYPE resetSource;
  void  getResetSource (void);
  void checkReset (void);

#ifdef	__cplusplus
}
#endif

#endif	/* UTILITIES_H */
