import queue
import threading
import time
import tkinter as tk
from tkinter import ttk, messagebox
import serial
import serial.tools.list_ports

try:
    import pyttsx3
except ImportError:
    pyttsx3 = None

class App:
    def __init__(self, root):
        self.root = root
        self.root.title("IMU Sign Language - USB Voice")
        self.root.geometry("760x560")
        self.ser = None
        self.running = False
        self.q = queue.Queue()
        self.last_spoken = ""
        self.last_time = 0.0
        self.port = tk.StringVar()
        self.status = tk.StringVar(value="Disconnected")
        self.sign = tk.StringVar(value="Waiting for sign...")
        self.data = tk.StringVar(value="No sensor data yet")
        self.voice = tk.BooleanVar(value=True)
        self.build()
        self.refresh()
        self.root.after(100, self.process)
        self.root.protocol("WM_DELETE_WINDOW", self.close)

    def build(self):
        ttk.Label(self.root, text="IMU SIGN LANGUAGE TRANSLATOR",
                  font=("Segoe UI", 20, "bold")).pack(pady=(18, 4))
        ttk.Label(self.root, text="Arduino UNO + 5 MPU6050 + TCA9548A + USB").pack()
        top = ttk.Frame(self.root)
        top.pack(fill="x", padx=25, pady=15)
        ttk.Label(top, text="COM Port").grid(row=0, column=0, padx=5)
        self.box = ttk.Combobox(top, textvariable=self.port, width=18, state="readonly")
        self.box.grid(row=0, column=1, padx=5)
        ttk.Button(top, text="Refresh", command=self.refresh).grid(row=0, column=2, padx=5)
        self.connect_btn = ttk.Button(top, text="Connect", command=self.toggle)
        self.connect_btn.grid(row=0, column=3, padx=5)
        ttk.Label(top, textvariable=self.status).grid(row=0, column=4, padx=10)

        frame = ttk.LabelFrame(self.root, text="Detected Sign", padding=18)
        frame.pack(fill="x", padx=25, pady=10)
        ttk.Label(frame, textvariable=self.sign, font=("Segoe UI", 28, "bold"),
                  anchor="center").pack(fill="x", pady=20)

        controls = ttk.Frame(self.root)
        controls.pack(fill="x", padx=25)
        ttk.Checkbutton(controls, text="Speak automatically",
                        variable=self.voice).pack(side="left")
        ttk.Button(controls, text="Speak Again",
                   command=self.speak_current).pack(side="right")

        data_frame = ttk.LabelFrame(self.root, text="Live Sensor Data", padding=12)
        data_frame.pack(fill="x", padx=25, pady=15)
        ttk.Label(data_frame, textvariable=self.data, font=("Consolas", 9),
                  anchor="w").pack(fill="x")

        log_frame = ttk.LabelFrame(self.root, text="Serial Log", padding=8)
        log_frame.pack(fill="both", expand=True, padx=25, pady=(0, 18))
        self.log = tk.Text(log_frame, height=8, state="disabled")
        self.log.pack(fill="both", expand=True)

    def refresh(self):
        ports = [p.device for p in serial.tools.list_ports.comports()]
        self.box["values"] = ports
        if ports and self.port.get() not in ports:
            self.port.set(ports[0])
        self.status.set("Port found" if ports else "No COM port")

    def toggle(self):
        self.disconnect() if self.running else self.connect()

    def connect(self):
        if not self.port.get():
            messagebox.showwarning("COM Port", "Select the Arduino COM port.")
            return
        try:
            self.ser = serial.Serial(self.port.get(), 9600, timeout=0.5)
            time.sleep(2)
            self.running = True
            self.connect_btn.config(text="Disconnect")
            self.status.set("Connected: " + self.port.get())
            self.log_line("[CONNECTED] " + self.port.get())
            threading.Thread(target=self.reader, daemon=True).start()
        except Exception as e:
            messagebox.showerror("Connection error", str(e))

    def disconnect(self):
        self.running = False
        if self.ser:
            try: self.ser.close()
            except Exception: pass
        self.ser = None
        self.connect_btn.config(text="Connect")
        self.status.set("Disconnected")

    def reader(self):
        while self.running and self.ser:
            try:
                raw = self.ser.readline()
                if raw:
                    self.q.put(raw.decode("utf-8", errors="ignore").strip())
            except Exception as e:
                self.q.put("__ERROR__" + str(e))
                return

    def process(self):
        try:
            while True:
                line = self.q.get_nowait()
                if line.startswith("__ERROR__"):
                    self.log_line("[ERROR] " + line[9:])
                    self.disconnect()
                    break
                if not line: continue
                self.log_line(line)
                if line.startswith("SIGN:"):
                    text = line[5:].strip()
                    self.sign.set(text.upper())
                    if self.voice.get(): self.speak_async(text)
                elif line.startswith("DATA,"):
                    self.data.set(line[5:])
        except queue.Empty:
            pass
        self.root.after(100, self.process)

    def speak_async(self, text):
        threading.Thread(target=self.speak, args=(text,), daemon=True).start()

    def speak(self, text):
        now = time.time()
        if text == self.last_spoken and now - self.last_time < 1.5: return
        self.last_spoken, self.last_time = text, now
        if pyttsx3 is None:
            self.q.put("__ERROR__Install pyttsx3")
            return
        try:
            engine = pyttsx3.init()
            engine.setProperty("rate", 155)
            engine.say(text)
            engine.runAndWait()
            engine.stop()
        except Exception as e:
            self.q.put("__ERROR__" + str(e))

    def speak_current(self):
        text = self.sign.get()
        if text != "Waiting for sign...": self.speak_async(text)

    def log_line(self, text):
        self.log.config(state="normal")
        self.log.insert("end", text + "\n")
        self.log.see("end")
        self.log.config(state="disabled")

    def close(self):
        self.disconnect()
        self.root.destroy()

if __name__ == "__main__":
    root = tk.Tk()
    App(root).mainloop()
