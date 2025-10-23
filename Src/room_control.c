#include "room_control.h"

#include "gpio.h"    // Para controlar LEDs
#include "systick.h" // Para obtener ticks y manejar tiempos
#include "uart.h"    // Para enviar mensajes
#include "tim.h"     // Para controlar el PWM

// Estados de la sala
typedef enum {
    ROOM_IDLE,
    ROOM_OCCUPIED
} room_state_t;

// Variable de estado global
room_state_t current_state = ROOM_IDLE;
static uint32_t led_on_time = 10; // Tiempo en ms cuando el LED fue encendido
volatile uint8_t Duty_Cycle_Actual = 20;

void room_control_app_init(void)
{
    // Inicializar PWM al duty cycle inicial (estado IDLE -> LED apagado)
    tim3_ch1_pwm_set_duty_cycle(PWM_INITIAL_DUTY);
}

void room_control_on_button_press(void)
{
    if (current_state == ROOM_IDLE) {
        current_state = ROOM_OCCUPIED;
        tim3_ch1_pwm_set_duty_cycle(100);  // PWM al 100%
        led_on_time = systick_get_ms();// 
        uart_send_string("Controlador de la sala v2.0\r\n");
    } else {
        current_state = ROOM_IDLE;
        tim3_ch1_pwm_set_duty_cycle(20);  // PWM al 20%
        uart_send_string("Estado inicial\r\n");
    }
}

void room_control_on_uart_receive(char received_char)
{
    switch (received_char) {
        case 's':
        case 'S':
            uart_send(Duty_Cycle_Actual);
            uart_send_string("\r\n");
            break;
        case 'h':
        case 'H':
            tim3_ch1_pwm_set_duty_cycle(100);
            uart_send_string("PWM: 100%\r\n");
            break;
        case 'l':
        case 'L':
            tim3_ch1_pwm_set_duty_cycle(0);
            uart_send_string("PWM: 0%\r\n");
            break;
        case 'O':
        case 'o':
            current_state = ROOM_OCCUPIED;
            tim3_ch1_pwm_set_duty_cycle(100);
            led_on_time = systick_get_ms();
            uart_send_string("Sala ocupada\r\n");
            break;
        case 'I':
        case 'i':
            current_state = ROOM_IDLE;
            tim3_ch1_pwm_set_duty_cycle(0);
            uart_send_string("Sala vacía\r\n");
            break;
        case '1':
            tim3_ch1_pwm_set_duty_cycle(10);
            uart_send_string("PWM: 10%\r\n");
            break;
        case '2':
            tim3_ch1_pwm_set_duty_cycle(20);
            uart_send_string("PWM: 20%\r\n");
            break;
        case '3':
            tim3_ch1_pwm_set_duty_cycle(30);
            uart_send_string("PWM: 30%\r\n");
            break;
        case '4':
            tim3_ch1_pwm_set_duty_cycle(40);
            uart_send_string("PWM: 40%\r\n");
            break;
        case '5':
            tim3_ch1_pwm_set_duty_cycle(50);
            uart_send_string("PWM: 50%\r\n");
            break;
        case '?':
            uart_send_string("Comandos disponibles:\r\n");
            uart_send_string("'1'-'5': ajuste del brillo lámpara (10%, 20%, 30%, 40%, 50%) \r\n");
            uart_send_string(" '0': Apagar lámpara\r\n");
            uart_send_string(" 'o': abrir Puerta( Ocupar sala)\r\n");
            uart_send_string(" 'c': Cerrar puerta (Sala vacía)\r\n");
            uart_send_string(" 's': Estado del sistema \r\n");
            uart_send_string("  '?': ayuda\r\n");
            break;
        case 'g':
        case 'G':
            current_state = ROOM_OCCUPIED;
            tim3_ch1_pwm_set_duty_cycle(100);
            led_on_time = systick_get_ms();
            uart_send_string("Sala ocupada\r\n");
            break;

        default:
            uart_send_string("Comando desconocido: ");
            uart_send(received_char);
            uart_send_string("\r\n");
            break;
    }
}

void room_control_update(void) // Llamar periódicamente en el bucle principal
{
    if (current_state == ROOM_OCCUPIED) { // Solo verificar timeout si la sala está ocupada
        if (systick_get_ms() - led_on_time >= LED_TIMEOUT_MS) { // Timeout alcanzado
            current_state = ROOM_IDLE; // Cambiar a estado IDLE
            tim3_ch1_pwm_set_duty_cycle(0); // Apagar LED
            uart_send_string("Timeout: Sala vacía\r\n"); // Notificar por UART
        }
    }

}