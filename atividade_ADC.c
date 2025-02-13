#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"

#include "include/ssd1306.h"
#include "include/font.h"

static volatile uint32_t last_time_a = 0;
static volatile uint32_t last_time_sel = 0;
static volatile bool flag_led = true;
static volatile bool flag_display = false;
static volatile uint8_t border_style = 0; // Estilo da borda: 0 = sem borda, 1 = borda simples, 2 = borda tracejada

// ---------------- Defines - Início ----------------

#define JSK_AJUST_MAX 2197
#define JSK_AJUST_MIN 1897

#define WRAP_VALUE 4095
#define DIV_VALUE 1.0

#define BUTTON_A 5

#define GREEN_LED 11
#define BLUE_LED 12
#define RED_LED 13

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C

#define JSK_SEL 22
#define JSK_X 26
#define JSK_Y 27

#define CALC_PWM(value) ((value) * WRAP_VALUE / 4095) // Correção do cálculo do PWM

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

// Função para mapear valores
#define MAP(value, in_min, in_max, out_min, out_max) \
    ((value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)

// ---------------- Inicializações - Início ----------------

void init_button() {
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
}

void init_led_pwm(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(pin);
    pwm_set_clkdiv(slice, DIV_VALUE);
    pwm_set_wrap(slice, WRAP_VALUE);
    pwm_set_gpio_level(pin, 0);
    pwm_set_enabled(slice, true);
}

void init_rgb() {
    gpio_init(GREEN_LED);
    gpio_set_dir(GREEN_LED, GPIO_OUT);
    gpio_put(GREEN_LED, 0);

    init_led_pwm(BLUE_LED);
    init_led_pwm(RED_LED);
}

void init_display(ssd1306_t *ssd) {
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(ssd, DISPLAY_WIDTH, DISPLAY_HEIGHT, false, ENDERECO, I2C_PORT);
    ssd1306_config(ssd);
    ssd1306_send_data(ssd);

    ssd1306_fill(ssd, false);
    ssd1306_send_data(ssd);
}

void init_joystick() {
    gpio_init(JSK_SEL);
    gpio_set_dir(JSK_SEL, GPIO_IN);
    gpio_pull_up(JSK_SEL);

    adc_init();
    adc_gpio_init(JSK_X);
    adc_gpio_init(JSK_Y);
}

// ---------------- Inicializações - Fim ----------------

uint16_t read_x() {
    adc_select_input(0);
    return adc_read();
}

uint16_t read_y() {
    adc_select_input(1);
    return adc_read();
}

// Função de debounce para os botões
int64_t debounce_callback(alarm_id_t id, void *user_data) {
    volatile bool *flag = (volatile bool *)user_data;
    *flag = !(*flag);
    return 0; // Não repetir o alarme
}

void update_border(ssd1306_t *ssd) {
    switch (border_style) {
        case 0:
            // Sem borda
            ssd1306_rect(ssd, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, false, false);
            break;
        case 1:
            // Borda simples
            ssd1306_rect(ssd, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, true, false);
            break;
        case 2:
            // Borda tracejada
            for (int i = 0; i < DISPLAY_WIDTH; i += 4) {
                ssd1306_pixel(ssd, i, 0, true);
                ssd1306_pixel(ssd, i, DISPLAY_HEIGHT - 1, true);
            }
            for (int i = 0; i < DISPLAY_HEIGHT; i += 4) {
                ssd1306_pixel(ssd, 0, i, true);
                ssd1306_pixel(ssd, DISPLAY_WIDTH - 1, i, true);
            }
            break;
    }
    ssd1306_send_data(ssd);
}

void gpio_irq_callback(uint gpio, uint32_t events) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    if ((gpio == BUTTON_A) && (current_time - last_time_a > 200)) {
        last_time_a = current_time;
        add_alarm_in_ms(5, debounce_callback, (void *)&flag_led, false);
    }
    if ((gpio == JSK_SEL) && (current_time - last_time_sel > 200)) {
        last_time_sel = current_time;
        add_alarm_in_ms(5, debounce_callback, (void *)&flag_display, false);
        gpio_put(GREEN_LED, !gpio_get(GREEN_LED));

        // Alterar o estilo da borda
        border_style = (border_style + 1) % 3;
    }
}

int main() {
    uint16_t x_value, y_value;
    uint8_t pos_x = 3, pos_y = 3;
    ssd1306_t ssd;
    uint32_t current_print_time, last_print_time = 0;

    stdio_init_all();
    init_button();
    init_rgb();
    init_display(&ssd);
    init_joystick();

    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_callback);
    gpio_set_irq_enabled_with_callback(JSK_SEL, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_callback);

    static uint8_t last_pos_x = 0, last_pos_y = 0;

    while (1) {
        sleep_ms(100);

        x_value = read_x();
        y_value = read_y();

        if (flag_led) {
            pwm_set_gpio_level(BLUE_LED, CALC_PWM(x_value));
            pwm_set_gpio_level(RED_LED, CALC_PWM(y_value));
        } else {
            pwm_set_gpio_level(BLUE_LED, 0);
            pwm_set_gpio_level(RED_LED, 0);
        }

        pos_x = MAP(x_value, 0, 4095, 3, DISPLAY_WIDTH - 8);
        pos_y = MAP(y_value, 0, 4095, 3, DISPLAY_HEIGHT - 8);

        if (pos_x != last_pos_x || pos_y != last_pos_y || flag_display) {
            last_pos_x = pos_x;
            last_pos_y = pos_y;

            ssd1306_fill(&ssd, false);
            ssd1306_rect(&ssd, pos_x, pos_y, 8, 8, true, true);
            update_border(&ssd); // Atualizar a borda
        }

        current_print_time = to_ms_since_boot(get_absolute_time());
        if ((current_print_time - last_print_time) >= 200) {
            last_print_time = current_print_time;
            printf("X: %u Y: %u ", y_value, x_value);
            printf("POS X: %u POS Y: %u\n", pos_y, pos_x);
        }
    }
    return 0;
}
