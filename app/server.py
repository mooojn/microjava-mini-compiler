import os
import subprocess
import tempfile
from flask import Flask, render_template, request, jsonify

app = Flask(__name__)

# Path to the compiler executable (located in the parent directory of this app folder)
BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
COMPILER_EXE = os.path.join(BASE_DIR, "compiler.exe" if os.name == 'nt' else "compiler")

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/compile', methods=['POST'])
def compile_code():
    data = request.json
    code = data.get('code', '')
    mode = data.get('mode', 'all')
    dump_symbols = data.get('dumpSymbols', False)

    if not code.strip():
        return jsonify({'output': 'No code provided', 'error': True})

    # Check if compiler exists
    if not os.path.exists(COMPILER_EXE):
        # We might need to build it run 'make compiler' or 'build.bat'
        # To avoid unexpected slow compilations, just report the error.
        return jsonify({'output': f'Compiler executable "{COMPILER_EXE}" not found. Please compile the C project first.', 'error': True})

    try:
        # Create a temporary file with the code (.mj extension)
        fd, temp_path = tempfile.mkstemp(suffix=".mj")
        with os.fdopen(fd, 'w', encoding='utf-8') as f:
            f.write(code)

        # Build the command
        cmd = [COMPILER_EXE, mode, temp_path]
        if dump_symbols and mode != 'lexer':
            cmd.append("--dump-symbols")

        # Run process
        process = subprocess.Popen(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,  # Combine stdout and stderr
            text=True,
            encoding='utf-8',
            errors='replace'
        )

        stdout_data, _ = process.communicate(timeout=10)
        
        # Clean up temp file
        os.remove(temp_path)

        return jsonify({'output': stdout_data, 'error': process.returncode != 0})
    except subprocess.TimeoutExpired:
        process.kill()
        return jsonify({'output': 'Compilation timed out.', 'error': True})
    except Exception as e:
        return jsonify({'output': str(e), 'error': True})

@app.route('/api/tests', methods=['GET'])
def list_tests():
    tests_dir = os.path.join(BASE_DIR, "test", "samples")
    if not os.path.exists(tests_dir):
        return jsonify({'error': True, 'tests': []})
    
    test_files = [f for f in os.listdir(tests_dir) if f.endswith('.mj')]
    return jsonify({'error': False, 'tests': test_files})

@app.route('/api/tests/<filename>', methods=['GET'])
def get_test(filename):
    if not filename.endswith('.mj'):
        return jsonify({'error': True, 'content': 'Invalid file type'})
        
    filepath = os.path.join(BASE_DIR, "test", "samples", filename)
    if not os.path.exists(filepath):
        return jsonify({'error': True, 'content': 'File not found'})
        
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
        
    return jsonify({'error': False, 'content': content})

if __name__ == '__main__':
    app.run(debug=True, port=5000)
