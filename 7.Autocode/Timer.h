/*
 * File: Timer.h
 *
 * Real-Time Workshop code generated for Simulink model Timer.
 *
 * Model version                        : 1.7
 * Real-Time Workshop file version      : 7.5  (R2010a)  25-Jan-2010
 * Real-Time Workshop file generated on : Tue Nov 24 18:23:18 2020
 * TLC version                          : 7.5 (Jan 19 2010)
 * C/C++ source code generated on       : Tue Nov 24 18:23:19 2020
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Atmel->AVR
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_Timer_h_
#define RTW_HEADER_Timer_h_
#ifndef Timer_COMMON_INCLUDES_
# define Timer_COMMON_INCLUDES_
#include <string.h>
#include "rtwtypes.h"
#endif                                 /* Timer_COMMON_INCLUDES_ */

#include "Timer_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

/* Block signals (auto storage) */
typedef struct {
  real_T mode;                         /* '<Root>/Chart' */
  real_T porte;                        /* '<Root>/Chart' */
} BlockIO_Timer;

/* Block states (auto storage) for system '<Root>' */
typedef struct {
  uint32_T presentTicks;               /* '<Root>/Chart' */
  uint32_T elapsedTicks;               /* '<Root>/Chart' */
  uint32_T previousTicks;              /* '<Root>/Chart' */
  uint16_T temporalCounter_i1;         /* '<Root>/Chart' */
  uint8_T is_active_c1_Timer;          /* '<Root>/Chart' */
  uint8_T is_c1_Timer;                 /* '<Root>/Chart' */
  uint8_T is_stepOne;                  /* '<Root>/Chart' */
  uint8_T was_stepOne;                 /* '<Root>/Chart' */
} D_Work_Timer;

/* External inputs (root inport signals with auto storage) */
typedef struct {
  real_T In1;                          /* '<Root>/In1' */
} ExternalInputs_Timer;

/* External outputs (root outports fed by signals with auto storage) */
typedef struct {
  real_T Out1;                         /* '<Root>/Out1' */
  real_T Out2;                         /* '<Root>/Out2' */
} ExternalOutputs_Timer;

/* Real-time Model Data Structure */
struct RT_MODEL_Timer {
  const char_T * volatile errorStatus;

  /*
   * Timing:
   * The following substructure contains information regarding
   * the timing information for the model.
   */
  struct {
    uint32_T clockTick0;
    uint32_T clockTickH0;
  } Timing;
};

/* Block signals (auto storage) */
extern BlockIO_Timer Timer_B;

/* Block states (auto storage) */
extern D_Work_Timer Timer_DWork;

/* External inputs (root inport signals with auto storage) */
extern ExternalInputs_Timer Timer_U;

/* External outputs (root outports fed by signals with auto storage) */
extern ExternalOutputs_Timer Timer_Y;

/* Model entry point functions */
extern void Timer_initialize(void);
extern void Timer_step(void);
extern void Timer_terminate(void);

/* Real-time Model object */
extern struct RT_MODEL_Timer *Timer_M;

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
 * '<Root>' : Timer
 * '<S1>'   : Timer/Chart
 */
#endif                                 /* RTW_HEADER_Timer_h_ */

/*
 * File trailer for Real-Time Workshop generated code.
 *
 * [EOF]
 */
