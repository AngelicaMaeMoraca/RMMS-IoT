import serial
import time
import os
import csv

# ================= CONFIGURATION =================
SERIAL_PORT = "COM3"   
BAUD_RATE = 115200    
# Updated to your specific folder path
SAVE_PATH = r"C:\Users\Angelica Mae Moraca\Desktop\MPU6500"
# Updated to your specific CSV filename
CSV_FILE = os.path.join(SAVE_PATH, "road_report.csv")
# =================================================

# Create the MPU6500 folder if it doesn't exist
if not os.path.exists(SAVE_PATH):
    os.makedirs(SAVE_PATH)

# Create CSV and write headers if it's new
if not os.path.exists(CSV_FILE):
    with open(CSV_FILE, 'w', newline='') as f:
        writer = csv.writer(f)
        # Headers include all your required parameters
        writer.writerow(["Timestamp", "Latitude", "Longitude", "Acceleration_Z", "Tilt_X", "Tilt_Y", "Image_Filename"])

print(f"RMMS Logger Active. Monitoring {SERIAL_PORT}...")
print(f"Data will be saved to: {CSV_FILE}")

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    # Clear buffer to avoid old data
    ser.flushInput()
    
    while True:
        # Read the incoming line from Arduino
        line = ser.readline().decode('utf-8', errors='ignore').strip()

        # Look for the DATA trigger
        if line.startswith("DATA,"):
            try:
                # Split: DATA, Lat, Lng, AccZ, TiltX, TiltY, Size
                parts = line.split(",")
                lat = parts[1]
                lng = parts[2]
                accZ = parts[3]
                tX = parts[4]
                tY = parts[5]
                img_size = int(parts[6])
                
                print(f"\n[!] Anomaly Detected at {lat}, {lng}")
                print(f"    Receiving image ({img_size} bytes)...")

                # Read EXACTLY the number of bytes specified by img_size
                img_data = ser.read(img_size)

                if len(img_data) == img_size:
                    # Create unique filename using timestamp
                    timestamp_str = time.strftime("%Y%m%d-%H%M%S")
                    img_name = f"pothole_{timestamp_str}.jpg"
                    img_full_path = os.path.join(SAVE_PATH, img_name)

                    # 1. Save Picture to the MPU6500 folder
                    with open(img_full_path, "wb") as f:
                        f.write(img_data)

                    # 2. Append data to road_report.csv
                    with open(CSV_FILE, 'a', newline='') as f:
                        writer = csv.writer(f)
                        writer.writerow([time.ctime(), lat, lng, accZ, tX, tY, img_name])
                    
                    print(f"    SUCCESS: Data logged to CSV and image saved as {img_name}")
                else:
                    print(f"    ERROR: Incomplete image (Got {len(img_data)} of {img_size} bytes)")
            
            except Exception as parse_error:
                print(f"    ERROR: Parsing failed: {parse_error}")

except KeyboardInterrupt:
    print("\nStopping Logger...")
except serial.SerialException as e:
    print(f"\nSerial Error: {e}. Check if Serial Monitor is open!")