#!/usr/bin/python3

import subprocess
import time
import signal

audioState = False
lastAudioState = False

displayState = False
lastDisplayState = False

streamState = False
lastStreamState = False
base_cmd = "/usr/bin/gsettings set org.cinnamon.settings-daemon.plugins.power sleep-inactive-ac-timeout "
sleep_get_cmd = "/usr/bin/gsettings get org.cinnamon.settings-daemon.plugins.power sleep-inactive-ac-timeout"
status_get_cmd = "netstat -a -n | grep -e :8895 -e :23424 | grep ESTABLISHED | grep -v 192.168.0.1"

def get_stream_state():
    global streamState
    global lastStreamState
    global systemSleepTime
    global temp_cmd
    global sleep_get_cmd
    global status_get_cmd

    cmd = subprocess.run(status_get_cmd, shell=True, capture_output=True, encoding='utf-8')


    if cmd.stdout:
        streamState = True
#        print("Result match found")
    else:
        streamState = False
#        print("No Result match found")

    # Check if the streaming state changed
    if lastStreamState != streamState:

        # Streaming started
        if streamState == True:
            print("Streaming started");
            subprocess.Popen('/bin/echo Streaming Start: $(date) >> /tmp/timestampsStream', shell=True)

            # Get current system sleep time and save it
            proc = subprocess.Popen(sleep_get_cmd, shell=True, stdout=subprocess.PIPE)
            systemSleepTime = int(proc.stdout.read())
            #print(systemSleepTime)
            proc.stdout.close()

            temp_cmd = base_cmd + "0"      # Set system sleep time to never (0)
            #print(temp_cmd)
            subprocess.Popen(temp_cmd, shell=True)

        # Streaming ended
        else:
            print("Streaming ended");
            subprocess.Popen('/bin/echo Streaming end: $(date) >> /tmp/timestampsStream', shell=True)

            temp_cmd = base_cmd + str(systemSleepTime)       # Restore saved system sleep time
            #print(temp_cmd)
            subprocess.Popen(temp_cmd, shell=True)

        lastStreamState = streamState
# end Stream

def get_audio_state():
    global audioState
    global lastAudioState

    cmd = subprocess.run('cat /proc/asound/card0/pcm0p/sub0/status | grep -e RUNNING', shell=True, capture_output=True, encoding='utf-8')

    if cmd.stdout:
        audioState = True
#        print("Result match found")
    else:
        audioState = False
#        print("No Result match found")

    if lastAudioState != audioState:
        if audioState == True:
            print("Audio on")
            subprocess.Popen('/bin/echo Audio changed to Running: $(date) >> /tmp/timestamps', shell=True)
            subprocess.Popen('/bin/echo -en A > /dev/ttyACM0', shell=True)
        else:
            print("Audio off")
            subprocess.Popen('/bin/echo Audio changed to Closed: $(date) >> /tmp/timestamps', shell=True)
            subprocess.Popen('/bin/echo -en a7 > /dev/ttyACM0', shell=True)

        lastAudioState = audioState
# end Audio



def get_display_state():

    global displayState
    global lastDisplayState

    cmd = subprocess.run('/usr/bin/xset q -display :0 | grep -e On', shell=True, capture_output=True, encoding='utf-8')

    if cmd.stdout:
        displayState = True
#        print("Result match found")
    else:
        displayState = False
#        print("No Result match found")

    if lastDisplayState != displayState:
        if displayState == True:
            print("Display on")
            subprocess.Popen('/bin/echo Display changed to ON: $(date) >> /tmp/timestamps', shell=True)
            subprocess.Popen('/bin/echo -en D > /dev/ttyACM0', shell=True)
        else:
            print("Display off")
            subprocess.Popen('/bin/echo Display changed to Off: $(date) >> /tmp/timestamps', shell=True)
            subprocess.Popen('/bin/echo -en d > /dev/ttyACM0', shell=True)

        lastDisplayState = displayState



# end Display

class GracefulKiller:
  kill_now = False
  def __init__(self):
    signal.signal(signal.SIGINT, self.exit_gracefully)
    signal.signal(signal.SIGTERM, self.exit_gracefully)

  def exit_gracefully(self,signum, frame):
    self.kill_now = True

if __name__ == '__main__':
  killer = GracefulKiller()
  
  subprocess.Popen('/bin/echo "Script Started $(date)" >> /tmp/timestamps', shell=True)
#  subprocess.Popen('/bin/echo "Started $(date)" > /dev/ttyUSB0', shell=True)

  while True:
    get_audio_state()
    get_display_state()
    get_stream_state()
    time.sleep(2)

    if killer.kill_now:
        subprocess.Popen('/bin/echo "Script Stopped $(date)" >> /tmp/timestamps', shell=True)
#        subprocess.Popen('/bin/echo "Stopped $(date)" > /dev/ttyUSB0', shell=True)
        break

  print("End of the program. I was killed gracefully :)")
  exit(0)
