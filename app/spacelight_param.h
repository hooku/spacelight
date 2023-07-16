#ifndef __SPACELIGHT_PARAM_DEFAULT_H_
#define __SPACELIGHT_PARAM_DEFAULT_H_

#include "spacelight.h"

#include <stdint.h>

/* tuner setting */
#define DIM_DEFAULT 12
#define DIM_MIN 0
#define DIM_MAX 100
#define DIM_STEP 1

#define CCT_DEFAULT 3400
#define CCT_MIN 2600
#define CCT_MAX 6000
#define CCT_STEP 100

#define BLINK_SPD_DEFAULT 50
#define BLINK_SPD_MIN 1
#define BLINK_SPD_MAX 100
#define BLINK_SPD_STEP 1

#define BLINK_DUTY_DEFAULT 50
#define BLINK_DUTY_MIN 1
#define BLINK_DUTY_MAX 100
#define BLINK_DUTY_STEP 1

#define BREATHE_SPD_DEFAULT 100
#define BREATHE_SPD_MIN 1
#define BREATHE_SPD_MAX 100
#define BREATHE_SPD_STEP 1

#define ROTATE_SPD_DEFAULT 100
#define ROTATE_SPD_MIN 1
#define ROTATE_SPD_MAX 100
#define ROTATE_SPD_STEP 1

#define LIGHTNING_SPD_DEFAULT 9
#define LIGHTNING_SPD_MIN 1
#define LIGHTNING_SPD_MAX 100
#define LIGHTNING_SPD_STEP 1

#define LIGHTNING_MAG_DEFAULT 20
#define LIGHTNING_MAG_MIN 20
#define LIGHTNING_MAG_MAX 100
#define LIGHTNING_MAG_STEP 1

#define CCT_DRIFT_SPD_DEFAULT 100
#define CCT_DRIFT_SPD_MIN 1
#define CCT_DRIFT_SPD_MAX 100
#define CCT_DRIFT_SPD_STEP 1

#define CCT_DRIFT_CCT1_DEFAULT 3400
#define CCT_DRIFT_CCT1_MIN CCT_MIN
#define CCT_DRIFT_CCT1_MAX CCT_MAX
#define CCT_DRIFT_CCT1_STEP CCT_STEP

#define CCT_DRIFT_CCT2_DEFAULT 2600
#define CCT_DRIFT_CCT2_MIN CCT_MIN
#define CCT_DRIFT_CCT2_MAX CCT_MAX
#define CCT_DRIFT_CCT2_STEP CCT_STEP

#define FIRE_SPD_DEFAULT 100
#define FIRE_SPD_MIN 5
#define FIRE_SPD_MAX 100
#define FIRE_SPD_STEP 1

#define FIRE_MAG_DEFAULT 92
#define FIRE_MAG_MIN 40
#define FIRE_MAG_MAX 100
#define FIRE_MAG_STEP 1

#define INDEP_DIM_DEFAULT 10
#define INDEP_DIM_MIN 0
#define INDEP_DIM_MAX 100
#define INDEP_DIM_STEP 1

#define INDEP_CCT_DEFAULT 4300
#define INDEP_CCT_MIN CCT_MIN
#define INDEP_CCT_MAX CCT_MAX
#define INDEP_CCT_STEP CCT_STEP

#define MENUMAIN_DEFAULT 0
#define MENUMAIN_MIN 0
#define MENUMAIN_MAX MENUMAIN_MIN
#define MENUMAIN_STEP 1

#define LAMPCOUNT_DEFAULT 4
#define LAMPCOUNT_MIN 1
#define LAMPCOUNT_MAX 4
#define LAMPCOUNT_STEP 1

#define DMXADDR_DEFAULT 1
#define DMXADDR_MIN 1
#define DMXADDR_MAX 512
#define DMXADDR_STEP 2

#define LOCKTIME_DEFAULT 10 // 60
#define LOCKTIME_MIN 10
#define LOCKTIME_MAX 300
#define LOCKTIME_STEP 1

#define DMXMODE_DEFAULT 0
#define DMXMODE_MIN 0
#define DMXMODE_MAX 2
#define DMXMODE_STEP 1

#define WIRELESS_DEFAULT 0
#define WIRELESS_MIN 0
#define WIRELESS_MAX 1
#define WIRELESS_STEP 1

/* param count */
#define PARA_CNT_CCT 2
#define PARA_CNT_BLINK 4
#define PARA_CNT_BREATHE 3
#define PARA_CNT_ROTATE 3
#define PARA_CNT_LIGHTNING 4
#define PARA_CNT_DRIFT 4
#define PARA_CNT_FIRE 4
#define PARA_CNT_INDEP 8
#define PARA_CNT_LAMPCOUNT 1
#define PARA_CNT_DMXADDR 1
#define PARA_CNT_LOCKTIME 1
#define PARA_CNT_DMXMODE 1
#define PARA_CNT_WIRELESS 1

typedef enum
{
    PARAM_DIM,
    PARAM_CCT,
    PARAM_BLINK_SPD,
    PARAM_BLINK_DUTY,
    PARAM_BREATHE_SPD,
    PARAM_ROTATE_SPD,
    PARAM_LIGHTNING_SPD,
    PARAM_LIGHTNING_MAG,
    PARAM_CCT_DRIFT_SPD,
    PARAM_CCT_DRIFT_CCT1,
    PARAM_CCT_DRIFT_CCT2,
    PARAM_FIRE_SPD,
    PARAM_FIRE_MAG,
    PARAM_INDEP_DIM1,
    PARAM_INDEP_DIM2,
    PARAM_INDEP_DIM3,
    PARAM_INDEP_DIM4,
    PARAM_INDEP_CCT1,
    PARAM_INDEP_CCT2,
    PARAM_INDEP_CCT3,
    PARAM_INDEP_CCT4,
    PARAM_MENU,
    PARAM_LAMPCOUNT,
    PARAM_DMXADDR,
    PARAM_LOCKTIME,
    PARAM_DMXMODE,
    PARAM_WIRELSS,
} SlParaName;

typedef struct
{
    uint16_t value;
    uint16_t new_value;
    uint16_t min;
    uint16_t max;
    uint16_t step;
} TunerParam;

typedef struct
{
    TunerParam dim;
    TunerParam cct;
    TunerParam blink_spd;
    TunerParam blink_duty;
    TunerParam breathe_spd;
    TunerParam rotate_spd;
    TunerParam lightning_spd;
    TunerParam lightning_mag;
    TunerParam cct_drift_spd;
    TunerParam cct_drift_cct[2];
    TunerParam fire_spd;
    TunerParam fire_mag;
    TunerParam indep_dim[4];
    TunerParam indep_cct[4];
    TunerParam menumain;
    TunerParam lampcount;
    TunerParam dmxaddr;
    TunerParam locktime;
    TunerParam dmxmode;
    TunerParam wireless;
} SlParam;

typedef struct
{
    SlParaName name;
    TunerParam *param;
    char desc[MAX_TEXT_LEN_SHORT];
} NameParamMap;

typedef struct
{
    uint16_t stage;
    uint16_t param_count;
    const SlParaName *name;
} StageNameMap;

extern const StageNameMap stage_name_map[];
extern const NameParamMap name_param_map[];

#endif // __SPACELIGHT_PARAM_DEFAULT_H_
