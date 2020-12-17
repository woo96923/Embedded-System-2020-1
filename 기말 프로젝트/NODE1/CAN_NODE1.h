/*
 * File: CAN_NODE1.h
 *
 * Real-Time Workshop code generated for Simulink model CAN_NODE1.
 *
 * Model version                        : 1.6
 * Real-Time Workshop file version      : 7.5  (R2010a)  25-Jan-2010
 * Real-Time Workshop file generated on : Tue Dec 01 14:57:49 2020
 * TLC version                          : 7.5 (Jan 19 2010)
 * C/C++ source code generated on       : Tue Dec 01 14:57:50 2020
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Atmel->AVR
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_CAN_NODE1_h_
#define RTW_HEADER_CAN_NODE1_h_
#ifndef CAN_NODE1_COMMON_INCLUDES_
# define CAN_NODE1_COMMON_INCLUDES_
#include <stddef.h>
#include <string.h>
#include "rtwtypes.h"
#endif                                 /* CAN_NODE1_COMMON_INCLUDES_ */

#include "CAN_NODE1_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

/* Block signals (auto storage) */
typedef struct {
  real_T current_mode;                 /* '<Root>/Chart' */
  real_T current_speed;                /* '<Root>/Chart' */
  real_T voltage;                      /* '<Root>/Chart' */
  real_T msg2_data_0_;                 /* '<Root>/Chart' */
  real_T msg2_data_2_;                 /* '<Root>/Chart' */
  real_T msg2_data_4_;                 /* '<Root>/Chart' */
  real_T msg1_data_0_;                 /* '<Root>/Chart' */
  real_T msg1_data_2_;                 /* '<Root>/Chart' */
  real_T portb;                        /* '<Root>/Chart' */
} BlockIO_CAN_NODE1;

/* Block states (auto storage) for system '<Root>' */
typedef struct {
  uint8_T is_active_c1_CAN_NODE1;      /* '<Root>/Chart' */
  uint8_T is_c1_CAN_NODE1;             /* '<Root>/Chart' */
  uint8_T is_working;                  /* '<Root>/Chart' */
} D_Work_CAN_NODE1;

/* External inputs (root inport signals with auto storage) */
typedef struct {
  real_T button_input_acc;             /* '<Root>/button_input_acc' */
  real_T button_input_slow;            /* '<Root>/button_input_slow' */
  real_T waring;                       /* '<Root>/waring' */
  real_T mode_input;                   /* '<Root>/mode_input' */
  real_T ADdata;                       /* '<Root>/ADdata' */
} ExternalInputs_CAN_NODE1;

/* External outputs (root outports fed by signals with auto storage) */
typedef struct {
  real_T current_mode;                 /* '<Root>/current_mode' */
  real_T current_speed;                /* '<Root>/current_speed' */
  real_T voltage;                      /* '<Root>/voltage' */
  real_T msg2_0;                       /* '<Root>/msg2_0' */
  real_T msg2_2;                       /* '<Root>/msg2_2' */
  real_T msg2_4;                       /* '<Root>/msg2_4' */
  real_T msg1_0;                       /* '<Root>/msg1_0' */
  real_T msg1_2;                       /* '<Root>/msg1_2' */
  real_T portb;                        /* '<Root>/portb' */
} ExternalOutputs_CAN_NODE1;

/* Real-time Model Data Structure */
struct RT_MODEL_CAN_NODE1 {
  const char_T * volatile errorStatus;
};

/* Block signals (auto storage) */
extern BlockIO_CAN_NODE1 CAN_NODE1_B;

/* Block states (auto storage) */
extern D_Work_CAN_NODE1 CAN_NODE1_DWork;

/* External inputs (root inport signals with auto storage) */
extern ExternalInputs_CAN_NODE1 CAN_NODE1_U;

/* External outputs (root outports fed by signals with auto storage) */
extern ExternalOutputs_CAN_NODE1 CAN_NODE1_Y;

/* Model entry point functions */
extern void CAN_NODE1_initialize(void);
extern void CAN_NODE1_step(void);
extern void CAN_NODE1_terminate(void);

/* Real-time Model object */
extern struct RT_MODEL_CAN_NODE1 *CAN_NODE1_M;

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
 * '<Root>' : CAN_NODE1
 * '<S1>'   : CAN_NODE1/Chart
 */
#endif                                 /* RTW_HEADER_CAN_NODE1_h_ */

/*
 * File trailer for Real-Time Workshop generated code.
 *
 * [EOF]
 */
