
# 🧰 File Manager & 🧠 SJF Scheduling Simulator (with GUI)

This repository contains two separate GUI-based tools developed for educational purposes and systems programming practice:

1. **File Manager GUI** – A C program using GTK to manage files and directories.
2. **Shortest Job First (SJF) Scheduling Simulator** – A Python program simulating a CPU scheduler using the SJF (Non-Preemptive) algorithm.

---

## 📁 1. File Manager GUI (C with GTK)

### ✅ Description

A desktop file manager built using C and the GTK+ library for Linux systems. This program allows users to:

- Browse a directory's contents
- Create or delete files and directories
- Change file permissions
- Create symbolic links
- View file details (size, permissions, type)

### 🛠️ Technologies

- **Language**: C
- **GUI Library**: GTK+ 3
- **Platform**: Ubuntu / Linux

### ▶️ How to Build and Run

1. **Install GTK 3 (if not installed):**

   ```bash
   sudo apt install libgtk-3-dev
   ```

2. **Compile the program:**

   Replace `main.c` with your actual source file name.

   ```bash
   gcc main.c -o filemanager `pkg-config --cflags --libs gtk+-3.0`
   ```

3. **Run the program:**

   ```bash
   ./filemanager
   ```

---

## 🧠 2. Shortest Job First (SJF) Scheduling Simulator (Python with Tkinter)

### ✅ Description

A GUI simulator that implements the **Shortest Job First (Non-Preemptive)** CPU scheduling algorithm. This tool allows the user to:

- Define the number of processes
- Input **Arrival Time (AT)** and **Burst Time (BT)** for each process
- Compute:
  - **Start Time (ST)**
  - **Completion Time (CT)**
  - **Waiting Time (WT)**
  - **Turnaround Time (TAT)**
  - **Response Time (RT)**
- Display average WT, TAT, and RT
- Show a **Gantt chart** of the scheduling order

### 🛠️ Technologies

- **Language**: Python 3
- **GUI**: Tkinter
- **Charting**: Matplotlib

### ▶️ How to Run

#### 1. Open Terminal and navigate to the project folder:
```bash
cd ~/SJF
```

#### 2. Create and activate a virtual environment (only once):
```bash
python3 -m venv venv
source venv/bin/activate
```

#### 3. Install required Python packages:
```bash
pip install matplotlib
```

#### 4. Run the simulator:
```bash
python sjf_simulator.py
```

#### 5. Exit the virtual environment when done:
```bash
deactivate
```

### 🧪 Input Validation

- All time inputs must be **non-negative integers**
- Burst time must be **greater than 0**
- GUI will show error messages for invalid input


---

## 📌 Notes

- The SJF simulator includes a clear, dynamic Gantt chart.
- The file manager supports basic operations and is beginner-friendly.
- Designed to run on Ubuntu 20.04+ or similar Linux distributions.

---

## 👤 Author

- **Name**: Hossam
- **Machine**: `hossam@hossam-VMware-Virtual-Platform`
- **Contact**: hossam.hassan.pc@gmail.com

---

## 📜 License

This project is for educational use. You may modify and redistribute it as needed.
