/*
 * File: Timer.c
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

#include "Timer.h"
#include "Timer_private.h"

/* Named constants for Stateflow: '<Root>/Chart' */
#define Timer_IN_Green                 (1U)
#define Timer_IN_NO_ACTIVE_CHILD       (0UL)
#define Timer_IN_Red                   (2U)
#define Timer_IN_SW_on                 (1U)
#define Timer_IN_Yellow                (3U)
#define Timer_IN_stepOne               (2U)

/* Block signals (auto storage) */
BlockIO_Timer Timer_B;

/* Block states (auto storage) */
D_Work_Timer Timer_DWork;

/* External inputs (root inport signals with auto storage) */
ExternalInputs_Timer Timer_U;

/* External outputs (root outports fed by signals with auto storage) */
ExternalOutputs_Timer Timer_Y;

/* Real-time model */
RT_MODEL_Timer Timer_M_;
RT_MODEL_Timer *Timer_M = &Timer_M_;

/* Forward declaration for local functions */
static void Timer_enter_internal_stepOne(void);

/* Function for Stateflow: '<Root>/Chart' */
static void Timer_enter_internal_stepOne(void)
{
  switch (Timer_DWork.was_stepOne) {
   case Timer_IN_Green:
    /* Entry 'Green': '<S1>:5' */
    Timer_DWork.is_stepOne = Timer_IN_Green;
    Timer_DWork.was_stepOne = Timer_IN_Green;
    Timer_DWork.temporalCounter_i1 = 0U;
    Timer_B.mode = 1.0;
    Timer_B.porte = (real_T)0x7f;
    break;

   case Timer_IN_Red:
    /* Entry 'Red': '<S1>:7' */
    Timer_DWork.is_stepOne = Timer_IN_Red;
    Timer_DWork.was_stepOne = Timer_IN_Red;
    Timer_DWork.temporalCounter_i1 = 0U;
    Timer_B.mode = 3.0;
    Timer_B.porte = (real_T)0xdf;
    break;

   case Timer_IN_Yellow:
    /* Entry 'Yellow': '<S1>:6' */
    Timer_DWork.is_stepOne = Timer_IN_Yellow;
    Timer_DWork.was_stepOne = Timer_IN_Yellow;
    Timer_DWork.temporalCounter_i1 = 0U;
    Timer_B.mode = 2.0;
    Timer_B.porte = (real_T)0xbf;
    break;

   default:
    /* Transition: '<S1>:21' */
    /* Entry 'Green': '<S1>:5' */
    Timer_DWork.is_stepOne = Timer_IN_Green;
    Timer_DWork.was_stepOne = Timer_IN_Green;
    Timer_DWork.temporalCounter_i1 = 0U;
    Timer_B.mode = 1.0;
    Timer_B.porte = (real_T)0x7f;
    break;
  }
}

/* Model step function */
void Timer_step(void)
{
  /* Stateflow: '<Root>/Chart' incorporates:
   *  Inport: '<Root>/In1'
   */
  Timer_DWork.presentTicks = Timer_M->Timing.clockTick0;
  Timer_DWork.elapsedTicks = Timer_DWork.presentTicks -
    Timer_DWork.previousTicks;
  Timer_DWork.previousTicks = Timer_DWork.presentTicks;
  if ((uint32_T)Timer_DWork.temporalCounter_i1 + Timer_DWork.elapsedTicks <=
      1023UL) {
    Timer_DWork.temporalCounter_i1 = (uint16_T)((uint32_T)
      Timer_DWork.temporalCounter_i1 + Timer_DWork.elapsedTicks);
  } else {
    Timer_DWork.temporalCounter_i1 = 1023U;
  }

  /* Gateway: Chart */
  /* During: Chart */
  if (Timer_DWork.is_active_c1_Timer == 0) {
    /* Entry: Chart */
    Timer_DWork.is_active_c1_Timer = 1U;

    /* Transition: '<S1>:32' */
    /* Entry 'stepOne': '<S1>:4' */
    Timer_DWork.is_c1_Timer = Timer_IN_stepOne;
    Timer_enter_internal_stepOne();
  } else {
    switch (Timer_DWork.is_c1_Timer) {
     case Timer_IN_SW_on:
      /* During 'SW_on': '<S1>:28' */
      if (Timer_U.In1 == 0.0) {
        /* Transition: '<S1>:30' */
        /* Exit 'SW_on': '<S1>:28' */
        /* Entry 'stepOne': '<S1>:4' */
        Timer_DWork.is_c1_Timer = Timer_IN_stepOne;
        Timer_enter_internal_stepOne();
      }
      break;

     case Timer_IN_stepOne:
      /* During 'stepOne': '<S1>:4' */
      if (Timer_U.In1 == 1.0) {
        /* Transition: '<S1>:29' */
        /* Exit 'Green': '<S1>:5' */
        Timer_DWork.is_stepOne = (uint8_T)Timer_IN_NO_ACTIVE_CHILD;

        /* Exit 'Red': '<S1>:7' */
        /* Exit 'Yellow': '<S1>:6' */
        /* Exit 'stepOne': '<S1>:4' */
        /* Entry 'SW_on': '<S1>:28' */
        Timer_DWork.is_c1_Timer = Timer_IN_SW_on;
        Timer_B.mode = 4.0;
        Timer_B.porte = (real_T)0xff;
      } else {
        switch (Timer_DWork.is_stepOne) {
         case Timer_IN_Green:
          /* During 'Green': '<S1>:5' */
          if (Timer_DWork.temporalCounter_i1 >= 1000U) {
            /* Transition: '<S1>:10' */
            /* Exit 'Green': '<S1>:5' */
            /* Entry 'Yellow': '<S1>:6' */
            Timer_DWork.is_stepOne = Timer_IN_Yellow;
            Timer_DWork.was_stepOne = Timer_IN_Yellow;
            Timer_DWork.temporalCounter_i1 = 0U;
            Timer_B.mode = 2.0;
            Timer_B.porte = (real_T)0xbf;
          }
          break;

         case Timer_IN_Red:
          /* During 'Red': '<S1>:7' */
          if (Timer_DWork.temporalCounter_i1 >= 1000U) {
            /* Transition: '<S1>:33' */
            /* Exit 'Red': '<S1>:7' */
            /* Entry 'Green': '<S1>:5' */
            Timer_DWork.is_stepOne = Timer_IN_Green;
            Timer_DWork.was_stepOne = Timer_IN_Green;
            Timer_DWork.temporalCounter_i1 = 0U;
            Timer_B.mode = 1.0;
            Timer_B.porte = (real_T)0x7f;
          }
          break;

         case Timer_IN_Yellow:
          /* During 'Yellow': '<S1>:6' */
          if (Timer_DWork.temporalCounter_i1 >= 1000U) {
            /* Transition: '<S1>:11' */
            /* Exit 'Yellow': '<S1>:6' */
            /* Entry 'Red': '<S1>:7' */
            Timer_DWork.is_stepOne = Timer_IN_Red;
            Timer_DWork.was_stepOne = Timer_IN_Red;
            Timer_DWork.temporalCounter_i1 = 0U;
            Timer_B.mode = 3.0;
            Timer_B.porte = (real_T)0xdf;
          }
          break;

         default:
          Timer_enter_internal_stepOne();
          break;
        }
      }
      break;

     default:
      /* Transition: '<S1>:32' */
      /* Entry 'stepOne': '<S1>:4' */
      Timer_DWork.is_c1_Timer = Timer_IN_stepOne;
      Timer_enter_internal_stepOne();
      break;
    }
  }

  /* Outport: '<Root>/Out1' */
  Timer_Y.Out1 = Timer_B.mode;

  /* Outport: '<Root>/Out2' */
  Timer_Y.Out2 = Timer_B.porte;

  /* Update absolute time for base rate */
  /* The "clockTick0" counts the number of times the code of this task has
   * been executed. The resolution of this integer timer is 1.0, which is the step size
   * of the task. Size of "clockTick0" ensures timer will not overflow during the
   * application lifespan selected.
   * Timer of this task consists of two 32 bit unsigned integers.
   * The two integers represent the low bits Timing.clockTick0 and the high bits
   * Timing.clockTickH0. When the low bit overflows to 0, the high bits increment.
   */
  Timer_M->Timing.clockTick0++;
  if (!Timer_M->Timing.clockTick0) {
    Timer_M->Timing.clockTickH0++;
  }
}

/* Model initialize function */
void Timer_initialize(void)
{
  /* Registration code */

  /* initialize real-time model */
  (void) memset((void *)Timer_M, 0,
                sizeof(RT_MODEL_Timer));

  /* block I/O */
  (void) memset(((void *) &Timer_B), 0,
                sizeof(BlockIO_Timer));

  /* states (dwork) */
  (void) memset((void *)&Timer_DWork, 0,
                sizeof(D_Work_Timer));

  /* external inputs */
  Timer_U.In1 = 0.0;

  /* external outputs */
  (void) memset((void *)&Timer_Y, 0,
                sizeof(ExternalOutputs_Timer));

  /* InitializeConditions for Stateflow: '<Root>/Chart' */
  Timer_DWork.is_stepOne = 0U;
  Timer_DWork.was_stepOne = 0U;
  Timer_DWork.is_active_c1_Timer = 0U;
  Timer_DWork.is_c1_Timer = 0U;
  Timer_B.mode = 0.0;
  Timer_B.porte = 0.0;
  Timer_DWork.presentTicks = 0UL;
  Timer_DWork.elapsedTicks = 0UL;
  Timer_DWork.previousTicks = 0UL;

  /* Enable for Stateflow: '<Root>/Chart' */
  Timer_DWork.presentTicks = Timer_M->Timing.clockTick0;
  Timer_DWork.previousTicks = Timer_DWork.presentTicks;
}

/* Model terminate function */
void Timer_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for Real-Time Workshop generated code.
 *
 * [EOF]
 */
