import tkinter as tk
from tkinter import ttk
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np
import pandas as pd
from scipy.ndimage import convolve
import time
import serial

def sobel_filter(image, threshold=None):
    sobel_x = np.array([[-1, 0, 1],
                        [-2, 0, 2],
                        [-1, 0, 1]])
    sobel_y = np.array([[-1, -2, -1],
                        [0,  0,  0],
                        [1,  2,  1]])
    image = image.astype(np.float32)
    grad_x = convolve(image, sobel_x, mode='constant', cval=0.0)
    grad_y = convolve(image, sobel_y, mode='constant', cval=0.0)
    grad_mag = np.hypot(grad_x, grad_y)
    max_val = grad_mag.max()
    if max_val > 0:
        grad_mag /= max_val
    if threshold is not None:
        grad_mag = (grad_mag > threshold).astype(np.float32)
    return grad_mag, grad_x, grad_y

def load_mnist(csv_path):
    data = pd.read_csv(csv_path)
    labels = data.iloc[:, 0].values
    images = data.iloc[:, 1:].values.reshape(-1, 28, 28) / 255.0
    return images, labels

images, labels = load_mnist("./data/mnist_train.csv")

window = tk.Tk()
window.title("MNIST Sobel Edge Detection")
window.geometry("700x400")
window.configure(bg="#bfc3c6")

main_frame = tk.Frame(window, bg="#bfc3c6")
main_frame.pack(expand=True, fill=tk.BOTH, padx=10, pady=10)

left_frame = tk.Frame(main_frame, bg="#2d5d6c", width=350, height=350)
left_frame.grid(row=0, column=0, rowspan=2, padx=10, pady=10, sticky="nsew")
left_frame.grid_propagate(False)

right_frame = tk.Frame(main_frame, bg="#bfc3c6", width=120)
right_frame.grid(row=0, column=1, padx=10, pady=10, sticky="ns")

bottom_frame = tk.Frame(main_frame, bg="#bfc3c6")
bottom_frame.grid(row=1, column=1, padx=10, pady=(0,10), sticky="ew")

main_frame.grid_columnconfigure(0, weight=3)
main_frame.grid_columnconfigure(1, weight=1)
main_frame.grid_rowconfigure(0, weight=1)

canvas_frame = tk.Frame(left_frame, bg="#2d5d6c")
canvas_frame.pack(expand=True, fill=tk.BOTH, padx=20, pady=20)
canvas = None

current_idx = [0]  
showing_filtered = [False]

def show_image(img, title="MNIST Image"):
    global canvas
    for widget in canvas_frame.winfo_children():
        widget.destroy()
    fig, ax = plt.subplots(figsize=(3,3))
    ax.imshow(img, cmap='gray')
    ax.axis('off')
    plt.tight_layout()
    canvas = FigureCanvasTkAgg(fig, master=canvas_frame)
    canvas.draw()
    canvas.get_tk_widget().pack(expand=True)
    plt.close(fig)

def update_image():
    idx = current_idx[0]
    image_idx.set(idx)
    show_image(images[idx])
    showing_filtered[0] = False
    update_info()

def on_slider_change(event=None):
    idx = image_idx.get()
    current_idx[0] = idx
    update_image()

def prev_image():
    if current_idx[0] > 0:
        current_idx[0] -= 1
        update_image()

def next_image():
    if current_idx[0] < len(images) - 1:
        current_idx[0] += 1
        update_image()

def apply_sobel():
    idx = current_idx[0]
    try:
        thresh = float(threshold_entry.get())
    except ValueError:
        thresh = None
    start = time.time()
    filtered, _, _ = sobel_filter(images[idx], threshold=thresh)
    latency = (time.time() - start) * 1000  # ms
    show_image(filtered, title="Sobel")
    showing_filtered[0] = True
    update_info(latency_ms=latency)

def send_to_fpga():
    idx = current_idx[0]
    img = images[idx].astype(np.float32)
    try:
        with serial.Serial('COM3', 115200, timeout=5) as ser:
            # Send image
            ser.write(img.tobytes())
            ser.flush()
            
            # Receive result
            result_bytes = ser.read(28*28*4)
            
            if len(result_bytes) == 28*28*4:
                filtered = np.frombuffer(result_bytes, dtype=np.float32).reshape(28,28)
                show_image(filtered, title="FPGA Sobel")
                showing_filtered[0] = True
            else:
                print(f"Received {len(result_bytes)} bytes (expected {28*28*4})")
    except Exception as e:
        print(f"Communication error: {e}")

nav_frame = tk.Frame(left_frame, bg="#2d5d6c")
nav_frame.pack(expand=True, fill=tk.BOTH, padx=20, pady=20)

btn_prev = tk.Button(nav_frame, text="Prev", width=6, bg="#2d5d6c", fg="white", command=prev_image)
btn_prev.pack(side=tk.LEFT, padx=(0,10), pady=10)

canvas_frame = tk.Frame(nav_frame, bg="#2d5d6c", width=200, height=200)
canvas_frame.pack(side=tk.LEFT, expand=True, fill=tk.BOTH)
canvas_frame.pack_propagate(False)

btn_next = tk.Button(nav_frame, text="Next", width=6, bg="#2d5d6c", fg="white", command=next_image)
btn_next.pack(side=tk.LEFT, padx=(10,0), pady=10)

canvas = None

# --- Controls at the bottom ---
btn_dataset = tk.Button(bottom_frame, text="DATASET", width=10, bg="#2d5d6c", fg="white")
btn_dataset.grid(row=0, column=0, padx=10, pady=10)
btn_alg = tk.Button(bottom_frame, text="ALG", width=10, bg="#2d5d6c", fg="white")
btn_alg.grid(row=0, column=1, padx=10, pady=10)
btn_run = tk.Button(bottom_frame, text="RUN", width=10, bg="#2d5d6c", fg="white", command=apply_sobel)
btn_run.grid(row=0, column=2, padx=10, pady=10)
btn_run = tk.Button(bottom_frame, text="RUN FPGA", width=10, bg="#2d5d6c", fg="white", command=send_to_fpga)
btn_run.grid(row=0, column=3, padx=10, pady=10)

# --- Image index slider and threshold ---
slider_frame = tk.Frame(left_frame, bg="#2d5d6c")
slider_frame.pack(fill=tk.X, padx=10, pady=(0,10))
tk.Label(slider_frame, text="Image Index:", bg="#2d5d6c", fg="white").pack(side=tk.LEFT)
image_idx = tk.Scale(slider_frame, from_=0, to=len(images)-1, orient=tk.HORIZONTAL, length=180, command=on_slider_change, bg="#2d5d6c", fg="white", highlightbackground="#2d5d6c")
image_idx.pack(side=tk.LEFT, padx=5)
image_idx.set(0)

thresh_frame = tk.Frame(left_frame, bg="#2d5d6c")
thresh_frame.pack(fill=tk.X, padx=10, pady=(0,10))
tk.Label(thresh_frame, text="Threshold:", bg="#2d5d6c", fg="white").pack(side=tk.LEFT)
threshold_entry = tk.Entry(thresh_frame, width=5)
threshold_entry.insert(0, "0.2")
threshold_entry.pack(side=tk.LEFT, padx=5)

# --- Right info buttons (now as labels for dynamic update) ---
info_vars = {
    "Data num": tk.StringVar(value=f"{len(images)}"),
    "Index": tk.StringVar(value="0"),
    "Latency (ms)": tk.StringVar(value="0"),
}

info_labels = []
for key in info_vars:
    frame = tk.Frame(right_frame, bg="#bfc3c6")
    frame.pack(pady=5)
    label = tk.Label(frame, text=key, width=12, height=1, bg="#2d5d6c", fg="white")
    label.pack()
    value = tk.Label(frame, textvariable=info_vars[key], width=12, height=1, bg="#2d5d6c", fg="yellow")
    value.pack()
    info_labels.append((label, value))

def update_info(latency_ms=None):
    info_vars["Data num"].set(f"{len(images)}")
    info_vars["Index"].set(str(current_idx[0]))
    if latency_ms is not None:
        info_vars["Latency (ms)"].set(f"{latency_ms:.2f}")
    else:
        info_vars["Latency (ms)"].set("0")



# Show the first image by default
update_image()

window.mainloop()