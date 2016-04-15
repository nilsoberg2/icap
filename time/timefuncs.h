// ==============================================================================
// ICAP License
// ==============================================================================
// University of Illinois/NCSA
// Open Source License
// 
// Copyright (c) 2014-2016 University of Illinois at Urbana-Champaign.
// All rights reserved.
// 
// Developed by:
// 
//     Nils Oberg
//     Blake J. Landry, PhD
//     Arthur R. Schmidt, PhD
//     Ven Te Chow Hydrosystems Lab
// 
//     University of Illinois at Urbana-Champaign
// 
//     https://vtchl.illinois.edu
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal with
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
// 
//     * Redistributions of source code must retain the above copyright notice,
//       this list of conditions and the following disclaimers.
// 
//     * Redistributions in binary form must reproduce the above copyright notice,
//       this list of conditions and the following disclaimers in the
//       documentation and/or other materials provided with the distribution.
// 
//     * Neither the names of the Ven Te Chow Hydrosystems Lab, University of
// 	  Illinois at Urbana-Champaign, nor the names of its contributors may be
// 	  used to endorse or promote products derived from this Software without
// 	  specific prior written permission.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE
// SOFTWARE.

#ifndef TIMEFUNCS_H__
#define TIMEFUNCS_H__

#if !defined(SWMM_GEOMETRY)

//-----------------------------------------------------------------------------
//   datetime.h
//
//   Project:  EPA SWMM5
//   Version:  5.0
//   Date:     6/19/07   (Build 5.0.010)
//   Author:   L. Rossman
//
//   The DateTime type is used to store date and time values. It is
//   equivalent to a double floating point type.
//
//   The integral part of a DateTime value is the number of days that have
//   passed since 12/31/1899. The fractional part of a DateTime value is the
//   fraction of a 24 hour day that has elapsed.
//-----------------------------------------------------------------------------

typedef double DateTime;

#define Y_M_D 0
#define M_D_Y 1
#define D_M_Y 2
#define NO_DATE -693594 // 1/1/0001

// Functions for encoding a date or time value to a DateTime value
DateTime datetime_encodeDate(int year, int month, int day);
DateTime datetime_encodeTime(int hour, int minute, int second);

// Functions for decoding a DateTime value to a date and time
void datetime_decodeDate(DateTime date, int* y, int* m, int* d);
void datetime_decodeTime(DateTime time, int* h, int* m, int* s);

// Function for finding day of week for a date (1 = Sunday)
// month of year, days per month, and hour of day
int  datetime_monthOfYear(DateTime date);
int  datetime_dayOfYear(DateTime date);
int  datetime_dayOfWeek(DateTime date);
int  datetime_hourOfDay(DateTime date);
int  datetime_daysPerMonth(int year, int month);

// Functions for converting a DateTime value to a string
void datetime_dateToStr(DateTime date, char* s);
void datetime_timeToStr(DateTime time, char* s);

// Functions for converting a string date or time to a DateTime value
int  datetime_findMonth(const char* s);
int  datetime_strToDate(const char* s, DateTime* d);
int  datetime_strToTime(const char* s, DateTime* t);

// Function for setting date format
void datetime_setDateFormat(int fmt);

// Functions for adding and subtracting dates
DateTime datetime_addSeconds(DateTime date1, double seconds);
DateTime datetime_addDays(DateTime date1, DateTime date2);
long     datetime_timeDiff(DateTime date1, DateTime date2);


#endif// !defined(SWMM_GEOMETRY)

#endif//TIMEFUNCS_H__
