# Backend (Flask) for microjava-mini-compiler web UI

Quickstart

1. Install dependencies (in a virtualenv recommended):

```powershell
python -m venv .venv
.\.venv\Scripts\activate
pip install -r requirements.txt
```

2. Run the server:

```powershell
python app.py
```

3. POST source code to http://127.0.0.1:5000/api/compile with JSON {"source": "...", "mode": "all"}

Dependencies
- `gcc` must be installed and available on `PATH` because `build.bat` compiles the compiler binary before each first run.
- On Windows, install MinGW/MSYS2 or another toolchain that provides `gcc`.

Notes
- The backend will call `build.bat` in the repo root to build `compiler.exe` if it is missing.
- For security, do not expose this server to untrusted users without sandboxing (the compiler runs native code).
