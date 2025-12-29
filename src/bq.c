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

/**
 * dII impl
 */

// private
static inline float dfII_tick_(dfII* self, float xn) {
    float yn = (self->a0 * xn) + self->x_z1;
    yn = check_float_underflow(yn);

    self->x_z1 = (self->a1 * xn) - (self->b1 * yn) + self->x_z2;
    self->x_z2 = (self->a2 * xn) - (self->b2 * yn);

    return self->d0 * xn + self->c0 * yn;
}

// public

void dfII_init(dfII* self, float sr) {
    // coeffs (a is the denominator)
    self->a0 = 0.0;
    self->a1 = 0.0;
    self->a2 = 0.0;
    self->b1 = 0.0;
    self->b2 = 0.0;
    self->c0 = 1.0;  // full wet -- reserved for use with shelving filters
    self->d0 = 0.0;
    // state registers
    self->x_z1 = 0.0;
    self->x_z2 = 0.0;
    self->sr = fabs(sr);
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
    float theta_c = TWO_PI * fc / self->sr;
    float gamma = cosf(theta_c) / (1.0 + sinf(theta_c));

    self->a0 = (1.0 - gamma) / 2.0;
    self->a1 = self->a0;
    self->b1 = -gamma;
}

static inline void first_order_hpf(dfII* self, float fc) {
    float theta_c = TWO_PI * fc / self->sr;
    float gamma = cosf(theta_c) / (1.0 + sinf(theta_c));

    self->a0 = (1.0 + gamma) / 2.0;
    self->a1 = -self->a0;
    self->b1 = -gamma;
}

static inline void first_order_apf(dfII* self, float fc) {
    float a = tanf(DSP_PI * fc / self->sr);
    float alpha = (a - 1.0) / (a + 1.0);

    self->a0 = alpha;
    self->a1 = 1.0;
    self->b1 = alpha;
}

/**
 * 2nd order butterworth / linkwitz-riley low pass filters.
 */

static inline void butterworth_lp(dfII* self, float fc) {
    float theta_c = DSP_PI * fc / self->sr;
    float c = 1.0 / tanf(theta_c);

    self->a0 = 1.0 / (1.0 + SQRT_TWO * c + c * c);
    self->a1 = 2.0 * self->a0;
    self->a2 = self->a0;
    self->b1 = 2.0 * self->a0 * (1.0 - c * c);
    self->b2 = self->a0 * (1.0 - SQRT_TWO * c + c * c);
}

static inline void linkwitz_riley_lp(dfII* self, float fc) {
    float omega_c = DSP_PI * fc;
    float theta_c = DSP_PI * fc / self->sr;

    float k = omega_c / tanf(theta_c);
    float d = k * k + omega_c * omega_c + 2.0 * k * omega_c;
    float b1_n = -2.0 * k * k + 2.0 * omega_c * omega_c;
    float b2_n = -2.0 * k * omega_c + k * k + omega_c * omega_c;

    self->a0 = omega_c * omega_c / d;
    self->a1 = 2.0 * omega_c * omega_c / d;
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
    float theta_c = TWO_PI * fc / self->sr;
    float d = 1.0 / q;

    float d2 = ((d / 2.0) * (sinf(theta_c)));

    float beta = 0.5 * ((1.0 - d2) / (1.0 + d2));
    float gamma = (0.5 + beta) * cosf(theta_c);
    float alpha = (0.5 + beta - gamma) / 2.0;

    self->a0 = alpha;
    self->a1 = 2.0 * alpha;
    self->a2 = alpha;
    self->b1 = -2.0 * gamma;
    self->b2 = 2.0 * beta;
}

static void resonant_hpf(dfII* self, float fc, float q) {
    float theta_c = TWO_PI * fc / self->sr;
    float d = 1.0 / q;
    float d2 = ((d / 2.0) * (sinf(theta_c)));

    float beta = 0.5 * ((1.0 - d2) / (1.0 + d2));
    float gamma = (0.5 + beta) * cosf(theta_c);
    float alpha = (0.5 + beta + gamma) / 2.0;

    self->a0 = alpha;
    self->a1 = -2.0 * alpha;
    self->a2 = alpha;
    self->b1 = -2.0 * gamma;
    self->b2 = 2.0 * beta;
}

static void resonant_bpf(dfII* self, float fc, float q) {
    float k = tanf(DSP_PI * fc / self->sr);
    float delta = k * k * q + k + q;

    self->a0 = k / delta;
    self->a1 = 0.0;
    self->a2 = -k / delta;
    self->b1 = 2.0 * q * (k * k - 1.0) / delta;
    self->b2 = (k * k * q - k + q) / delta;
}

static void resonant_bsf(dfII* self, float fc, float q) {
    float k = tanf(DSP_PI * fc / self->sr);
    float delta = k * k * q + k + q;

    self->a0 = q * (1.0 + k * k) / delta;
    self->a1 = 2.0 * q * (k * k - 1.0) / delta;
    self->a2 = self->a0;
    self->b1 = self->a1;
    self->b2 = (k * k * q - k + q) / delta;
}

static void resonant_reson(dfII* self, float fc, float q) {
    float theta_c = TWO_PI * fc / self->sr;
    float bw = fc / q;

    float b2 = expf(-TWO_PI * (bw / self->sr));
    float b1 = ((-4.0 * b2) / (1.0 + b2)) * cosf(theta_c);
    float a0 = 1.0 - powf(b2, 0.5);

    self->a0 = a0;
    self->a1 = 0.0;
    self->a2 = -a0;
    self->b1 = b1;
    self->b2 = b2;
}

static void resonant_apf(dfII* self, float fc, float q) {
    float theta_c = TWO_PI * fc / self->sr;
    float bw = fc / q;

    float x = DSP_PI * bw / self->sr;
    if (x >= TANGENT_THRESHOLD)
        x = TANGENT_THRESHOLD;

    float tan_x = tanf(x);
    float alpha = (tan_x - 1.0) / (tan_x + 1.0);
    float beta = -cosf(theta_c);

    self->a0 = -alpha;
    self->a1 = beta * (1.0 - alpha);
    self->a2 = 1.0;
    self->b1 = beta * (1.0 - alpha);
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
    float A = powf(10.0, gain_db / 40.0);
    float omega0 = TWO_PI * fc / self->sr;
    float cos_omega0 = cosf(omega0);
    float sin_omega0 = sinf(omega0);

    float alpha = sin_omega0 * sqrtf((A * A + 1.0) * (1.0 / q - 1.0) + 2.0 * A);

    float sqrt_A = sqrtf(A);

    float b0 = A * ((A + 1.0) - (A - 1.0) * cos_omega0 + 2.0 * sqrt_A * alpha);
    float b1 = 2.0 * A * ((A - 1.0) - (A + 1.0) * cos_omega0);
    float b2 = A * ((A + 1.0) - (A - 1.0) * cos_omega0 - 2.0 * sqrt_A * alpha);
    float a0 = (A + 1.0) + (A - 1.0) * cos_omega0 + 2.0 * sqrt_A * alpha;
    float a1 = -2.0 * ((A - 1.0) + (A + 1.0) * cos_omega0);
    float a2 = (A + 1.0) + (A - 1.0) * cos_omega0 - 2.0 * sqrt_A * alpha;

    self->a0 = a0 / b0;
    self->a1 = a1 / b0;
    self->a2 = a2 / b0;
    self->b1 = b1 / b0;
    self->b2 = b2 / b0;
}

static void hshlf(dfII* self, float fc, float q, float gain_db) {
    float A = powf(10.0, gain_db / 40.0);
    float omega0 = TWO_PI * fc / self->sr;
    float cos_omega0 = cosf(omega0);
    float sin_omega0 = sinf(omega0);

    float alpha = sin_omega0 * sqrtf((A * A + 1) * (1.0 / q - 1.0) + 2.0 * A);

    float sqrt_A = sqrtf(A);

    float b0 = A * ((A + 1.0) + (A - 1.0) * cos_omega0 + 2.0 * sqrt_A * alpha);
    float b1 = -2.0 * A * ((A - 1.0) + (A + 1.0) * cos_omega0);
    float b2 = A * ((A + 1.0) + (A - 1.0) * cos_omega0 - 2.0 * sqrt_A * alpha);
    float a0 = (A + 1.0) - (A - 1.0) * cos_omega0 + 2.0 * sqrt_A * alpha;
    float a1 = 2.0 * ((A - 1.0) - (A + 1.0) * cos_omega0);
    float a2 = (A + 1.0) - (A - 1.0) * cos_omega0 - 2.0 * sqrt_A * alpha;

    self->a0 = a0 / b0;
    self->a1 = a1 / b0;
    self->a2 = a2 / b0;
    self->b1 = b1 / b0;
    self->b2 = b2 / b0;
}

static void peak(dfII* self, float fc, float q, float gain_db) {
    float A = powf(10.0, gain_db / 40.0);
    float omega0 = TWO_PI * fc / self->sr;

    float alpha = sinf(omega0) / (2.0 * q);

    float a0 = 1.0 + (alpha * A);
    float a1 = -2.0 * cosf(omega0);
    float a2 = 1.0 - (alpha * A);
    float b0 = 1.0 + (alpha / A);
    float b1 = -2.0 * cosf(omega0);
    float b2 = 1.0 - (alpha / A);

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
    self->sr = fabs(sr);

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
    self->sr = fabs(sr);
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
    self->sr = fabs(sr);

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
