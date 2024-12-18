import serial
import time
import pandas as pd


humidity = 0.0
temperature = 0.0
co2 = 0.0
sensor_value = 0
output_count = 0
data_list = []

def parse_data(data):

    global humidity, temperature, co2, sensor_value
    try:
        parts = data.strip().split(",")
        if len(parts) == 4:
            humidity = float(parts[0])       # Влажность
            temperature = float(parts[1])    # Температура
            co2 = float(parts[2])            # CO2
            sensor_value = int(parts[3])     # Значение аналогового сенсора
    except ValueError:
        print("Ошибка разбора данных:", data)

def print_data():
    print(f"Влажность: {humidity:.2f}% | Температура: {temperature:.2f}°C | CO2: {co2:.2f} ppm | Значение сенсора: {sensor_value}") #Вывод текущих значений.

def save_to_excel():

    df = pd.DataFrame(data_list, columns=["Humidity", "Temperature", "CO2", "Sensor Value"])
    df.to_excel("sensor_data.xlsx", index=False)  #('sensor_data.xlsx') - обязательно

def main():
    port = "COM3"
    baud_rate = 9600

    try:
        arduino = serial.Serial(port, baud_rate, timeout=1)
        time.sleep(2)

        print("Подключено к Arduino! Ожидание данных...\n")

        while True:
            if arduino.in_waiting > 0:
                # Чтение строки данных с Arduino
                line = arduino.readline().decode("utf-8").strip()
                if line:
                    parse_data(line)
                    global output_count
                    # первые 5 выводов отметаем
                    if output_count >= 5:
                        print_data()
                        data_list.append([humidity, temperature, co2, sensor_value])
                    output_count += 1
                    if output_count >= 105:
                        break

        save_to_excel()

    except serial.SerialException as e:
        print(f"Ошибка подключения к порту {port}: {e}")
    except KeyboardInterrupt:
        print("Завершение работы программы...")
    finally:
        if 'arduino' in locals() and arduino.is_open:
            arduino.close()
            print("Соединение закрыто.")

if __name__ == "__main__":
    main()
