#include "ppg_ibi.h"
#include <assert.h>
#include "../src/ppg_ibi_internal.h"

static ppg_ibi_sample_t mk(uint32_t ts, int32_t v, uint8_t allow) {
    ppg_ibi_sample_t s; s.timestamp_ms = ts; s.ppg[0]=v; s.ppg[1]=100; s.ppg[2]=100; s.ppg[3]=100; s.allow_measure=allow; return s;
}

static void seed_first_pulse(ppg_ibi_context_t *ctx, ppg_ibi_event_t *e, uint32_t ts0) {
    assert(ppg_ibi_process(ctx, &(ppg_ibi_sample_t){ts0,{100,100,100,100},1}, e)==PPG_IBI_STATUS_NO_EVENT);
    assert(ppg_ibi_process(ctx, &(ppg_ibi_sample_t){ts0+20,{250,100,100,100},1}, e)==PPG_IBI_STATUS_NO_EVENT);
    assert(ppg_ibi_process(ctx, &(ppg_ibi_sample_t){ts0+40,{100,100,100,100},1}, e)==PPG_IBI_STATUS_NO_EVENT);
    assert(ctx->has_last_pulse==1u);
}

int main(void) {
    ppg_ibi_context_t ctx; ppg_ibi_event_t e;
    assert(ppg_ibi_init(&ctx,NULL)==PPG_IBI_STATUS_OK);

    /* INIT -> ACQUIRE */
    assert(ppg_ibi_process(&ctx,&(ppg_ibi_sample_t){1000,{100,100,100,100},1},&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(e.state==PPG_IBI_STATE_ACQUIRE);

    /* allow_measure=false -> HOLD */
    assert(ppg_ibi_process(&ctx,&(ppg_ibi_sample_t){1020,{100,100,100,100},0},&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(e.state==PPG_IBI_STATE_HOLD && e.reject_reason==PPG_IBI_REJECT_ALLOW_MEASURE_FALSE);

    /* HOLD -> REACQUIRE */
    assert(ppg_ibi_process(&ctx,&(ppg_ibi_sample_t){1040,{100,100,100,100},1},&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(e.state==PPG_IBI_STATE_REACQUIRE);

    assert(ppg_ibi_reset(&ctx)==PPG_IBI_STATUS_OK);
    seed_first_pulse(&ctx,&e,2000);
    ctx.state = PPG_IBI_STATE_TRACK;
    /* TRACK + allow=false strict reset */
    assert(ppg_ibi_process(&ctx,&(ppg_ibi_sample_t){2060,{100,100,100,100},0},&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(e.state==PPG_IBI_STATE_HOLD && e.reject_reason==PPG_IBI_REJECT_ALLOW_MEASURE_FALSE);
    assert(ctx.has_last_pulse==0u && ctx.has_prev_sample==0u && ctx.has_prev2_sample==0u);

    assert(ppg_ibi_reset(&ctx)==PPG_IBI_STATUS_OK);
    seed_first_pulse(&ctx,&e,3000);
    ctx.state = PPG_IBI_STATE_TRACK;
    /* TRACK + TIMESTAMP_GAP */
    assert(ppg_ibi_process(&ctx,&(ppg_ibi_sample_t){3055,{100,100,100,100},1},&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(e.state==PPG_IBI_STATE_REACQUIRE && e.reject_reason==PPG_IBI_REJECT_TIMESTAMP_GAP);
    assert(ctx.has_last_pulse==0u && ctx.has_prev_sample==0u && ctx.has_prev2_sample==0u);

    assert(ppg_ibi_reset(&ctx)==PPG_IBI_STATUS_OK);
    seed_first_pulse(&ctx,&e,4000);
    ctx.state = PPG_IBI_STATE_TRACK;
    /* TRACK + SAMPLE_DROP */
    assert(ppg_ibi_process(&ctx,&(ppg_ibi_sample_t){4085,{100,100,100,100},1},&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(e.state==PPG_IBI_STATE_REACQUIRE && e.reject_reason==PPG_IBI_REJECT_SAMPLE_DROP);
    assert(ctx.has_last_pulse==0u && ctx.has_prev_sample==0u && ctx.has_prev2_sample==0u);

    assert(ppg_ibi_reset(&ctx)==PPG_IBI_STATUS_OK);
    seed_first_pulse(&ctx,&e,5000);
    ctx.state = PPG_IBI_STATE_TRACK;
    /* TRACK + LOW_SIGNAL_QUALITY near saturation */
    assert(ppg_ibi_process(&ctx,&(ppg_ibi_sample_t){5060,{PPG_IBI_PPG_MAX_24BIT-500,PPG_IBI_PPG_MAX_24BIT-500,PPG_IBI_PPG_MAX_24BIT-500,PPG_IBI_PPG_MAX_24BIT-500},1},&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(e.state==PPG_IBI_STATE_REACQUIRE && e.reject_reason==PPG_IBI_REJECT_LOW_SIGNAL_QUALITY);
    assert(ctx.has_last_pulse==0u && ctx.has_prev_sample==0u && ctx.has_prev2_sample==0u);

    assert(ppg_ibi_reset(&ctx)==PPG_IBI_STATUS_OK);
    /* First candidate establishes last pulse, no event */
    assert(ppg_ibi_process(&ctx,&(ppg_ibi_sample_t){6000,{100,100,100,100},1},&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(ppg_ibi_process(&ctx,&(ppg_ibi_sample_t){6020,{250,100,100,100},1},&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(ppg_ibi_process(&ctx,&(ppg_ibi_sample_t){6040,{100,100,100,100},1},&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(ctx.has_last_pulse==1u);

    for (uint32_t t=6060; t<6320; t+=20) { ppg_ibi_sample_t x=mk(t,100,1); assert(ppg_ibi_process(&ctx,&x,&e)==PPG_IBI_STATUS_NO_EVENT); }
    assert(ppg_ibi_process(&ctx,&(ppg_ibi_sample_t){6320,{280,100,100,100},1},&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(ppg_ibi_process(&ctx,&(ppg_ibi_sample_t){6340,{100,100,100,100},1},&e)==PPG_IBI_STATUS_EVENT_READY);
    assert(e.state==PPG_IBI_STATE_TRACK && e.ibi_ms==300u && e.timestamp_ms==6320u);

    /* no immediate duplicate event */
    assert(ppg_ibi_process(&ctx,&(ppg_ibi_sample_t){6360,{100,100,100,100},1},&e)==PPG_IBI_STATUS_NO_EVENT);

    for (uint32_t t=6380; t<6640; t+=20) { ppg_ibi_sample_t x=mk(t,100,1); assert(ppg_ibi_process(&ctx,&x,&e)==PPG_IBI_STATUS_NO_EVENT); }
    assert(ppg_ibi_process(&ctx,&(ppg_ibi_sample_t){6640,{300,100,100,100},1},&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(ppg_ibi_process(&ctx,&(ppg_ibi_sample_t){6660,{100,100,100,100},1},&e)==PPG_IBI_STATUS_EVENT_READY);
    assert(e.timestamp_ms==6640u && e.sample_index==33u && e.state==PPG_IBI_STATE_TRACK);
    assert(e.selected_channel==0u && e.signal_quality==PPG_IBI_CHANNEL_QUALITY_BASIC_VALID && e.confidence==PPG_IBI_CHANNEL_QUALITY_BASIC_VALID);

    /* IBI out of range -> no event, non-TRACK */
    assert(ppg_ibi_process(&ctx,&(ppg_ibi_sample_t){6680,{100,100,100,100},1},&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(ppg_ibi_process(&ctx,&(ppg_ibi_sample_t){6700,{260,100,100,100},1},&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(ppg_ibi_process(&ctx,&(ppg_ibi_sample_t){6720,{100,100,100,100},1},&e)==PPG_IBI_STATUS_NO_EVENT);
    assert(e.reject_reason==PPG_IBI_REJECT_IBI_OUT_OF_RANGE);
    return 0;
}
