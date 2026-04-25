#include <math.h>

#include <dsp/assert.h>
#include <dsp/follow.h>
#include <dsp/utils.h>

static const float TLD_AUDIO_ENVELOPE_ANALOG_TC = -0.99967234081320612357829304641019f;

static inline float calculate_time_(float t_ms, float sr) {
    return expf(TLD_AUDIO_ENVELOPE_ANALOG_TC / (t_ms * sr * 0.000001f));
}

static inline void update_atk_time_(follow* self) {
    self->atk_time_ = calculate_time_(self->atk_ms, self->sr);
}

static inline void update_rel_time_(follow* self) {
    self->rel_time_ = calculate_time_(self->rel_ms, self->sr);
}

static inline float peak_tick_(follow* self, float xn) {
    float current_envelope = 0.0f;
    float last_envelope = self->last_envelope_;
    xn = fabsf(xn);

    if (xn > last_envelope) {
        current_envelope = self->atk_time_ * (last_envelope - xn) + xn;
    } else {
        current_envelope = self->rel_time_ * (last_envelope - xn) + xn;
    }
    current_envelope = check_float_underflow(current_envelope);
    current_envelope = dsp_max(current_envelope, 0.0f);

    self->last_envelope_ = current_envelope;
    return current_envelope;
}

static inline float rms_tick_(follow* self, float xn) {
    float current_envelope = 0.0f;
    float last_envelope = self->last_envelope_;

    xn = fabsf(xn);
    xn *= xn;  // scale rms;

    if (xn > last_envelope) {
        current_envelope = self->atk_time_ * (last_envelope - xn) + xn;
    } else {
        current_envelope = self->rel_time_ * (last_envelope - xn) + xn;
    }

    current_envelope = check_float_underflow(current_envelope);
    current_envelope = dsp_max(current_envelope, 0.0f);

    current_envelope = powf(current_envelope, 0.5f);  // sqrt for rms;
    self->last_envelope_ = current_envelope;
    return current_envelope;
}

void follow_init(follow* self, float atk_ms, float rel_ms, float sr) {
    self->atk_ms = atk_ms;
    self->rel_ms = rel_ms;
    self->sr = sr;
    self->last_envelope_ = 1e-9f;

    update_atk_time_(self);
    update_rel_time_(self);
}

void follow_rms_tick_block(follow* self,
                           float* out,
                           float* in,
                           float* atk_ms,
                           float* rel_ms,
                           uint32_t start,
                           uint32_t nsmps) {

    for (uint32_t i = start; i < nsmps; i++) {
        float atk_ms_ = atk_ms[i];
        float rel_ms_ = rel_ms[i];

        bool atk_ms_eq = check_float_equal(atk_ms_, self->atk_ms);
        bool rel_ms_eq = check_float_equal(rel_ms_, self->rel_ms);

        if (!atk_ms_eq) {
            self->atk_ms = atk_ms_;
            update_atk_time_(self);
        }

        if (!rel_ms_eq) {
            self->rel_ms = rel_ms_;
            update_rel_time_(self);
        }

        out[i] = rms_tick_(self, in[i]);
    }
}

void follow_peak_tick_block(follow* self,
                            float* out,
                            float* in,
                            float* atk_ms,
                            float* rel_ms,
                            uint32_t start,
                            uint32_t nsmps) {

    for (uint32_t i = start; i < nsmps; i++) {
        float atk_ms_ = atk_ms[i];
        float rel_ms_ = rel_ms[i];

        bool atk_ms_eq = check_float_equal(atk_ms_, self->atk_ms);
        bool rel_ms_eq = check_float_equal(rel_ms_, self->rel_ms);

        if (!atk_ms_eq) {
            self->atk_ms = atk_ms_;
            update_atk_time_(self);
        }

        if (!rel_ms_eq) {
            self->rel_ms = rel_ms_;
            update_rel_time_(self);
        }

        out[i] = peak_tick_(self, in[i]);
    }
}
