#include <VLogger.h>
#include <thread>
int main() {
    size_t num = 0;
    while (num < 100) {
        // all print
        Log->logAssert("Assert:%llu\n", num);
        Log->logError("Error:%llu\n", num);
        Log->logWarn("Warn:%llu\n", num);
        Log->logDebug("Debug:%llu\n", num);
        Log->logInfo("Info:%llu\n", num);
        Log->logVerbose("Verbose:%llu\n", num);

        Log->logDebugVerbose("\n\n===================================================\n\n");

        // debug print
        Log->logDebugAssert("only Debug print Assert:%llu\n", num);
        Log->logDebugError("only Debug print Error:%llu\n", num);
        Log->logDebugWarn("only Debug print Warn:%llu\n", num);
        Log->logDebugInfo("only Debug print Info:%llu\n", num);
        Log->logDebugVerbose("only Debug print Verbose:%llu\n", num);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        ++num;
    }
  return 0;
}