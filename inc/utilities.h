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

void test_reset_source(void);
void  report_reset_source(void);

#ifdef	__cplusplus
}
#endif

#endif	/* UTILITIES_H */
