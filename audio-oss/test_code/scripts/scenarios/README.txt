
IMPORTANT:
Each scenario file starts with a foo and ends with a bar test.
these are just for the sake for pan which tends to execute the first test 
of a test set once the tests are complete.

+-----------------------------------------------------------------------+
|TEST SCENARIO ID /|   Test Description   |          COMMENTS           |
|   Test case ID   |                      |                             |
|------------------+----------------------+-----------------------------|
|                  |This tests for the    |This test will attempt to    |
|L_DD_AUDIO_0001   |existence of the Devfs|open the Devfs devices in    |
|                  |devices and checks if |read/write/readwrite modes.  |
|                  |they are operable     |                             |
|------------------+----------------------+-----------------------------|
|                  |                      |This shall attempt to        |
|                  |                      |                             |
|                  |                      |Retrieve all supported       |
|                  |                      |information of the current   |
|                  |                      |device configuration and test|
|                  |                      |the setting of:              |
|                  |This tests for the    |                             |
|                  |basic configurability |1.       All supported sample|
|L_DD_AUDIO_0002   |of the device by      |rates                        |
|                  |checking the ioctls on|                             |
|                  |the device            |2.       All supported sample|
|                  |                      |sizes                        |
|                  |                      |                             |
|                  |                      |3.       All supported       |
|                  |                      |channels                     |
|                  |                      |                             |
|                  |                      |4.       Gain setting        |
|------------------+----------------------+-----------------------------|
|                  |                      |This shall attempt to        |
|                  |                      |playback audio using         |
|                  |                      |combinations of:             |
|                  |                      |                             |
|                  |                      |1.  All supported sample     |
|                  |                      |rates                        |
|                  |                      |                             |
|                  |                      |2.  All supported sample     |
|                  |This tests for        |sizes                        |
|L_DD_AUDIO_0003   |playback capability of|                             |
|                  |the device            |3.  All supported channels   |
|                  |                      |                             |
|                  |                      |4.  Playback Gain Values     |
|                  |                      |                             |
|                  |                      |5.  Playback sources         |
|                  |                      |                             |
|                  |                      |6.  Varied buffer sizes      |
|                  |                      |                             |
|                  |                      |7.  Power management - scale |
|                  |                      |and deep sleep               |
|------------------+----------------------+-----------------------------|
|                  |                      |This shall attempt to record |
|                  |                      |audio using  combinations of:|
|                  |                      |                             |
|                  |                      |1.  All supported sample     |
|                  |                      |rates                        |
|                  |                      |                             |
|                  |                      |2.  All supported sample     |
|                  |                      |sizes                        |
|                  |This tests for the    |                             |
|L_DD_AUDIO_0004   |record capability of  |3.  All supported channels   |
|                  |the device            |                             |
|                  |                      |4.  Record Gain Values       |
|                  |                      |                             |
|                  |                      |5.  Record sources           |
|                  |                      |                             |
|                  |                      |6.  Varied buffer sizes      |
|                  |                      |                             |
|                  |                      |7.  Power management - scale |
|                  |                      |and deep sleep               |
|------------------+----------------------+-----------------------------|
|                  |                      |This shall attempt to play   |
|                  |                      |and record audio at the same |
|                  |                      |time using  combinations of: |
|                  |                      |                             |
|                  |                      |1.  All supported sample     |
|                  |                      |rates                        |
|                  |                      |                             |
|                  |                      |2.  All supported sample     |
|                  |                      |sizes                        |
|                  |This tests the full   |                             |
|L_DD_AUDIO_0005   |duplex capability of  |3.  All supported channels   |
|                  |the device            |                             |
|                  |                      |4.  Playback and Record Gain |
|                  |                      |Values                       |
|                  |                      |                             |
|                  |                      |5.  Playback and Record      |
|                  |                      |sources                      |
|                  |                      |                             |
|                  |                      |6.  Varied buffer sizes      |
|                  |                      |                             |
|                  |                      |7.  Power management - scale |
|                  |                      |and deep sleep               |
|------------------+----------------------+-----------------------------|
|                  |                      |This shall attempt to test   |
|                  |                      |the mixer functionality of   |
|L_DD_AUDIO_0006   |This tests the mixer  |the device using standard    |
|                  |device operation      |mixer applications such as   |
|                  |                      |aumix (refer to section 8.4  |
|                  |                      |for more details).           |
|------------------+----------------------+-----------------------------|
|                  |This tests for the    |This shall test the          |
|                  |dynamic module        |capability of the driver to  |
|L_DD_AUDIO_0007   |capability of the     |handle module load unload    |
|                  |driver                |capability while the driver  |
|                  |                      |is in use and when it is not.|
|------------------+----------------------+-----------------------------|
|                  |This tests the basic  |This shall test the driver   |
|                  |co-existence of the   |operation in conjunction with|
|L_DD_AUDIO_0008   |device with other     |other drivers sharing paths  |
|                  |devices               |such as DMA or the target    |
|                  |                      |chip.                        |
|------------------+----------------------+-----------------------------|
|                  |                      |This shall attempt to test   |
|                  |This tests if the     |the driver's adherence to    |
|                  |driver is compatible  |standards by testing it      |
|L_DD_AUDIO_0009   |with standard         |against standard applications|
|                  |application           |such as madplay, sox and     |
|                  |                      |aumix (refer to section 8.4  |
|                  |                      |for more details).           |
|------------------+----------------------+-----------------------------|
|                  |                      |This shall test the config   |
|                  |                      |the codec configuration      |
|                  |This tests the off    |                             |
|                  |mode configuration of |                             |
|L_DD_AUDIO_0010   |the device by testing |                             |
|                  |the DSP Side rendering|                             |
|                  |and capture support   |                             |
|                  |                      |                             |
|------------------+----------------------+-----------------------------|
| L_DD_UTR_x.x     | UTR for a release    | These are basic tests for   |
|                  |                      | the release satisfying all  |
|                  |                      | requirements of that release|
+-----------------------------------------------------------------------+
