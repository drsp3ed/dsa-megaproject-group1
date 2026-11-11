import tkinter as tk
from tkinter import messagebox, filedialog
import datetime
import json
import os


# --- Doubly Linked List Node ---
class HistoryNode:
    def __init__(self, url, title=None, timestamp=None):
        self.url = url
        self.title = title if title else url
        if timestamp:
            self.timestamp = timestamp
        else:
            self.timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        self.prev = None
        self.next = None


# --- Browser History Management (Doubly Linked List from scratch) ---
class BrowserHistory:
    def __init__(self, homepage="https://www.google.com", capacity=50):
        self.head = None
        self.tail = None
        self.current = None
        self.size = 0
        self.capacity = capacity
        # stack to store deleted nodes for restore
        self.deleted_stack = []

        if homepage:
            self.visit(homepage)

    def visit(self, url, title=None):
        new_node = HistoryNode(url, title)

        if self.current is None:
            self.head = new_node
            self.tail = new_node
            self.current = new_node
            self.size = 1
            return

        # clear forward history
        temp = self.current.next
        while temp is not None:
            nxt = temp.next
            temp.prev = None
            temp.next = None
            self.size -= 1
            temp = nxt

        # append new page
        self.current.next = new_node
        new_node.prev = self.current
        self.current = new_node
        self.tail = new_node
        self.size += 1

        # remove oldest if exceeding capacity
        if self.size > self.capacity:
            old_head = self.head
            self.head = self.head.next
            if self.head:
                self.head.prev = None
            self.size -= 1

    def back(self, steps=1):
        if self.current is None or steps <= 0:
            return self.current.url if self.current else ""

        while steps > 0 and self.current.prev is not None:
            self.current = self.current.prev
            steps -= 1

        return self.current.url

    def forward(self, steps=1):
        if self.current is None or steps <= 0:
            return self.current.url if self.current else ""

        while steps > 0 and self.current.next is not None:
            self.current = self.current.next
            steps -= 1

        return self.current.url

    def current_page(self):
        if self.current is None:
            return None
        return {
            "url": self.current.url,
            "title": self.current.title,
            "timestamp": self.current.timestamp
        }

    def delete_history(self):
        self.head = None
        self.tail = None
        self.current = None
        self.size = 0
        self.deleted_stack.clear()

    def delete_url(self, url):
        temp = self.head
        pos = 0

        while temp is not None:
            if temp.url == url:
                was_current = (temp == self.current)
                # save to stack
                self.deleted_stack.append({
                    "url": temp.url,
                    "title": temp.title,
                    "timestamp": temp.timestamp,
                    "pos": pos,
                    "was_current": was_current
                })

                prev_node = temp.prev
                next_node = temp.next

                if prev_node:
                    prev_node.next = next_node
                else:
                    self.head = next_node

                if next_node:
                    next_node.prev = prev_node
                else:
                    self.tail = prev_node

                if was_current:
                    self.current = prev_node if prev_node else next_node

                self.size -= 1
                return True

            temp = temp.next
            pos += 1

        return False

    def restore(self):
        if not self.deleted_stack:
            return None

        data = self.deleted_stack.pop()
        node = HistoryNode(data["url"], data["title"], data["timestamp"])

        if self.head is None or data["pos"] <= 0:
            node.next = self.head
            if self.head:
                self.head.prev = node
            self.head = node
            if self.tail is None:
                self.tail = node
        else:
            temp = self.head
            for _ in range(data["pos"] - 1):
                if temp.next is None:
                    break
                temp = temp.next

            nxt = temp.next
            node.prev = temp
            node.next = nxt
            temp.next = node
            if nxt:
                nxt.prev = node
            else:
                self.tail = node

        if data["was_current"]:
            self.current = node

        self.size += 1
        if self.size > self.capacity:
            old_head = self.head
            self.head = self.head.next
            if self.head:
                self.head.prev = None
            if self.current == old_head:
                self.current = self.head
            self.size -= 1

        return data["url"]

    def export_history(self, filename):
        items = []
        temp = self.head
        curr_idx = 0
        idx = 0
        while temp is not None:
            if temp == self.current:
                curr_idx = idx
            items.append({
                "url": temp.url,
                "title": temp.title,
                "timestamp": temp.timestamp
            })
            temp = temp.next
            idx += 1

        if filename.endswith(".json"):
            with open(filename, "w", encoding="utf-8") as f:
                json.dump({"current": curr_idx, "history": items}, f, indent=2)
        else:
            with open(filename, "w", encoding="utf-8") as f:
                for i, it in enumerate(items):
                    tag = "CURRENT" if i == curr_idx else "PAGE"
                    f.write(f"{tag}\t{it['url']}\t{it['title']}\t{it['timestamp']}\n")

    def import_history(self, filename):
        if not os.path.exists(filename):
            return False

        self.delete_history()

        if filename.endswith(".json"):
            with open(filename, "r", encoding="utf-8") as f:
                content = json.load(f)
                items = content.get("history", [])
                curr_idx = content.get("current", 0)

            for it in items:
                node = HistoryNode(it["url"], it.get("title"), it.get("timestamp"))
                self._append(node)

            self._set_current(curr_idx)
        else:
            curr_idx = 0
            with open(filename, "r", encoding="utf-8") as f:
                for idx, line in enumerate(f):
                    parts = line.strip().split("\t")
                    if len(parts) >= 4:
                        if parts[0] == "CURRENT":
                            curr_idx = idx
                        node = HistoryNode(parts[1], parts[2], parts[3])
                        self._append(node)

            self._set_current(curr_idx)

        return True

    def _append(self, node):
        if self.head is None:
            self.head = node
            self.tail = node
            self.current = node
            self.size = 1
        else:
            self.tail.next = node
            node.prev = self.tail
            self.tail = node
            self.size += 1

    def _set_current(self, idx):
        temp = self.head
        for _ in range(idx):
            if temp and temp.next:
                temp = temp.next
        self.current = temp if temp else self.head


# --- GUI Interface (Tkinter) ---
class BrowserGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Browser History Simulator")
        self.root.geometry("750x520")

        self.history = BrowserHistory("https://www.google.com")

        self.setup_ui()
        self.update_ui()

    def setup_ui(self):
        # top bar (back, forward, entry, visit)
        nav_frame = tk.Frame(self.root, pady=8, padx=10)
        nav_frame.pack(fill=tk.X)

        self.btn_back = tk.Button(nav_frame, text="< Back", width=8, command=self.go_back)
        self.btn_back.pack(side=tk.LEFT, padx=3)

        self.btn_forward = tk.Button(nav_frame, text="Forward >", width=8, command=self.go_forward)
        self.btn_forward.pack(side=tk.LEFT, padx=3)

        self.url_entry = tk.Entry(nav_frame, font=("Arial", 10))
        self.url_entry.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=6)
        self.url_entry.bind("<Return>", lambda event: self.visit_page())

        self.btn_visit = tk.Button(nav_frame, text="Visit", width=8, bg="#e0e0e0", command=self.visit_page)
        self.btn_visit.pack(side=tk.LEFT, padx=3)

        # metadata display
        self.info_frame = tk.LabelFrame(self.root, text="Current Page Metadata", padx=10, pady=8)
        self.info_frame.pack(fill=tk.X, padx=10, pady=5)

        self.lbl_url = tk.Label(self.info_frame, text="URL: -", font=("Arial", 10, "bold"), anchor="w")
        self.lbl_url.pack(fill=tk.X)

        self.lbl_meta = tk.Label(self.info_frame, text="Opened At: -", fg="#555555", anchor="w")
        self.lbl_meta.pack(fill=tk.X)

        # history listbox
        hist_frame = tk.LabelFrame(self.root, text="Browsing History (Doubly Linked List)", padx=10, pady=8)
        hist_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)

        scrollbar = tk.Scrollbar(hist_frame)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        self.history_listbox = tk.Listbox(hist_frame, font=("Courier", 10), yscrollcommand=scrollbar.set)
        self.history_listbox.pack(fill=tk.BOTH, expand=True)
        scrollbar.config(command=self.history_listbox.yview)

        # buttons at the bottom
        btn_frame = tk.Frame(self.root, pady=8, padx=10)
        btn_frame.pack(fill=tk.X)

        self.btn_del = tk.Button(btn_frame, text="Delete Selected", command=self.delete_selected)
        self.btn_del.pack(side=tk.LEFT, padx=4)

        self.btn_restore = tk.Button(btn_frame, text="Restore (Undo)", command=self.restore_page)
        self.btn_restore.pack(side=tk.LEFT, padx=4)

        self.btn_clear = tk.Button(btn_frame, text="Clear All", command=self.clear_all)
        self.btn_clear.pack(side=tk.LEFT, padx=4)

        self.btn_export = tk.Button(btn_frame, text="Export History", command=self.export_file)
        self.btn_export.pack(side=tk.RIGHT, padx=4)

        self.btn_import = tk.Button(btn_frame, text="Import History", command=self.import_file)
        self.btn_import.pack(side=tk.RIGHT, padx=4)

        # status bar
        self.lbl_status = tk.Label(self.root, text="", bd=1, relief=tk.SUNKEN, anchor="w", padx=6, pady=2, fg="#333333")
        self.lbl_status.pack(side=tk.BOTTOM, fill=tk.X)

    def visit_page(self):
        url = self.url_entry.get().strip()
        if not url:
            return
        if not url.startswith("http://") and not url.startswith("https://"):
            url = "https://" + url

        self.history.visit(url)
        self.update_ui()

    def go_back(self):
        self.history.back(1)
        self.update_ui()

    def go_forward(self):
        self.history.forward(1)
        self.update_ui()

    def delete_selected(self):
        idx = self.history_listbox.curselection()
        if not idx:
            messagebox.showwarning("Warning", "Please select a URL from the history list.")
            return

        # extract url from current list
        items = []
        temp = self.history.head
        while temp:
            items.append(temp.url)
            temp = temp.next

        if idx[0] < len(items):
            url_to_delete = items[idx[0]]
            self.history.delete_url(url_to_delete)
            self.update_ui()

    def restore_page(self):
        restored = self.history.restore()
        if restored:
            self.update_ui()
        else:
            messagebox.showinfo("Restore", "No deleted pages to restore.")

    def clear_all(self):
        if messagebox.askyesno("Confirm", "Delete all browsing history?"):
            self.history.delete_history()
            self.update_ui()

    def export_file(self):
        filename = filedialog.asksaveasfilename(
            defaultextension=".json",
            filetypes=[("JSON files", "*.json"), ("Text files", "*.txt")]
        )
        if filename:
            self.history.export_history(filename)
            messagebox.showinfo("Export", f"History saved to {filename}")

    def import_file(self):
        filename = filedialog.askopenfilename(
            filetypes=[("JSON files", "*.json"), ("Text files", "*.txt")]
        )
        if filename:
            if self.history.import_history(filename):
                self.update_ui()
                messagebox.showinfo("Import", "History loaded successfully.")
            else:
                messagebox.showerror("Error", "Could not load history.")

    def update_ui(self):
        # update current page info
        curr = self.history.current_page()
        if curr:
            self.url_entry.delete(0, tk.END)
            self.url_entry.insert(0, curr["url"])
            self.lbl_url.config(text=f"URL: {curr['url']}")
            self.lbl_meta.config(text=f"Opened At: {curr['timestamp']}  |  Title: {curr['title']}")
        else:
            self.url_entry.delete(0, tk.END)
            self.lbl_url.config(text="URL: (No page)")
            self.lbl_meta.config(text="Opened At: -")

        # update history listbox
        self.history_listbox.delete(0, tk.END)
        temp = self.history.head
        selected_index = -1
        idx = 0

        while temp:
            if temp == self.history.current:
                line = f"[<] {temp.url}  ({temp.timestamp})"
                selected_index = idx
            else:
                line = f"    {temp.url}  ({temp.timestamp})"
            self.history_listbox.insert(tk.END, line)
            temp = temp.next
            idx += 1

        if selected_index != -1:
            self.history_listbox.selection_set(selected_index)
            self.history_listbox.see(selected_index)

        # update back/forward button states
        if self.history.current:
            self.btn_back.config(state=tk.NORMAL if self.history.current.prev else tk.DISABLED)
            self.btn_forward.config(state=tk.NORMAL if self.history.current.next else tk.DISABLED)
        else:
            self.btn_back.config(state=tk.DISABLED)
            self.btn_forward.config(state=tk.DISABLED)

        # update status bar
        deleted_count = len(self.history.deleted_stack)
        self.lbl_status.config(text=f"Total pages: {self.history.size}/{self.history.capacity} | Deleted in stack: {deleted_count}")


if __name__ == "__main__":
    root = tk.Tk()
    app = BrowserGUI(root)
    root.mainloop()
