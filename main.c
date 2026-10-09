#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

void _init(void)
{
}

/* =========================================================
 * LED configuration
 * ========================================================= */

typedef struct
{
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
    uint32_t toggle_delay_ms;
} LED_TaskConfig_t;


/* =========================================================
 * Function prototypes
 * ========================================================= */

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

static void LED_Task(void *argument);
void Error_Handler(void);


/* =========================================================
 * LED task configurations
 * ========================================================= */

/*
 * Tần số nhấp nháy:
 *
 * f = 0.1 Hz -> chu kỳ = 10 s
 * Toggle mỗi 5 s
 *
 * f = 1 Hz -> chu kỳ = 1 s
 * Toggle mỗi 500 ms
 *
 * f = 10 Hz -> chu kỳ = 100 ms
 * Toggle mỗi 50 ms
 */

LED_TaskConfig_t LED1_Config =
{
    GPIOA,
    GPIO_PIN_0,
    5000
};

LED_TaskConfig_t LED2_Config =
{
    GPIOA,
    GPIO_PIN_1,
    500
};

LED_TaskConfig_t LED3_Config =
{
    GPIOA,
    GPIO_PIN_2,
    50
};


/* =========================================================
 * Main
 * ========================================================= */

int main(void)
{
    /* Initialize HAL */
    HAL_Init();

    /* Configure system clock */
    SystemClock_Config();

    /* Initialize GPIO */
    MX_GPIO_Init();


    /* Create LED1 task - 0.1 Hz */
    xTaskCreate(
        LED_Task,
        "LED1",
        128,
        &LED1_Config,
        1,
        NULL
    );


    /* Create LED2 task - 1 Hz */
    xTaskCreate(
        LED_Task,
        "LED2",
        128,
        &LED2_Config,
        1,
        NULL
    );


    /* Create LED3 task - 10 Hz */
    xTaskCreate(
        LED_Task,
        "LED3",
        128,
        &LED3_Config,
        1,
        NULL
    );


    /* Start FreeRTOS scheduler */
    vTaskStartScheduler();


    /* Should never reach here */
    while (1)
    {
    }
}


/* =========================================================
 * LED Task
 * ========================================================= */

static void LED_Task(void *argument)
{
    LED_TaskConfig_t *config =
        (LED_TaskConfig_t *)argument;

    while (1)
    {
        /* Toggle LED */
        HAL_GPIO_TogglePin(
            config->GPIOx,
            config->GPIO_Pin
        );

        /* Wait */
        vTaskDelay(
            pdMS_TO_TICKS(config->toggle_delay_ms)
        );
    }
}


/* =========================================================
 * System Clock Configuration
 * ========================================================= */

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* HSE + PLL */
    RCC_OscInitStruct.OscillatorType =
        RCC_OSCILLATORTYPE_HSE;

    RCC_OscInitStruct.HSEState =
        RCC_HSE_ON;

    RCC_OscInitStruct.HSEPredivValue =
        RCC_HSE_PREDIV_DIV1;

    RCC_OscInitStruct.HSIState =
        RCC_HSI_ON;

    RCC_OscInitStruct.PLL.PLLState =
        RCC_PLL_ON;

    RCC_OscInitStruct.PLL.PLLSource =
        RCC_PLLSOURCE_HSE;

    RCC_OscInitStruct.PLL.PLLMUL =
        RCC_PLL_MUL9;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct)
        != HAL_OK)
    {
        Error_Handler();
    }


    /* Configure clocks */
    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK |
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource =
        RCC_SYSCLKSOURCE_PLLCLK;

    RCC_ClkInitStruct.AHBCLKDivider =
        RCC_SYSCLK_DIV1;

    RCC_ClkInitStruct.APB1CLKDivider =
        RCC_HCLK_DIV2;

    RCC_ClkInitStruct.APB2CLKDivider =
        RCC_HCLK_DIV1;


    if (HAL_RCC_ClockConfig(
            &RCC_ClkInitStruct,
            FLASH_LATENCY_2
        ) != HAL_OK)
    {
        Error_Handler();
    }
}


/* =========================================================
 * GPIO Configuration
 * ========================================================= */

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};


    /* Enable GPIOA clock */
    __HAL_RCC_GPIOA_CLK_ENABLE();


    /* Initial LED state = OFF */
    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_0 |
        GPIO_PIN_1 |
        GPIO_PIN_2,
        GPIO_PIN_RESET
    );


    /* Configure PA0 PA1 PA2 as output */
    GPIO_InitStruct.Pin =
        GPIO_PIN_0 |
        GPIO_PIN_1 |
        GPIO_PIN_2;

    GPIO_InitStruct.Mode =
        GPIO_MODE_OUTPUT_PP;

    GPIO_InitStruct.Pull =
        GPIO_NOPULL;

    GPIO_InitStruct.Speed =
        GPIO_SPEED_FREQ_LOW;


    HAL_GPIO_Init(
        GPIOA,
        &GPIO_InitStruct
    );
}


/* =========================================================
 * Error Handler
 * ========================================================= */

void Error_Handler(void)
{
    __disable_irq();

    while (1)
    {
    }
}
