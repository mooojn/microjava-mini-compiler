from flask import Flask, request, jsonify
import os
import subprocess
import tempfile
import time

app = Flask(__name__)


def repo_root():
    # repo root is two levels up from this file (web/backend -> web -> repo)
    return os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))


BUILD_BAT = os.path.join(repo_root(), 'build.bat')
COMPILER_EXE = os.path.join(repo_root(), 'compiler.exe')


def ensure_compiler():
    if os.path.exists(COMPILER_EXE):
        return True, None
    if not os.path.exists(BUILD_BAT):
        return False, f"build.bat not found at {BUILD_BAT}"
    try:
        proc = subprocess.run([
            'cmd.exe', '/c', 'call', BUILD_BAT
        ], cwd=repo_root(), capture_output=True, text=True)
        if proc.returncode != 0:
            return False, f"Build failed with exit code {proc.returncode}. stdout:\n{proc.stdout}\nstderr:\n{proc.stderr}"
        if not os.path.exists(COMPILER_EXE):
            return False, f"Build completed but compiler.exe was not created. stdout:\n{proc.stdout}\nstderr:\n{proc.stderr}"
        return True, None
    except Exception as e:
        return False, f"Build exception: {e}"


@app.after_request
def add_cors(resp):
    resp.headers['Access-Control-Allow-Origin'] = '*'
    resp.headers['Access-Control-Allow-Headers'] = 'Content-Type'
    return resp


@app.route('/api/compile', methods=['POST'])
def compile_source():
    data = request.get_json(force=True)
    source = data.get('source')
    mode = data.get('mode', 'all')
    timeout = int(data.get('timeout', 10))

    if not source:
        return jsonify({'error': 'no source provided'}), 400

    ok, msg = ensure_compiler()
    if not ok:
        return jsonify({'error': 'compiler not available', 'detail': msg}), 500

    tmpdir = os.path.join(repo_root(), 'web', 'tmp')
    os.makedirs(tmpdir, exist_ok=True)
    ts = int(time.time() * 1000)
    src_path = os.path.join(tmpdir, f'source_{ts}.mj')
    with open(src_path, 'w', encoding='utf-8') as f:
        f.write(source)

    cmd = [COMPILER_EXE, mode, src_path]
    try:
        proc = subprocess.run(cmd, cwd=repo_root(), capture_output=True, text=True, timeout=timeout)

        # Log full compiler output to the server terminal for debugging/visibility
        try:
            print("--- compiler stdout ---", flush=True)
            print(proc.stdout, flush=True)
            print("--- compiler stderr ---", flush=True)
            print(proc.stderr, flush=True)
        except Exception:
            pass

        # Parse stderr for structured errors of the form:
        # "<Kind> error at <line>:<column>: <message>"
        errors = []
        for line in proc.stderr.splitlines():
            line = line.strip()
            if not line:
                continue
            import re
            m = re.match(r'^(?P<kind>\w+) error at (?P<line>\d+):(?P<column>\d+): (?P<msg>.*)$', line)
            if m:
                errors.append({
                    'kind': m.group('kind'),
                    'line': int(m.group('line')),
                    'column': int(m.group('column')),
                    'message': m.group('msg')
                })

        return jsonify({'stdout': proc.stdout, 'stderr': proc.stderr, 'errors': errors, 'returncode': proc.returncode})
    except subprocess.TimeoutExpired:
        return jsonify({'error': 'timeout', 'detail': f'Process exceeded {timeout} seconds'}), 504
    except Exception as e:
        return jsonify({'error': 'exception', 'detail': str(e)}), 500


if __name__ == '__main__':
    app.run(host='127.0.0.1', port=5000, debug=True)
