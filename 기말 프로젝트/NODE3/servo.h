/*
 * File: servo.h
 *
 * Real-Time Workshop code generated for Simulink model servo.
 *
 * Model version                        : 1.71
 * Real-Time Workshop file version      : 7.5  (R2010a)  25-Jan-2010
 * Real-Time Workshop file generated on : Sat Jan 02 01:05:08 2016
 * TLC version                          : 7.5 (Jan 19 2010)
 * C/C++ source code generated on       : Sat Jan 02 01:05:08 2016
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Atmel->AVR
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_servo_h_
#define RTW_HEADER_servo_h_
#ifndef servo_COMMON_INCLUDES_
# define servo_COMMON_INCLUDES_
#include <stddef.h>
#include <string.h>
#include "rtwtypes.h"
#endif                                 /* servo_COMMON_INCLUDES_ */

#include "servo_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

/* Block signals (auto storage) */
typedef struct {
  real_T warn_stop;                    /* '<Root>/Chart' */
  real_T angle;                        /* '<Root>/Chart' */
  real_T cds;                          /* '<Root>/Chart' */
  real_T porte;                        /* '<Root>/Chart' */
  real_T portb;                        /* '<Root>/Chart' */
  int16_T ocr3c;                       /* '<Root>/Chart' */
} BlockIO_servo;

/* Block states (auto storage) for system '<Root>' */
typedef struct {
  int8_T warn;                         /* '<Root>/Chart' */
  uint8_T is_active_c1_servo;          /* '<Root>/Chart' */
  uint8_T is_c1_servo;                 /* '<Root>/Chart' */
  uint8_T is_active_warn_check;        /* '<Root>/Chart' */
  uint8_T is_warn_check;               /* '<Root>/Chart' */
  uint8_T is_active_jodo_0to90;        /* '<Root>/Chart' */
  uint8_T is_jodo_0to90;               /* '<Root>/Chart' */
  uint8_T is_active_control_bar;       /* '<Root>/Chart' */
  uint8_T is_control_bar;              /* '<Root>/Chart' */
  uint8_T is_active_bar_check;         /* '<Root>/Chart' */
  uint8_T is_bar_check;                /* '<Root>/Chart' */
  boolean_T open;                      /* '<Root>/Chart' */
} D_Work_servo;

/* External inputs (root inport signals with auto storage) */
typedef struct {
  real_T current_mode;                 /* '<Root>/current_mode' */
  real_T current_speed;                /* '<Root>/current_speed' */
  real_T ADdata;                       /* '<Root>/ADdata' */
  int16_T ting;                        /* '<Root>/ting' */
} ExternalInputs_servo;

/* External outputs (root outports fed by signals with auto storage) */
typedef struct {
  real_T warn_stop;                    /* '<Root>/warn_stop' */
  real_T angle;                        /* '<Root>/angle' */
  real_T cds;                          /* '<Root>/cds' */
  int16_T ocr3c;                       /* '<Root>/ocr3c' */
  real_T porte;                        /* '<Root>/porte' */
  real_T portb;                        /* '<Root>/portb' */
} ExternalOutputs_servo;

/* Real-time Model Data Structure */
struct RT_MODEL_servo {
  const char_T * volatile errorStatus;
};

/* Block signals (auto storage) */
extern BlockIO_servo servo_B;

/* Block states (auto storage) */
extern D_Work_servo servo_DWork;

/* External inputs (root inport signals with auto storage) */
extern ExternalInputs_servo servo_U;

/* External outputs (root outports fed by signals with auto storage) */
extern ExternalOutputs_servo servo_Y;

/* Model entry point functions */
extern void servo_initialize(void);
extern void servo_step(void);
extern void servo_terminate(void);

/* Real-time Model object */
extern struct RT_MODEL_servo *servo_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : servo
 * '<S1>'   : servo/Chart
 */
#endif                                 /* RTW_HEADER_servo_h_ */

/*
 * File trailer for Real-Time Workshop generated code.
 *
 * [EOF]
 */
