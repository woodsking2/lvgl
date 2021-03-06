/**
 * @file lv_math.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_math.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static const int16_t sin0_90_table[] = {
    0,     572,   1144,  1715,  2286,  2856,  3425,  3993,  4560,  5126,  5690,  6252,  6813,  7371,  7927,  8481,
    9032,  9580,  10126, 10668, 11207, 11743, 12275, 12803, 13328, 13848, 14364, 14876, 15383, 15886, 16383, 16876,
    17364, 17846, 18323, 18794, 19260, 19720, 20173, 20621, 21062, 21497, 21925, 22347, 22762, 23170, 23571, 23964,
    24351, 24730, 25101, 25465, 25821, 26169, 26509, 26841, 27165, 27481, 27788, 28087, 28377, 28659, 28932, 29196,
    29451, 29697, 29934, 30162, 30381, 30591, 30791, 30982, 31163, 31335, 31498, 31650, 31794, 31927, 32051, 32165,
    32269, 32364, 32448, 32523, 32587, 32642, 32687, 32722, 32747, 32762, 32767};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Return with sinus of an angle
 * @param angle
 * @return sinus of 'angle'. sin(-90) = -32767, sin(90) = 32767
 */
int16_t lv_trigo_sin(int16_t angle)
{
    int16_t ret = 0;
    angle       = angle % 360;

    if(angle < 0) angle = 360 + angle;

    if(angle < 90) {
        ret = sin0_90_table[angle];
    } else if(angle >= 90 && angle < 180) {
        angle = 180 - angle;
        ret   = sin0_90_table[angle];
    } else if(angle >= 180 && angle < 270) {
        angle = angle - 180;
        ret   = -sin0_90_table[angle];
    } else { /*angle >=270*/
        angle = 360 - angle;
        ret   = -sin0_90_table[angle];
    }

    return ret;
}

/**
 * Calculate a value of a Cubic Bezier function.
 * @param t time in range of [0..LV_BEZIER_VAL_MAX]
 * @param u0 start values in range of [0..LV_BEZIER_VAL_MAX]
 * @param u1 control value 1 values in range of [0..LV_BEZIER_VAL_MAX]
 * @param u2 control value 2 in range of [0..LV_BEZIER_VAL_MAX]
 * @param u3 end values in range of [0..LV_BEZIER_VAL_MAX]
 * @return the value calculated from the given parameters in range of [0..LV_BEZIER_VAL_MAX]
 */
int32_t lv_bezier3(uint32_t t, int32_t u0, int32_t u1, int32_t u2, int32_t u3)
{
    uint32_t t_rem  = 1024 - t;
    uint32_t t_rem2 = (t_rem * t_rem) >> 10;
    uint32_t t_rem3 = (t_rem2 * t_rem) >> 10;
    uint32_t t2     = (t * t) >> 10;
    uint32_t t3     = (t2 * t) >> 10;

    uint32_t v1 = ((uint32_t)t_rem3 * u0) >> 10;
    uint32_t v2 = ((uint32_t)3 * t_rem2 * t * u1) >> 20;
    uint32_t v3 = ((uint32_t)3 * t_rem * t2 * u2) >> 20;
    uint32_t v4 = ((uint32_t)t3 * u3) >> 10;

    return v1 + v2 + v3 + v4;
}
#define BITSPERLONG 32UL

#define TOP2BITS(x) ((x & (3UL << (BITSPERLONG-2))) >> (BITSPERLONG-2))

void lv_sqrt(uint32_t x, lv_sqrt_res_t * q)
{

    if(x == 0) {
        q->f = 0;
        q->i = 0;
        return;
    }

    /*Look up for x=1..1024*/
    /*Look up for x=1..1024*/
     static const uint8_t ci[] = {
             1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
             6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8,
             8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10,
             10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11,
             11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
             12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
             13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
             14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15,
             15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16,
             16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
             16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
             17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
             18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19,
             19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
             19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
             20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21,
             21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
             21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
             22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
             22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
             23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
             23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
             24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
             24, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
             25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
             26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
             26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
             26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
             27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
             27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
             28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
             28, 28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
             29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
             29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
             30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
             30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31,
             31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
             31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
             31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 32,
     };

     static const uint8_t cf[] =  {
             0, 106, 187, 0, 60, 115, 165, 212, 0, 42, 81, 119, 155, 190, 223, 0, 32, 62, 92, 121, 149, 177, 204, 230,
             0, 25, 50, 75, 99, 122, 145, 168, 191, 213, 235, 0, 21, 42, 63, 83, 103, 123, 143, 162, 181, 200, 219,
             238, 0, 18, 36, 54, 72, 89, 107, 124, 141, 158, 174, 191, 207, 224, 240, 0, 16, 32, 47, 63, 78, 94, 109,
             124, 139, 154, 169, 184, 198, 213, 227, 242, 0, 14, 28, 42, 56, 70, 84, 97, 111, 125, 138, 151, 165, 178,
             191, 204, 217, 230, 243, 0, 13, 25, 38, 51, 63, 76, 88, 100, 113, 125, 137, 149, 161, 173, 185, 197, 209,
             221, 233, 244, 0, 12, 23, 35, 46, 58, 69, 80, 92, 103, 114, 125, 136, 147, 158, 169, 180, 191, 202, 213,
             224, 235, 245, 0, 11, 21, 32, 42, 53, 63, 74, 84, 95, 105, 115, 125, 136, 146, 156, 166, 176, 186, 196,
             206, 216, 226, 236, 246, 0, 10, 20, 29, 39, 49, 59, 68, 78, 87, 97, 107, 116, 126, 135, 145, 154, 163,
             173, 182, 191, 201, 210, 219, 228, 238, 247, 0, 9, 18, 27, 36, 45, 54, 63, 72, 81, 90, 99, 108, 117, 126,
             135, 143, 152, 161, 170, 178, 187, 196, 204, 213, 222, 230, 239, 247, 0, 9, 17, 26, 34, 42, 51, 59, 68,
             76, 84, 93, 101, 109, 118, 126, 134, 142, 151, 159, 167, 175, 183, 191, 200, 208, 216, 224, 232, 240, 248,
             0, 8, 16, 24, 32, 40, 48, 56, 64, 71, 79, 87, 95, 103, 111, 118, 126, 134, 142, 149, 157, 165, 172, 180,
             188, 195, 203, 211, 218, 226, 233, 241, 248, 0, 8, 15, 23, 30, 37, 45, 52, 60, 67, 75, 82, 89, 97, 104,
             112, 119, 126, 133, 141, 148, 155, 163, 170, 177, 184, 192, 199, 206, 213, 220, 227, 235, 242, 249, 0, 7,
             14, 21, 28, 35, 42, 50, 57, 64, 71, 78, 85, 92, 99, 105, 112, 119, 126, 133, 140, 147, 154, 161, 168, 174,
             181, 188, 195, 202, 209, 215, 222, 229, 236, 243, 249, 0, 7, 13, 20, 27, 34, 40, 47, 54, 60, 67, 74, 80,
             87, 93, 100, 107, 113, 120, 126, 133, 139, 146, 153, 159, 166, 172, 179, 185, 192, 198, 205, 211, 217,
             224, 230, 237, 243, 250, 0, 6, 13, 19, 26, 32, 38, 45, 51, 57, 64, 70, 76, 83, 89, 95, 101, 108, 114, 120,
             126, 133, 139, 145, 151, 158, 164, 170, 176, 182, 189, 195, 201, 207, 213, 219, 225, 232, 238, 244, 250,
             0, 6, 12, 18, 24, 30, 36, 42, 49, 55, 61, 67, 73, 79, 85, 91, 97, 103, 109, 115, 121, 127, 132, 138, 144,
             150, 156, 162, 168, 174, 180, 186, 192, 198, 203, 209, 215, 221, 227, 233, 239, 244, 250, 0, 6, 12, 17,
             23, 29, 35, 41, 46, 52, 58, 64, 69, 75, 81, 87, 92, 98, 104, 109, 115, 121, 127, 132, 138, 144, 149, 155,
             161, 166, 172, 178, 183, 189, 194, 200, 206, 211, 217, 223, 228, 234, 239, 245, 250, 0, 6, 11, 17, 22, 28,
             33, 39, 44, 50, 55, 61, 66, 72, 77, 83, 88, 94, 99, 105, 110, 116, 121, 127, 132, 138, 143, 148, 154, 159,
             165, 170, 175, 181, 186, 192, 197, 202, 208, 213, 219, 224, 229, 235, 240, 245, 251, 0, 5, 11, 16, 21, 27,
             32, 37, 43, 48, 53, 58, 64, 69, 74, 79, 85, 90, 95, 101, 106, 111, 116, 121, 127, 132, 137, 142, 148, 153,
             158, 163, 168, 174, 179, 184, 189, 194, 199, 205, 210, 215, 220, 225, 230, 235, 241, 246, 251, 0, 5, 10,
             15, 20, 26, 31, 36, 41, 46, 51, 56, 61, 66, 71, 76, 81, 86, 92, 97, 102, 107, 112, 117, 122, 127, 132,
             137, 142, 147, 152, 157, 162, 167, 172, 177, 182, 187, 192, 197, 202, 207, 212, 216, 221, 226, 231, 236,
             241, 246, 251, 0, 5, 10, 15, 20, 25, 29, 34, 39, 44, 49, 54, 59, 64, 69, 73, 78, 83, 88, 93, 98, 103, 107,
             112, 117, 122, 127, 132, 136, 141, 146, 151, 156, 161, 165, 170, 175, 180, 185, 189, 194, 199, 204, 208,
             213, 218, 223, 227, 232, 237, 242, 247, 251, 0, 5, 9, 14, 19, 24, 28, 33, 38, 43, 47, 52, 57, 61, 66, 71,
             75, 80, 85, 89, 94, 99, 104, 108, 113, 118, 122, 127, 131, 136, 141, 145, 150, 155, 159, 164, 169, 173,
             178, 182, 187, 192, 196, 201, 206, 210, 215, 219, 224, 229, 233, 238, 242, 247, 251, 0, 5, 9, 14, 18, 23,
             27, 32, 36, 41, 46, 50, 55, 59, 64, 68, 73, 77, 82, 86, 91, 95, 100, 104, 109, 113, 118, 122, 127, 131,
             136, 140, 145, 149, 154, 158, 163, 167, 172, 176, 181, 185, 189, 194, 198, 203, 207, 212, 216, 221, 225,
             229, 234, 238, 243, 247, 252, 0, 4, 9, 13, 18, 22, 26, 31, 35, 40, 44, 48, 53, 57, 62, 66, 70, 75, 79, 83,
             88, 92, 96, 101, 105, 110, 114, 118, 123, 127, 131, 136, 140, 144, 149, 153, 157, 162, 166, 170, 175, 179,
             183, 187, 192, 196, 200, 205, 209, 213, 218, 222, 226, 230, 235, 239, 243, 247, 252, 0, 4, 9, 13, 17, 21,
             26, 30, 34, 38, 43, 47, 51, 55, 60, 64, 68, 72, 76, 81, 85, 89, 93, 98, 102, 106, 110, 114, 119, 123,
             127, 131, 135, 140, 144, 148, 152, 156, 160, 165, 169, 173, 177, 181, 185, 190, 194, 198, 202, 206, 210,
             215, 219, 223, 227, 231, 235, 239, 244, 248, 252, 0, 4, 8, 12, 16, 21, 25, 29, 33, 37, 41, 45, 49, 53, 58,
             62, 66, 70, 74, 78, 82, 86, 90, 94, 98, 103, 107, 111, 115, 119, 123, 127, 131, 135, 139, 143, 147, 151,
             155, 159, 163, 168, 172, 176, 180, 184, 188, 192, 196, 200, 204, 208, 212, 216, 220, 224, 228, 232, 236,
             240, 244, 248, 252, 0,

     };
    if(x <= 64) {
        x--;
        q->i = ci[x];
        q->f = cf[x];
        return;
    }

    /*
     * Source:
     * http://web.archive.org/web/20080303101624/http://c.snippets.org/snip_lister.php?fname=isqrt.c
     * https://stackoverflow.com/questions/1100090/looking-for-an-efficient-integer-square-root-algorithm-for-arm-thumb2
     */

    uint32_t a = 0L;                   /* accumulator      */
    uint32_t r = 0L;                   /* remainder        */
    uint32_t e = 0L;                   /* trial product    */

    uint32_t i;
    for (i = 0; i < BITSPERLONG / 2 + 8; i++) {
          r = (r << 2) + TOP2BITS(x); x <<= 2;
          a <<= 1;
          e = (a << 1) + 1;
          if (r >= e)  {
                r -= e;
                a++;
          }
    }

    q->f = a & 0xFF;
    q->i = a >> 8;
}

/**
 * Calculate the atan2 of a vector.
 * @param x
 * @param y
 * @return the angle in degree calculated from the given parameters in range of [0..360]
 */
uint16_t lv_atan2(int x, int y)
{
    // Fast XY vector to integer degree algorithm - Jan 2011 www.RomanBlack.com
    // Converts any XY values including 0 to a degree value that should be
    // within +/- 1 degree of the accurate value without needing
    // large slow trig functions like ArcTan() or ArcCos().
    // NOTE! at least one of the X or Y values must be non-zero!
    // This is the full version, for all 4 quadrants and will generate
    // the angle in integer degrees from 0-360.
    // Any values of X and Y are usable including negative values provided
    // they are between -1456 and 1456 so the 16bit multiply does not overflow.

    unsigned char negflag;
    unsigned char tempdegree;
    unsigned char comp;
    unsigned int degree;     // this will hold the result
    //signed int x;            // these hold the XY vector at the start
    //signed int y;            // (and they will be destroyed)
    unsigned int ux;
    unsigned int uy;

    // Save the sign flags then remove signs and get XY as unsigned ints
    negflag = 0;
    if(x < 0) {
        negflag += 0x01;    // x flag bit
        x = (0 - x);        // is now +
    }
    ux = x;                // copy to unsigned var before multiply
    if(y < 0) {
        negflag += 0x02;    // y flag bit
        y = (0 - y);        // is now +
    }
    uy = y;                // copy to unsigned var before multiply

    // 1. Calc the scaled "degrees"
    if(ux > uy) {
        degree = (uy * 45) / ux;   // degree result will be 0-45 range
        negflag += 0x10;    // octant flag bit
    } else {
        degree = (ux * 45) / uy;   // degree result will be 0-45 range
    }

    // 2. Compensate for the 4 degree error curve
    comp = 0;
    tempdegree = degree;    // use an unsigned char for speed!
    if(tempdegree > 22) {    // if top half of range
        if(tempdegree <= 44) comp++;
        if(tempdegree <= 41) comp++;
        if(tempdegree <= 37) comp++;
        if(tempdegree <= 32) comp++;  // max is 4 degrees compensated
    } else { // else is lower half of range
        if(tempdegree >= 2) comp++;
        if(tempdegree >= 6) comp++;
        if(tempdegree >= 10) comp++;
        if(tempdegree >= 15) comp++;  // max is 4 degrees compensated
    }
    degree += comp;   // degree is now accurate to +/- 1 degree!

    // Invert degree if it was X>Y octant, makes 0-45 into 90-45
    if(negflag & 0x10) degree = (90 - degree);

    // 3. Degree is now 0-90 range for this quadrant,
    // need to invert it for whichever quadrant it was in
    if(negflag & 0x02) { // if -Y
        if(negflag & 0x01)   // if -Y -X
            degree = (180 + degree);
        else        // else is -Y +X
            degree = (180 - degree);
    } else { // else is +Y
        if(negflag & 0x01)   // if +Y -X
            degree = (360 - degree);
    }
    return degree;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
