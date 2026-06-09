import serial
import wave
import sys
import array
import keyboard
import time
import os



PORT_COM = "COM12"  #CHANGE TO THE PORT THAT YOU SEE ON ARDUINO IDE
BAUD_RATE = 115200
OUTPUT_FOLDER = "Reaver"
OUTPUT_FILENAME = "Reaver"
INDEX_FILE = 0

CHANNELS = 1
SAMPLE_RATE = 16000
SAMPLE_WIDTH = 2

# Create the directory
try:
    os.mkdir(OUTPUT_FOLDER)
    print(f"Directory '{OUTPUT_FOLDER}' created successfully.")
except FileExistsError:
    print(f"Directory '{OUTPUT_FOLDER}' already exists.")

print(f"Connecting to the XIAO on Port {PORT_COM}")

try:
    ser = serial.Serial(PORT_COM, BAUD_RATE, timeout=1)
except Exception as e:
    print(f"Error cannot open {PORT_COM}: {e}")
    sys.exit()
print("Recording in process, Press Q to Quit and R to Record one sample")
while keyboard.is_pressed("q") is False:
    audio_data = bytearray()
    time.sleep(0.2)  # to not create empty files if you Press R to long
    print(f"\nRecording of {OUTPUT_FILENAME + str(INDEX_FILE)}")

    while keyboard.is_pressed("r") is False and keyboard.is_pressed("q") is False:
        # Waiting for data in the duffer transmited to the PC
        if ser.in_waiting > 0:
            data = ser.read(ser.in_waiting)
            audio_data.extend(data)
            print(f"\rData Captured : {len(audio_data) // 1024} KB", end="")

    print(f"\nRecording of {OUTPUT_FILENAME + str(INDEX_FILE)} Stopped.")

    if len(audio_data) > 0:
        # Config Volume amplification OPTIONAL COMMENT THIS PART IF NOT NEEDED
        GAIN = 20

        # 1. Convert Raw data in 16-bit signed data
        samples = array.array('h', audio_data)
        amplified_samples = array.array('h')

        # 2. Volume amplification and limit for no saturation
        for sample in samples:
            amp_value = int(sample * GAIN)

            if amp_value > 32767:
                amp_value = 32767
            elif amp_value < -32768:
                amp_value = -32768

            amplified_samples.append(amp_value)
        # END OF VOLUME AMPLIFICATION
        # 3. Create and write the audio to a wav file
        print(f"Creating file {OUTPUT_FILENAME + str(INDEX_FILE)}.wav")
        with wave.open(f"{OUTPUT_FOLDER}/{OUTPUT_FILENAME + str(INDEX_FILE)}.wav", 'wb') as wav_file:
            wav_file.setnchannels(CHANNELS)
            wav_file.setsampwidth(SAMPLE_WIDTH)
            wav_file.setframerate(SAMPLE_RATE)
            # CHANGE TO "wav_file.writeframes(audio_data)" IF YOU DIDNT USE VOLUME AMPLIFICATION
            wav_file.writeframes(amplified_samples.tobytes())
        INDEX_FILE += 1
    else:
        print("No data captured")

ser.close()
print("Serial closed")