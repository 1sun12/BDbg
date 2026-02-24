
/**
 * @title DEBUG_ENABLED 
 * @brief If set to 1, debug messages will print. If set to 0, debug messages will not print
 * @Use `#define DEBUG_ENABLED 1` (or) `#define DEBUG_ENABLED 0`
 */
#define DEBUG_ENABLED 0

/**
 *  @title DBG(msg)
 *  @brief Use to print debug messages with some embeded formatting
 *  @Use `DBG("main: Starting main...");`
 */
#define DBG(msg) do {if (DEBUG_ENABLED) {printf("\n[DEBUG] %s\n", msg);}} while(0)
