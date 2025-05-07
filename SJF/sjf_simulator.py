    #source venv/bin/activate
    #python sjf_simulator.py
import tkinter as tk
from tkinter import messagebox
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

class SJFSimulator:
    def __init__(self, root):
        self.root = root
        self.root.title("SJF Scheduling Simulator")
        self.entries = []
        self.process_data = []

        self.create_input_frame()

    def create_input_frame(self):
        frame = tk.Frame(self.root, padx=20, pady=20)
        frame.pack()

        tk.Label(frame, text="Number of Processes:").grid(row=0, column=0)
        self.num_entry = tk.Entry(frame, width=5)
        self.num_entry.grid(row=0, column=1)
        tk.Button(frame, text="Submit", command=self.create_process_inputs).grid(row=0, column=2, padx=10)

        self.input_frame = frame
        self.process_frame = None
        self.result_frame = None

    def create_process_inputs(self):
        try:
            num = int(self.num_entry.get())
            if num <= 0:
                raise ValueError
        except ValueError:
            messagebox.showerror("Invalid Input", "Enter a valid positive integer.")
            return

        if self.process_frame:
            self.process_frame.destroy()
        if self.result_frame:
            self.result_frame.destroy()

        self.entries.clear()
        self.process_frame = tk.Frame(self.root, padx=20, pady=10)
        self.process_frame.pack()

        headers = ["PID", "Arrival Time", "Burst Time"]
        for j, h in enumerate(headers):
            tk.Label(self.process_frame, text=h, width=12, font=('Arial', 10, 'bold')).grid(row=0, column=j)

        for i in range(num):
            pid_label = tk.Label(self.process_frame, text=f"P{i+1}")
            pid_label.grid(row=i+1, column=0)

            at_entry = tk.Entry(self.process_frame, width=10)
            bt_entry = tk.Entry(self.process_frame, width=10)
            at_entry.grid(row=i+1, column=1)
            bt_entry.grid(row=i+1, column=2)
            self.entries.append((at_entry, bt_entry))

        tk.Button(self.process_frame, text="Simulate", command=self.simulate).grid(row=num+1, column=1, pady=10)

    def simulate(self):
        self.process_data.clear()
        for i, (at_entry, bt_entry) in enumerate(self.entries):
            try:
                at = int(at_entry.get())
                bt = int(bt_entry.get())
                if at < 0 or bt <= 0:
                    raise ValueError
                self.process_data.append([i + 1, at, bt])
            except ValueError:
                messagebox.showerror("Invalid Input", f"Invalid values for Process {i+1}")
                return

        self.process_data.sort(key=lambda x: (x[1], x[2]))  # sort by arrival time then burst time

        time = 0
        complete = []
        remaining = self.process_data.copy()
        while remaining:
            available = [p for p in remaining if p[1] <= time]
            if not available:
                time = remaining[0][1]
                available = [p for p in remaining if p[1] <= time]

            next_proc = min(available, key=lambda x: x[2])
            remaining.remove(next_proc)
            pid, at, bt = next_proc
            st = time
            ct = st + bt
            wt = st - at
            tat = ct - at
            rt = wt
            complete.append((pid, at, bt, st, ct, wt, tat, rt))
            time = ct

        self.show_results(complete)

    def show_results(self, complete):
        if self.result_frame:
            self.result_frame.destroy()
        self.result_frame = tk.Frame(self.root, padx=20, pady=10)
        self.result_frame.pack()

        headers = ["PID", "AT", "BT", "ST", "CT", "WT", "TAT", "RT"]
        for j, h in enumerate(headers):
            tk.Label(self.result_frame, text=h, width=10, font=('Arial', 10, 'bold')).grid(row=0, column=j)

        wt_sum = tat_sum = rt_sum = 0
        for i, row in enumerate(complete):
            for j, val in enumerate(row):
                tk.Label(self.result_frame, text=str(val), width=10).grid(row=i+1, column=j)
            wt_sum += row[5]
            tat_sum += row[6]
            rt_sum += row[7]

        n = len(complete)
        tk.Label(self.result_frame, text=f"Avg WT: {wt_sum/n:.2f}", font=('Arial', 10, 'bold')).grid(row=n+2, column=0, columnspan=2)
        tk.Label(self.result_frame, text=f"Avg TAT: {tat_sum/n:.2f}", font=('Arial', 10, 'bold')).grid(row=n+2, column=2, columnspan=2)
        tk.Label(self.result_frame, text=f"Avg RT: {rt_sum/n:.2f}", font=('Arial', 10, 'bold')).grid(row=n+2, column=4, columnspan=2)

        self.draw_gantt_chart(complete)

    def draw_gantt_chart(self, complete):
        gantt_window = tk.Toplevel(self.root)
        gantt_window.title("Gantt Chart")

        fig, ax = plt.subplots(figsize=(10, 2))
        for p in complete:
            pid, _, _, st, ct, _, _, _ = p
            ax.barh(0, ct - st, left=st, height=0.5)
            ax.text((st + ct)/2, 0, f"P{pid}", ha='center', va='center', color='white')

        ax.set_yticks([])
        ax.set_xlabel("Time")
        ax.set_xlim(0, complete[-1][4] + 2)
        ax.set_title("Gantt Chart")
        ax.grid(True, axis='x')

        canvas = FigureCanvasTkAgg(fig, master=gantt_window)
        canvas.draw()
        canvas.get_tk_widget().pack()


if __name__ == "__main__":
    root = tk.Tk()
    app = SJFSimulator(root)
    root.mainloop()
