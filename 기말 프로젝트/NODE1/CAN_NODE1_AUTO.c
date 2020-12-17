/*
 * File: CAN_NODE1_AUTO.c
 *
 * Real-Time Workshop code generated for Simulink model CAN_NODE1_AUTO.
 *
 * Model version                        : 1.19
 * Real-Time Workshop file version      : 7.5  (R2010a)  25-Jan-2010
 * Real-Time Workshop file generated on : Tue Dec 15 16:42:23 2020
 * TLC version                          : 7.5 (Jan 19 2010)
 * C/C++ source code generated on       : Tue Dec 15 16:42:23 2020
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Atmel->AVR
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "CAN_NODE1_AUTO.h"
#include "CAN_NODE1_AUTO_private.h"

/* Named constants for Stateflow: '<Root>/Chart' */
#define CAN_NODE1_AUTO_IN_acc          (1U)
#define CAN_NODE1_AUTO_IN_none         (2U)
#define CAN_NODE1_AUTO_IN_slow         (2U)
#define CAN_NODE1_AUTO_IN_wait         (1U)
#define CAN_NODE1_AUTO_IN_working      (2U)
#define CAN_NODE1_AU_IN_NO_ACTIVE_CHILD (0UL)
#define CAN_NODE1_AU_IN_distance_checkF (1U)

/* Block signals (auto storage) */
BlockIO_CAN_NODE1_AUTO CAN_NODE1_AUTO_B;

/* Block states (auto storage) */
D_Work_CAN_NODE1_AUTO CAN_NODE1_AUTO_DWork;

/* External inputs (root inport signals with auto storage) */
ExternalInputs_CAN_NODE1_AUTO CAN_NODE1_AUTO_U;

/* External outputs (root outports fed by signals with auto storage) */
ExternalOutputs_CAN_NODE1_AUTO CAN_NODE1_AUTO_Y;

/* Real-time model */
RT_MODEL_CAN_NODE1_AUTO CAN_NODE1_AUTO_M_;
RT_MODEL_CAN_NODE1_AUTO *CAN_NODE1_AUTO_M = &CAN_NODE1_AUTO_M_;

/* Model step function */
void CAN_NODE1_AUTO_step(void)
{
  /* Stateflow: '<Root>/Chart' incorporates:
   *  Inport: '<Root>/ADdata'
   *  Inport: '<Root>/button_input_acc'
   *  Inport: '<Root>/button_input_slow'
   *  Inport: '<Root>/distande'
   */
  /* Gateway: Chart */
  /* During: Chart */
  if (CAN_NODE1_AUTO_DWork.is_active_c1_CAN_NODE1_AUTO == 0) {
    /* Entry: Chart */
    CAN_NODE1_AUTO_DWork.is_active_c1_CAN_NODE1_AUTO = 1U;

    /* Transition: '<S1>:2' */
    /* Entry 'wait': '<S1>:1' */
    CAN_NODE1_AUTO_DWork.is_c1_CAN_NODE1_AUTO = CAN_NODE1_AUTO_IN_wait;
    CAN_NODE1_AUTO_B.current_mode = 0.0;
    CAN_NODE1_AUTO_B.distance_check = 0.0;
    CAN_NODE1_AUTO_B.portb = (real_T)0xff;
  } else {
    switch (CAN_NODE1_AUTO_DWork.is_c1_CAN_NODE1_AUTO) {
     case CAN_NODE1_AUTO_IN_wait:
      /* During 'wait': '<S1>:1' */
      if (CAN_NODE1_AUTO_U.button_input_acc == 1.0) {
        /* Transition: '<S1>:29' */
        /* Exit 'wait': '<S1>:1' */
        /* Entry 'working': '<S1>:23' */
        CAN_NODE1_AUTO_DWork.is_c1_CAN_NODE1_AUTO = CAN_NODE1_AUTO_IN_working;

        /* Transition: '<S1>:51' */
        /* Entry 'moter': '<S1>:61' */
        CAN_NODE1_AUTO_DWork.is_active_moter = 1U;

        /* Entry 'acc': '<S1>:50' */
        CAN_NODE1_AUTO_DWork.is_moter = CAN_NODE1_AUTO_IN_acc;
        CAN_NODE1_AUTO_B.current_mode = 1.0;

        /* Entry 'distance_check': '<S1>:67' */
        CAN_NODE1_AUTO_DWork.is_active_distance_check = 1U;

        /* Transition: '<S1>:72' */
        /* Entry 'none': '<S1>:66' */
        CAN_NODE1_AUTO_DWork.is_distance_check = CAN_NODE1_AUTO_IN_none;
      } else {
        CAN_NODE1_AUTO_B.voltage = CAN_NODE1_AUTO_U.ADdata * 100.0 / 1023.0;
        CAN_NODE1_AUTO_B.msg1_data_0_ = CAN_NODE1_AUTO_B.current_mode;
        CAN_NODE1_AUTO_B.msg2_data_0_ = CAN_NODE1_AUTO_B.current_mode;
        CAN_NODE1_AUTO_B.msg2_data_4_ = CAN_NODE1_AUTO_B.voltage;
      }
      break;

     case CAN_NODE1_AUTO_IN_working:
      /* During 'working': '<S1>:23' */
      if (CAN_NODE1_AUTO_B.distance_check > 100.0) {
        /* Transition: '<S1>:36' */
        /* Exit 'distance_checkF': '<S1>:65' */
        CAN_NODE1_AUTO_DWork.is_distance_check = (uint8_T)
          CAN_NODE1_AU_IN_NO_ACTIVE_CHILD;

        /* Exit 'none': '<S1>:66' */
        /* Exit 'distance_check': '<S1>:67' */
        CAN_NODE1_AUTO_DWork.is_active_distance_check = 0U;

        /* Exit 'acc': '<S1>:50' */
        CAN_NODE1_AUTO_DWork.is_moter = (uint8_T)CAN_NODE1_AU_IN_NO_ACTIVE_CHILD;

        /* Exit 'slow': '<S1>:53' */
        /* Exit 'moter': '<S1>:61' */
        CAN_NODE1_AUTO_DWork.is_active_moter = 0U;

        /* Exit 'working': '<S1>:23' */
        /* Entry 'wait': '<S1>:1' */
        CAN_NODE1_AUTO_DWork.is_c1_CAN_NODE1_AUTO = CAN_NODE1_AUTO_IN_wait;
        CAN_NODE1_AUTO_B.current_mode = 0.0;
        CAN_NODE1_AUTO_B.distance_check = 0.0;
        CAN_NODE1_AUTO_B.portb = (real_T)0xff;
      } else {
        /* During 'moter': '<S1>:61' */
        switch (CAN_NODE1_AUTO_DWork.is_moter) {
         case CAN_NODE1_AUTO_IN_acc:
          /* During 'acc': '<S1>:50' */
          if (CAN_NODE1_AUTO_U.button_input_slow == 1.0) {
            /* Transition: '<S1>:56' */
            /* Exit 'acc': '<S1>:50' */
            /* Entry 'slow': '<S1>:53' */
            CAN_NODE1_AUTO_DWork.is_moter = CAN_NODE1_AUTO_IN_slow;
            CAN_NODE1_AUTO_B.current_mode = 2.0;
          } else {
            CAN_NODE1_AUTO_B.msg1_data_0_ = CAN_NODE1_AUTO_B.current_mode;
            CAN_NODE1_AUTO_B.msg2_data_0_ = CAN_NODE1_AUTO_B.current_mode;
          }
          break;

         case CAN_NODE1_AUTO_IN_slow:
          /* During 'slow': '<S1>:53' */
          if (CAN_NODE1_AUTO_U.button_input_acc == 1.0) {
            /* Transition: '<S1>:58' */
            /* Exit 'slow': '<S1>:53' */
            /* Entry 'acc': '<S1>:50' */
            CAN_NODE1_AUTO_DWork.is_moter = CAN_NODE1_AUTO_IN_acc;
            CAN_NODE1_AUTO_B.current_mode = 1.0;
          } else {
            CAN_NODE1_AUTO_B.msg1_data_0_ = CAN_NODE1_AUTO_B.current_mode;
            CAN_NODE1_AUTO_B.msg2_data_0_ = CAN_NODE1_AUTO_B.current_mode;
          }
          break;
        }

        /* During 'distance_check': '<S1>:67' */
        switch (CAN_NODE1_AUTO_DWork.is_distance_check) {
         case CAN_NODE1_AU_IN_distance_checkF:
          /* During 'distance_checkF': '<S1>:65' */
          if (CAN_NODE1_AUTO_U.distande > 100.0) {
            /* Transition: '<S1>:71' */
            /* Exit 'distance_checkF': '<S1>:65' */
            /* Entry 'none': '<S1>:66' */
            CAN_NODE1_AUTO_DWork.is_distance_check = CAN_NODE1_AUTO_IN_none;
          } else {
            CAN_NODE1_AUTO_B.distance_check = CAN_NODE1_AUTO_B.distance_check +
              1.0;
          }
          break;

         case CAN_NODE1_AUTO_IN_none:
          /* During 'none': '<S1>:66' */
          if (CAN_NODE1_AUTO_U.distande <= 100.0) {
            /* Transition: '<S1>:70' */
            /* Exit 'none': '<S1>:66' */
            /* Entry 'distance_checkF': '<S1>:65' */
            CAN_NODE1_AUTO_DWork.is_distance_check =
              CAN_NODE1_AU_IN_distance_checkF;
            CAN_NODE1_AUTO_B.distance_check = 0.0;
          }
          break;

         default:
          /* Transition: '<S1>:72' */
          /* Entry 'none': '<S1>:66' */
          CAN_NODE1_AUTO_DWork.is_distance_check = CAN_NODE1_AUTO_IN_none;
          break;
        }
      }
      break;

     default:
      /* Transition: '<S1>:2' */
      /* Entry 'wait': '<S1>:1' */
      CAN_NODE1_AUTO_DWork.is_c1_CAN_NODE1_AUTO = CAN_NODE1_AUTO_IN_wait;
      CAN_NODE1_AUTO_B.current_mode = 0.0;
      CAN_NODE1_AUTO_B.distance_check = 0.0;
      CAN_NODE1_AUTO_B.portb = (real_T)0xff;
      break;
    }
  }

  /* Outport: '<Root>/current_mode' */
  CAN_NODE1_AUTO_Y.current_mode = CAN_NODE1_AUTO_B.current_mode;

  /* Outport: '<Root>/voltage' */
  CAN_NODE1_AUTO_Y.voltage = CAN_NODE1_AUTO_B.voltage;

  /* Outport: '<Root>/msg2_0' */
  CAN_NODE1_AUTO_Y.msg2_0 = CAN_NODE1_AUTO_B.msg2_data_0_;

  /* Outport: '<Root>/msg2_4' */
  CAN_NODE1_AUTO_Y.msg2_4 = CAN_NODE1_AUTO_B.msg2_data_4_;

  /* Outport: '<Root>/msg1_0' */
  CAN_NODE1_AUTO_Y.msg1_0 = CAN_NODE1_AUTO_B.msg1_data_0_;

  /* Outport: '<Root>/portb' */
  CAN_NODE1_AUTO_Y.portb = CAN_NODE1_AUTO_B.portb;

  /* Outport: '<Root>/distance_check' */
  CAN_NODE1_AUTO_Y.distance_check = CAN_NODE1_AUTO_B.distance_check;
}

/* Model initialize function */
void CAN_NODE1_AUTO_initialize(void)
{
  /* Registration code */

  /* initialize error status */
  rtmSetErrorStatus(CAN_NODE1_AUTO_M, (NULL));

  /* block I/O */
  (void) memset(((void *) &CAN_NODE1_AUTO_B), 0,
                sizeof(BlockIO_CAN_NODE1_AUTO));

  /* states (dwork) */
  (void) memset((void *)&CAN_NODE1_AUTO_DWork, 0,
                sizeof(D_Work_CAN_NODE1_AUTO));

  /* external inputs */
  (void) memset((void *)&CAN_NODE1_AUTO_U, 0,
                sizeof(ExternalInputs_CAN_NODE1_AUTO));

  /* external outputs */
  (void) memset((void *)&CAN_NODE1_AUTO_Y, 0,
                sizeof(ExternalOutputs_CAN_NODE1_AUTO));

  /* InitializeConditions for Stateflow: '<Root>/Chart' */
  CAN_NODE1_AUTO_DWork.is_active_distance_check = 0U;
  CAN_NODE1_AUTO_DWork.is_distance_check = 0U;
  CAN_NODE1_AUTO_DWork.is_active_moter = 0U;
  CAN_NODE1_AUTO_DWork.is_moter = 0U;
  CAN_NODE1_AUTO_DWork.is_active_c1_CAN_NODE1_AUTO = 0U;
  CAN_NODE1_AUTO_DWork.is_c1_CAN_NODE1_AUTO = 0U;
  CAN_NODE1_AUTO_B.current_mode = 0.0;
  CAN_NODE1_AUTO_B.voltage = 0.0;
  CAN_NODE1_AUTO_B.msg2_data_0_ = 0.0;
  CAN_NODE1_AUTO_B.msg2_data_4_ = 0.0;
  CAN_NODE1_AUTO_B.msg1_data_0_ = 0.0;
  CAN_NODE1_AUTO_B.portb = 0.0;
  CAN_NODE1_AUTO_B.distance_check = 0.0;
}

/* Model terminate function */
void CAN_NODE1_AUTO_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for Real-Time Workshop generated code.
 *
 * [EOF]
 */
