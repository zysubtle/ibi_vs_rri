#include "ppg_ibi.h"
#include <assert.h>
#include "../src/ppg_ibi_internal.h"

static ppg_ibi_sample_t s(uint32_t ts, int32_t v, uint8_t allow){
    ppg_ibi_sample_t x = {0};
    x.timestamp_ms = ts;
    x.ppg[0]=v; x.ppg[1]=100; x.ppg[2]=100; x.ppg[3]=100;
    x.allow_measure = allow;
    return x;
}

static void prime_track(ppg_ibi_context_t *ctx, ppg_ibi_event_t *e){
    ppg_ibi_sample_t x;
    x=s(1000,100,1); assert(ppg_ibi_process(ctx,&x,e)==PPG_IBI_STATUS_NO_EVENT);
    x=s(1020,200,1); assert(ppg_ibi_process(ctx,&x,e)==PPG_IBI_STATUS_NO_EVENT);
    x=s(1040,100,1); assert(ppg_ibi_process(ctx,&x,e)==PPG_IBI_STATUS_NO_EVENT);
    assert(ctx->has_prev_sample==1u);
}

int main(void){
    ppg_ibi_context_t ctx; ppg_ibi_event_t e; ppg_ibi_sample_t x;
    assert(ppg_ibi_init(&ctx,NULL)==PPG_IBI_STATUS_OK);

    prime_track(&ctx,&e);
    x=s(1060,100,0); assert(ppg_ibi_process(&ctx,&x,&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(e.state==PPG_IBI_STATE_HOLD && ctx.state==PPG_IBI_STATE_HOLD);
    assert(ctx.has_prev_sample==0u && ctx.has_prev2_sample==0u && ctx.has_last_pulse==0u);

    assert(ppg_ibi_reset(&ctx)==PPG_IBI_STATUS_OK); prime_track(&ctx,&e);
    x=s(1060,PPG_IBI_PPG_MAX_24BIT,1); assert(ppg_ibi_process(&ctx,&x,&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason==PPG_IBI_REJECT_SATURATED);
    assert(e.state==PPG_IBI_STATE_REACQUIRE && ctx.state==PPG_IBI_STATE_REACQUIRE);
    assert(ctx.has_prev_sample==0u && ctx.has_prev2_sample==0u && ctx.has_last_pulse==0u);

    assert(ppg_ibi_reset(&ctx)==PPG_IBI_STATUS_OK); prime_track(&ctx,&e);
    x=s(1059,100,1); assert(ppg_ibi_process(&ctx,&x,&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason==PPG_IBI_REJECT_TIMESTAMP_GAP);
    assert(e.state==PPG_IBI_STATE_REACQUIRE && ctx.state==PPG_IBI_STATE_REACQUIRE);
    assert(ctx.has_prev_sample==0u && ctx.has_prev2_sample==0u && ctx.has_last_pulse==0u);

    assert(ppg_ibi_reset(&ctx)==PPG_IBI_STATUS_OK); prime_track(&ctx,&e);
    x=s(1080,100,1); assert(ppg_ibi_process(&ctx,&x,&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason==PPG_IBI_REJECT_SAMPLE_DROP);
    assert(e.state==PPG_IBI_STATE_REACQUIRE && ctx.state==PPG_IBI_STATE_REACQUIRE);
    assert(ctx.has_prev_sample==0u && ctx.has_prev2_sample==0u && ctx.has_last_pulse==0u);

    assert(ppg_ibi_reset(&ctx)==PPG_IBI_STATUS_OK); prime_track(&ctx,&e);
    x.timestamp_ms=1060; x.ppg[0]=PPG_IBI_PPG_MAX_24BIT-10; x.ppg[1]=PPG_IBI_PPG_MAX_24BIT-10; x.ppg[2]=PPG_IBI_PPG_MAX_24BIT-10; x.ppg[3]=PPG_IBI_PPG_MAX_24BIT-10; x.allow_measure=1; assert(ppg_ibi_process(&ctx,&x,&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason==PPG_IBI_REJECT_LOW_SIGNAL_QUALITY);
    assert(e.state==PPG_IBI_STATE_REACQUIRE && ctx.state==PPG_IBI_STATE_REACQUIRE);
    assert(ctx.has_prev_sample==0u && ctx.has_prev2_sample==0u && ctx.has_last_pulse==0u);

    assert(ppg_ibi_reset(&ctx)==PPG_IBI_STATUS_OK);
    x=s(2000,100,1); assert(ppg_ibi_process(&ctx,&x,&e)==PPG_IBI_STATUS_NO_EVENT);
    x=s(2020,250,1); assert(ppg_ibi_process(&ctx,&x,&e)==PPG_IBI_STATUS_NO_EVENT);
    x=s(2040,100,1); assert(ppg_ibi_process(&ctx,&x,&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(ctx.has_last_pulse==1u);
    x=s(2060,240,1); assert(ppg_ibi_process(&ctx,&x,&e)==PPG_IBI_STATUS_NO_EVENT);
    x=s(2080,100,1); assert(ppg_ibi_process(&ctx,&x,&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason==PPG_IBI_REJECT_IBI_OUT_OF_RANGE);
    assert(e.state==PPG_IBI_STATE_REACQUIRE && ctx.state==PPG_IBI_STATE_REACQUIRE);
    assert(ctx.has_prev_sample==0u && ctx.has_prev2_sample==0u && ctx.has_last_pulse==0u);

    assert(ppg_ibi_reset(&ctx)==PPG_IBI_STATUS_OK);
    x=s(3000,100,1); assert(ppg_ibi_process(&ctx,&x,&e)==PPG_IBI_STATUS_NO_EVENT);
    x=s(3020,250,1); assert(ppg_ibi_process(&ctx,&x,&e)==PPG_IBI_STATUS_NO_EVENT);
    x=s(3040,100,1); assert(ppg_ibi_process(&ctx,&x,&e)==PPG_IBI_STATUS_NO_EVENT);
    for(uint32_t ts=3060; ts<3340; ts+=20){ x=s(ts,100,1); assert(ppg_ibi_process(&ctx,&x,&e)==PPG_IBI_STATUS_NO_EVENT); }
    x=s(3340,280,1); assert(ppg_ibi_process(&ctx,&x,&e)==PPG_IBI_STATUS_NO_EVENT);
    x=s(3360,100,1); assert(ppg_ibi_process(&ctx,&x,&e)==PPG_IBI_STATUS_EVENT_READY);
    x=s(3380,100,1); assert(ppg_ibi_process(&ctx,&x,&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason!=PPG_IBI_REJECT_IBI_OUT_OF_RANGE);
    for(uint32_t ts=3400; ts<3660; ts+=20){ x=s(ts,100,1); assert(ppg_ibi_process(&ctx,&x,&e)==PPG_IBI_STATUS_NO_EVENT); }
    x=s(3660,290,1); assert(ppg_ibi_process(&ctx,&x,&e)==PPG_IBI_STATUS_NO_EVENT);
    x=s(3680,100,1);
    ppg_ibi_status_t st = ppg_ibi_process(&ctx,&x,&e);
    if(st!=PPG_IBI_STATUS_EVENT_READY){
        for(uint32_t ts=3700; ts<3980; ts+=20){ x=s(ts,100,1); assert(ppg_ibi_process(&ctx,&x,&e)==PPG_IBI_STATUS_NO_EVENT); }
        x=s(3980,300,1); assert(ppg_ibi_process(&ctx,&x,&e)==PPG_IBI_STATUS_NO_EVENT);
        x=s(4000,100,1); st = ppg_ibi_process(&ctx,&x,&e);
    }
    assert(st==PPG_IBI_STATUS_EVENT_READY);

    return 0;
}
