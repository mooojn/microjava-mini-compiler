document.addEventListener('DOMContentLoaded', () => {
    const testListEl = document.getElementById('testList');
    const sourceCodeEl = document.getElementById('sourceCode');
    const currentFileNameEl = document.getElementById('currentFileName');
    const compileBtn = document.getElementById('compileBtn');
    const outputConsole = document.getElementById('outputConsole');
    const clearBtn = document.getElementById('clearBtn');
    
    // UI Elements
    const modeSelect = document.getElementById('mode');
    const dumpSymbolsCheckbox = document.getElementById('dumpSymbols');

    // Fetch and list tests
    async function fetchTests() {
        try {
            const response = await fetch('/api/tests');
            const data = await response.json();
            
            if (data.error || !data.tests) {
                testListEl.innerHTML = '<li class="error">Failed to load tests</li>';
                return;
            }

            testListEl.innerHTML = ''; // Clear loading
            
            if (data.tests.length === 0) {
                testListEl.innerHTML = '<li>No tests found</li>';
                return;
            }

            data.tests.forEach(testFile => {
                const li = document.createElement('li');
                li.textContent = testFile;
                li.addEventListener('click', () => loadTest(testFile, li));
                testListEl.appendChild(li);
            });
            
        } catch (error) {
            testListEl.innerHTML = `<li class="error">Error: ${error.message}</li>`;
        }
    }

    // Load specific test
    async function loadTest(filename, listItem) {
        // Update active state
        document.querySelectorAll('.test-list li').forEach(li => li.classList.remove('active'));
        if (listItem) listItem.classList.add('active');

        try {
            const response = await fetch(`/api/tests/${filename}`);
            const data = await response.json();
            
            if (data.error) {
                alert(`Failed to load ${filename}: ${data.content}`);
                return;
            }

            sourceCodeEl.value = data.content;
            currentFileNameEl.textContent = filename;
            outputConsole.textContent = 'Ready.';
            outputConsole.classList.remove('error');
            
        } catch (error) {
            alert(`Error loading test: ${error.message}`);
        }
    }

    // Compile code
    compileBtn.addEventListener('click', async () => {
        const code = sourceCodeEl.value;
        const mode = modeSelect.value;
        const dumpSymbols = dumpSymbolsCheckbox.checked;

        compileBtn.disabled = true;
        compileBtn.innerHTML = `
            <svg class="animate-spin" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" style="animation: spin 1s linear infinite;"><line x1="12" y1="2" x2="12" y2="6"></line><line x1="12" y1="18" x2="12" y2="22"></line><line x1="4.93" y1="4.93" x2="7.76" y2="7.76"></line><line x1="16.24" y1="16.24" x2="19.07" y2="19.07"></line><line x1="2" y1="12" x2="6" y2="12"></line><line x1="18" y1="12" x2="22" y2="12"></line><line x1="4.93" y1="19.07" x2="7.76" y2="16.24"></line><line x1="16.24" y1="7.76" x2="19.07" y2="4.93"></line></svg>
            Compiling...
        `;
        
        outputConsole.textContent = 'Compiling...';
        outputConsole.classList.remove('error');

        try {
            const response = await fetch('/compile', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ code, mode, dumpSymbols })
            });

            const result = await response.json();
            
            if (result.error) {
                outputConsole.classList.add('error');
            }
            outputConsole.textContent = result.output || "No output returned.";
        } catch (err) {
            outputConsole.classList.add('error');
            outputConsole.textContent = `Error: ${err.message}`;
        } finally {
            compileBtn.disabled = false;
            compileBtn.innerHTML = `
                <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><polygon points="5 3 19 12 5 21 5 3"></polygon></svg>
                Compile
            `;
        }
    });

    // Clear console
    clearBtn.addEventListener('click', () => {
        outputConsole.textContent = 'Ready.';
        outputConsole.classList.remove('error');
    });

    // Add spin animation CSS dynamically
    const style = document.createElement('style');
    style.innerHTML = `
        @keyframes spin { 100% { transform: rotate(360deg); } }
    `;
    document.head.appendChild(style);

    // Initial load
    fetchTests();
});
