/*
 * File: servo.c
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

#include "servo.h"
#include "servo_private.h"

/* Named constants for Stateflow: '<Root>/Chart' */
#define se_IN_node3_init_or_move_biking (1U)
#define servo_IN_NO_ACTIVE_CHILD       (0UL)
#define servo_IN_bright1               (1U)
#define servo_IN_bright2               (2U)
#define servo_IN_bright3               (3U)
#define servo_IN_bright4               (4U)
#define servo_IN_closed                (1U)
#define servo_IN_jodo_high             (1U)
#define servo_IN_jodo_low              (2U)
#define servo_IN_move_bar              (1U)
#define servo_IN_open                  (2U)
#define servo_IN_servo_jodo            (2U)
#define servo_IN_warning               (2U)

/* Block signals (auto storage) */
BlockIO_servo servo_B;

/* Block states (auto storage) */
D_Work_servo servo_DWork;

/* External inputs (root inport signals with auto storage) */
ExternalInputs_servo servo_U;

/* External outputs (root outports fed by signals with auto storage) */
ExternalOutputs_servo servo_Y;

/* Real-time model */
RT_MODEL_servo servo_M_;
RT_MODEL_servo *servo_M = &servo_M_;

/* Forward declaration for local functions */
static void servo_enter_internal_jodo_0to90(void);

/* Function for Stateflow: '<Root>/Chart' */
static void servo_enter_internal_jodo_0to90(void)
{
  /* Transition: '<S1>:83' */
  if (servo_U.ADdata > 900.0) {
    /* Transition: '<S1>:84' */
    /* Entry 'bright1': '<S1>:76' */
    servo_DWork.is_jodo_0to90 = servo_IN_bright1;
    servo_B.cds = 90.0;
  } else if (servo_U.ADdata > 800.0) {
    /* Transition: '<S1>:85' */
    /* Entry 'bright2': '<S1>:79' */
    servo_DWork.is_jodo_0to90 = servo_IN_bright2;
    servo_B.cds = 80.0;
  } else if (servo_U.ADdata > 700.0) {
    /* Transition: '<S1>:123' */
    /* Entry 'bright3': '<S1>:119' */
    servo_DWork.is_jodo_0to90 = servo_IN_bright3;
    servo_B.cds = 10.0;
  } else {
    if (servo_U.ADdata <= 600.0) {
      /* Transition: '<S1>:124' */
      /* Entry 'bright4': '<S1>:120' */
      servo_DWork.is_jodo_0to90 = servo_IN_bright4;
      servo_B.cds = 0.0;
    }
  }
}

/* Model step function */
void servo_step(void)
{
  /* Stateflow: '<Root>/Chart' incorporates:
   *  Inport: '<Root>/ADdata'
   *  Inport: '<Root>/current_mode'
   *  Inport: '<Root>/current_speed'
   *  Inport: '<Root>/ting'
   */
  /* Gateway: Chart */
  /* During: Chart */
  if (servo_DWork.is_active_c1_servo == 0) {
    /* Entry: Chart */
    servo_DWork.is_active_c1_servo = 1U;

    /* Transition: '<S1>:129' */
    /* Entry 'node3_init_or_move_biking': '<S1>:8' */
    servo_DWork.is_c1_servo = se_IN_node3_init_or_move_biking;
  } else {
    switch (servo_DWork.is_c1_servo) {
     case se_IN_node3_init_or_move_biking:
      /* During 'node3_init_or_move_biking': '<S1>:8' */
      if ((servo_U.current_mode == 0.0) && (servo_U.current_speed == 0.0)) {
        /* Transition: '<S1>:43' */
        /* Exit 'node3_init_or_move_biking': '<S1>:8' */
        /* Entry 'servo_jodo': '<S1>:9' */
        servo_DWork.is_c1_servo = servo_IN_servo_jodo;

        /* Entry 'jodo_0to90': '<S1>:125' */
        servo_DWork.is_active_jodo_0to90 = 1U;
        servo_enter_internal_jodo_0to90();

        /* Entry 'control_bar': '<S1>:69' */
        servo_DWork.is_active_control_bar = 1U;
        if (servo_U.ting != 0) {
          /* Transition: '<S1>:130' */
          /* Entry 'move_bar': '<S1>:45' */
          servo_DWork.is_control_bar = servo_IN_move_bar;
          servo_B.porte = 252.0;
        }

        /* Entry 'warn_check': '<S1>:21' */
        servo_DWork.is_active_warn_check = 1U;

        /* Transition: '<S1>:41' */
        /* Entry 'jodo_low': '<S1>:23' */
        servo_DWork.is_warn_check = servo_IN_jodo_low;
        servo_DWork.warn = 0;

        /* Entry 'bar_check': '<S1>:191' */
        servo_DWork.is_active_bar_check = 1U;

        /* Transition: '<S1>:197' */
        /* Entry 'closed': '<S1>:193' */
        servo_DWork.is_bar_check = servo_IN_closed;
        servo_DWork.open = FALSE;
      }
      break;

     case servo_IN_servo_jodo:
      /* During 'servo_jodo': '<S1>:9' */
      if ((servo_U.current_mode != 0.0) || (servo_U.current_speed != 0.0)) {
        /* Transition: '<S1>:174' */
        /* Exit 'closed': '<S1>:193' */
        servo_DWork.is_bar_check = (uint8_T)servo_IN_NO_ACTIVE_CHILD;

        /* Exit 'open': '<S1>:192' */
        /* Exit 'bar_check': '<S1>:191' */
        servo_DWork.is_active_bar_check = 0U;

        /* Exit 'jodo_high': '<S1>:10' */
        servo_DWork.is_warn_check = (uint8_T)servo_IN_NO_ACTIVE_CHILD;

        /* Exit 'jodo_low': '<S1>:23' */
        /* Exit 'warn_check': '<S1>:21' */
        servo_DWork.is_active_warn_check = 0U;
        if (servo_DWork.is_control_bar == servo_IN_warning) {
          /* Exit 'warning': '<S1>:62' */
          servo_B.warn_stop = 0.0;
          servo_DWork.is_control_bar = (uint8_T)servo_IN_NO_ACTIVE_CHILD;
        } else {
          servo_DWork.is_control_bar = (uint8_T)servo_IN_NO_ACTIVE_CHILD;

          /* Exit 'move_bar': '<S1>:45' */
        }

        /* Exit 'control_bar': '<S1>:69' */
        servo_DWork.is_active_control_bar = 0U;

        /* Exit 'bright1': '<S1>:76' */
        servo_DWork.is_jodo_0to90 = (uint8_T)servo_IN_NO_ACTIVE_CHILD;

        /* Exit 'bright2': '<S1>:79' */
        /* Exit 'bright3': '<S1>:119' */
        /* Exit 'bright4': '<S1>:120' */
        /* Exit 'jodo_0to90': '<S1>:125' */
        servo_DWork.is_active_jodo_0to90 = 0U;

        /* Exit 'servo_jodo': '<S1>:9' */
        /* Entry 'node3_init_or_move_biking': '<S1>:8' */
        servo_DWork.is_c1_servo = se_IN_node3_init_or_move_biking;
      } else {
        /* During 'jodo_0to90': '<S1>:125' */
        switch (servo_DWork.is_jodo_0to90) {
         case servo_IN_bright1:
          /* During 'bright1': '<S1>:76' */
          /* Transition: '<S1>:167' */
          if (servo_U.ADdata > 900.0) {
            /* Transition: '<S1>:84' */
            /* Exit 'bright1': '<S1>:76' */
            /* Entry 'bright1': '<S1>:76' */
            servo_DWork.is_jodo_0to90 = servo_IN_bright1;
            servo_B.cds = 90.0;
          } else if (servo_U.ADdata > 800.0) {
            /* Transition: '<S1>:85' */
            /* Exit 'bright1': '<S1>:76' */
            /* Entry 'bright2': '<S1>:79' */
            servo_DWork.is_jodo_0to90 = servo_IN_bright2;
            servo_B.cds = 80.0;
          } else if (servo_U.ADdata > 700.0) {
            /* Transition: '<S1>:123' */
            /* Exit 'bright1': '<S1>:76' */
            /* Entry 'bright3': '<S1>:119' */
            servo_DWork.is_jodo_0to90 = servo_IN_bright3;
            servo_B.cds = 10.0;
          } else {
            if (servo_U.ADdata <= 600.0) {
              /* Transition: '<S1>:124' */
              /* Exit 'bright1': '<S1>:76' */
              /* Entry 'bright4': '<S1>:120' */
              servo_DWork.is_jodo_0to90 = servo_IN_bright4;
              servo_B.cds = 0.0;
            }
          }
          break;

         case servo_IN_bright2:
          /* During 'bright2': '<S1>:79' */
          /* Transition: '<S1>:168' */
          if (servo_U.ADdata > 900.0) {
            /* Transition: '<S1>:84' */
            /* Exit 'bright2': '<S1>:79' */
            /* Entry 'bright1': '<S1>:76' */
            servo_DWork.is_jodo_0to90 = servo_IN_bright1;
            servo_B.cds = 90.0;
          } else if (servo_U.ADdata > 800.0) {
            /* Transition: '<S1>:85' */
            /* Exit 'bright2': '<S1>:79' */
            /* Entry 'bright2': '<S1>:79' */
            servo_DWork.is_jodo_0to90 = servo_IN_bright2;
            servo_B.cds = 80.0;
          } else if (servo_U.ADdata > 700.0) {
            /* Transition: '<S1>:123' */
            /* Exit 'bright2': '<S1>:79' */
            /* Entry 'bright3': '<S1>:119' */
            servo_DWork.is_jodo_0to90 = servo_IN_bright3;
            servo_B.cds = 10.0;
          } else {
            if (servo_U.ADdata <= 600.0) {
              /* Transition: '<S1>:124' */
              /* Exit 'bright2': '<S1>:79' */
              /* Entry 'bright4': '<S1>:120' */
              servo_DWork.is_jodo_0to90 = servo_IN_bright4;
              servo_B.cds = 0.0;
            }
          }
          break;

         case servo_IN_bright3:
          /* During 'bright3': '<S1>:119' */
          /* Transition: '<S1>:169' */
          if (servo_U.ADdata > 900.0) {
            /* Transition: '<S1>:84' */
            /* Exit 'bright3': '<S1>:119' */
            /* Entry 'bright1': '<S1>:76' */
            servo_DWork.is_jodo_0to90 = servo_IN_bright1;
            servo_B.cds = 90.0;
          } else if (servo_U.ADdata > 800.0) {
            /* Transition: '<S1>:85' */
            /* Exit 'bright3': '<S1>:119' */
            /* Entry 'bright2': '<S1>:79' */
            servo_DWork.is_jodo_0to90 = servo_IN_bright2;
            servo_B.cds = 80.0;
          } else if (servo_U.ADdata > 700.0) {
            /* Transition: '<S1>:123' */
            /* Exit 'bright3': '<S1>:119' */
            /* Entry 'bright3': '<S1>:119' */
            servo_DWork.is_jodo_0to90 = servo_IN_bright3;
            servo_B.cds = 10.0;
          } else {
            if (servo_U.ADdata <= 600.0) {
              /* Transition: '<S1>:124' */
              /* Exit 'bright3': '<S1>:119' */
              /* Entry 'bright4': '<S1>:120' */
              servo_DWork.is_jodo_0to90 = servo_IN_bright4;
              servo_B.cds = 0.0;
            }
          }
          break;

         case servo_IN_bright4:
          /* During 'bright4': '<S1>:120' */
          /* Transition: '<S1>:170' */
          if (servo_U.ADdata > 900.0) {
            /* Transition: '<S1>:84' */
            /* Exit 'bright4': '<S1>:120' */
            /* Entry 'bright1': '<S1>:76' */
            servo_DWork.is_jodo_0to90 = servo_IN_bright1;
            servo_B.cds = 90.0;
          } else if (servo_U.ADdata > 800.0) {
            /* Transition: '<S1>:85' */
            /* Exit 'bright4': '<S1>:120' */
            /* Entry 'bright2': '<S1>:79' */
            servo_DWork.is_jodo_0to90 = servo_IN_bright2;
            servo_B.cds = 80.0;
          } else if (servo_U.ADdata > 700.0) {
            /* Transition: '<S1>:123' */
            /* Exit 'bright4': '<S1>:120' */
            /* Entry 'bright3': '<S1>:119' */
            servo_DWork.is_jodo_0to90 = servo_IN_bright3;
            servo_B.cds = 10.0;
          } else {
            if (servo_U.ADdata <= 600.0) {
              /* Transition: '<S1>:124' */
              /* Exit 'bright4': '<S1>:120' */
              /* Entry 'bright4': '<S1>:120' */
              servo_DWork.is_jodo_0to90 = servo_IN_bright4;
              servo_B.cds = 0.0;
            }
          }
          break;

         default:
          servo_enter_internal_jodo_0to90();
          break;
        }

        /* During 'control_bar': '<S1>:69' */
        switch (servo_DWork.is_control_bar) {
         case servo_IN_move_bar:
          /* During 'move_bar': '<S1>:45' */
          if (((int16_T)servo_DWork.open == 0) && (servo_DWork.warn == 1)) {
            /* Transition: '<S1>:198' */
            /* Exit 'move_bar': '<S1>:45' */
            /* Entry 'warning': '<S1>:62' */
            servo_DWork.is_control_bar = servo_IN_warning;
            servo_B.warn_stop = 1.0;
          } else {
            servo_B.ocr3c = (int16_T)(((8.3333333333333328E-004 * (real_T)
              servo_U.ting + 0.6) + 8.8888888888888889E-003 * (real_T)
              servo_U.ting) * 125.0);
          }
          break;

         case servo_IN_warning:
          /* During 'warning': '<S1>:62' */
          if (((int16_T)servo_DWork.open == 1) || (servo_DWork.warn == 0)) {
            /* Transition: '<S1>:172' */
            /* Exit 'warning': '<S1>:62' */
            servo_B.warn_stop = 0.0;

            /* Entry 'move_bar': '<S1>:45' */
            servo_DWork.is_control_bar = servo_IN_move_bar;
            servo_B.porte = 252.0;
          }
          break;

         default:
          if (servo_U.ting != 0) {
            /* Transition: '<S1>:130' */
            /* Entry 'move_bar': '<S1>:45' */
            servo_DWork.is_control_bar = servo_IN_move_bar;
            servo_B.porte = 252.0;
          }
          break;
        }

        /* During 'warn_check': '<S1>:21' */
        switch (servo_DWork.is_warn_check) {
         case servo_IN_jodo_high:
          /* During 'jodo_high': '<S1>:10' */
          if (servo_U.ADdata <= 800.0) {
            /* Transition: '<S1>:173' */
            /* Exit 'jodo_high': '<S1>:10' */
            /* Entry 'jodo_low': '<S1>:23' */
            servo_DWork.is_warn_check = servo_IN_jodo_low;
            servo_DWork.warn = 0;
          }
          break;

         case servo_IN_jodo_low:
          /* During 'jodo_low': '<S1>:23' */
          if (servo_U.ADdata > 800.0) {
            /* Transition: '<S1>:40' */
            /* Exit 'jodo_low': '<S1>:23' */
            /* Entry 'jodo_high': '<S1>:10' */
            servo_DWork.is_warn_check = servo_IN_jodo_high;
            servo_DWork.warn = 1;
          }
          break;

         default:
          /* Transition: '<S1>:41' */
          /* Entry 'jodo_low': '<S1>:23' */
          servo_DWork.is_warn_check = servo_IN_jodo_low;
          servo_DWork.warn = 0;
          break;
        }

        /* During 'bar_check': '<S1>:191' */
        switch (servo_DWork.is_bar_check) {
         case servo_IN_closed:
          /* During 'closed': '<S1>:193' */
          if (servo_U.ting > 30) {
            /* Transition: '<S1>:194' */
            /* Exit 'closed': '<S1>:193' */
            /* Entry 'open': '<S1>:192' */
            servo_DWork.is_bar_check = servo_IN_open;
            servo_DWork.open = TRUE;
          }
          break;

         case servo_IN_open:
          /* During 'open': '<S1>:192' */
          if (servo_U.ting <= 30) {
            /* Transition: '<S1>:195' */
            /* Exit 'open': '<S1>:192' */
            /* Entry 'closed': '<S1>:193' */
            servo_DWork.is_bar_check = servo_IN_closed;
            servo_DWork.open = FALSE;
          }
          break;

         default:
          /* Transition: '<S1>:197' */
          /* Entry 'closed': '<S1>:193' */
          servo_DWork.is_bar_check = servo_IN_closed;
          servo_DWork.open = FALSE;
          break;
        }
      }
      break;

     default:
      /* Transition: '<S1>:129' */
      /* Entry 'node3_init_or_move_biking': '<S1>:8' */
      servo_DWork.is_c1_servo = se_IN_node3_init_or_move_biking;
      break;
    }
  }

  /* Outport: '<Root>/warn_stop' */
  servo_Y.warn_stop = servo_B.warn_stop;

  /* Outport: '<Root>/angle' */
  servo_Y.angle = servo_B.angle;

  /* Outport: '<Root>/cds' */
  servo_Y.cds = servo_B.cds;

  /* Outport: '<Root>/ocr3c' */
  servo_Y.ocr3c = servo_B.ocr3c;

  /* Outport: '<Root>/porte' */
  servo_Y.porte = servo_B.porte;

  /* Outport: '<Root>/portb' */
  servo_Y.portb = servo_B.portb;
}

/* Model initialize function */
void servo_initialize(void)
{
  /* Registration code */

  /* initialize error status */
  rtmSetErrorStatus(servo_M, (NULL));

  /* block I/O */
  (void) memset(((void *) &servo_B), 0,
                sizeof(BlockIO_servo));

  /* states (dwork) */
  (void) memset((void *)&servo_DWork, 0,
                sizeof(D_Work_servo));

  /* external inputs */
  (void) memset((void *)&servo_U, 0,
                sizeof(ExternalInputs_servo));

  /* external outputs */
  (void) memset((void *)&servo_Y, 0,
                sizeof(ExternalOutputs_servo));

  /* InitializeConditions for Stateflow: '<Root>/Chart' */
  servo_DWork.is_active_bar_check = 0U;
  servo_DWork.is_bar_check = 0U;
  servo_DWork.is_active_control_bar = 0U;
  servo_DWork.is_control_bar = 0U;
  servo_DWork.is_active_jodo_0to90 = 0U;
  servo_DWork.is_jodo_0to90 = 0U;
  servo_DWork.is_active_warn_check = 0U;
  servo_DWork.is_warn_check = 0U;
  servo_DWork.is_active_c1_servo = 0U;
  servo_DWork.is_c1_servo = 0U;
  servo_DWork.warn = 0;
  servo_DWork.open = FALSE;
  servo_B.warn_stop = 0.0;
  servo_B.angle = 0.0;
  servo_B.cds = 0.0;
  servo_B.ocr3c = 0;
  servo_B.porte = 0.0;
  servo_B.portb = 0.0;
}

/* Model terminate function */
void servo_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for Real-Time Workshop generated code.
 *
 * [EOF]
 */
