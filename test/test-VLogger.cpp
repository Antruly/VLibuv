#include <VLogger.h>
#include <thread>
int main() {
    size_t num = 0;
    while (num < 100) {
        // all print
        Log->logAssert("Assert:%zu\n", num);
        Log->logError("Error:%zu\n", num);
        Log->logWarn("Warn:%zu\n", num);
        Log->logDebug("Debug:%zu\n", num);
        Log->logInfo("Info:%zu\n", num);
        Log->logVerbose("Verbose:%zu\n", num);

        Log->logDebugVerbose("\n\n===================================================\n\n");

        // debug print
        Log->logDebugAssert("only Debug print Assert:%zu\n", num);
        Log->logDebugError("only Debug print Error:%zu\n", num);
        Log->logDebugWarn("only Debug print Warn:%zu\n", num);
        Log->logDebugInfo("only Debug print Info:%zu\n", num);
        Log->logDebugVerbose("only Debug print Verbose:%zu\n", num);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        ++num;
    }
  return 0;
}