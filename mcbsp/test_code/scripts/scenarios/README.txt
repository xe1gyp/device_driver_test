The current kernel releases for OMAP3 do not support poll mode for mcbsp
and hence this testsuite cannot be used. Sanity testing should be perfomed
through alsa, although some scenarios will remain blocked.

Please refer to latest UTR for more information.

Current releases are:
L23.I3.4 (LO-sync)
http://dev.omapzoom.org/?p=integration/kernel-omap3.git;a=summary

L25.Inc2.5 (25x)
git://git.omapzoom.org/kernel/omap.git

