/**
 *  @author  Tyler Lucero
 */
 
/** @todo implement in <code>numconv.c</code> based on documentation contained 
 *  in <code>numconv.h</code>.
 */
#include <stdio.h>
int origR = 0;
char int2char (int radix, int value) {
  if (radix > 36 || radix < 2 || value >= radix || value < 0){
    return '?'; }
  if (value < 10){
    return (char) (value + 48); }
  if (value >= 10){
    return (char) (value + 55); }
  return '?';
} 

/** @todo implement in <code>numconv.c</code> based on documentation contained 
 *  in <code>numconv.h</code>.
 */
int char2int (int radix, char digit) 
{
  if (radix > 36 || radix < 2)
  {
    return -1;
  }

  int dig = (int) digit;
  int rdig = (int) digit;

  if ((dig < 58) && (dig > 47))
  {
   rdig = rdig - 48;
   if (rdig < radix)
   {
     return rdig;
   } 
  }

  if ((dig > 64) && (dig < 91))
  {
   rdig = rdig - 55;
   if (rdig < radix) 
   {
     return rdig;
   }
  }

  if ((dig > 96) && (dig < 123))
  {
   rdig = rdig - 87;
   if (rdig < radix) 
   {
     return rdig;
   }
  }

  return -1;
}

/** @todo implement in <code>numconv.c</code> based on documentation contained 
 *  in <code>numconv.h</code>.
 */
void divRem (int numerator, int divisor, int* quotient, int* remainder) 
{
 *quotient = numerator / divisor;
 *remainder = numerator % divisor;
}

/** @todo implement in <code>numconv.c</code> based on documentation contained 
 *  in <code>numconv.h</code>.
 */
int ascii2int (int radix, int valueOfPrefix) {
  if (radix > 36 || radix < 2)
  {
    return -1;
  }
  char n = getchar();
  if (n != '\n')
  {
    int newChar;
    if (n < 58 && n > 47)
      newChar = (int)n - 48;
    if (n < 91 && n > 64)
      newChar = (int)n - 55;
    if (n < 123 && n > 96)
      newChar = (int)n - 87;
    valueOfPrefix = radix * valueOfPrefix + newChar;
    return ascii2int(radix, valueOfPrefix);
  }
  return valueOfPrefix; 

}

/** @todo implement in <code>numconv.c</code> based on documentation contained 
 *  in <code>numconv.h</code>.
 */
void int2ascii (int radix, int value) {
  int remain;
  remain = value % radix;
  value = value / radix;
  if (value != 0)
   {
     int2ascii(radix, value);
   } 
   putchar(int2char(radix, remain));
}

/** @todo implement in <code>numconv.c</code> based on documentation contained 
 *  in <code>numconv.h</code>.
 */
double frac2double (int radix) 
{
if (origR == 0)
{
 origR = radix;
}
  char c = getchar();
  double result;
  double val = c + 0;

  if ((val < 58) && (val > 47))
  {
   val = val - 48;
  }

  if ((val > 64) && (val < 91))
  {
   val = val - 55;
  }

  if ((val > 96) && (val < 123))
  {
   val = val - 87;
  }

  if (c != '\n')
  {
    result = frac2double(origR * radix) + (val / radix);
  }
  
  return result; 
}

