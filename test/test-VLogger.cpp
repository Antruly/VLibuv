#include <VLogger.h>
#include <thread>
int main() {
    size_t num = 0;
    while (num < 100) {
        // all print
        VLogger::Log->logAssert("Assert:%zu\n", num);
        VLogger::Log->logError("Error:%zu\n", num);
        VLogger::Log->logWarn("Warn:%zu\n", num);
        VLogger::Log->logDebug("Debug:%zu\n", num);
        VLogger::Log->logInfo("Info:%zu\n", num);
        VLogger::Log->logVerbose("Verbose:%zu\n", num);

        VLogger::Log->logDebugVerbose("\n\n===================================================\n\n");

        // debug print
        VLogger::Log->logDebugAssert("only Debug print Assert:%zu\n", num);
        VLogger::Log->logDebugError("only Debug print Error:%zu\n", num);
        VLogger::Log->logDebugWarn("only Debug print Warn:%zu\n", num);
        VLogger::Log->logDebugInfo("only Debug print Info:%zu\n", num);
        VLogger::Log->logDebugVerbose("only Debug print Verbose:%zu\n", num);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        ++num;
    }
  return 0;
}