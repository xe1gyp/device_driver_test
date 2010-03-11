#!/bin/sh
set -x

LOCAL_COMMAND=$1

if [ "$LOCAL_COMMAND" = "set_all" ]; then
  valid_c_state="C1 C2 C3 C4 C5 C6 C7 C8 C9"
  for i in $valid_c_state
	 do
		echo "$UTILOMAP3/cpuIdle $i"
		$UTILOMAP3/cpuIdle $i
  done

elif [ "$LOCAL_COMMAND" = "set_all_invalid" ]; then

  invalid_c_state="C10 C11 C12 C13 C14 C15 C16 C17 C18"
  for i in $invalid_c_state
	 do
		echo "$UTILOMAP3/cpuIdle $i"
		$UTILOMAP3/cpuIdle $i
  done

elif [ "$LOCAL_COMMAND" = "set_optimal" ]; then

  optimal_c_state="C3"
  for i in $optimal_c_state
	 do
		echo "$UTILOMAP3/cpuIdle $i"
		$UTILOMAP3/cpuIdle $i
  done

elif [ "$LOCAL_COMMAND" = "set_invalid" ]; then

  optimal_c_state="C10"
  for i in $optimal_c_state
	 do
		echo "$UTILOMAP3/cpuIdle $i"
		$UTILOMAP3/cpuIdle $i
  done

else
  echo "Command in cpuFrequencyScaling is not supported"
  exit 1
fi

# End of file
