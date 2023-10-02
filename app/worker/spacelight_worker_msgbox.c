#include "spacelight_worker.h"

TX_TIMER tmr_msgbox;

#define MSGBOX_TIMEOUT_MS 3000

typedef struct
{
    MsgType msg_type;
    MsgStyle msg_style;
    const char *text;
} MsgTextMap;

MsgTextMap msg_text_map[] = {
    {MSG_INDEP_LAMPON, MSG_EXCLAMATION, STR_MSG_INDEP_LAMPON},
    {MSG_CTL_BY_DMX, MSG_EXCLAMATION, STR_MSG_CTL_BY_DMX},
};

static void msgbox_expired(ULONG param)
{
    UINT status;
    status = tx_timer_delete(&tmr_msgbox);
    assert_param(status == TX_SUCCESS);
}

const char *worker_msgbox_init(MsgType msg_type)
{
    UINT status;
    status = tx_timer_create(&tmr_msgbox, "msgbox",
                             msgbox_expired, 0, MSGBOX_TIMEOUT_MS, MSGBOX_TIMEOUT_MS,
                             TX_AUTO_ACTIVATE);
    assert_param(status == TX_SUCCESS);

    return msg_text_map[msg_type].text;
}
