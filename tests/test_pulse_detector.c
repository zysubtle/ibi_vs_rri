#include "ppg_ibi.h"
#include <assert.h>
#include "../src/ppg_ibi_internal.h"

static ppg_ibi_sample_t make_sample(uint32_t ts, int32_t c0, int32_t c1, int32_t c2, int32_t c3, uint8_t allow){ppg_ibi_sample_t s; s.timestamp_ms=ts; s.ppg[0]=c0; s.ppg[1]=c1; s.ppg[2]=c2; s.ppg[3]=c3; s.allow_measure=allow; return s;}

int main(void){
    ppg_ibi_context_t ctx; ppg_ibi_event_t e; ppg_ibi_sample_t s;
    assert(ppg_ibi_init(&ctx,NULL)==PPG_IBI_STATUS_OK);

    s=make_sample(1000,100,100,100,100,1); assert(ppg_ibi_process(&ctx,&s,&e)==PPG_IBI_STATUS_NO_EVENT);
    s=make_sample(1020,200,100,100,100,1); assert(ppg_ibi_process(&ctx,&s,&e)==PPG_IBI_STATUS_NO_EVENT);
    s=make_sample(1040,PPG_IBI_PPG_MAX_24BIT-10,250,100,100,1); assert(ppg_ibi_process(&ctx,&s,&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(ctx.has_last_pulse==0u);

    s=make_sample(1060,PPG_IBI_PPG_MAX_24BIT,100,100,100,1); assert(ppg_ibi_process(&ctx,&s,&e)==PPG_IBI_STATUS_NO_EVENT); assert(ctx.has_last_pulse==0u);
    s=make_sample(1080,100,100,100,100,0); assert(ppg_ibi_process(&ctx,&s,&e)==PPG_IBI_STATUS_NO_EVENT); assert(ctx.has_last_pulse==0u);
    s=make_sample(1101,100,100,100,100,1); assert(ppg_ibi_process(&ctx,&s,&e)==PPG_IBI_STATUS_NO_EVENT); assert(ctx.has_last_pulse==0u);

    assert(ppg_ibi_reset(&ctx)==PPG_IBI_STATUS_OK);

    s=make_sample(2000,100,100,100,100,1); assert(ppg_ibi_process(&ctx,&s,&e)==PPG_IBI_STATUS_NO_EVENT);
    s=make_sample(2020,250,100,100,100,1); assert(ppg_ibi_process(&ctx,&s,&e)==PPG_IBI_STATUS_NO_EVENT);
    s=make_sample(2040,100,100,100,100,1); assert(ppg_ibi_process(&ctx,&s,&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(ctx.has_last_pulse==1u);

    uint32_t ts=2060;
    for(int i=0;i<14;i++){ s=make_sample(ts,100,100,100,100,1); assert(ppg_ibi_process(&ctx,&s,&e)==PPG_IBI_STATUS_NO_EVENT); ts+=20; }
    s=make_sample(2340,280,100,100,100,1); assert(ppg_ibi_process(&ctx,&s,&e)==PPG_IBI_STATUS_NO_EVENT);
    s=make_sample(2360,100,100,100,100,1); assert(ppg_ibi_process(&ctx,&s,&e)==PPG_IBI_STATUS_EVENT_READY);

    assert(e.state==PPG_IBI_STATE_TRACK);
    assert(e.timestamp_ms==2340u);
    assert(e.selected_channel==0u);
    assert(e.signal_quality==PPG_IBI_CHANNEL_QUALITY_BASIC_VALID);
    assert(e.confidence==PPG_IBI_CHANNEL_QUALITY_BASIC_VALID);
    assert(e.ibi_ms==320u);

    assert(ppg_ibi_reset(&ctx)==PPG_IBI_STATUS_OK);
    s=make_sample(3000,100,100,100,100,1); assert(ppg_ibi_process(&ctx,&s,&e)==PPG_IBI_STATUS_NO_EVENT);
    s=make_sample(3020,250,100,100,100,1); assert(ppg_ibi_process(&ctx,&s,&e)==PPG_IBI_STATUS_NO_EVENT);
    s=make_sample(3040,100,100,100,100,1); assert(ppg_ibi_process(&ctx,&s,&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(ctx.has_last_pulse==1u);
    s=make_sample(3060,250,100,100,100,1); assert(ppg_ibi_process(&ctx,&s,&e)==PPG_IBI_STATUS_NO_EVENT);
    s=make_sample(3080,100,100,100,100,1); assert(ppg_ibi_process(&ctx,&s,&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason==PPG_IBI_REJECT_IBI_OUT_OF_RANGE);
    assert(e.state==PPG_IBI_STATE_REACQUIRE);
    assert(ctx.state==PPG_IBI_STATE_REACQUIRE);
    assert(ctx.has_prev_sample==0u && ctx.has_prev2_sample==0u);
    assert(ctx.has_last_pulse==0u);

    return 0;
}
