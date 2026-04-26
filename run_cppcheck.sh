#!/bin/bash
echo "=== Running Cppcheck on user code only (excluding stm32* files) ==="

#!/bin/bash
echo "=== Running Cppcheck (filtered) ==="

cppcheck --enable=all --inconclusive \
         --inline-suppr \
         --suppress=missingIncludeSystem \
         -ICore/Inc \
         --force \
         Core/Src Core/Inc 2>&1 | \
grep -v -E "(stm32f4xx_it\.c|syscalls\.c|stm32f4xx_hal_msp\.c|system_stm32f4xx\.c|Drivers/|Middlewares/|Startup/)" \
> cppcheck_results.txt

echo "Done. Results saved to cppcheck_results.txt"
echo "Showing only warnings from your code:"

# Show the filtered results in terminal (excluding summary lines if you want)
cat cppcheck_results.txt | grep -E "(warning|error|style|performance|portability|unused)"
