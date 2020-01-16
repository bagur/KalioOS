/* KalioOS (C) 2020 Pranav Bagur */

#include "if/common.h"

/* -------------------------------------------------------------------------- 
                         Export functions
   -------------------------------------------------------------------------- */ 
/* 
 * EF: get_num_digits - get number of digits in a num
 * 
 * ARGS :-
 *   num - to find digits
 *
 * RET -
 *   num of digits
 */
ub4
get_num_digits(ub4 num)
{
  ub4 digits = 0;

  while (num > 0) {
    digits++;
    num = (ub4)(num / 10);
  }

  return digits;  
}

/* 
 * EF: convert_to_str - convert num to string
 * 
 * ARGS :-
 *   num - number to convert
 *   str - ptr to char arr
 *
 * We don't have the ability to allocate mem yet :(
 *
 * RET -
 */
void
convert_to_str(ub4 num, char *str, ub4 len)
{
  sb4 i = len - 1;

  while (i >= 0) {
    str[i--] = (char)((num % 10) + 48);
    num = (ub4)(num / 10);
  }
}

/* 
 * EF: strlen - length of str
 * 
 * ARGS :-
 *   str - string
 *
 * RET -
 *   length of string
 */
ub4 
strlen(ub1 *str) {
  ub4 i = 0;

  while (str[i] != '\0') ++i;
  return i;
}

/* 
 * EF: strcmp - compare 2 strings
 * 
 * ARGS :-
 *   str1 - string1
 *   str2 - string2
 *
 * RET -
 * less than 0 if str1 < str2, 
 * equal to 0 if str1 == str2,
 * greater than 0 otherwise
 */
ub4
strcmp(ub1 *str1, ub1 *str2) 
{
  ub4 i;

  for (i = 0; str1[i] == str2[i]; i++) {
    if (str1[i] == '\0') 
      return 0;
  }

  return str1[i] - str2[i];
}
