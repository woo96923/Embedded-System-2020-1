/*
 * File: CAN_NODE1_AUTO.h
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

#ifndef RTW_HEADER_CAN_NODE1_AUTO_h_
#define RTW_HEADER_CAN_NODE1_AUTO_h_
#ifndef CAN_NODE1_AUTO_COMMON_INCLUDES_
# define CAN_NODE1_AUTO_COMMON_INCLUDES_
#include <stddef.h>
#include <string.h>
#include "rtwtypes.h"
#endif                                 /* CAN_NODE1_AUTO_COMMON_INCLUDES_ */

#include "CAN_NODE1_AUTO_types.h"

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
  real_T voltage;                      /* '<Root>/Chart' */
  real_T msg2_data_0_;                 /* '<Root>/Chart' */
  real_T msg2_data_4_;                 /* '<Root>/Chart' */
  real_T msg1_data_0_;                 /* '<Root>/Chart' */
  real_T portb;                        /* '<Root>/Chart' */
  real_T distance_check;               /* '<Root>/Chart' */
} BlockIO_CAN_NODE1_AUTO;

/* Block states (auto storage) for system '<Root>' */
typedef struct {
  uint8_T is_active_c1_CAN_NODE1_AUTO; /* '<Root>/Chart' */
  uint8_T is_c1_CAN_NODE1_AUTO;        /* '<Root>/Chart' */
  uint8_T is_active_distance_check;    /* '<Root>/Chart' */
  uint8_T is_distance_check;           /* '<Root>/Chart' */
  uint8_T is_active_moter;             /* '<Root>/Chart' */
  uint8_T is_moter;                    /* '<Root>/Chart' */
} D_Work_CAN_NODE1_AUTO;

/* External inputs (root inport signals with auto storage) */
typedef struct {
  real_T button_input_acc;             /* '<Root>/button_input_acc' */
  real_T button_input_slow;            /* '<Root>/button_input_slow' */
  real_T ADdata;                       /* '<Root>/ADdata' */
  real_T distande;                     /* '<Root>/distande' */
} ExternalInputs_CAN_NODE1_AUTO;

/* External outputs (root outports fed by signals with auto storage) */
typedef struct {
  real_T current_mode;                 /* '<Root>/current_mode' */
  real_T voltage;                      /* '<Root>/voltage' */
  real_T msg2_0;                       /* '<Root>/msg2_0' */
  real_T msg2_4;                       /* '<Root>/msg2_4' */
  real_T msg1_0;                       /* '<Root>/msg1_0' */
  real_T portb;                        /* '<Root>/portb' */
  real_T distance_check;               /* '<Root>/distance_check' */
} ExternalOutputs_CAN_NODE1_AUTO;

/* Real-time Model Data Structure */
struct RT_MODEL_CAN_NODE1_AUTO {
  const char_T * volatile errorStatus;
};

/* Block signals (auto storage) */
extern BlockIO_CAN_NODE1_AUTO CAN_NODE1_AUTO_B;

/* Block states (auto storage) */
extern D_Work_CAN_NODE1_AUTO CAN_NODE1_AUTO_DWork;

/* External inputs (root inport signals with auto storage) */
extern ExternalInputs_CAN_NODE1_AUTO CAN_NODE1_AUTO_U;

/* External outputs (root outports fed by signals with auto storage) */
extern ExternalOutputs_CAN_NODE1_AUTO CAN_NODE1_AUTO_Y;

/* Model entry point functions */
extern void CAN_NODE1_AUTO_initialize(void);
extern void CAN_NODE1_AUTO_step(void);
extern void CAN_NODE1_AUTO_terminate(void);

/* Real-time Model object */
extern struct RT_MODEL_CAN_NODE1_AUTO *CAN_NODE1_AUTO_M;

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
 * '<Root>' : CAN_NODE1_AUTO
 * '<S1>'   : CAN_NODE1_AUTO/Chart
 */
#endif                                 /* RTW_HEADER_CAN_NODE1_AUTO_h_ */

/*
 * File trailer for Real-Time Workshop generated code.
 *
 * [EOF]
 */
