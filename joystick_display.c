#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"

#define BTN_A 5
#define LED_VERDE 11
#define LED_AZUL 12
#define LED_VERMELHO 13
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C
#define JOYSTICK_BTN 22
#define JOYSTICK_X_PIN 26
#define JOYSTICK_Y_PIN 27
#define QUADRADO 8

ssd1306_t ssd; // Inicializa a estrutura do display
uint16_t adc_value_x, adc_value_y; // Guardam os valores dos eixos.
uint slice_azul;

// **Variáveis globais de estado**
volatile bool leds_ligados = true;  // Controla apenas LEDs RGB (Vermelho e Azul)
volatile bool led_verde_ligado = false;  // Controla apenas o LED Verde
volatile bool borda_visivel = false;  // Controla a borda do display

// Interrupção para os botões
void botao_irq(uint gpio, uint32_t events) {
    static uint32_t ultima_acao = 0;
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());

    if (tempo_atual - ultima_acao > 200) {  // Debounce de 200ms
        if (gpio == BTN_A) {
            leds_ligados = !leds_ligados;  // Alterna apenas os LEDs RGB
        } else if (gpio == JOYSTICK_BTN) {
            led_verde_ligado = !led_verde_ligado;  // Alterna o LED Verde
            borda_visivel = !borda_visivel;  // Alterna a borda do display
            gpio_put(LED_VERDE, led_verde_ligado);  // Atualiza o LED Verde
        }
        ultima_acao = tempo_atual;
    }
}

// Função para desenhar o quadrado no display
void desenhar_quadrado(int x, int y) {
    ssd1306_fill(&ssd, false);

    // Se a borda estiver ativada, desenha no display
    if (borda_visivel) {
        ssd1306_rect(&ssd, 0, 0, 127, 63, true, false);
    }

    // Desenha o quadrado móvel
    ssd1306_rect(&ssd, x, y, QUADRADO, QUADRADO, true, true);

    ssd1306_send_data(&ssd);
}

// Função para controlar a intensidade dos LEDs RGB
void intensidade_leds(int x, int y) {
    if (leds_ligados) {
        // Calcula a intensidade baseada na proximidade das extremidades
        int distancia_x = abs(x - 2048);  // Centro do ADC (12 bits)
        int distancia_y = abs(y - 2048);

        // Mapeia a distância para o range PWM (0-65535)
        uint16_t duty_vermelho = (distancia_x * 65535) / 2048;
        uint16_t duty_azul = (distancia_y * 65535) / 2048;

        // Define o duty cycle do PWM
        pwm_set_chan_level(slice_azul, PWM_CHAN_A, duty_azul);    // LED Azul (Y)
        pwm_set_chan_level(slice_azul, PWM_CHAN_B, duty_vermelho); // LED Vermelho (X)
    } else {
        // Se os LEDs estiverem desligados, zera apenas os LEDs RGB
        pwm_set_chan_level(slice_azul, PWM_CHAN_A, 0);
        pwm_set_chan_level(slice_azul, PWM_CHAN_B, 0);
    }
}

int main() {
    stdio_init_all();

    // Configuração do Botão A
    gpio_init(BTN_A);
    gpio_set_dir(BTN_A, GPIO_IN);
    gpio_pull_up(BTN_A);

    // Configuração do Botão do Joystick
    gpio_init(JOYSTICK_BTN);
    gpio_set_dir(JOYSTICK_BTN, GPIO_IN);
    gpio_pull_up(JOYSTICK_BTN);

    // Ativar Interrupção para ambos os botões
    gpio_set_irq_enabled_with_callback(BTN_A, GPIO_IRQ_EDGE_FALL, true, &botao_irq);
    gpio_set_irq_enabled_with_callback(JOYSTICK_BTN, GPIO_IRQ_EDGE_FALL, true, &botao_irq);

    // Configuração do I2C para o Display SSD1306
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&ssd, 128, 64, false, ENDERECO, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Configuração do ADC
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);

    // Configuração do LED Verde
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_put(LED_VERDE, 0);  // Começa desligado

    // Configuração do PWM para os LEDs RGB
    gpio_set_function(LED_AZUL, GPIO_FUNC_PWM);
    gpio_set_function(LED_VERMELHO, GPIO_FUNC_PWM);

    slice_azul = pwm_gpio_to_slice_num(LED_AZUL);
    pwm_set_enabled(slice_azul, true);

    while (true) {
        adc_select_input(0);  // Eixo X
        adc_value_x = adc_read();
        adc_select_input(1);  // Eixo Y
        adc_value_y = adc_read();

        desenhar_quadrado(adc_value_x / 72, adc_value_y / 34);
        intensidade_leds(adc_value_x, adc_value_y);

        sleep_ms(50);  // Reduz a taxa de atualização para melhor performance
    }
}
