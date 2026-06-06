# MicroJava Web IDE

A full-stack web interface for the MicroJava compiler with Monaco editor, real-time error highlighting, and multiple parsing strategies.

## Features

- **Monaco Editor** with Java syntax highlighting and line numbers
- **Real-time error highlighting** (lexical, syntax, semantic) with clickable line:column references
- **Multiple parsing modes**: Lexer, Recursive Descent, Predictive (LL), SLR (LR) parsers
- **Sample code loader** with pre-built test programs (hello, factorial, arrays, invalid examples)
- **Tabbed output** showing errors, stdout, and stderr separately
- **Keyboard shortcut**: Ctrl+Enter to compile
- **Download compiled code** as `.mj` files
- **Professional UI** with dark accents and responsive layout

## Architecture

```
backend/  ─── Flask API, runs build.bat and compiler.exe, parses errors
frontend/ ─── React + Vite, Monaco editor, tabbed error/output view
```

## Quick Start

### Prerequisites

- **Python 3.8+** with pip
- **Node.js 16+** with npm
- **GCC** on PATH (e.g., `gcc --version` works from PowerShell)
  - On Windows: install MSYS2 MinGW-w64 or TDM-GCC and add `bin` to PATH

### Setup

#### Backend (Flask)

```powershell
cd web/backend
python -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install -r requirements.txt
python app.py
# Server runs at http://127.0.0.1:5000
```

#### Frontend (React + Vite)

In a separate terminal:

```powershell
cd web/frontend
npm install
npm run dev
# Open http://localhost:5173 (or URL printed by Vite)
```

### Usage

1. **Start with a sample**: Click any sample button (Hello, Factorial, Arrays, etc.)
2. **Select a parsing mode** from the dropdown (default: All Parsers)
3. **Click Compile or press Ctrl+Enter**
4. **View results**:
   - **Errors tab**: clickable error list with line:column; click to jump to location
   - **Stdout tab**: program output (e.g., from lexer or parser trace)
   - **Stderr tab**: error messages and parser diagnostics
5. **Download code**: Click the Download button to export `.mj` file
6. **Edit and re-compile**: changes are preserved across compilations

## Parsing Modes

- **Lexer**: Tokenization only; detects lexical errors (invalid characters)
- **Recursive Descent**: Top-down parsing; reports syntax errors with LL(1) lookahead issues
- **Predictive (LL)**: Predictive parsing table; reports LL(1) conflicts
- **LR Parser (SLR)**: Bottom-up parsing; detailed state trace and LR error entries
- **All**: Runs all parsers sequentially; useful for comparison

## Error Highlighting

Errors are parsed from compiler stderr in the format:
```
<Kind> error at <line>:<column>: <message>
```

Each error is:
- Underlined in the editor with a red wavy border
- Listed in the Errors tab (clickable, jumps to location)
- Displayed with kind, line, column, and full message

## Architecture Details

### Backend (Flask)

**Endpoint**: `POST /api/compile`

**Request**:
```json
{
  "source": "class Hello { ... }",
  "mode": "all",
  "timeout": 10
}
```

**Response**:
```json
{
  "stdout": "...",
  "stderr": "...",
  "errors": [
    {"kind": "Syntax", "line": 2, "column": 3, "message": "expected }, found KEYWORD 'public'"}
  ],
  "returncode": 1
}
```

**Features**:
- Auto-builds `compiler.exe` via `build.bat` if missing
- Writes temp `.mj` file to `web/tmp/`
- Runs compiler with timeout protection (default 10s)
- Parses stderr for structured errors
- CORS headers enabled for local frontend

### Frontend (React + Vite)

**Components**:
- Editor: Monaco editor with decorations for errors
- Mode selector: dropdown for compile mode
- Sample loader: quick-load test files
- Output tabs: errors, stdout, stderr views
- Error list: clickable errors that jump to editor location

**Keyboard shortcuts**:
- **Ctrl+Enter**: Compile
- Click error in list: Jump to location

## Troubleshooting

### "gcc not found"

Ensure `gcc` is on your `PATH`:
```powershell
gcc --version
where.exe gcc
```

If not, add the GCC bin folder to PATH, e.g.:
```powershell
$env:Path += ';C:\msys64\mingw64\bin'
```

### Flask server won't start

- Check that port 5000 is not in use: `netstat -ano | findstr :5000`
- Ensure Python venv is activated: `.\.venv\Scripts\Activate.ps1`
- Verify Flask is installed: `python -m pip show flask`

### Frontend won't load

- Ensure `npm install` completed without errors
- Check that `npm run dev` is running and shows a localhost URL
- Verify backend is running (should see output in Flask terminal)
- Check browser console (F12) for CORS or network errors

### Compile errors all parsers

- This is expected for invalid or unsupported MicroJava syntax
- Try a sample first (e.g., Hello)
- MicroJava syntax is simplified: no `public`/`static`, no `String`, uses `print()` not `System.out.println()`

## Development

### Backend changes

Edit `web/backend/app.py` and restart Flask:
```powershell
# Ctrl+C to stop
python app.py
```

### Frontend changes

Vite auto-reloads on file save. Edit `web/frontend/src/App.jsx` or `src/index.css` and refresh the browser.

To rebuild for production:
```powershell
npm run build  # outputs to web/frontend/dist/
```

## Future Enhancements

- [ ] File persistence (save/load projects to localStorage)
- [ ] Theme toggle (dark/light mode)
- [ ] Semantic error highlighting
- [ ] Compilation time display
- [ ] AST viewer for successful parses
- [ ] Multiple file support / project upload

## License

Part of the MicroJava mini-compiler project.
