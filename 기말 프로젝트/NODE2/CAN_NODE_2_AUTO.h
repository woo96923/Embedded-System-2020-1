/*
 * File: CAN_NODE_2_AUTO.h
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

#ifndef RTW_HEADER_CAN_NODE_2_AUTO_h_
#define RTW_HEADER_CAN_NODE_2_AUTO_h_
#ifndef CAN_NODE_2_AUTO_COMMON_INCLUDES_
# define CAN_NODE_2_AUTO_COMMON_INCLUDES_
#include <stddef.h>
#include <string.h>
#include "rtwtypes.h"
#endif                                 /* CAN_NODE_2_AUTO_COMMON_INCLUDES_ */

#include "CAN_NODE_2_AUTO_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

/* Block signals (auto storage) */
typedef struct {
  real_T msg3_data_0_;                 /* '<Root>/Chart' */
  real_T ocr3a;                        /* '<Root>/Chart' */
  real_T ocr3b;                        /* '<Root>/Chart' */
  real_T porte;                        /* '<Root>/Chart' */
} BlockIO_CAN_NODE_2_AUTO;

/* Block states (auto storage) for system '<Root>' */
typedef struct {
  uint8_T is_active_c1_CAN_NODE_2_AUTO;/* '<Root>/Chart' */
  uint8_T is_c1_CAN_NODE_2_AUTO;       /* '<Root>/Chart' */
  uint8_T is_ACCR;                     /* '<Root>/Chart' */
  uint8_T is_SLOW;                     /* '<Root>/Chart' */
  uint8_T is_Moter;                    /* '<Root>/Chart' */
} D_Work_CAN_NODE_2_AUTO;

/* External inputs (root inport signals with auto storage) */
typedef struct {
  real_T msg1_data_0_;                 /* '<Root>/msg1_data_0_' */
  real_T speed;                        /* '<Root>/speed' */
  real_T gyro_x;                       /* '<Root>/gyro_x' */
} ExternalInputs_CAN_NODE_2_AUTO;

/* External outputs (root outports fed by signals with auto storage) */
typedef struct {
  real_T msg3_data_0_;                 /* '<Root>/msg3_data_0_' */
  real_T ocr3a;                        /* '<Root>/ocr3a' */
  real_T ocr3b;                        /* '<Root>/ocr3b' */
  real_T porte;                        /* '<Root>/porte' */
} ExternalOutputs_CAN_NODE_2_AUTO;

/* Real-time Model Data Structure */
struct RT_MODEL_CAN_NODE_2_AUTO {
  const char_T * volatile errorStatus;
};

/* Block signals (auto storage) */
extern BlockIO_CAN_NODE_2_AUTO CAN_NODE_2_AUTO_B;

/* Block states (auto storage) */
extern D_Work_CAN_NODE_2_AUTO CAN_NODE_2_AUTO_DWork;

/* External inputs (root inport signals with auto storage) */
extern ExternalInputs_CAN_NODE_2_AUTO CAN_NODE_2_AUTO_U;

/* External outputs (root outports fed by signals with auto storage) */
extern ExternalOutputs_CAN_NODE_2_AUTO CAN_NODE_2_AUTO_Y;

/* Model entry point functions */
extern void CAN_NODE_2_AUTO_initialize(void);
extern void CAN_NODE_2_AUTO_step(void);
extern void CAN_NODE_2_AUTO_terminate(void);

/* Real-time Model object */
extern struct RT_MODEL_CAN_NODE_2_AUTO *CAN_NODE_2_AUTO_M;

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
 * '<Root>' : CAN_NODE_2_AUTO
 * '<S1>'   : CAN_NODE_2_AUTO/Chart
 */
#endif                                 /* RTW_HEADER_CAN_NODE_2_AUTO_h_ */

/*
 * File trailer for Real-Time Workshop generated code.
 *
 * [EOF]
 */
