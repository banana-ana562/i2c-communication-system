# keil/ – Keil uVision Project Folder

This folder contains the **Keil uVision project configuration files**.

It connects the IDE to the source code located in the `/src` directory.

---

## 📁 Folder Structure

```
keil/
├── Project.uvprojx # Keil project file (REQUIRED)
├── Project.uvoptx # User-specific settings (DO NOT COMMIT)
└── README.md
```

---

# 🔧 Purpose of This Folder

This folder stores:

- Keil project configuration (`.uvprojx`)
- Build settings (compiler, linker, target)
- Debug configuration

⚠️ This folder does **NOT** contain system logic.

All source code must remain in:
```
/src
```

---

# 🔄 Workflow Alignment (3-Step)

| Workflow Step | Uses Keil? |
|---------------|-----------|
| Step 1 – Requirements & Design | ❌ No |
| Step 2 – Module Implementation & Testing | ✅ Yes |
| Step 3 – Integration & Validation | ✅ Yes |

Keil is used only for:
- Building code  
- Debugging modules  
- Running the system  

It is **not used for design or documentation**.

---

# 📌 Important Rules

- Do NOT move `.c` or `.h` files into this folder  
- Do NOT duplicate source files inside Keil  
- All code must stay in `/src`  
- Always add new files to Keil using:
```
Project → Add Existing Files
```
- Keep all paths **relative** (no absolute paths)

---

# ⚠️ Version Control Rules (VERY IMPORTANT)

### ✅ Commit:
```
*.uvprojx
```

### ❌ Do NOT commit:
```
*.uvoptx
*.axf
*.map
Objects/
Listings/
Debug/
```

Why:
- `.uvoptx` is machine-specific (causes merge conflicts)
- Build outputs clutter the repo and break grading

---

# 🧱 Recommended Workflow

When adding a new module:

1. Create files in:
```
src/modules/
```
2. Create drivers in:

src/drivers/

3. Add files to Keil project
4. Test using `ModuleTest.c`

---

# 🧪 How Keil Fits into Your Workflow

### Step 2 (Modules)
- Build and test each module individually  
- Debug using breakpoints, registers, UART output  
- Save evidence in `/evidence/Step2_ModuleTests/`

### Step 3 (Integration)
- Build full system  
- Verify system behavior  
- Run robustness tests  
- Capture demo evidence  

---

# 🏭 Industry Alignment

This structure mirrors professional embedded systems projects:

- `/src` → Firmware code  
- `/docs` → Design & documentation  
- `/evidence` → Test & validation artifacts  
- `/keil` → Toolchain configuration  

Separating these improves:

- Maintainability  
- Portability  
- Team collaboration  
- Debugging clarity  

---

# ⚠️ Common Mistakes (Avoid These)

- ❌ Putting code inside `/keil`
- ❌ Committing `.uvoptx` files
- ❌ Using absolute file paths
- ❌ Not adding new files to project

---

This folder is only for **building and debugging your system** — not for writing your system.
