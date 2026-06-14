#ifndef INC_SPECTRUM_H_
#define INC_SPECTRUM_H_

#include "stm32f7xx_hal.h"
#include "audio_capture.h"

#define LCD_W               480
#define LCD_H               272

#define SPECTRUM_X0         10
#define SPECTRUM_Y_BOTTOM   255
#define SPECTRUM_MAX_H      210

#define NB_BARS             60
#define BAR_W               6
#define BAR_GAP             1

void Spectrum_Init(void);
void Spectrum_Process(int16_t *audio_data, uint16_t num_samples);

#endif
