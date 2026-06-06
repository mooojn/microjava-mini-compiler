# Mini-Java Compiler Web Interface

This directory contains the web-based graphical user interface for the Mini-Java Compiler, built using Flask (Python). It provides an interactive environment to write code, select compiler passes, and view compiler output in real-time.

## Features

- **Modern UI**: A clean, light-mode interface with glassmorphic elements and syntax-friendly typography.
- **Prebuilt Tests Browser**: A sidebar that automatically lists all `.mj` test files located in the `../test/samples/` directory. You can click on any test file to instantly load its contents into the editor.
- **Interactive Editor**: A built-in code editor area for writing and modifying your Mini-Java code.
- **Compiler Options**: 
  - A dropdown to select specific compiler modes (`Lexer`, `Recursive Descent`, `Predictive LL(1)`, `LR Parser`, or `All Passes`).
  - A toggle to "Dump Symbols" during compilation.
- **Output Console**: A dedicated console window that captures and displays the standard output (and standard error) from the underlying C compiler executable.

## Directory Structure

- `server.py`: The main Flask server application. Handles rendering the frontend, API routes for fetching tests, and executing the `compiler.exe` sub-process.
- `templates/`: Contains the HTML layout for the frontend.
  - `index.html`: The main markup structure of the UI.
- `static/`: Contains the frontend assets.
  - `css/style.css`: Stylesheet implementing the modern aesthetic.
  - `js/script.js`: Client-side logic for API communication, test loading, and UI interactivity.

## Getting Started

### Prerequisites
Make sure you have Python installed and the compiler executable built. The server looks for the compiled executable (`compiler` or `compiler.exe`) in the parent directory of this `app` folder.

### Running the App
1. Open a terminal and navigate to the root directory of your project.
2. Run the Flask server:
   ```bash
   python app/server.py
   ```
3. Open a web browser and navigate to `http://localhost:5000` (or the port specified by the server).

## API Endpoints (Used by Frontend)

- `GET /`: Renders the main user interface.
- `GET /api/tests`: Returns a JSON list of available `.mj` files found in the test directory.
- `GET /api/tests/<filename>`: Returns the raw code content of a specific test file.
- `POST /compile`: Receives code and compiler options from the frontend, writes the code to a temporary file, executes the C compiler, and returns the console output.
