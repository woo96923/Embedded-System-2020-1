/*
 * File: CAN_NODE_2_AUTO.c
 *
 * Real-Time Workshop code generated for Simulink model CAN_NODE_2_AUTO.
 *
 * Model version                        : 1.27
 * Real-Time Workshop file version      : 7.5  (R2010a)  25-Jan-2010
 * Real-Time Workshop file generated on : Mon Dec 14 21:43:37 2020
 * TLC version                          : 7.5 (Jan 19 2010)
 * C/C++ source code generated on       : Mon Dec 14 21:43:38 2020
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Atmel->AVR
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "CAN_NODE_2_AUTO.h"
#include "CAN_NODE_2_AUTO_private.h"

/* Named constants for Stateflow: '<Root>/Chart' */
#define CAN_NODE_2_AUTO_IN_ACCR        (1U)
#define CAN_NODE_2_AUTO_IN_Body_L_ACCR (1U)
#define CAN_NODE_2_AUTO_IN_Body_L_SLOW (1U)
#define CAN_NODE_2_AUTO_IN_Body_R_ACCR (2U)
#define CAN_NODE_2_AUTO_IN_Body_R_SLOW (2U)
#define CAN_NODE_2_AUTO_IN_Moter       (1U)
#define CAN_NODE_2_AUTO_IN_SLOW        (2U)
#define CAN_NODE_2_AUTO_IN_wait        (2U)
#define CAN_NODE_2_A_IN_NO_ACTIVE_CHILD (0UL)

/* Block signals (auto storage) */
BlockIO_CAN_NODE_2_AUTO CAN_NODE_2_AUTO_B;

/* Block states (auto storage) */
D_Work_CAN_NODE_2_AUTO CAN_NODE_2_AUTO_DWork;

/* External inputs (root inport signals with auto storage) */
ExternalInputs_CAN_NODE_2_AUTO CAN_NODE_2_AUTO_U;

/* External outputs (root outports fed by signals with auto storage) */
ExternalOutputs_CAN_NODE_2_AUTO CAN_NODE_2_AUTO_Y;

/* Real-time model */
RT_MODEL_CAN_NODE_2_AUTO CAN_NODE_2_AUTO_M_;
RT_MODEL_CAN_NODE_2_AUTO *CAN_NODE_2_AUTO_M = &CAN_NODE_2_AUTO_M_;

/* Model step function */
void CAN_NODE_2_AUTO_step(void)
{
  /* Stateflow: '<Root>/Chart' incorporates:
   *  Inport: '<Root>/gyro_x'
   *  Inport: '<Root>/msg1_data_0_'
   *  Inport: '<Root>/speed'
   */
  /* Gateway: Chart */
  /* During: Chart */
  if (CAN_NODE_2_AUTO_DWork.is_active_c1_CAN_NODE_2_AUTO == 0) {
    /* Entry: Chart */
    CAN_NODE_2_AUTO_DWork.is_active_c1_CAN_NODE_2_AUTO = 1U;

    /* Transition: '<S1>:2' */
    /* Entry 'wait': '<S1>:1' */
    CAN_NODE_2_AUTO_DWork.is_c1_CAN_NODE_2_AUTO = CAN_NODE_2_AUTO_IN_wait;
    CAN_NODE_2_AUTO_B.msg3_data_0_ = 0.0;
    CAN_NODE_2_AUTO_B.ocr3a = 100.0;
    CAN_NODE_2_AUTO_B.ocr3b = 100.0;
    CAN_NODE_2_AUTO_B.porte = (real_T)0xfc;
  } else {
    switch (CAN_NODE_2_AUTO_DWork.is_c1_CAN_NODE_2_AUTO) {
     case CAN_NODE_2_AUTO_IN_Moter:
      /* During 'Moter': '<S1>:65' */
      if (CAN_NODE_2_AUTO_U.msg1_data_0_ == 0.0) {
        /* Transition: '<S1>:88' */
        /* Exit 'Body_L_ACCR': '<S1>:9' */
        CAN_NODE_2_AUTO_DWork.is_ACCR = (uint8_T)CAN_NODE_2_A_IN_NO_ACTIVE_CHILD;

        /* Exit 'Body_R_ACCR': '<S1>:8' */
        /* Exit 'ACCR': '<S1>:5' */
        CAN_NODE_2_AUTO_DWork.is_Moter = (uint8_T)
          CAN_NODE_2_A_IN_NO_ACTIVE_CHILD;

        /* Exit 'Body_L_SLOW': '<S1>:11' */
        CAN_NODE_2_AUTO_DWork.is_SLOW = (uint8_T)CAN_NODE_2_A_IN_NO_ACTIVE_CHILD;

        /* Exit 'Body_R_SLOW': '<S1>:10' */
        /* Exit 'SLOW': '<S1>:6' */
        /* Exit 'Moter': '<S1>:65' */
        /* Entry 'wait': '<S1>:1' */
        CAN_NODE_2_AUTO_DWork.is_c1_CAN_NODE_2_AUTO = CAN_NODE_2_AUTO_IN_wait;
        CAN_NODE_2_AUTO_B.msg3_data_0_ = 0.0;
        CAN_NODE_2_AUTO_B.ocr3a = 100.0;
        CAN_NODE_2_AUTO_B.ocr3b = 100.0;
        CAN_NODE_2_AUTO_B.porte = (real_T)0xfc;
      } else {
        switch (CAN_NODE_2_AUTO_DWork.is_Moter) {
         case CAN_NODE_2_AUTO_IN_ACCR:
          /* During 'ACCR': '<S1>:5' */
          if (CAN_NODE_2_AUTO_U.msg1_data_0_ == 2.0) {
            /* Transition: '<S1>:12' */
            /* Exit 'Body_L_ACCR': '<S1>:9' */
            CAN_NODE_2_AUTO_DWork.is_ACCR = (uint8_T)
              CAN_NODE_2_A_IN_NO_ACTIVE_CHILD;

            /* Exit 'Body_R_ACCR': '<S1>:8' */
            /* Exit 'ACCR': '<S1>:5' */
            /* Entry 'SLOW': '<S1>:6' */
            CAN_NODE_2_AUTO_DWork.is_Moter = CAN_NODE_2_AUTO_IN_SLOW;

            /* Transition: '<S1>:70' */
            /* Entry 'Body_L_SLOW': '<S1>:11' */
            CAN_NODE_2_AUTO_DWork.is_SLOW = CAN_NODE_2_AUTO_IN_Body_L_SLOW;
            CAN_NODE_2_AUTO_B.msg3_data_0_ = 2.0;
          } else {
            switch (CAN_NODE_2_AUTO_DWork.is_ACCR) {
             case CAN_NODE_2_AUTO_IN_Body_L_ACCR:
              /* During 'Body_L_ACCR': '<S1>:9' */
              if (CAN_NODE_2_AUTO_U.gyro_x > 30.0) {
                /* Transition: '<S1>:30' */
                /* Exit 'Body_L_ACCR': '<S1>:9' */
                /* Entry 'Body_R_ACCR': '<S1>:8' */
                CAN_NODE_2_AUTO_DWork.is_ACCR = CAN_NODE_2_AUTO_IN_Body_R_ACCR;
                CAN_NODE_2_AUTO_B.msg3_data_0_ = 1.0;
              } else {
                CAN_NODE_2_AUTO_B.ocr3a = CAN_NODE_2_AUTO_U.speed * 1.6E+007 /
                  2.0 / 64.0 / 1000.0 * 25.0 / 5.0;
                CAN_NODE_2_AUTO_B.porte = (real_T)0xfe;
              }
              break;

             case CAN_NODE_2_AUTO_IN_Body_R_ACCR:
              /* During 'Body_R_ACCR': '<S1>:8' */
              if (CAN_NODE_2_AUTO_U.gyro_x < -30.0) {
                /* Transition: '<S1>:31' */
                /* Exit 'Body_R_ACCR': '<S1>:8' */
                /* Entry 'Body_L_ACCR': '<S1>:9' */
                CAN_NODE_2_AUTO_DWork.is_ACCR = CAN_NODE_2_AUTO_IN_Body_L_ACCR;
                CAN_NODE_2_AUTO_B.msg3_data_0_ = 1.0;
              } else {
                CAN_NODE_2_AUTO_B.ocr3a = CAN_NODE_2_AUTO_U.speed * 1.6E+007 /
                  2.0 / 64.0 / 1000.0 * 25.0 / 5.0;
                CAN_NODE_2_AUTO_B.porte = (real_T)0xfd;
              }
              break;

             default:
              /* Transition: '<S1>:68' */
              /* Entry 'Body_R_ACCR': '<S1>:8' */
              CAN_NODE_2_AUTO_DWork.is_ACCR = CAN_NODE_2_AUTO_IN_Body_R_ACCR;
              CAN_NODE_2_AUTO_B.msg3_data_0_ = 1.0;
              break;
            }
          }
          break;

         case CAN_NODE_2_AUTO_IN_SLOW:
          /* During 'SLOW': '<S1>:6' */
          if (CAN_NODE_2_AUTO_U.msg1_data_0_ == 1.0) {
            /* Transition: '<S1>:14' */
            /* Exit 'Body_L_SLOW': '<S1>:11' */
            CAN_NODE_2_AUTO_DWork.is_SLOW = (uint8_T)
              CAN_NODE_2_A_IN_NO_ACTIVE_CHILD;

            /* Exit 'Body_R_SLOW': '<S1>:10' */
            /* Exit 'SLOW': '<S1>:6' */
            /* Entry 'ACCR': '<S1>:5' */
            CAN_NODE_2_AUTO_DWork.is_Moter = CAN_NODE_2_AUTO_IN_ACCR;

            /* Transition: '<S1>:68' */
            /* Entry 'Body_R_ACCR': '<S1>:8' */
            CAN_NODE_2_AUTO_DWork.is_ACCR = CAN_NODE_2_AUTO_IN_Body_R_ACCR;
            CAN_NODE_2_AUTO_B.msg3_data_0_ = 1.0;
          } else {
            switch (CAN_NODE_2_AUTO_DWork.is_SLOW) {
             case CAN_NODE_2_AUTO_IN_Body_L_SLOW:
              /* During 'Body_L_SLOW': '<S1>:11' */
              if (CAN_NODE_2_AUTO_U.gyro_x > 30.0) {
                /* Transition: '<S1>:39' */
                /* Exit 'Body_L_SLOW': '<S1>:11' */
                /* Entry 'Body_R_SLOW': '<S1>:10' */
                CAN_NODE_2_AUTO_DWork.is_SLOW = CAN_NODE_2_AUTO_IN_Body_R_SLOW;
                CAN_NODE_2_AUTO_B.msg3_data_0_ = 2.0;
              } else {
                CAN_NODE_2_AUTO_B.ocr3a = CAN_NODE_2_AUTO_U.speed * 1.6E+007 /
                  2.0 / 64.0 / 1000.0 * 25.0 / 5.0;
                CAN_NODE_2_AUTO_B.porte = (real_T)0xfd;
              }
              break;

             case CAN_NODE_2_AUTO_IN_Body_R_SLOW:
              /* During 'Body_R_SLOW': '<S1>:10' */
              if (CAN_NODE_2_AUTO_U.gyro_x < -30.0) {
                /* Transition: '<S1>:40' */
                /* Exit 'Body_R_SLOW': '<S1>:10' */
                /* Entry 'Body_L_SLOW': '<S1>:11' */
                CAN_NODE_2_AUTO_DWork.is_SLOW = CAN_NODE_2_AUTO_IN_Body_L_SLOW;
                CAN_NODE_2_AUTO_B.msg3_data_0_ = 2.0;
              } else {
                CAN_NODE_2_AUTO_B.ocr3a = CAN_NODE_2_AUTO_U.speed * 1.6E+007 /
                  2.0 / 64.0 / 1000.0 * 25.0 / 5.0;
                CAN_NODE_2_AUTO_B.porte = (real_T)0xfe;
              }
              break;

             default:
              /* Transition: '<S1>:70' */
              /* Entry 'Body_L_SLOW': '<S1>:11' */
              CAN_NODE_2_AUTO_DWork.is_SLOW = CAN_NODE_2_AUTO_IN_Body_L_SLOW;
              CAN_NODE_2_AUTO_B.msg3_data_0_ = 2.0;
              break;
            }
          }
          break;

         default:
          /* Transition: '<S1>:67' */
          /* Entry 'ACCR': '<S1>:5' */
          CAN_NODE_2_AUTO_DWork.is_Moter = CAN_NODE_2_AUTO_IN_ACCR;

          /* Transition: '<S1>:68' */
          /* Entry 'Body_R_ACCR': '<S1>:8' */
          CAN_NODE_2_AUTO_DWork.is_ACCR = CAN_NODE_2_AUTO_IN_Body_R_ACCR;
          CAN_NODE_2_AUTO_B.msg3_data_0_ = 1.0;
          break;
        }
      }
      break;

     case CAN_NODE_2_AUTO_IN_wait:
      /* During 'wait': '<S1>:1' */
      if (CAN_NODE_2_AUTO_U.msg1_data_0_ == 1.0) {
        /* Transition: '<S1>:7' */
        /* Exit 'wait': '<S1>:1' */
        /* Entry 'Moter': '<S1>:65' */
        CAN_NODE_2_AUTO_DWork.is_c1_CAN_NODE_2_AUTO = CAN_NODE_2_AUTO_IN_Moter;

        /* Transition: '<S1>:67' */
        /* Entry 'ACCR': '<S1>:5' */
        CAN_NODE_2_AUTO_DWork.is_Moter = CAN_NODE_2_AUTO_IN_ACCR;

        /* Transition: '<S1>:68' */
        /* Entry 'Body_R_ACCR': '<S1>:8' */
        CAN_NODE_2_AUTO_DWork.is_ACCR = CAN_NODE_2_AUTO_IN_Body_R_ACCR;
        CAN_NODE_2_AUTO_B.msg3_data_0_ = 1.0;
      }
      break;

     default:
      /* Transition: '<S1>:2' */
      /* Entry 'wait': '<S1>:1' */
      CAN_NODE_2_AUTO_DWork.is_c1_CAN_NODE_2_AUTO = CAN_NODE_2_AUTO_IN_wait;
      CAN_NODE_2_AUTO_B.msg3_data_0_ = 0.0;
      CAN_NODE_2_AUTO_B.ocr3a = 100.0;
      CAN_NODE_2_AUTO_B.ocr3b = 100.0;
      CAN_NODE_2_AUTO_B.porte = (real_T)0xfc;
      break;
    }
  }

  /* Outport: '<Root>/msg3_data_0_' */
  CAN_NODE_2_AUTO_Y.msg3_data_0_ = CAN_NODE_2_AUTO_B.msg3_data_0_;

  /* Outport: '<Root>/ocr3a' */
  CAN_NODE_2_AUTO_Y.ocr3a = CAN_NODE_2_AUTO_B.ocr3a;

  /* Outport: '<Root>/ocr3b' */
  CAN_NODE_2_AUTO_Y.ocr3b = CAN_NODE_2_AUTO_B.ocr3b;

  /* Outport: '<Root>/porte' */
  CAN_NODE_2_AUTO_Y.porte = CAN_NODE_2_AUTO_B.porte;
}

/* Model initialize function */
void CAN_NODE_2_AUTO_initialize(void)
{
  /* Registration code */

  /* initialize error status */
  rtmSetErrorStatus(CAN_NODE_2_AUTO_M, (NULL));

  /* block I/O */
  (void) memset(((void *) &CAN_NODE_2_AUTO_B), 0,
                sizeof(BlockIO_CAN_NODE_2_AUTO));

  /* states (dwork) */
  (void) memset((void *)&CAN_NODE_2_AUTO_DWork, 0,
                sizeof(D_Work_CAN_NODE_2_AUTO));

  /* external inputs */
  (void) memset((void *)&CAN_NODE_2_AUTO_U, 0,
                sizeof(ExternalInputs_CAN_NODE_2_AUTO));

  /* external outputs */
  (void) memset((void *)&CAN_NODE_2_AUTO_Y, 0,
                sizeof(ExternalOutputs_CAN_NODE_2_AUTO));

  /* InitializeConditions for Stateflow: '<Root>/Chart' */
  CAN_NODE_2_AUTO_DWork.is_Moter = 0U;
  CAN_NODE_2_AUTO_DWork.is_ACCR = 0U;
  CAN_NODE_2_AUTO_DWork.is_SLOW = 0U;
  CAN_NODE_2_AUTO_DWork.is_active_c1_CAN_NODE_2_AUTO = 0U;
  CAN_NODE_2_AUTO_DWork.is_c1_CAN_NODE_2_AUTO = 0U;
  CAN_NODE_2_AUTO_B.msg3_data_0_ = 0.0;
  CAN_NODE_2_AUTO_B.ocr3a = 0.0;
  CAN_NODE_2_AUTO_B.ocr3b = 0.0;
  CAN_NODE_2_AUTO_B.porte = 0.0;
}

/* Model terminate function */
void CAN_NODE_2_AUTO_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for Real-Time Workshop generated code.
 *
 * [EOF]
 */
