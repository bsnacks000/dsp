/**
 * @file biquad implementation
 *
 * Parameters are updated at krate using the first sample index of the vector.
 */

#include <math.h>
#include <stdbool.h>

#include <dsp/bq.h>
#include <dsp/constants.h>
#include <dsp/utils.h>

// 0.3 - change to double precision
static inline float dfII_tick_(dfII* self, float xn) {

    double yn = (double) self->a0 * (double) xn + self->x_z1;

    self->x_z1 = (double) self->a1 * (double) xn - (double) self->b1 * yn + self->x_z2;
    self->x_z2 = (double) self->a2 * (double) xn - (double) self->b2 * yn;

    return self->d0 * xn + self->c0 * (float) yn;
}

void dfII_init(dfII* self, float sr) {
    // coeffs (a is the denominator)
    self->a0 = 0.0f;
    self->a1 = 0.0f;
    self->a2 = 0.0f;
    self->b1 = 0.0f;
    self->b2 = 0.0f;
    self->c0 = 1.0f;  // full wet -- reserved for use with shelving filters
    self->d0 = 0.0f;
    // state registers
    self->x_z1 = 0.0;
    self->x_z2 = 0.0;
    self->sr = fabsf(sr);
}

float dfII_tick(dfII* self, float xn) {
    return dfII_tick_(self, xn);
}

/**
 * 1st order design equations
 * - lpf, hpf, apf
 *
 */

static inline void first_order_lpf(dfII* self, float fc) {
    float theta_c = TWO_PI_F * fc / self->sr;
    float gamma = cosf(theta_c) / (1.0f + sinf(theta_c));

    self->a0 = (1.0f - gamma) / 2.0f;
    self->a1 = self->a0;
    self->b1 = -gamma;
}

static inline void first_order_hpf(dfII* self, float fc) {
    float theta_c = TWO_PI_F * fc / self->sr;
    float gamma = cosf(theta_c) / (1.0f + sinf(theta_c));

    self->a0 = (1.0f + gamma) / 2.0f;
    self->a1 = -self->a0;
    self->b1 = -gamma;
}

static inline void first_order_apf(dfII* self, float fc) {
    float a = tanf((float) PI_F * fc / self->sr);
    float alpha = (a - 1.0f) / (a + 1.0f);

    self->a0 = alpha;
    self->a1 = 1.0f;
    self->b1 = alpha;
}

/**
 * 2nd order butterworth / linkwitz-riley low pass filters.
 */

static inline void butterworth_lp(dfII* self, float fc) {
    float theta_c = PI_F * fc / self->sr;
    float c = 1.0f / tanf(theta_c);

    self->a0 = 1.0f / (1.0f + (float) SQRT_TWO * c + c * c);
    self->a1 = 2.0f * self->a0;
    self->a2 = self->a0;
    self->b1 = 2.0f * self->a0 * (1.0f - c * c);
    self->b2 = self->a0 * (1.0f - (float) SQRT_TWO * c + c * c);
}

static inline void linkwitz_riley_lp(dfII* self, float fc) {
    float omega_c = PI_F * fc;
    float theta_c = PI_F * fc / self->sr;

    float k = omega_c / tanf(theta_c);
    float d = k * k + omega_c * omega_c + 2.0f * k * omega_c;
    float b1_n = -2.0f * k * k + 2.0f * omega_c * omega_c;
    float b2_n = -2.0f * k * omega_c + k * k + omega_c * omega_c;

    self->a0 = omega_c * omega_c / d;
    self->a1 = 2.0f * omega_c * omega_c / d;
    self->a2 = self->a0;
    self->b1 = b1_n / d;
    self->b2 = b2_n / d;
}

static inline dfII_non_resonant_design_equation dfII_get_first_order_design_equation(
    bq_non_resonant_type t) {
    switch (t) {
        case BQ_FIRST_ORDER_LPF:
            return first_order_lpf;

        case BQ_FIRST_ORDER_HPF:
            return first_order_hpf;

        case BQ_FIRST_ORDER_APF:
            return first_order_apf;

        case BQ_BUTTERWORTH:
            return butterworth_lp;

        case BQ_LINKWITZ_RILEY:
            return linkwitz_riley_lp;  // TODO:

        default:  // unreachable
            return first_order_lpf;
    }
}

/**
 * 2nd order design equations
 *
 */

static void resonant_lpf(dfII* self, float fc, float q) {
    float theta_c = TWO_PI_F * fc / self->sr;
    float d = 1.0f / q;

    float d2 = ((d / 2.0f) * (sinf(theta_c)));

    float beta = 0.5f * ((1.0f - d2) / (1.0f + d2));
    float gamma = (0.5f + beta) * cosf(theta_c);
    float alpha = (0.5f + beta - gamma) / 2.0f;

    self->a0 = alpha;
    self->a1 = 2.0f * alpha;
    self->a2 = alpha;
    self->b1 = -2.0f * gamma;
    self->b2 = 2.0f * beta;
}

static void resonant_hpf(dfII* self, float fc, float q) {
    float theta_c = TWO_PI_F * fc / self->sr;
    float d = 1.0f / q;
    float d2 = ((d / 2.0f) * (sinf(theta_c)));

    float beta = 0.5f * ((1.0f - d2) / (1.0f + d2));
    float gamma = (0.5f + beta) * cosf(theta_c);
    float alpha = (0.5f + beta + gamma) / 2.0f;

    self->a0 = alpha;
    self->a1 = -2.0f * alpha;
    self->a2 = alpha;
    self->b1 = -2.0f * gamma;
    self->b2 = 2.0f * beta;
}

static void resonant_bpf(dfII* self, float fc, float q) {
    float k = tanf(PI_F * fc / self->sr);
    float delta = k * k * q + k + q;

    self->a0 = k / delta;
    self->a1 = 0.0f;
    self->a2 = -k / delta;
    self->b1 = 2.0f * q * (k * k - 1.0f) / delta;
    self->b2 = (k * k * q - k + q) / delta;
}

static void resonant_bsf(dfII* self, float fc, float q) {
    float k = tanf(PI_F * fc / self->sr);
    float delta = k * k * q + k + q;

    self->a0 = q * (1.0f + k * k) / delta;
    self->a1 = 2.0f * q * (k * k - 1.0f) / delta;
    self->a2 = self->a0;
    self->b1 = self->a1;
    self->b2 = (k * k * q - k + q) / delta;
}

static void resonant_reson(dfII* self, float fc, float q) {
    float theta_c = TWO_PI_F * fc / self->sr;
    float bw = fc / q;

    float b2 = expf(-TWO_PI_F * (bw / self->sr));
    float b1 = ((-4.0f * b2) / (1.0f + b2)) * cosf(theta_c);
    float a0 = 1.0f - powf(b2, 0.5f);

    self->a0 = a0;
    self->a1 = 0.0f;
    self->a2 = -a0;
    self->b1 = b1;
    self->b2 = b2;
}

static void resonant_apf(dfII* self, float fc, float q) {
    float theta_c = TWO_PI_F * fc / self->sr;
    float bw = fc / q;

    float x = PI_F * bw / self->sr;
    if (x >= TANGENT_THRESHOLD_F)
        x = TANGENT_THRESHOLD_F;

    float tan_x = tanf(x);
    float alpha = (tan_x - 1.0f) / (tan_x + 1.0f);
    float beta = -cosf(theta_c);

    self->a0 = -alpha;
    self->a1 = beta * (1.0f - alpha);
    self->a2 = 1.0f;
    self->b1 = beta * (1.0f - alpha);
    self->b2 = -alpha;
}

static dfII_resonant_design_equation dfII_get_resonant_design_equation(
    bq_resonant_type t) {
    switch (t) {
        case BQ_SECOND_ORDER_LPF:
            return resonant_lpf;

        case BQ_SECOND_ORDER_HPF:
            return resonant_hpf;

        case BQ_SECOND_ORDER_BPF:
            return resonant_bpf;

        case BQ_SECOND_ORDER_BSF:
            return resonant_bsf;

        case BQ_SECOND_ORDER_RESON:
            return resonant_reson;

        case BQ_SECOND_ORDER_APF:
            return resonant_apf;
        default:
            return resonant_lpf;
    }
}

/**
 * parametric design equations
 *
 * NOTE: that A uses a scaling factor of 40 from db to linear. Normal Amplitude
 * scaling does not work with these audio eq cookbook equations.
 *
 */

static void lshlf(dfII* self, float fc, float q, float gain_db) {
    float A = powf(10.0f, gain_db / 40.0f);
    float omega0 = TWO_PI_F * fc / self->sr;
    float cos_omega0 = cosf(omega0);
    float sin_omega0 = sinf(omega0);

    float alpha = sin_omega0 * sqrtf((A * A + 1.0f) * (1.0f / q - 1.0f) + 2.0f * A);

    float sqrt_A = sqrtf(A);

    float b0 = A * ((A + 1.0f) - (A - 1.0f) * cos_omega0 + 2.0f * sqrt_A * alpha);
    float b1 = 2.0f * A * ((A - 1.0f) - (A + 1.0f) * cos_omega0);
    float b2 = A * ((A + 1.0f) - (A - 1.0f) * cos_omega0 - 2.0f * sqrt_A * alpha);
    float a0 = (A + 1.0f) + (A - 1.0f) * cos_omega0 + 2.0f * sqrt_A * alpha;
    float a1 = -2.0f * ((A - 1.0f) + (A + 1.0f) * cos_omega0);
    float a2 = (A + 1.0f) + (A - 1.0f) * cos_omega0 - 2.0f * sqrt_A * alpha;

    self->a0 = a0 / b0;
    self->a1 = a1 / b0;
    self->a2 = a2 / b0;
    self->b1 = b1 / b0;
    self->b2 = b2 / b0;
}

static void hshlf(dfII* self, float fc, float q, float gain_db) {
    float A = powf(10.0f, gain_db / 40.0f);
    float omega0 = TWO_PI_F * fc / self->sr;
    float cos_omega0 = cosf(omega0);
    float sin_omega0 = sinf(omega0);

    float alpha = sin_omega0 * sqrtf((A * A + 1.0f) * (1.0f / q - 1.0f) + 2.0f * A);

    float sqrt_A = sqrtf(A);

    float b0 = A * ((A + 1.0f) + (A - 1.0f) * cos_omega0 + 2.0f * sqrt_A * alpha);
    float b1 = -2.0f * A * ((A - 1.0f) + (A + 1.0f) * cos_omega0);
    float b2 = A * ((A + 1.0f) + (A - 1.0f) * cos_omega0 - 2.0f * sqrt_A * alpha);
    float a0 = (A + 1.0f) - (A - 1.0f) * cos_omega0 + 2.0f * sqrt_A * alpha;
    float a1 = 2.0f * ((A - 1.0f) - (A + 1.0f) * cos_omega0);
    float a2 = (A + 1.0f) - (A - 1.0f) * cos_omega0 - 2.0f * sqrt_A * alpha;

    self->a0 = a0 / b0;
    self->a1 = a1 / b0;
    self->a2 = a2 / b0;
    self->b1 = b1 / b0;
    self->b2 = b2 / b0;
}

static void peak(dfII* self, float fc, float q, float gain_db) {
    float A = powf(10.0f, gain_db / 40.0f);
    float omega0 = TWO_PI_F * fc / self->sr;

    float alpha = sinf(omega0) / (2.0f * q);

    float a0 = 1.0f + (alpha * A);
    float a1 = -2.0f * cosf(omega0);
    float a2 = 1.0f - (alpha * A);
    float b0 = 1.0f + (alpha / A);
    float b1 = -2.0f * cosf(omega0);
    float b2 = 1.0f - (alpha / A);

    self->a0 = a0 / b0;
    self->a1 = a1 / b0;
    self->a2 = a2 / b0;
    self->b1 = b1 / b0;
    self->b2 = b2 / b0;
}

static dfII_para_eq_design_equation dfII_get_para_eq_design_equation(
    bq_para_eq_type t) {
    switch (t) {
        case BQ_PARA_LSHLF:
            return lshlf;
        case BQ_PARA_HSHLF:
            return hshlf;
        case BQ_PARA_PEAK:
            return peak;
        default:
            return lshlf;
    }
}

// non-resonant filters
//

void bq_non_resonant_init(bq_non_resonant* self,
                          bq_non_resonant_type t,
                          float freq,
                          float sr) {
    self->freq = freq;
    self->t = t;
    self->sr = fabsf(sr);

    // set state
    dfII state;
    dfII_init(&state, self->sr);
    self->state_ = state;

    // set design equation
    self->update_ = dfII_get_first_order_design_equation(t);
    // TODO: assert not null

    self->update_(&self->state_, freq);
}

void bq_non_resonant_tick_block(bq_non_resonant* self,
                                float* out,
                                float* in,
                                float freq,
                                uint32_t start,
                                uint32_t nsmps) {

    if (!check_float_equal(freq, self->freq)) {
        self->freq = freq;
        self->update_(&self->state_, freq);
    }
    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = dfII_tick(&self->state_, in[i]);
    }
}

// resonant filters
//

void bq_resonant_init(bq_resonant* self,
                      bq_resonant_type t,
                      float freq,
                      float q,
                      float sr) {
    self->sr = fabsf(sr);
    self->t = t;
    self->freq = freq;
    self->q = q;

    dfII state;
    dfII_init(&state, self->sr);

    self->state_ = state;
    self->update_ = dfII_get_resonant_design_equation(t);

    self->update_(&self->state_, freq, q);
}

void bq_resonant_tick_block(bq_resonant* self,
                            float* out,
                            float* in,
                            float freq,
                            float q,
                            uint32_t start,
                            uint32_t nsmps) {

    bool freq_eq = check_float_equal(freq, self->freq);
    bool q_eq = check_float_equal(q, self->q);

    if (!freq_eq || !q_eq) {
        self->freq = freq;
        self->q = q;
        self->update_(&self->state_, freq, q);
    }

    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = dfII_tick(&self->state_, in[i]);
    }
}

// --- parametric impl
//

void bq_para_eq_init(bq_para_eq* self,
                     bq_para_eq_type t,
                     float freq,
                     float q,
                     float gain,
                     float sr) {

    self->t = t;
    self->freq = freq;
    self->q = q;
    self->gain = gain;
    self->sr = fabsf(sr);

    dfII state;
    dfII_init(&state, self->sr);
    self->state_ = state;

    self->update_ = dfII_get_para_eq_design_equation(t);

    self->update_(&self->state_, freq, q, gain);
}

void bq_para_eq_tick_block(bq_para_eq* self,
                           float* out,
                           float* in,
                           float freq,
                           float q,
                           float gain,
                           uint32_t start,
                           uint32_t nsmps) {

    bool freq_eq = check_float_equal(freq, self->freq);
    bool q_eq = check_float_equal(q, self->q);
    bool gain_eq = check_float_equal(gain, self->gain);

    if (!freq_eq || !q_eq || !gain_eq) {
        self->freq = freq;
        self->q = q;
        self->gain = gain;
        self->update_(&self->state_, freq, q, gain);
    }

    for (uint32_t i = start; i < nsmps; i++) {
        out[i] = dfII_tick(&self->state_, in[i]);
    }
}
