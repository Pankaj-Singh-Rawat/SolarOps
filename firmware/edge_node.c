#include "rsi_common.h"
#include "rsi_wifi_apis.h"
#include "rsi_mqtt_client.h"


#define SOLAR_LDR_PIN         1  // Analog peripheral mapping for panel yield
#define HOUSE_POT_PIN         2  // Analog peripheral mapping for building load
#define ROUTING_RELAY_PIN     3  // Active GPIO lane-switching trigger

// Robust State Machine Layout
typedef enum {
    ONLINE_OPTIMIZATION,
    LOCAL_AUTONOMY_FALLBACK
} system_state_t;

volatile system_state_t current_state = ONLINE_OPTIMIZATION;

/**
 * @brief Configures SiWx91x board-specific peripheral registers.
 */
void setup_silabs_hardware(void) {
    // Invoke native hardware abstraction layer routines
    rsi_hal_config_gpio_pin(ROUTING_RELAY_PIN, OUTPUT);
}

/**
 * @brief Samples analog telemetry indices from onboard hardware channels.
 */
void read_telemetry(uint32_t *solar_input, uint32_t *house_load) {
    *solar_input = rsi_hal_read_adc(SOLAR_LDR_PIN);
    *house_load = rsi_hal_read_adc(HOUSE_POT_PIN);
}

int main(void) {
    // Initialize system-level clock frames and drivers
    setup_silabs_hardware();
    
    uint32_t solar_input = 0;
    uint32_t house_load = 0;
    int32_t status = RSI_SUCCESS;

    while(1) {
        // Core Ingestion Phase
        read_telemetry(&solar_input, &house_load);

        if (current_state == ONLINE_OPTIMIZATION) {
            // Marshall metrics into a flat, structured transmission string
            char payload[128];
            snprintf(payload, sizeof(payload), "{\"solar\": %ld, \"load\": %ld}", solar_input, house_load);
            
            // Execute non-blocking wireless uplink publish via WiSeConnect SDK stack
            status = rsi_mqtt_publish(NULL, "solarops/telemetry", QOS_0, 0, (uint8_t *)payload, strlen(payload));
            
            // Catch backhaul drops or broker connection failures immediately
            if (status != RSI_SUCCESS) {
                current_state = LOCAL_AUTONOMY_FALLBACK;
            }
        } 
        else if (current_state == LOCAL_AUTONOMY_FALLBACK) {
            // Core Fallback Phase: On-chip hardware state machine takes execution priority
            if (solar_input > house_load) {
                rsi_hal_set_gpio(ROUTING_RELAY_PIN, HIGH); // Safely isolate configuration onto solar lanes
            } else {
                rsi_hal_set_gpio(ROUTING_RELAY_PIN, LOW);  // Gracefully transition onto the local utility grid
            }
            
            // Keep testing network frames to determine if the local Wi-Fi lane has healed
            if (rsi_wifi_check_connection_status() == RSI_SUCCESS) {
                current_state = ONLINE_OPTIMIZATION;
            }
        }

        // Enforce deterministic 2-second telemetry polling intervals
        rsi_delay_ms(2000); 
    }
    return 0;
}