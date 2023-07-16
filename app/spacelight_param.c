#include "spacelight_param.h"

#define VAL_NEWVAL(value) value, value

SlParam spacelight_param = {
    {VAL_NEWVAL(DIM_DEFAULT), DIM_MIN, DIM_MAX, DIM_STEP},
    {VAL_NEWVAL(CCT_DEFAULT), CCT_MIN, CCT_MAX, CCT_STEP},
    {VAL_NEWVAL(BLINK_SPD_DEFAULT), BLINK_SPD_MIN, BLINK_SPD_MAX, BLINK_SPD_STEP},
    {VAL_NEWVAL(BLINK_DUTY_DEFAULT), BLINK_DUTY_MIN, BLINK_DUTY_MAX, BLINK_DUTY_STEP},
    {VAL_NEWVAL(BREATHE_SPD_DEFAULT), BREATHE_SPD_MIN, BREATHE_SPD_MAX, BREATHE_SPD_STEP},
    {VAL_NEWVAL(ROTATE_SPD_DEFAULT), ROTATE_SPD_MIN, ROTATE_SPD_MAX, ROTATE_SPD_STEP},
    {VAL_NEWVAL(LIGHTNING_SPD_DEFAULT), LIGHTNING_SPD_MIN, LIGHTNING_SPD_MAX, LIGHTNING_SPD_STEP},
    {VAL_NEWVAL(LIGHTNING_MAG_DEFAULT), LIGHTNING_MAG_MIN, LIGHTNING_MAG_MAX, LIGHTNING_MAG_STEP},
    {VAL_NEWVAL(CCT_DRIFT_SPD_DEFAULT), CCT_DRIFT_SPD_MIN, CCT_DRIFT_SPD_MAX, CCT_DRIFT_SPD_STEP},
    {
        {VAL_NEWVAL(CCT_DRIFT_CCT1_DEFAULT), CCT_DRIFT_CCT1_MIN, CCT_DRIFT_CCT1_MAX, CCT_DRIFT_CCT1_STEP},
        {VAL_NEWVAL(CCT_DRIFT_CCT2_DEFAULT), CCT_DRIFT_CCT2_MIN, CCT_DRIFT_CCT2_MAX, CCT_DRIFT_CCT2_STEP},
    },
    {VAL_NEWVAL(FIRE_SPD_DEFAULT), FIRE_SPD_MIN, FIRE_SPD_MAX, FIRE_SPD_STEP},
    {VAL_NEWVAL(FIRE_MAG_DEFAULT), FIRE_MAG_MIN, FIRE_MAG_MAX, FIRE_MAG_STEP},
    {
        {VAL_NEWVAL(INDEP_DIM_DEFAULT), INDEP_DIM_MIN, INDEP_DIM_MAX, INDEP_DIM_STEP},
        {VAL_NEWVAL(INDEP_DIM_DEFAULT), INDEP_DIM_MIN, INDEP_DIM_MAX, INDEP_DIM_STEP},
        {VAL_NEWVAL(INDEP_DIM_DEFAULT), INDEP_DIM_MIN, INDEP_DIM_MAX, INDEP_DIM_STEP},
        {VAL_NEWVAL(INDEP_DIM_DEFAULT), INDEP_DIM_MIN, INDEP_DIM_MAX, INDEP_DIM_STEP},
    },
    {
        {VAL_NEWVAL(INDEP_CCT_DEFAULT), INDEP_CCT_MIN, INDEP_CCT_MAX, INDEP_CCT_STEP},
        {VAL_NEWVAL(INDEP_CCT_DEFAULT), INDEP_CCT_MIN, INDEP_CCT_MAX, INDEP_CCT_STEP},
        {VAL_NEWVAL(INDEP_CCT_DEFAULT), INDEP_CCT_MIN, INDEP_CCT_MAX, INDEP_CCT_STEP},
        {VAL_NEWVAL(INDEP_CCT_DEFAULT), INDEP_CCT_MIN, INDEP_CCT_MAX, INDEP_CCT_STEP},
    },
    {VAL_NEWVAL(MENUMAIN_DEFAULT), MENUMAIN_MIN, MENUMAIN_MAX, MENUMAIN_STEP},
    {VAL_NEWVAL(LAMPCOUNT_DEFAULT), LAMPCOUNT_MIN, LAMPCOUNT_MAX, LAMPCOUNT_STEP},
    {VAL_NEWVAL(DMXADDR_DEFAULT), DMXADDR_MIN, DMXADDR_MAX, DMXADDR_STEP},
    {VAL_NEWVAL(LOCKTIME_DEFAULT), LOCKTIME_MIN, LOCKTIME_MAX, LOCKTIME_STEP},
    {VAL_NEWVAL(DMXMODE_DEFAULT), DMXMODE_MIN, DMXMODE_MAX, DMXMODE_STEP},
    {VAL_NEWVAL(WIRELESS_DEFAULT), WIRELESS_MIN, WIRELESS_MAX, WIRELESS_STEP},
};

static const SlParaName name_cct[] = {
    {PARAM_DIM},
    {PARAM_CCT},
};

static const SlParaName name_blink[] = {
    {PARAM_DIM},
    {PARAM_CCT},
    {PARAM_BLINK_SPD},
    {PARAM_BLINK_DUTY},
};

static const SlParaName name_breathe[] = {
    {PARAM_DIM},
    {PARAM_CCT},
    {PARAM_BREATHE_SPD},
};

static const SlParaName name_rotate[] = {
    {PARAM_DIM},
    {PARAM_CCT},
    {PARAM_ROTATE_SPD},
};

static const SlParaName name_lightning[] = {
    {PARAM_DIM},
    {PARAM_CCT},
    {PARAM_LIGHTNING_SPD},
    {PARAM_LIGHTNING_MAG},
};

static const SlParaName name_cct_drift[] = {
    {PARAM_DIM},
    {PARAM_CCT_DRIFT_SPD},
    {PARAM_CCT_DRIFT_CCT1},
    {PARAM_CCT_DRIFT_CCT2},
};

static const SlParaName name_fire[] = {
    {PARAM_DIM},
    {PARAM_CCT},
    {PARAM_FIRE_SPD},
    {PARAM_FIRE_MAG},
};

static const SlParaName name_menu[] = {
    {PARAM_MENU},
};

static const SlParaName name_indep[] = {
    {PARAM_INDEP_DIM1},
    {PARAM_INDEP_DIM2},
    {PARAM_INDEP_DIM3},
    {PARAM_INDEP_DIM4},
    {PARAM_INDEP_CCT1},
    {PARAM_INDEP_CCT2},
    {PARAM_INDEP_CCT3},
    {PARAM_INDEP_CCT4},
};

static const SlParaName name_lampcount[] = {
    {PARAM_LAMPCOUNT},
};

static const SlParaName name_dmxaddr[] = {
    {PARAM_DMXADDR},
};

static const SlParaName name_locktime[] = {
    {PARAM_LOCKTIME},
};

static const SlParaName name_dmxmode[] = {
    {PARAM_DMXMODE},
};

static const SlParaName name_wireless[] = {
    {PARAM_WIRELSS},
};

const StageNameMap stage_name_map[] = {
    {MAIN_CCT, PARA_CNT_CCT, (const SlParaName *)&name_cct},
    {MAIN_BLINK, PARA_CNT_BLINK, (const SlParaName *)&name_blink},
    {MAIN_BREATHE, PARA_CNT_BREATHE, (const SlParaName *)&name_breathe},
    {MAIN_ROTATE, PARA_CNT_ROTATE, (const SlParaName *)&name_rotate},
    {MAIN_LIGHTNING, PARA_CNT_LIGHTNING, (const SlParaName *)&name_lightning},
    {MAIN_CCT_DRIFT, PARA_CNT_DRIFT, (const SlParaName *)&name_cct_drift},
    {MAIN_FIRE, PARA_CNT_FIRE, (const SlParaName *)&name_fire},
    {MAIN_INDEP, PARA_CNT_INDEP, (const SlParaName *)&name_indep},
    {MENU_MAIN, PARAM_MENU, (const SlParaName *)&name_menu},
    {MENU_EFFECT_MODE, PARAM_MENU, (const SlParaName *)&name_menu},
    {CFG_LAMP_COUNT, PARA_CNT_LAMPCOUNT, (const SlParaName *)&name_lampcount},
    {CFG_DMX_ADDR, PARA_CNT_DMXADDR, (const SlParaName *)&name_dmxaddr},
    {CFG_DMX_MODE, PARA_CNT_DMXMODE, (const SlParaName *)&name_dmxmode},
    {CFG_WIRELESS, PARA_CNT_WIRELESS, (const SlParaName *)&name_wireless},
    {CFG_LOCK_TIME, PARA_CNT_LOCKTIME, (const SlParaName *)&name_locktime},
    {CFG_VERSION, PARAM_MENU, (const SlParaName *)&name_menu},
};

const NameParamMap name_param_map[] = {
    {PARAM_DIM, &spacelight_param.dim, "DIM"},
    {PARAM_CCT, &spacelight_param.cct, "CCT"},
    {PARAM_BLINK_SPD, &spacelight_param.blink_spd, "SPD"},
    {PARAM_BLINK_DUTY, &spacelight_param.blink_duty, "DUTY"},
    {PARAM_BREATHE_SPD, &spacelight_param.blink_duty, "DUTY"},
    {PARAM_ROTATE_SPD, &spacelight_param.rotate_spd, "ROTATE"},
    {PARAM_LIGHTNING_SPD, &spacelight_param.lightning_spd, "SPD"},
    {PARAM_LIGHTNING_MAG, &spacelight_param.lightning_mag, "MAG"},
    {PARAM_CCT_DRIFT_SPD, &spacelight_param.cct_drift_spd, "SPD"},
    {PARAM_CCT_DRIFT_CCT1, &spacelight_param.cct_drift_cct[0], "CCT1"},
    {PARAM_CCT_DRIFT_CCT2, &spacelight_param.cct_drift_cct[1], "CCT2"},
    {PARAM_FIRE_SPD, &spacelight_param.fire_spd, "SPD"},
    {PARAM_FIRE_MAG, &spacelight_param.fire_mag, "MAG"},
    {PARAM_INDEP_DIM1, &spacelight_param.indep_dim[0], "DIM1"},
    {PARAM_INDEP_DIM2, &spacelight_param.indep_dim[1], "DIM2"},
    {PARAM_INDEP_DIM3, &spacelight_param.indep_dim[2], "DIM3"},
    {PARAM_INDEP_DIM4, &spacelight_param.indep_dim[3], "DIM4"},
    {PARAM_INDEP_CCT1, &spacelight_param.indep_cct[0], "CCT1"},
    {PARAM_INDEP_CCT2, &spacelight_param.indep_cct[1], "CCT2"},
    {PARAM_INDEP_CCT3, &spacelight_param.indep_cct[2], "CCT3"},
    {PARAM_INDEP_CCT4, &spacelight_param.indep_cct[3], "CCT4"},
    {PARAM_MENU, &spacelight_param.menumain, ""},
    {PARAM_LAMPCOUNT, &spacelight_param.lampcount, "Lamp Count"},
    {PARAM_DMXADDR, &spacelight_param.dmxaddr, "DMX Address"},
    {PARAM_LOCKTIME, &spacelight_param.locktime, "Lock Time"},
    {PARAM_DMXMODE, &spacelight_param.dmxmode, "DMX Mode"},
    {PARAM_WIRELSS, &spacelight_param.wireless, "Wireless"},
};
