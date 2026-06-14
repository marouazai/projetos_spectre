#include "spectrum.h"
#include "stm32746g_discovery_lcd.h"
#include "arm_math.h"
#include <math.h>
#include <string.h>

static arm_rfft_fast_instance_f32 fft_instance;
static float32_t fft_input[FFT_SIZE];
static float32_t fft_output[FFT_SIZE];
static float32_t fft_magnitude[FFT_SIZE / 2];
static float32_t bar_heights_prev[NB_BARS];
static float32_t hanning_window[FFT_SIZE];

static uint32_t bar_color(uint16_t bar_index)
{
    float ratio = (float)bar_index / (float)(NB_BARS - 1);
    uint8_t r = (uint8_t)(255.0f * ratio);
    uint8_t g = (uint8_t)(255.0f * (1.0f - ratio));
    return 0xFF000000 | ((uint32_t)r << 16) | ((uint32_t)g << 8);
}

void Spectrum_Init(void)
{
    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(0, 0xC0000000);
    BSP_LCD_SelectLayer(0);
    BSP_LCD_DisplayOn();
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_Clear(LCD_COLOR_BLACK);

    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(0, 5,
        (uint8_t *)"Analyseur Spectre Audio", CENTER_MODE);

    BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
    BSP_LCD_SetFont(&Font8);
    BSP_LCD_DisplayStringAt(SPECTRUM_X0,       SPECTRUM_Y_BOTTOM + 5,
                            (uint8_t *)"0",    LEFT_MODE);
    BSP_LCD_DisplayStringAt(SPECTRUM_X0 + 100, SPECTRUM_Y_BOTTOM + 5,
                            (uint8_t *)"2kHz", LEFT_MODE);
    BSP_LCD_DisplayStringAt(SPECTRUM_X0 + 200, SPECTRUM_Y_BOTTOM + 5,
                            (uint8_t *)"5kHz", LEFT_MODE);
    BSP_LCD_DisplayStringAt(SPECTRUM_X0 + 340, SPECTRUM_Y_BOTTOM + 5,
                            (uint8_t *)"10kHz", LEFT_MODE);

    arm_rfft_fast_init_f32(&fft_instance, FFT_SIZE);

    for (uint16_t i = 0; i < FFT_SIZE; i++)
    {
        hanning_window[i] = 0.5f * (1.0f - cosf(2.0f * (float)M_PI
                            * (float)i / (float)FFT_SIZE));
    }
    memset(bar_heights_prev, 0, sizeof(bar_heights_prev));
}

void Spectrum_Process(int16_t *audio_data, uint16_t num_samples)
{
    for (uint16_t i = 0; i < FFT_SIZE; i++)
    {
        float32_t sample = (float32_t)audio_data[i * 2] / 32768.0f;
        fft_input[i] = sample * hanning_window[i];
    }

    arm_rfft_fast_f32(&fft_instance, fft_input, fft_output, 0);
    arm_cmplx_mag_f32(&fft_output[2], fft_magnitude, FFT_SIZE / 2 - 1);

    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_FillRect(SPECTRUM_X0 - 1,
                     SPECTRUM_Y_BOTTOM - SPECTRUM_MAX_H - 1,
                     NB_BARS * (BAR_W + BAR_GAP) + 2,
                     SPECTRUM_MAX_H + 2);

    for (uint16_t b = 0; b < NB_BARS; b++)
    {
        uint16_t max_bin = (FFT_SIZE / 2) - 1;
        float ratio_start = powf((float)b / (float)NB_BARS, 1.8f);
        float ratio_end   = powf((float)(b + 1) / (float)NB_BARS, 1.8f);
        uint16_t bin_start = (uint16_t)(ratio_start * max_bin);
        uint16_t bin_end   = (uint16_t)(ratio_end * max_bin);
        if (bin_end <= bin_start) bin_end = bin_start + 1;

        float32_t sum = 0.0f;
        for (uint16_t k = bin_start; k < bin_end; k++)
            sum += fft_magnitude[k];
        float32_t avg = sum / (float32_t)(bin_end - bin_start);

        float32_t magnitude_db;
        if (avg > 0.0001f)
            magnitude_db = 20.0f * log10f(avg);
        else
            magnitude_db = -60.0f;

        float32_t height_f = (magnitude_db - (-60.0f))
                           / (0.0f - (-60.0f))
                           * (float)SPECTRUM_MAX_H * 3.0f;
        if (height_f < 0.0f) height_f = 0.0f;
        if (height_f > (float)SPECTRUM_MAX_H) height_f = (float)SPECTRUM_MAX_H;

        if (height_f > bar_heights_prev[b])
            bar_heights_prev[b] = 0.3f * bar_heights_prev[b] + 0.7f * height_f;
        else
            bar_heights_prev[b] = 0.85f * bar_heights_prev[b] + 0.15f * height_f;

        uint16_t bar_h = (uint16_t)bar_heights_prev[b];
        if (bar_h < 1) bar_h = 1;

        uint16_t x = SPECTRUM_X0 + b * (BAR_W + BAR_GAP);
        uint16_t y = SPECTRUM_Y_BOTTOM - bar_h;

        BSP_LCD_SetTextColor(bar_color(b));
        BSP_LCD_FillRect(x, y, BAR_W, bar_h);
    }
}
